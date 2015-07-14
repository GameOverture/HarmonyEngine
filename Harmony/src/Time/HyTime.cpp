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

HyTime::HyTime(HyInput &hyInputRef) :	m_InputRef(hyInputRef),
										m_dTimeManipulation(1.0f),
										m_eJournalState(REPLAY_Off),
										m_dCurDeltaTime(0.0),
										m_dTotalElapsedTime(0.0),
										m_dThrottledTime(0.0),
										m_uiJournalCount(0)
{
}

HyTime::~HyTime(void)
{
	while(m_TimeInstList.size() != 0)
	{
		HyWatch *pInst = m_TimeInstList[0];
		Remove(pInst);
	}
}

bool HyTime::ThrottleTime()
{
	// m_dCurDeltaTime will be set within SetCurDeltaTime()
	SetCurDeltaTime();// glfwGetTime();
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
		m_InputRef.ProcessInputs();

		if(m_eJournalState != REPLAY_Off)
		{
			// TODO: Need to implement gainput's input record/playback
			//
			//if(m_eJournalState == REPLAY_Saving)
			//	m_InputRef.SaveInputs(m_uiJournalCount);
			//else // REPLAY_Replaying
			//	m_InputRef.ApplyInputs(m_uiJournalCount);
			
			m_uiJournalCount++;
		}

		m_dThrottledTime -= sm_kdUpdateStep;
		return true;
	}

	return false;
}

HyTimerWatch *HyTime::CreateTimer()
{
	HyTimerWatch *pNewTimeInst = new HyTimerWatch();

	m_TimeInstList.push_back(pNewTimeInst);
	return pNewTimeInst;
}

HyStopwatch *HyTime::CreateStopwatch()
{
	HyStopwatch *pNewTimeInst = new HyStopwatch();

	m_TimeInstList.push_back(pNewTimeInst);
	return pNewTimeInst;
}

void HyTime::Remove(HyWatch *&pTimeInst)
{
	if(pTimeInst == NULL)
		return;

	for(vector<HyWatch*>::iterator it = m_TimeInstList.begin(); it != m_TimeInstList.end(); ++it)
	{
		if((*it) == pTimeInst)
		{
			it = m_TimeInstList.erase(it);
			delete pTimeInst;
			pTimeInst = NULL;
			break;
		}
	}
}
