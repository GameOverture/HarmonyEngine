/**************************************************************************
*	HyProfiler.cpp
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Diagnostics/Output/HyProfiler.h"

// Read Time-Stamp Counter
#ifdef HY_PLATFORM_WINDOWS
#include <intrin.h>
uint64 HyReadTsc() { return __rdtsc(); }
#else
uint64 HyReadTsc()
{
	uint32 lo, hi;
	__asm__ __volatile__("rdtsc" : "=a" (lo), "=d" (hi));
	return ((uint64)hi << 32) | lo;
}
#endif

HyProfiler::HyProfiler()
{
}

HyProfiler::~HyProfiler()
{
}

void HyProfiler::NewFrame()
{
	HyAssert(m_SectionStack.empty(), "HyProfiler::NewFrame was invoked without clearing its m_SectionStack");

	memset(m_uiSectionTicks, 0, sizeof(uint64) * HYNUM_PROFILERSECTION);
	m_eCurrentSection = HYPROFILERSECTION_None;
}

void HyProfiler::BeginSection(HyProfilerSection eSection)
{
	//HyAssert(m_eCurrentSection == HYPROFILERSECTION_None, "HyProfiler::BeginSection was invoked without its corresponding EndSe");

	
	m_SectionStack.push(eSection);
	m_uiSectionTicks[eSection] = HyReadTsc();


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

void HyProfiler::EndSection()
{
	//HyAssert(m_ProfileStackList[m_uiCurProfileStateIndex].szName != nullptr, "HyDiagnostics::ProfileEnd invoked without an initial call from ProfileBegin");
	//m_ProfileStackList[m_uiCurProfileStateIndex].time = clock() - m_ProfileStackList[m_uiCurProfileStateIndex].time;

	//m_TotalClockTicks += m_ProfileStackList[m_uiCurProfileStateIndex].time;
	//m_uiCurProfileStateIndex++;
}
