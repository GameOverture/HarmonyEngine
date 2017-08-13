/**************************************************************************
 *	HyTime.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Time/IHyTime.h"
#include "Scene/HyScene.h"
#include "Diagnostics/HyDiagnostics.h"
#include "Diagnostics/Console/HyConsole.h"

#define HYTIME_ThresholdWarningsEvery 25.0	// How often to print a warning
#define HYTIME_ThresholdMaxReset 100.0		// Maximum threshold until we hard reset

IHyTime::IHyTime(HyScene &sceneRef, uint32 uiUpdateTickMs) :	m_SceneRef(sceneRef),
																m_dTotalElapsedTime(0.0),
																m_dThrottledTime(0.0),
																m_dSpiralOfDeathCounter(HYTIME_ThresholdWarningsEvery),
																m_dCurDeltaTime(0.0)
{
	SetUpdateTickMs(uiUpdateTickMs);
}

IHyTime::~IHyTime(void)
{
	while(m_TimeInstList.size() != 0)
		RemoveTimeInst(m_TimeInstList[0]);
}

uint32 IHyTime::GetUpdateTickMs()
{
	return m_uiUpdateTickMs;
}

void IHyTime::SetUpdateTickMs(uint32 uiUpdateTickMs)
{
	if(uiUpdateTickMs == 0)
	{
		HyLogInfo("IHyTime::SetUpdateTickMs was passed '0', using default 16 instead.");
		uiUpdateTickMs = 20;
	}

	m_uiUpdateTickMs = uiUpdateTickMs;
	m_dUpdateTick_Seconds = (1.0 / static_cast<double>(m_uiUpdateTickMs)* 1000.0) / 1000.0;

	m_dThrottledTime = 0.0;
}

float IHyTime::GetUpdateStepSeconds()
{
	return static_cast<float>(m_dCurDeltaTime);//return static_cast<float>(m_dUpdateTick_Seconds);
}

float IHyTime::GetFrameDelta()
{
	return static_cast<float>(m_dCurDeltaTime);
}

void IHyTime::CalcTimeDelta()
{
	SetCurDeltaTime();	// m_dCurDeltaTime will be set within
	
	m_dTotalElapsedTime += m_dCurDeltaTime;
	m_dThrottledTime += m_dCurDeltaTime;
}

bool IHyTime::ThrottleUpdate()
{
	// Update all timers
	if(m_TimeInstList.empty() == false)
	{
		uint32 uiNumTimers = static_cast<uint32>(m_TimeInstList.size());
		for(uint32 i = 0; i < uiNumTimers; i++)
			m_TimeInstList[i]->Update(m_dCurDeltaTime);
	}

	if(m_dThrottledTime >= m_dUpdateTick_Seconds)
	{
		m_dThrottledTime -= m_dUpdateTick_Seconds;

		if(m_dThrottledTime >= m_dUpdateTick_Seconds)
		{
			// We're falling behind in updates, keep track to avoid "Spiral of Death"
			if(m_dThrottledTime >= m_dUpdateTick_Seconds * m_dSpiralOfDeathCounter)
			{
				HyLogWarning("IHyTime::ThrottleUpdate is behind by '" << static_cast<uint32>(m_dSpiralOfDeathCounter) << "' update thresholds");
				m_dSpiralOfDeathCounter += HYTIME_ThresholdWarningsEvery;

				if(m_dSpiralOfDeathCounter >= HYTIME_ThresholdMaxReset)
				{
					HyLogError("IHyTime::ThrottleUpdate behind by max '" << static_cast<uint32>(HYTIME_ThresholdMaxReset) << "' - resetting delta (this will corrupt input replays)");
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

void IHyTime::ResetDelta()
{
	SetCurDeltaTime();
	SetCurDeltaTime();
}

void IHyTime::AddTimeInst(IHyTimeInst *pTimeInst)
{
	if(pTimeInst == NULL)
		return;

	m_TimeInstList.push_back(pTimeInst);
}

void IHyTime::RemoveTimeInst(IHyTimeInst *pTimeInst)
{
	if(pTimeInst == NULL)
		return;

	for(std::vector<IHyTimeInst*>::iterator it = m_TimeInstList.begin(); it != m_TimeInstList.end(); ++it)
	{
		if((*it) == pTimeInst)
		{
			it = m_TimeInstList.erase(it);
			break;
		}
	}
}
