/**************************************************************************
*	HyProfiler.cpp
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Diagnostics/Output/HyProfiler.h"

HyProfiler::HyProfiler() :	m_bIsSetup(false),
							m_bOpen(false)
{
}

HyProfiler::~HyProfiler()
{
}

void HyProfiler::ProfileBegin(const char *szName)
{
	//if(m_bOpen == false)
	//{
	//	ProfileState newState;
	//	newState
	//	m_TopLevelProfileStateList.push_back(

	//	m_bOpen = true;
	//}

	//m_ProfileStackList[m_uiCurProfileStateIndex].szName = szName;
	//m_ProfileStackList[m_uiCurProfileStateIndex].time = clock();
}

void HyProfiler::ProfileEnd()
{
	//HyAssert(m_ProfileStackList[m_uiCurProfileStateIndex].szName != nullptr, "HyDiagnostics::ProfileEnd invoked without an initial call from ProfileBegin");
	//m_ProfileStackList[m_uiCurProfileStateIndex].time = clock() - m_ProfileStackList[m_uiCurProfileStateIndex].time;

	//m_TotalClockTicks += m_ProfileStackList[m_uiCurProfileStateIndex].time;
	//m_uiCurProfileStateIndex++;
}
