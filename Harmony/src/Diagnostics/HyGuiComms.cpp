/**************************************************************************
 *	HyGuiComms.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Diagnostics/HyGuiComms.h"

#ifndef HY_PLATFORM_GUI

HyGuiComms *HyGuiComms::sm_pInstance = NULL;

HyGuiComms::HyGuiComms(uint16 uiPort, HyFileIOInterop &fileIORef) : m_Acceptor(m_IOService, tcp::endpoint(tcp::v4(), uiPort)),
																	m_FileIORef(fileIORef),
																	m_Socket(m_IOService)
{
	HyAssert(sm_pInstance == NULL, "HyGuiComms was instantiated twice");
	sm_pInstance = this;

	AcceptGuiConnection();
}

HyGuiComms::~HyGuiComms(void)
{
}

void HyGuiComms::AcceptGuiConnection()
{
	m_Acceptor.async_accept(m_Socket,	[this](std::error_code ec)
										{
											if(!ec)
											{
												Session_Ptr pSession = std::make_shared<HyGuiSession>(*this, std::move(m_Socket));
												pSession->Init();

												participants_.insert(pSession);

												// TODO: Send over all the live variables to the newly connected Gui
												//
												//for(auto msg : recent_msgs_)
												//	participant->deliver(msg);
											}

											AcceptGuiConnection();
										});
}

void HyGuiComms::Update()
{
	// Send any dirty live params

	// Send all queued up log messages

	// Send diagnostics

	m_IOService.poll();
}

void HyGuiComms::DisconnectGui(Session_Ptr participant)
{
	participants_.erase(participant);
}

void HyGuiComms::ProcessMessage(HyGuiMessage &msgRef)
{

}

void HyGuiComms::Broadcast(eHyPacketType eType, uint32 uiDataSize, const void *pDataToCopy)
{
	HyGuiMessage_Ptr pMsg = std::make_shared<HyGuiMessage>(eType, uiDataSize, pDataToCopy);

	for(auto participant : participants_)
		participant->QueueMessage(pMsg);
}

void HyGuiComms::Log(const char *szMessage, uint32 uiLevel)
{
	sm_pInstance->Broadcast(static_cast<eHyPacketType>(uiLevel), static_cast<uint32>(strlen(szMessage)), szMessage);
}

#endif
