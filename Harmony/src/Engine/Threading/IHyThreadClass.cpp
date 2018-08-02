#include "Threading/IHyThreadClass.h"
#include "Diagnostics/Console/HyConsole.h"

#include <chrono>

IHyThreadClass::IHyThreadClass(uint32 uiUpdateThrottleMs /*= 0*/) :	m_eThreadState(HYTHREADSTATE_Inactive),
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
	if(m_eThreadState != HYTHREADSTATE_Inactive && m_eThreadState != HYTHREADSTATE_HasExited)
	{
		HyLogWarning("IHyThreadClass::ThreadStart failed becaused thread state is not inactive.");
		return false;
	}

	ThreadJoin();
	m_eThreadState = HYTHREADSTATE_Run;
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
	std::lock_guard<std::mutex> lock(stateMutex);
	m_bWaitComplete = true;
	stateEvent.notify_one();

	m_bAutoResetWaiting = bOnlyOneUpdate;
}

bool IHyThreadClass::ThreadStop()
{
	if(m_eThreadState != HYTHREADSTATE_Run && m_eThreadState != HYTHREADSTATE_ShouldExit)
	{
		HyLogWarning("IHyThreadClass::ThreadStop failed becaused thread state is not running.");
		return false;
	}

	m_eThreadState = HYTHREADSTATE_ShouldExit;
	ThreadContinue(false);
	return true;
}

bool IHyThreadClass::IsThreadFinished()
{
	if(m_eThreadState == HYTHREADSTATE_HasExited)
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

	while(pThis->m_eThreadState == HYTHREADSTATE_Run)
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
	pThis->m_eThreadState = HYTHREADSTATE_HasExited;
}
