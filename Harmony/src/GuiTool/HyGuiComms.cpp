/**************************************************************************
 *	HyGuiComms.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "GuiTool/HyGuiComms.h"

#ifndef HY_PLATFORM_GUI

HyGuiComms *HyGuiComms::sm_pInstance = NULL;

HyGuiComms::HyGuiComms(void)
{
	HyAssert(sm_pInstance == NULL, "HyGuiComms was instantiated twice");
	NL::init();

	m_pSocket = new NL::Socket("localhost", 1313);
	m_pSocket->blocking(false);

	m_pSocketGroup = new NL::SocketGroup();
	m_pSocketGroup->add(m_pSocket);

	m_pSocketGroup->setCmdOnRead(&m_OnRead);
	m_pSocketGroup->setCmdOnDisconnect(&m_OnDisconnect);

	sm_pInstance = this;
}

HyGuiComms::~HyGuiComms(void)
{
	delete m_pSocket;
}

void HyGuiComms::Update()
{
	//NL::Socket *pCli = m_pGuiServer->accept();




	//char input[256];
	//input[255] = '\0';
	//cout << "\n--> ";
	//cin.getline(input, 255);
	//if(!strcmp(input, "exit"))
	//	disconnect = true;
	//else
	//	socket.send(input, strlen(input)+1);
	//group.listen(500);






	//if(pCli)
	//{
	//	pCli->blocking(false);
	//	m_pClients.push_back(pCli);
	//}

	//char szBuffer[HYNETWORKING_MAX_PACKET_SIZE];
	//szBuffer[HYNETWORKING_MAX_PACKET_SIZE-1] = '\0';
	//for(uint32 i = 0; i < m_pClients.size(); ++i)
	//{
	//	m_pClients[i]->read(szBuffer, HYNETWORKING_MAX_PACKET_SIZE-1);
	//	
	//}

	// Check for connectivity

	// Send any dirty live params

	// Send all queued up log messages

	// Send diagnostics
}

void HyGuiComms::SendPacket(ePacketType eType, uint32 uiDataSize, const void *pDataToCopy)
{
	HyAssert(uiDataSize > HYNETWORKING_MAX_PACKET_SIZE - 8, "InitPacket received data larger than [HYNETWORKING_MAX_PACKET_SIZE - 8]");

	*reinterpret_cast<uint32 *>(&m_pPacketBuffer[4]) = eType;
	memcpy(&m_pPacketBuffer[8], pDataToCopy, uiDataSize);

	m_pSocket->send(m_pPacketBuffer, uiDataSize+8);
}

void HyGuiComms::OnReadCallbackClass::exec(NL::Socket *pSocket, NL::SocketGroup *pGroup, void *pParam)
{
	int32 iReadSize = pSocket->nextReadSize();
	HyAssert(iReadSize > HYNETWORKING_MAX_PACKET_SIZE, "HyGuiComms received a packet larger than HYNETWORKING_MAX_PACKET_SIZE");

	HyGuiComms *pThis = reinterpret_cast<HyGuiComms *>(pParam);

	pSocket->read(pThis->m_pPacketBuffer, HYNETWORKING_MAX_PACKET_SIZE);
	
	// Ensure packet is intended for us. Look at first 4 bytes for signature "~Hy" <-(3 characters + '\0')
	if(strcmp(pThis->m_pPacketBuffer, "~Hy") != 0)
		return;

	char *pCurReadPos = pThis->m_pPacketBuffer + 4;

	// The next 4 bytes is the packet type
	uint32 uiType = *reinterpret_cast<uint32 *>(&pThis->m_pPacketBuffer[3]);

	switch(uiType)
	{
	case HYPACKET_Int:
		break;

	case HYPACKET_Float:
		break;

	case HYPACKET_LogNormal:
		break;
	}
}

void HyGuiComms::OnDisconnectCallbackClass::exec(NL::Socket *pSocket, NL::SocketGroup *pGroup, void *pParam)
{
	
}

void HyGuiComms::ProcessPacket(char *&pCurReadPos)
{
}

void HyGuiComms::Log(const char *szMessage, uint32 uiLevel)
{
	sm_pInstance->SendPacket(static_cast<ePacketType>(uiLevel), strlen(szMessage), szMessage);
}

#endif
