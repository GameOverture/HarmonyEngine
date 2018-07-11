/**************************************************************************
 *	HyConsole.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2017 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Diagnostics/Console/HyConsole.h"

HyConsole *HyConsole::sm_pInstance = nullptr;
std::stringstream HyConsole::sm_ss;
std::mutex HyConsole::sm_csLog;

HyConsole::HyConsole() 
{
	HyAssert(sm_pInstance == nullptr, "An instance of HyConsole already exists");
	sm_pInstance = this;
}

HyConsole::~HyConsole()
{
	sm_pInstance = nullptr;
}

/*static*/ HyConsole *HyConsole::Get()
{
	HyAssert(sm_pInstance, "HyConsole::Get() was invoked before IHyApplication was initialized");
	return sm_pInstance;
}
