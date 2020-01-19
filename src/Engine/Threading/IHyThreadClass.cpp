/**************************************************************************
*	IHyThreadClass.cpp
*
*	Harmony Engine
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Threading/IHyThreadClass.h"
#include "Diagnostics/Console/HyConsole.h"

#include <chrono>

IHyThreadClass::IHyThreadClass(HyThreadPriority ePriority /*= HYTHREAD_Normal*/, uint32 uiUpdateThrottleMs /*= 0*/) :
	m_ePriority(ePriority),
	m_eThreadState(THREADSTATE_Inactive),
	m_uiTHROTTLE_MS(uiUpdateThrottleMs),
	m_bWaitEnabled(false),
	m_bWaitComplete(false),
	m_bAutoResetWaiting(false)
{
}

/*virtual*/ IHyThreadClass::~IHyThreadClass()
{
	ThreadStop();
	ThreadJoin();
}

bool IHyThreadClass::ThreadStart()
{
	if(m_eThreadState != THREADSTATE_Inactive && m_eThreadState != THREADSTATE_HasExited)
	{
		HyLogWarning("IHyThreadClass::ThreadStart failed becaused thread state is not inactive.");
		return false;
	}

	ThreadJoin();
	m_Thread = std::thread(&IHyThreadClass::ThreadFunc, this);

#ifndef HY_PLATFORM_GUI
	if(m_ePriority != HYTHREAD_Normal)
	{
#ifdef HY_PLATFORM_WINDOWS
		int iPriority = THREAD_PRIORITY_NORMAL;
		switch(m_ePriority)
		{
		case HYTHREAD_Lowest:		iPriority = THREAD_PRIORITY_LOWEST;			break;
		case HYTHREAD_BelowNormal:	iPriority = THREAD_PRIORITY_BELOW_NORMAL;	break;
		case HYTHREAD_AboveNormal:	iPriority = THREAD_PRIORITY_ABOVE_NORMAL;	break;
		case HYTHREAD_Highest:		iPriority = THREAD_PRIORITY_HIGHEST;		break;
		}
		if(false == SetThreadPriority(m_Thread.native_handle(), iPriority))
			HyLogWarning("Failed to set Thread scheduling : " << GetLastError());
#else
		sch_params.sched_priority = priority;
		if(pthread_setschedparam(th.native_handle(), policy, &sch_params))
		{
			HyLogWarning("Failed to set Thread scheduling : " << std::strerror(errno));
			return false;
		}
#endif
	}
#endif

	return true;
}

void IHyThreadClass::ThreadWait()
{
	std::lock_guard<std::mutex> lock(stateMutex);
	m_bWaitEnabled = true;
	m_bWaitComplete = false;
	stateEvent.notify_one();
}

void IHyThreadClass::ThreadContinue(bool bOnlyOneUpdate)
{
	{
		std::lock_guard<std::mutex> lock(stateMutex);
		m_bWaitComplete = true;
		m_bAutoResetWaiting = bOnlyOneUpdate;
	}
	stateEvent.notify_one();
}

void IHyThreadClass::ThreadStop()
{
	if(m_eThreadState != THREADSTATE_ShouldExit && m_eThreadState != THREADSTATE_HasExited)
		m_eThreadState = THREADSTATE_ShouldExit;

	ThreadContinue(false);
}

bool IHyThreadClass::IsThreadFinished()
{
	if(m_eThreadState == THREADSTATE_HasExited)
	{
		ThreadJoin();
		return true;
	}
	
	return false;
}

void IHyThreadClass::ThreadJoin()
{
	if(m_Thread.joinable())
		m_Thread.join();
}

/*static*/ void IHyThreadClass::ThreadFunc(IHyThreadClass *pThis)
{
	pThis->OnThreadInit();
	pThis->m_eThreadState = THREADSTATE_Running;

	while(pThis->m_eThreadState == THREADSTATE_Running)
	{
		if(pThis->m_bWaitEnabled)
		{
			std::unique_lock<std::mutex> ul(pThis->stateMutex);
			pThis->stateEvent.wait(ul, [&] { return pThis->m_bWaitComplete; });

			if(pThis->m_bAutoResetWaiting)
			{
				pThis->m_bWaitEnabled = true;
				pThis->m_bWaitComplete = false;
				pThis->m_bAutoResetWaiting = false;
			}
			else
				pThis->m_bWaitEnabled = false;
		}
		else if(pThis->m_uiTHROTTLE_MS != 0)
		{
			std::unique_lock<std::mutex> ul(pThis->stateMutex);
			pThis->stateEvent.wait_for(ul, std::chrono::milliseconds(pThis->m_uiTHROTTLE_MS));
		}

		pThis->OnThreadUpdate();
	}

	pThis->OnThreadShutdown();
	pThis->m_eThreadState = THREADSTATE_HasExited;
}
