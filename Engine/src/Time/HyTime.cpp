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

HyTime::HyTime(uint32 uiUpdateTickMs) :
	m_dTotalElapsedTime(0.0),
	m_dThrottledTime(0.0),
	m_dSpiralOfDeathCounter(HYTIME_ThresholdWarningsEvery),
	m_dPrevTime(0.0),
	m_dCurDeltaTime(0.0)
{
	IHyTimeInst::sm_pTime = this;
	SetUpdateTickMs(uiUpdateTickMs);
}

HyTime::~HyTime(void)
{
	IHyTimeInst::sm_pTime = nullptr;
}

uint32 HyTime::GetUpdateTickMs()
{
	return m_uiUpdateTickMs;
}

void HyTime::SetUpdateTickMs(uint32 uiUpdateTickMs)
{
	if(uiUpdateTickMs == 0)
	{
		HyLogInfo("HyTime::SetUpdateTickMs was passed '0', using default 16 instead.");
		uiUpdateTickMs = 20;
	}

	m_uiUpdateTickMs = uiUpdateTickMs;
	m_dUpdateTick_Seconds = (1.0 / static_cast<double>(m_uiUpdateTickMs)* 1000.0) / 1000.0;

	m_dThrottledTime = 0.0;
}

float HyTime::GetUpdateStepSeconds() const
{
	return static_cast<float>(m_dCurDeltaTime);//return static_cast<float>(m_dUpdateTick_Seconds);
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
#ifdef HY_USE_GLFW
	double dCurTime = glfwGetTime();

	m_dCurDeltaTime = dCurTime - m_dPrevTime;
	m_dPrevTime = dCurTime;
#else
	m_dCurDeltaTime = 0.0166667;
#endif
}

void HyTime::CalcTimeDelta()
{
	SetCurDeltaTime();	// m_dCurDeltaTime will be set within
	
	m_dTotalElapsedTime += m_dCurDeltaTime;
	m_dThrottledTime += m_dCurDeltaTime;

	// Update all timers
	if(m_TimeInstList.empty() == false)
	{
		uint32 uiNumTimers = static_cast<uint32>(m_TimeInstList.size());
		for(uint32 i = 0; i < uiNumTimers; i++)
		{
			if(m_TimeInstList[i]->IsEnabled())
				m_TimeInstList[i]->Update(m_dCurDeltaTime);
		}
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

/*friend*/ void HyAddTimeInst(HyTime &timeRef, IHyTimeInst *pTimeInst)
{
	if(pTimeInst == nullptr)
		return;

	timeRef.m_TimeInstList.push_back(pTimeInst);
}

/*friend*/ void HyRemoveTimeInst(HyTime &timeRef, IHyTimeInst *pTimeInst)
{
	if(pTimeInst == nullptr)
		return;

	for(std::vector<IHyTimeInst*>::iterator it = timeRef.m_TimeInstList.begin(); it != timeRef.m_TimeInstList.end(); ++it)
	{
		if((*it) == pTimeInst)
		{
			it = timeRef.m_TimeInstList.erase(it);
			break;
		}
	}
}
