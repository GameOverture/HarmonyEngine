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

/*static*/ const uint32			IHyTime::sm_uiUPDATESTEP_MILLISECONDS = 16;
/*static*/ const double			IHyTime::sm_dUPDATESTEP_SECONDS = sm_uiUPDATESTEP_MILLISECONDS / 1000.0;

IHyTime::IHyTime() :	m_dCurDeltaTime(0.0),
						m_dTotalElapsedTime(0.0),
						m_dThrottledTime(0.0),
						m_iThrottleSafetyCounter(0)
{
}

IHyTime::~IHyTime(void)
{
	while(m_TimeInstList.size() != 0)
		RemoveTimeInst(m_TimeInstList[0]);
}

bool IHyTime::ThrottleTime()
{
	// m_dCurDeltaTime will be set within SetCurDeltaTime()
	SetCurDeltaTime();
	m_dTotalElapsedTime += m_dCurDeltaTime;
	m_dThrottledTime += m_dCurDeltaTime;

	// Update all timers
	if(m_TimeInstList.empty() == false)
	{
		uint32 uiNumTimers = static_cast<uint32>(m_TimeInstList.size());
		for(uint32 i = 0; i < uiNumTimers; i++)
			m_TimeInstList[i]->Update(m_dCurDeltaTime);
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
			m_iThrottleSafetyCounter = 0;
		}

		return true;
	}

	m_iThrottleSafetyCounter = 0;

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
