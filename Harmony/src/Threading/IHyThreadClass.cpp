#include "Threading/IHyThreadClass.h"
#include "Diagnostics/Console/HyConsole.h"

IHyThreadClass::IHyThreadClass() : m_eThreadState(HYTHREADSTATE_Inactive)
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

bool IHyThreadClass::ThreadStop()
{
	if(m_eThreadState != HYTHREADSTATE_Run && m_eThreadState != HYTHREADSTATE_ShouldExit)
	{
		HyLogWarning("IHyThreadClass::ThreadStop failed becaused thread state is not running.");
		return false;
	}

	m_eThreadState = HYTHREADSTATE_ShouldExit;
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
		pThis->OnThreadUpdate();
	}

	pThis->OnThreadShutdown();
	pThis->m_eThreadState = HYTHREADSTATE_HasExited;
}
