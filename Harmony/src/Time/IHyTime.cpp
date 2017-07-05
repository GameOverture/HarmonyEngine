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
#include "Diagnostics/Console/HyConsole.h"

/*static*/ uint32			IHyTime::sm_uiUPDATESTEP_MILLISECONDS = 16;
/*static*/ double			IHyTime::sm_dUPDATESTEP_SECONDS = sm_uiUPDATESTEP_MILLISECONDS / 1000.0;
/*static*/ double			IHyTime::sm_dCurDeltaTime = 0.0;

IHyTime::IHyTime() :	m_dTotalElapsedTime(0.0),
						m_dThrottledTime(0.0),
						m_uiFpsFrameCount(0),
						m_dFpsElapsedTime(0.0),
						m_bDumpFPSToConsole(false)
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

	if(m_dThrottledTime >= sm_dUPDATESTEP_SECONDS)
	{
		m_dThrottledTime -= sm_dUPDATESTEP_SECONDS;

		// TODO: Handle this better or input replays will not work
		if(m_dThrottledTime >= sm_dUPDATESTEP_SECONDS * 5.0f)
		{
#ifndef HY_PLATFORM_GUI
			m_dThrottledTime = 0.0f;
#endif
		}

		return true;
	}

	if(m_bDumpFPSToConsole)
	{
		m_dFpsElapsedTime += sm_dCurDeltaTime;
		m_uiFpsFrameCount++;
		if(m_dFpsElapsedTime >= 1.0)
		{
			HyLog("Update: " << m_uiFpsFrameCount << "fps (" << (sm_dCurDeltaTime * 1000) << "ms)");
			HyLog("Render: " << HyScene::GetAndClearRenderedBufferCount() << "fps");

			m_dFpsElapsedTime = 0.0;
			m_uiFpsFrameCount = 0;
		}
	}

	return false;
}

void IHyTime::ResetDelta()
{
	SetCurDeltaTime();
	SetCurDeltaTime();
}

void IHyTime::ShowFps(bool bShow)
{
	m_bDumpFPSToConsole = bShow;
}

bool IHyTime::IsShowFps()
{
	return m_bDumpFPSToConsole;
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
