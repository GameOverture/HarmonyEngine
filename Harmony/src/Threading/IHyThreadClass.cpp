#include "Threading/IHyThreadClass.h"

IHyThreadClass::IHyThreadClass() : m_eThreadState(HYTHREADSTATE_Inactive)
{
}

/*virtual*/ IHyThreadClass::~IHyThreadClass()
{
}

bool IHyThreadClass::ThreadStart()
{
	if(m_eThreadState != HYTHREADSTATE_Inactive && m_eThreadState != HYTHREADSTATE_HasExited)
	{
		HyLogWarning("IHyThreadClass::ThreadStart failed becaused thread state is not inactive.");
		return false;
	}

	ThreadJoin();
	m_Thread = std::thread(&IHyThreadClass::ThreadFunc);
	m_eThreadState = HYTHREADSTATE_Run;

	return true;
}

bool IHyThreadClass::ThreadStop()
{
	if(m_eThreadState != HYTHREADSTATE_Run)
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

void IHyThreadClass::ThreadFunc()
{
	OnThreadInit();
	if(m_eThreadState == HYTHREADSTATE_Run)
	{
		OnThreadUpdate();
	}

	OnThreadShutdown();
	m_eThreadState = HYTHREADSTATE_HasExited;
}
