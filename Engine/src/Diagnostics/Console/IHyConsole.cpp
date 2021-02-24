/**************************************************************************
 *	IHyConsole.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2017 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Diagnostics/Console/IHyConsole.h"

IHyConsole *IHyConsole::sm_pInstance = nullptr;

IHyConsole::IHyConsole() 
{
	HyAssert(sm_pInstance == nullptr, "An instance of IHyConsole already exists");
	sm_pInstance = this;
}

IHyConsole::~IHyConsole()
{
	sm_pInstance = nullptr;
}

/*static*/ IHyConsole *IHyConsole::Get()
{
	HyAssert(sm_pInstance, "HyConsole::Get() was invoked before IHyApplication was initialized");
	return sm_pInstance;
}

void IHyConsole::Log(std::ostream &os, const char *szMsg, LogType eType)
{
	m_csLog.lock();
	OnLog(os, szMsg, eType);
	m_csLog.unlock();
}
