/**************************************************************************
 *	HyLiveVarManager.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Diagnostics/HyLiveVarManager.h"

HyLiveVarManager *HyLiveVarManager::sm_pInstance = NULL;

HyLiveVarManager::HyLiveVarManager(void)
{
}

HyLiveVarManager::~HyLiveVarManager(void)
{
}

HyLiveVarManager *HyLiveVarManager::GetInstance()
{
	if(sm_pInstance == NULL)
		sm_pInstance = new HyLiveVarManager();

	return sm_pInstance;
}
