/**************************************************************************
 *	HyLiveWatchManager.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "GuiTool/HyLiveWatchManager.h"

HyLiveWatchManager *HyLiveWatchManager::sm_pInstance = NULL;

HyLiveWatchManager::HyLiveWatchManager(void)
{
}

HyLiveWatchManager::~HyLiveWatchManager(void)
{
}

HyLiveWatchManager *HyLiveWatchManager::GetInstance()
{
	if(sm_pInstance == NULL)
		sm_pInstance = new HyLiveWatchManager();

	return sm_pInstance;
}
