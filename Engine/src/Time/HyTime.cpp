/**************************************************************************
 *	HyTime.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Time/HyTime.h"
#include "Scene/HyScene.h"
#include "Diagnostics/HyDiagnostics.h"
#include "Diagnostics/Console/HyConsole.h"

#define HYTIME_ThresholdWarningsEvery 25.0	// How often to print a warning
#define HYTIME_ThresholdMaxReset 100.0		// Maximum threshold until we hard reset

std::vector<IHyTimeInst *>	HyTime::sm_TimeInstList;

HyTime::HyTime(uint32 uiUpdateTickMs) :
	m_dTotalElapsedTime(0.0),
	m_dThrottledTime(0.0),
	m_dSpiralOfDeathCounter(HYTIME_ThresholdWarningsEvery),
	m_dCurDeltaTime(0.0)
{
	SetUpdateTickMs(uiUpdateTickMs);
}

HyTime::~HyTime(void)
{
}

void HyTime::SetUpdateTickMs(uint32 uiUpdateTickMs)
{
	if(uiUpdateTickMs == 0)
	{
		HyLogInfo("HyTime::SetUpdateTickMs was passed '0', using non-throttled, variable updating.");
		uiUpdateTickMs = 20;
	}

	m_dUpdateTick_Seconds = (1.0 / static_cast<double>(uiUpdateTickMs) * 1000.0) / 1000.0;
	m_dThrottledTime = 0.0;
}

float HyTime::GetUpdateStepSeconds() const
{
	return static_cast<float>(m_dCurDeltaTime);
}

double HyTime::GetUpdateStepSecondsDbl() const
{
	return m_dCurDeltaTime;
}

double HyTime::GetTotalElapsedTime() const
{
	return m_dTotalElapsedTime;
}

void HyTime::SetCurDeltaTime()
{
#ifdef HY_USE_SDL2
	m_uiPrev = m_uiCur;
	m_uiCur = SDL_GetPerformanceCounter();

	m_dCurDeltaTime = static_cast<double>(m_uiCur - m_uiPrev) / static_cast<double>(SDL_GetPerformanceFrequency());
#else
	m_dCurDeltaTime = 0.0166667;
#endif
}

void HyTime::CalcTimeDelta()
{
	SetCurDeltaTime();	// m_dCurDeltaTime will be set within
	
	m_dTotalElapsedTime += m_dCurDeltaTime;
	m_dThrottledTime += m_dCurDeltaTime;
	 
	// Update all timers/stopwatches
	for(auto timer: sm_TimeInstList)
	{
		if(timer->IsRunning())
			timer->Update(m_dCurDeltaTime);
	}
}

bool HyTime::ThrottleUpdate()
{
	if(m_dThrottledTime >= m_dUpdateTick_Seconds)
	{
		m_dThrottledTime -= m_dUpdateTick_Seconds;

		if(m_dThrottledTime >= m_dUpdateTick_Seconds)
		{
			// We're falling behind in updates, keep track to avoid "Spiral of Death"
			if(m_dThrottledTime >= m_dUpdateTick_Seconds * m_dSpiralOfDeathCounter)
			{
				HyLogWarning("HyTime::ThrottleUpdate is behind by '" << static_cast<uint32>(m_dSpiralOfDeathCounter) << "' update thresholds");
				m_dSpiralOfDeathCounter += HYTIME_ThresholdWarningsEvery;

				if(m_dSpiralOfDeathCounter >= HYTIME_ThresholdMaxReset)
				{
					HyLogError("HyTime::ThrottleUpdate behind by max '" << static_cast<uint32>(HYTIME_ThresholdMaxReset) << "' - resetting delta (this will corrupt input replays)");
					m_dThrottledTime = 0.0f;
				}
			}
		}
		else // We're on time, reset "SoD" counter
			m_dSpiralOfDeathCounter = HYTIME_ThresholdWarningsEvery;

		return true;
	}

	return false;
}

void HyTime::ResetDelta()
{
	SetCurDeltaTime();
	SetCurDeltaTime();
}

std::string HyTime::GetDateTime()
{
#ifdef HY_PLATFORM_WINDOWS
	char am_pm[] = "AM";

	__time64_t long_time;
	_time64(&long_time);
	// Convert to local time.
	tm newtime;
	errno_t err = _localtime64_s(&newtime, &long_time);
	if(err) {
		HyLogError("Invalid argument to _localtime64_s.");
	}

	if(newtime.tm_hour >= 12)	// Set up extension.
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
#else
	return "<GetDateTime not implemented for this platform>";
#endif
}

/*static*/ void HyTime::AddTimeInst(IHyTimeInst *pTimeInst)
{
	if(pTimeInst == nullptr)
		return;

	sm_TimeInstList.push_back(pTimeInst);
}

/*static*/ void HyTime::RemoveTimeInst(IHyTimeInst *pTimeInst)
{
	if(pTimeInst == nullptr)
		return;

	for(std::vector<IHyTimeInst*>::iterator it = sm_TimeInstList.begin(); it != sm_TimeInstList.end(); ++it)
	{
		if((*it) == pTimeInst)
		{
			it = sm_TimeInstList.erase(it);
			break;
		}
	}
}
