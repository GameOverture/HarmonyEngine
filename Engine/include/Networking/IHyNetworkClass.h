/**************************************************************************
*	IHyNetworkClass.h
*
*	Harmony Engine
*	Copyright (c) 2020 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef IHyNetworkClass_h__
#define IHyNetworkClass_h__

#include "Afx/HyInteropAfx.h"
#include "Threading/IHyThreadClass.h"

class HyNetworking;

class IHyNetworkClass : public IHyThreadClass
{
	friend class HyNetworking;
	static HyNetworking *		sm_pNetworking;

	const uint32				m_uiMAX_RECV_SIZE;

	enum eConnectionState
	{
		CONNECTION_TimeOut,
		CONNECTION_NotConnected,
		CONNECTION_Connected
	};
	eConnectionState			m_eConnState;

	std::string					m_sHost;
	uint16						m_uiPort;
	HyTcpSocket					m_hSock;
	uint32						m_uiAttemptCount;

	uint8 *						m_pRecvBuff;

public:
	IHyNetworkClass(uint32 uiRecvBufferSize, HyThreadPriority ePriority = HYTHREAD_Normal, uint32 uiUpdateThrottleMs = 15);
	virtual ~IHyNetworkClass();

	bool StartClient(const char *szHost, uint16 uiPort);
	
	uint8 *NetRecv(uint32 uiWaitBytes);
	uint8 *NetRecv(char cEndByte, uint32 &uiNumBytesOut);
	bool NetSend(const void *pData, uint32 uiNumBytes);
	
	void CleanupSocket();

protected:
	virtual void OnThreadInit() override final;
	virtual void OnThreadUpdate() override final;
	virtual void OnThreadShutdown() override final;

	virtual void OnNetConnect() = 0;
	virtual void OnNetUpdate() = 0;
	virtual void OnNetShutdown() = 0;
};

#endif /* IHyNetworkClass_h__ */
