/**************************************************************************
 *	HyConsole.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2017 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Diagnostics/HyConsole.h"

HyConsole *HyConsole::sm_pInstance = nullptr;
std::stringstream HyConsole::sm_ss;

HyConsole::HyConsole() 
{
	sm_pInstance = this;
}

HyConsole::~HyConsole()
{
}

/*static*/ HyConsole *HyConsole::Get()
{
	HyAssert(sm_pInstance, "HyConsole::Get() was invoked before engine initialized it");
	return sm_pInstance;
}
