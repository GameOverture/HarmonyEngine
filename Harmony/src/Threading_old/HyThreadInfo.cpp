/**************************************************************************
 *	HyThreadInfo.cpp
 *	
 *	Copyright (c) 2013 Overture Games, Inc.
 *	All Rights Reserved.
 *
 *  Permission to use, copy, modify, and distribute this software
 *  is hereby NOT granted.
 *************************************************************************/
#include "HyThreadInfo.h"

#include "HyThreadManager.h"
#include "Utilities/HyStrManip.h"

ThreadInfo::ThreadInfo() : m_RefCounter(1)
{
	// attach to current thread
#if defined(HY_PLATFORM_WINDOWS)
	m_ThreadId = InteropGetCurrentThreadId();

	// try to open thread handle with different access rights, does not work in some restricted processes in Vista
	m_hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, m_ThreadId);
	if(!m_hThread)
		m_hThread = OpenThread(SYNCHRONIZE | THREAD_TERMINATE, FALSE, m_ThreadId);
	if(!m_hThread)
		m_hThread = OpenThread(SYNCHRONIZE, FALSE, m_ThreadId);

	// create synchronization object if we are able to open a thread handle
	if(m_hThread)
		m_hThreadEvent = new BasicEvent(m_hThread, false);

#elif defined(HY_PLATFORM_UNIX)
	m_ThreadId = InteropGetCurrentThreadId();
	m_hThread = (ThreadHandle)m_ThreadId;
	m_hThreadEvent = new BasicEvent(true);
#endif

#if defined(PLATFORM_NATIVE)
	SetThreadName(_T("Attached Thread"));
#endif

	m_bAttached = true;
	m_pThreadProc = NULL;
	m_pThreadParam = NULL;
	m_TriggerTime = HyGetTickCount();
	m_bIgnoreTrigger = false;
}

ThreadInfo::ThreadInfo(PCTSTR p_ThreadName, PTHREAD_START_PROCEDURE p_pThreadProc, PVOID p_pParam) : m_RefCounter(1)
{
	m_ThreadId = 0;
	m_hThread = NULL;
	m_hThreadEvent = NULL;
	m_bAttached = false;
	m_ThreadName = p_ThreadName;
	m_pThreadProc = p_pThreadProc;
	m_pThreadParam = p_pParam;
	m_TriggerTime = HyGetTickCount();
	m_bIgnoreTrigger = false;
}

ThreadInfo::~ThreadInfo()
{
	if(m_hThreadEvent != NULL)
		delete(m_hThreadEvent);

#if defined(HY_PLATFORM_WINDOWS)
	if(m_hThread)
		CloseHandle(m_hThread);
#endif
}


uint32 ThreadInfo::GetThreadId()
{
	return(m_ThreadId);
}

ThreadHandle ThreadInfo::GetThreadHandle()
{
	HyAssert(m_hThread, "ThreadInfo::GetThreadHandle() thread handle is NULL");
	return(m_hThread);
}

BasicEvent& ThreadInfo::GetThreadEvent()
{
	HyAssert(m_hThreadEvent, "ThreadInfo::GetThreadEvent() thread handle is NULL");
	return(*m_hThreadEvent);
}

void ThreadInfo::SetThreadName(PCTSTR p_Name)
{
	//ASSERT_EXPR(IsCurrentThread());
	if(!IsCurrentThread())
		return;

	// set thread name
	m_ThreadName = p_Name;

	// set debugger thread name
#if defined(_MSC_VER)
	#if defined(UNICODE)
		SetDebugThreadName(GetThreadId(), WStringToString(m_ThreadName).c_str());
	#else
		SetDebugThreadName(GetThreadId(), m_ThreadName.c_str());
	#endif
#endif
}

HyString ThreadInfo::GetThreadName()
{
	return(m_ThreadName);
}


bool ThreadInfo::IsAlive()
{
#if defined(HY_PLATFORM_WINDOWS)
	HyAssert(m_hThread, "ThreadInfo::IsAlive() thread handle is NULL");
	return(WaitForSingleObject(m_hThread, 0) == WAIT_TIMEOUT);
#elif defined(HY_PLATFORM_UNIX)
	return(pthread_kill(m_hThread, 0) == 0);
#endif
}

bool ThreadInfo::IsAttached()
{
	return(m_bAttached);
}

bool ThreadInfo::IsCurrentThread()
{
	return(GetThreadId() == InteropGetCurrentThreadId());
}


void ThreadInfo::Abort()
{
#if defined(HY_PLATFORM_WINDOWS)
	HyAssert(m_hThread, "ThreadInfo::Abort() thread handle is NULL");
	TerminateThread(m_hThread, 0);
#elif defined(HY_PLATFORM_UNIX)
	// INFO: SIGTERM signal will be sent to the thread, the signal handler of
	// this thread will be called and handles the thread termination
	pthread_kill(m_hThread, SIGTERM);
#endif
}

void ThreadInfo::WaitForThreadStop()
{
	HyAssert(!IsCurrentThread(), "ThreadInfo::WaitForThreadStop() was invoked on a thread that isn't the owner");
#if defined(HY_PLATFORM_WINDOWS)
	HyAssert(m_hThread, "ThreadInfo::WaitForThreadStop() thread handle is NULL");
	WaitForSingleObject(m_hThread, INFINITE);
#elif defined(HY_PLATFORM_UNIX)
	GetThreadEvent().Wait();
#endif
}

bool ThreadInfo::WaitForThreadStop(uint32 p_TimeoutMs)
{
	HyAssert(!IsCurrentThread(), "ThreadInfo::WaitForThreadStop() was invoked on a thread that isn't hte owner");
#if defined(HY_PLATFORM_WINDOWS)
	HyAssert(m_hThread, "ThreadInfo::WaitForThreadStop() thread handle is NULL");
	return(WaitForSingleObject(m_hThread, p_TimeoutMs) == WAIT_OBJECT_0);
#elif defined(HY_PLATFORM_UNIX)
	return(GetThreadEvent().Wait(p_TimeoutMs));
#endif
}

void ThreadInfo::DetachThread()
{
	HyAssert(IsCurrentThread(), "ThreadInfo::DetachThread() ");
	HyAssert(!m_bAttached, "ThreadInfo::DetatchThread()");
	m_bAttached = true;
}

void ThreadInfo::StartThread()
{
	HyAssert(!m_bAttached && m_ThreadId == 0 && m_hThread == NULL, "ThreadInfo::StartThread() may be already attached or thread ID or handle may be invalid");

	// set one reference for this thread
	AddRef();

#if defined(HY_PLATFORM_WINDOWS)
	m_hThread = CreateThread(NULL, 0, ThreadProc, this, CREATE_SUSPENDED, (LPDWORD)&m_ThreadId);
	if(!m_hThread)
	{
		// don't forget to release thread reference in case of an error!
		Release();
		//THROW_ERROR_LASTERROR();
	}
	else
	{
		m_hThreadEvent = new BasicEvent(m_hThread, false);
		if(::ResumeThread(m_hThread) == (DWORD)-1)
		{
			// don't forget to release thread reference in case of an error!
			Release();
			//THROW_ERROR_LASTERROR();
		}
	}

#elif defined(HY_PLATFORM_UNIX)

	// create thread event
	m_hThreadEvent = new BasicEvent(true);

	// thread attribute
	pthread_attr_t threadAttr;

	// initialize the thread attribute
	pthread_attr_init(&threadAttr);

	// set thread to detached state. no need for pthread_join, automatic cleanup at thread stop
	pthread_attr_setdetachstate(&threadAttr, PTHREAD_CREATE_DETACHED);

	// create the thread
	m_ThreadId = 0;
	int rc = pthread_create(&m_hThread, &threadAttr, ThreadProc, this);
	if(rc != 0)
	{
		pthread_attr_destroy(&threadAttr);
		// don't forget to release thread reference in case of an error!
		Release();
		THROW_UNIX_ERROR(rc);
	}

	// destroy and cleanup the thread attributes
	pthread_attr_destroy(&threadAttr);

#endif
}

void ThreadInfo::ThreadStartup()
{
	// initialize thread instance
	HyAssert(!m_bAttached, "ThreadInfo::ThreadStartup() thread is already attached");

#if defined(HY_PLATFORM_UNIX)
	// INFO: get thread id from thread instance, starting thread will do the same (race condition)
	m_ThreadId = InteropGetCurrentThreadId();
#endif

	SetThreadName(m_ThreadName.c_str());

	// thread started
	//LOG(GetThreadName(), _T("Run"));

	try
	{
		// start user thread function
		m_pThreadProc(m_pThreadParam);
	}
	catch(...)
	{
		// TODO: Add your error handling code here ...
		//TRACE(_T("User thread '") << GetThreadName() << _T("' terminated abnormal with an unhandled exception.\n"));
	}

	// check for any pending wait object operations
	if(HyThreadManager::Get()->HasThreadWaitObjectInfo() || m_WaitObjectInfo.GetSize() > 0)
	{
		//LOGWARNING(_T("Thread has pending wait object operations!\n") << Global::GetThreadManager()->DumpThreadWaitObjectInfo());
#ifdef HY_DEBUG
		//ASSERT_FAILED(_T("Thread has pending wait object operations!"));
		
		//Utils::BeepErrorWaitObject();
#endif
	}

	// thread stopped
	//LOG(GetThreadName(), _T("Stop"));

	// unregister thread
	HyThreadManager::Get()->RemoveThreadInfo(GetThreadId());

#if defined(HY_PLATFORM_UNIX)
	// trigger stop event
	if(m_hThreadEvent)
		m_hThreadEvent->Set();
#endif

	// release reference for this thread
	Release();
}


void ThreadInfo::TriggerThread()
{
	m_TriggerTime = HyGetTickCount();
}

uint32 ThreadInfo::GetTriggerTime()
{
	return(m_TriggerTime);
}

uint32 ThreadInfo::GetTriggerTimeDiff()
{
	return (HyGetTickCount() - GetTriggerTime());
}

bool ThreadInfo::IsTriggerTimeout(uint32 p_Timeout)
{
	return(!IgnoreTriggerTimeout() && GetTriggerTimeDiff() > p_Timeout);
}

bool ThreadInfo::IgnoreTriggerTimeout()
{
	return(m_bIgnoreTrigger);
}

void ThreadInfo::SetIgnoreTrigger(bool p_bValue)
{
	m_bIgnoreTrigger = p_bValue;
}

#if defined(_MSC_VER)
	void SetDebugThreadName(uint32 p_ThreadId, PCSTR p_ThreadName)
	{
	#if defined(HY_PLATFORM_WINDOWS)
	#ifdef HY_DEBUG
		if(!IsDebuggerPresent())
			return;

		struct THREADNAME_INFO
		{
			DWORD dwType;		// must be 0x1000
			PCSTR szName;		// pointer to name (in user addr space)
			DWORD dwThreadID;	// thread ID (-1=caller thread)
			DWORD dwFlags;		// reserved for future use, must be zero
		};

		THREADNAME_INFO info;
		info.dwType = 0x1000;
		info.szName = p_ThreadName;
		info.dwThreadID = p_ThreadId;
		info.dwFlags = 0;

		__try
		{
			RaiseException(0x406D1388, 0, sizeof(info) / sizeof(DWORD), (ULONG_PTR*)&info);
		}
		__except(EXCEPTION_CONTINUE_EXECUTION)
		{
		}
	#else
		UNREFERENCED_PARAMETER(p_ThreadId);
		UNREFERENCED_PARAMETER(p_ThreadName);
	#endif
	#else
		UNREFERENCED_PARAMETER(p_ThreadId);
		UNREFERENCED_PARAMETER(p_ThreadName);
	#endif
	}
#endif
