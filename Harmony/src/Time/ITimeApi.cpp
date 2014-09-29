/**************************************************************************
 *	ITimeApi.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Time/ITimeApi.h"

#include "Input/HyInput.h"

/*static*/ const uint32			ITimeApi::sm_kuiUpdateStep = 10;
/*static*/ const double			ITimeApi::sm_kdUpdateStep = ITimeApi::sm_kuiUpdateStep / 1000.0;

ITimeApi::ITimeApi(HyInput &hyInputRef) :	m_InputRef(hyInputRef),
											m_dTimeManipulation(1.0f),
											m_eJournalState(REPLAY_Off),
											m_dCurDeltaTime(0.0),
											m_dTotalElapsedTime(0.0),
											m_dThrottledTime(0.0),
											m_uiJournalCount(0)
{
}

ITimeApi::~ITimeApi(void)
{
	while(m_TimeInstList.size() != 0)
	{
		HyWatch *pInst = m_TimeInstList[0];
		Remove(pInst);
	}
}

bool ITimeApi::ThrottleTime()
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
			if(m_eJournalState == REPLAY_Saving)
				m_InputRef.SaveInputs(m_uiJournalCount);
			else // REPLAY_Replaying
				m_InputRef.ApplyInputs(m_uiJournalCount);
			
			m_uiJournalCount++;
		}

		m_dThrottledTime -= sm_kdUpdateStep;
		return true;
	}

	return false;
}

HyTimerWatch *ITimeApi::CreateTimer()
{
	HyTimerWatch *pNewTimeInst = new HyTimerWatch();

	m_TimeInstList.push_back(pNewTimeInst);
	return pNewTimeInst;
}

HyStopwatch *ITimeApi::CreateStopwatch()
{
	HyStopwatch *pNewTimeInst = new HyStopwatch();

	m_TimeInstList.push_back(pNewTimeInst);
	return pNewTimeInst;
}

void ITimeApi::Remove(HyWatch *&pTimeInst)
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
