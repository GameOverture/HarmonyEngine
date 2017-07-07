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

/*static*/ uint32			IHyTime::sm_uiUPDATESTEP_MILLISECONDS = 16;
/*static*/ double			IHyTime::sm_dUPDATESTEP_SECONDS = sm_uiUPDATESTEP_MILLISECONDS / 1000.0;
/*static*/ double			IHyTime::sm_dCurDeltaTime = 0.0;

#define HYTIME_ThresholdWarningsEvery 25.0	// How often to print a warning
#define HYTIME_ThresholdMaxReset 100.0		// Maximum threshold until we hard reset

IHyTime::IHyTime(HyScene &sceneRef, HyDiagnostics &diagRef) :	m_SceneRef(sceneRef),
																m_DiagosticsRef(diagRef),
																m_dTotalElapsedTime(0.0),
																m_dThrottledTime(0.0),
																m_uiCurFpsCount(0),
																m_uiFps_Update(0),
																m_uiFps_Render(0),
																m_dFpsElapsedTime(0.0),
																m_dSpiralOfDeathCounter(HYTIME_ThresholdWarningsEvery)
{
}

IHyTime::~IHyTime(void)
{
	while(m_TimeInstList.size() != 0)
		RemoveTimeInst(m_TimeInstList[0]);
}

///*static*/ uint32 IHyTime::GetUpdateStepMilliseconds()
//{
//	return sm_uiUPDATESTEP_MILLISECONDS;
//}

/*static*/ float IHyTime::GetUpdateStepSeconds()
{
#ifdef HYSETTING_ThrottleUpdate
	return static_cast<float>(sm_dUPDATESTEP_SECONDS);
#else
	return static_cast<float>(sm_dCurDeltaTime);
#endif
}

bool IHyTime::ThrottleTime()
{
	// sm_dCurDeltaTime will be set within SetCurDeltaTime()
	SetCurDeltaTime();
	m_dTotalElapsedTime += sm_dCurDeltaTime;
	m_dThrottledTime += sm_dCurDeltaTime;

	// Update all timers
	if(m_TimeInstList.empty() == false)
	{
		uint32 uiNumTimers = static_cast<uint32>(m_TimeInstList.size());
		for(uint32 i = 0; i < uiNumTimers; i++)
			m_TimeInstList[i]->Update(sm_dCurDeltaTime);
	}

	// FPS diagnostics
	m_dFpsElapsedTime += sm_dCurDeltaTime;
	if(m_dFpsElapsedTime >= 1.0)
	{
		m_uiFps_Update = m_uiCurFpsCount;
		m_uiFps_Render = m_SceneRef.GetAndClearRenderedBufferCount();

		m_dFpsElapsedTime = 0.0;
		m_uiCurFpsCount = 0;

		m_DiagosticsRef.SetCurrentFps(m_uiFps_Update, m_uiFps_Render);
	}

#ifdef HYSETTING_ThrottleUpdate
	if(m_dThrottledTime >= sm_dUPDATESTEP_SECONDS)
	{
		m_dThrottledTime -= sm_dUPDATESTEP_SECONDS;

		if(m_dThrottledTime >= sm_dUPDATESTEP_SECONDS)
		{
			// We're falling behind in updates, keep track to avoid "Spiral of Death"
			if(m_dThrottledTime >= sm_dUPDATESTEP_SECONDS * m_dSpiralOfDeathCounter)
			{
				HyLogWarning("IHyTime::ThrottleTime is behind by '" << static_cast<uint32>(m_dSpiralOfDeathCounter) << "' update thresholds");
				m_dSpiralOfDeathCounter += HYTIME_ThresholdWarningsEvery;

				if(m_dSpiralOfDeathCounter >= HYTIME_ThresholdMaxReset)
				{
					HyLogError("IHyTime::ThrottleTime behind by max '" << static_cast<uint32>(HYTIME_ThresholdMaxReset) << "' - resetting delta (this will corrupt input replays)");
					m_dThrottledTime = 0.0f;
				}
			}
		}
		else // We're on time, reset "SoD" counter
			m_dSpiralOfDeathCounter = HYTIME_ThresholdWarningsEvery;

		m_uiCurFpsCount++;
		return true;
	}
#else
	m_uiCurFpsCount++;
#endif

	return false;
}

void IHyTime::ResetDelta()
{
	SetCurDeltaTime();
	SetCurDeltaTime();
}

uint32 IHyTime::GetFps_Update()
{
	return m_uiFps_Update;
}

uint32 IHyTime::GetFps_Render()
{
	return m_uiFps_Render;
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
