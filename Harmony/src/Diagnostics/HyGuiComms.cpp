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

HyGuiComms::HyGuiComms(uint16 uiPort) : m_Acceptor(m_IOService, tcp::endpoint(tcp::v4(), uiPort)),
										m_Socket(m_IOService)
{
	HyAssert(sm_pInstance == NULL, "HyGuiComms was instantiated twice");
	sm_pInstance = this;

	DoAcceptConnection();
}

HyGuiComms::~HyGuiComms(void)
{
}

void HyGuiComms::DoAcceptConnection()
{
	m_Acceptor.async_accept(m_Socket,	[this](std::error_code ec)
										{
											if(!ec)
											{
												std::make_shared<session>(std::move(m_Socket))->start();
											}

											DoAcceptConnection();
										});
}

void HyGuiComms::Update()
{
	m_IOService.poll();

	// Send any dirty live params

	// Send all queued up log messages

	// Send diagnostics
}

void HyGuiComms::SendToGui(ePacketType eType, uint32 uiDataSize, const void *pDataToCopy)
{
}

void HyGuiComms::Log(const char *szMessage, uint32 uiLevel)
{
	sm_pInstance->SendToGui(static_cast<ePacketType>(uiLevel), static_cast<uint32>(strlen(szMessage)), szMessage);
}

#endif
