/**************************************************************************
 *	HyGlobal.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "HyGuiApp.h"

HyGuiApp::HyGuiApp(ItemProject *pProject)
{
    HarmonyInit initStruct;
    initStruct.szDataDir = pProject->GetAssetsRelPath();
}

HyGuiApp::~HyGuiApp()
{
    
}


