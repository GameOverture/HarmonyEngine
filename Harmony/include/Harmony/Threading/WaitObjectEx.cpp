
#include "WaitObject.h"


#include "WaitObjectEx.h"


//*****************************************************************************
//** ThreadWaitEvent implementation
//*****************************************************************************

ThreadWaitEvent::ThreadWaitEvent(ThreadInfoPtr p_hThread) : WaitObject(p_hThread->GetThreadName().c_str(), true), m_hThread(p_hThread)
{
}

ThreadWaitEvent::~ThreadWaitEvent()
{
}

bool ThreadWaitEvent::Wait()
{
	SetStateWait(INFINITE);
	bool bRes = m_hThread->GetThreadEvent().Wait();
	RemoveStateWait();
	return(bRes);
}

bool ThreadWaitEvent::Wait(uint32 p_Timeout)
{
	SetStateWait(INFINITE);
	bool bRes = m_hThread->GetThreadEvent().Wait(p_Timeout);
	RemoveStateWait();
	return(bRes);
}

SyncHandle ThreadWaitEvent::GetHandle()
{
	return(m_hThread->GetThreadEvent().GetHandle());
}

bool ThreadWaitEvent::Lock()
{
	return(Wait());
}

bool ThreadWaitEvent::Lock(uint32 p_Timeout)
{
	return(Wait(p_Timeout));
}

void ThreadWaitEvent::Unlock()
{
	THROW_ERROR_NOTSUPPORTED();
}

void ThreadWaitEvent::LockedExtern()
{
}

void ThreadWaitEvent::UnlockedExtern()
{
}

