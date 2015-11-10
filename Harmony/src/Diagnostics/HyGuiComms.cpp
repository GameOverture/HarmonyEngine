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

#define HY_SERVER_PORT 1313

#include "Time/IHyTime.h"

HyGuiComms *HyGuiComms::sm_pInstance = NULL;

HyGuiComms::HyGuiComms(void)
{
	HyAssert(sm_pInstance == NULL, "HyGuiComms was instantiated twice");
	sm_pInstance = this;
}

HyGuiComms::~HyGuiComms(void)
{
}

void HyGuiComms::Update()
{
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
