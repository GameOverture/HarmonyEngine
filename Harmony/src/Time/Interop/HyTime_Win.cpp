/**************************************************************************
 *	HyTimeApi_Win.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Time/Interop/HyTime_Win.h"

//#include <mmsystem.h>

HyTime_Win::HyTime_Win() : IHyTime()
{
	int64 i64PerfCnt;
	if (QueryPerformanceFrequency((LARGE_INTEGER *) &i64PerfCnt)) 
	{
		//m_bUsingPerformanceTimer = true;

		// set timer scaling factor
		m_dTimeFactor = 1.0 / i64PerfCnt;

		// read initial time
		QueryPerformanceCounter((LARGE_INTEGER *) &m_i64LastTime);
	}
	else
	{
		HyError("Windows - QueryPerformanceFrequency failed, Must run Windows XP or later.");
	}
	//else 
	//{
	//	m_bUsingPerformanceTimer = false;

	//	// no performance counter, read in using timeGetTime
	//	m_i64LastTime = timeGetTime();

	//	// set timer scaling factor
	//	m_dTimeFactor = 0.001;
	//}
}

HyTime_Win::~HyTime_Win(void)
{
}

// Sets member variable 'sm_dCurDeltaTime' to the delta seconds from its previous call (or from its initialization)
// Delta time is in seconds.
/*virtual*/ void HyTime_Win::SetCurDeltaTime()
{
	// read appropriate counter
	//if (m_bUsingPerformanceTimer)
		QueryPerformanceCounter((LARGE_INTEGER *) &m_i64CurTime);
	//else
	//	m_i64CurTime = timeGetTime();

	// scale time value and save
	sm_dCurDeltaTime = (m_i64CurTime - m_i64LastTime) * m_dTimeFactor;

	// save new time reading for next pass through the loop
	m_i64LastTime = m_i64CurTime;
}
