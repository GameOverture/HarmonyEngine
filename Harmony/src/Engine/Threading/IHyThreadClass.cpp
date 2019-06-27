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

IHyThreadClass::IHyThreadClass(uint32 uiUpdateThrottleMs /*= 0*/) :
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
