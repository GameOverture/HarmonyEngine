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
#include <time.h>

#include "Diagnostics/Console/HyConsole.h"

HyTime_Win::HyTime_Win(HyScene &sceneRef, uint32 uiUpdateTickMs) : IHyTime(sceneRef, uiUpdateTickMs)
{
	int64 i64PerfCnt;
	if (QueryPerformanceFrequency((LARGE_INTEGER *) &i64PerfCnt)) 
	{
		// set timer scaling factor
		m_dTimeFactor = 1.0 / i64PerfCnt;

		// read initial time
		QueryPerformanceCounter((LARGE_INTEGER *) &m_i64LastTime);
	}
	else
		HyError("Windows - QueryPerformanceFrequency failed, Must run Windows XP or later.");
}

HyTime_Win::~HyTime_Win(void)
{
}

// Sets member variable 'm_dCurDeltaTime' to the delta seconds from its previous call (or from its initialization)
// Delta time is in seconds.
/*virtual*/ void HyTime_Win::SetCurDeltaTime()
{
	QueryPerformanceCounter((LARGE_INTEGER *) &m_i64CurTime);

	// scale time value and save
	m_dCurDeltaTime = (m_i64CurTime - m_i64LastTime) * m_dTimeFactor;

	// save new time reading for next pass through the loop
	m_i64LastTime = m_i64CurTime;
}

/*virtual*/ std::string HyTime_Win::GetDateTime() /*override*/
{
	char am_pm[] = "AM";

	__time64_t long_time;
	_time64(&long_time);
	// Convert to local time.
	tm newtime;
	errno_t err = _localtime64_s(&newtime, &long_time);
	if(err) {
		HyLogError("Invalid argument to _localtime64_s.");
	}

	if(newtime.tm_hour > 12)	// Set up extension.
		strcpy_s(am_pm, sizeof(am_pm), "PM");
	if(newtime.tm_hour > 12)	// Convert from 24-hour
		newtime.tm_hour -= 12;	// to 12-hour clock.
	if(newtime.tm_hour == 0)	// Set hour to 12 if midnight.
		newtime.tm_hour = 12;

	// Convert to an ASCII representation.
	char szBuffer[32];
	err = asctime_s(szBuffer, 32, &newtime);
	if(err) {
		HyLogError("Invalid argument to asctime_s.");
	}

	sprintf_s(szBuffer, 32, "%.19s %s", szBuffer, am_pm);
	return std::string(szBuffer);
}

