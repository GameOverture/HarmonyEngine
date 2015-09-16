/**************************************************************************
 *	HyTime.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Time/HyTime.h"

#include "Input/HyInput.h"

/*static*/ const uint32			HyTime::sm_kuiUpdateStep = 10;
/*static*/ const double			HyTime::sm_kdUpdateStep = HyTime::sm_kuiUpdateStep / 1000.0;

HyTime::HyTime() :	m_dTimeManipulation(1.0f),
					m_dCurDeltaTime(0.0),
					m_dTotalElapsedTime(0.0),
					m_dThrottledTime(0.0)
{
}

HyTime::~HyTime(void)
{
	while(m_TimeInstList.size() != 0)
		RemoveTimeInst(m_TimeInstList[0]);
}

bool HyTime::ThrottleTime()
{
	// m_dCurDeltaTime will be set within SetCurDeltaTime()
	SetCurDeltaTime();
	m_dTotalElapsedTime += m_dCurDeltaTime;

	m_dThrottledTime += m_dCurDeltaTime * m_dTimeManipulation;

	// Update all timers
	if(m_TimeInstList.empty() == false)
	{
		size_t iNumTimers = m_TimeInstList.size();
		for(unsigned int i = 0; i < iNumTimers; i++)
			m_TimeInstList[i]->Update(m_dCurDeltaTime);
	}

	if(m_dThrottledTime >= sm_kdUpdateStep)
	{
		m_dThrottledTime -= sm_kdUpdateStep;
		return true;
	}

	return false;
}

void HyTime::AddTimeInst(HyWatch *pTimeInst)
{
	if(pTimeInst == NULL)
		return;

	m_TimeInstList.push_back(pTimeInst);
}

void HyTime::RemoveTimeInst(HyWatch *pTimeInst)
{
	if(pTimeInst == NULL)
		return;

	for(vector<HyWatch*>::iterator it = m_TimeInstList.begin(); it != m_TimeInstList.end(); ++it)
	{
		if((*it) == pTimeInst)
		{
			it = m_TimeInstList.erase(it);
			break;
		}
	}
}
