/**************************************************************************
*	IHyNetworkClass.cpp
*
*	Harmony Engine
*	Copyright (c) 2020 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Networking/IHyNetworkClass.h"
#include "Networking/HyNetworking.h"
#include "Diagnostics/Console/HyConsole.h"
#include "Utilities/HyMath.h"

/*static*/ HyNetworking *IHyNetworkClass::sm_pNetworking = nullptr;

IHyNetworkClass::IHyNetworkClass(uint32 uiRecvBufferSize, HyThreadPriority ePriority /*= HYTHREAD_Normal*/, uint32 uiUpdateThrottleMs /*= 15*/) :
	IHyThreadClass(ePriority, uiUpdateThrottleMs),
	m_uiMAX_RECV_SIZE(uiRecvBufferSize),
	m_eConnState(CONNECTION_NotConnected),
	m_uiPort(0),
	m_hSock(0),
	m_uiAttemptCount(0),
	m_pRecvBuff(HY_NEW uint8[m_uiMAX_RECV_SIZE])
{
	memset(m_pRecvBuff, 0, m_uiMAX_RECV_SIZE);
}

/*virtual*/ IHyNetworkClass::~IHyNetworkClass()
{
	CleanupSocket();
	delete[] m_pRecvBuff;
}

bool IHyNetworkClass::StartClient(const char *szHost, uint16 uiPort)
{
	if(m_hSock)
	{
		HyLogError("HyNetworking::CreateClient - socket already established");
		return false;
	}

	m_sHost = szHost;
	m_uiPort = uiPort;
	m_uiAttemptCount = 0;

	return ThreadStart();
}

uint8 *IHyNetworkClass::NetRecv(uint32 uiWaitBytes)
{
	uiWaitBytes = HyClamp(uiWaitBytes, 0u, m_uiMAX_RECV_SIZE);
	if(sm_pNetworking->TcpRecv(m_hSock, m_pRecvBuff, static_cast<int>(uiWaitBytes)) <= 0)
		return nullptr;

	return m_pRecvBuff;
}

uint8 *IHyNetworkClass::NetRecv(char cEndByte, uint32 &uiNumBytesOut)
{
	uiNumBytesOut = 0;
	do
	{
		if(sm_pNetworking->TcpRecv(m_hSock, m_pRecvBuff + uiNumBytesOut, 1) <= 0)
			return nullptr;
		uiNumBytesOut++;
	} while(static_cast<char>(m_pRecvBuff[uiNumBytesOut-1]) != cEndByte && uiNumBytesOut < m_uiMAX_RECV_SIZE);

	return m_pRecvBuff;
}

bool IHyNetworkClass::NetSend(const void *pData, uint32 uiNumBytes)
{
	int32 iReturn = sm_pNetworking->TcpSend(m_hSock, pData, uiNumBytes);
	if(iReturn != uiNumBytes)
	{
		HyLogError("IHyNetworkClass::NetSend failed - returned '" << iReturn << "' instead of '" << uiNumBytes << "'");
		return false;
	}

	return true;
}

void IHyNetworkClass::CleanupSocket()
{
	sm_pNetworking->CloseConnection(m_hSock);
	m_hSock = 0;
	m_uiAttemptCount = 0;
	m_eConnState = CONNECTION_NotConnected;

	OnNetShutdown();
}

/*virtual*/ void IHyNetworkClass::OnThreadInit() /*override final*/
{
}

/*virtual*/ void IHyNetworkClass::OnThreadUpdate() /*override final*/
{
	switch(m_eConnState)
	{
	case CONNECTION_TimeOut:
		HyLog("Attmempting to reconnect in 5 seconds...");
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
		m_eConnState = CONNECTION_NotConnected;
		break;
		
	case CONNECTION_NotConnected:
		if(sm_pNetworking->CreateClient(m_sHost, m_uiPort, m_hSock) == false)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			m_uiAttemptCount++;
			if(m_uiAttemptCount > 5)
				m_eConnState = CONNECTION_TimeOut;
			break;
		}

		HyLog(m_sHost << " connected sucessfully");
		m_eConnState = CONNECTION_Connected;
		OnNetConnect();
		break;

	case CONNECTION_Connected:
		OnNetUpdate();
		break;
	}
}

/*virtual*/ void IHyNetworkClass::OnThreadShutdown() /*override final*/
{
	CleanupSocket();
}
