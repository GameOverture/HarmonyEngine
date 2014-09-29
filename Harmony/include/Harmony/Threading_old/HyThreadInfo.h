/**************************************************************************
 *	HyThreadInfo.h
 *	
 *	Copyright (c) 2013 Overture Games, Inc.
 *	All Rights Reserved.
 *
 *  Permission to use, copy, modify, and distribute this software
 *  is hereby NOT granted.
 *************************************************************************/
#ifndef __HyThreadInfo_h__
#define __HyThreadInfo_h__

#include "Afx/HyThreadAfx.h"

#include "Utilities/HyRefCounters.h"
#include "Utilities/Containers/HyStaticArray.h"
#include "Sync/HyEvent.h"

// set the thread name for visual studio debugger to see the name in the debugging environment
static void SetDebugThreadName(uint32 p_ThreadId, PCSTR p_ThreadName);

class ThreadInfo
{
	// reference counting implementation
	HyRefCountMT m_RefCounter;

		// public types
public:

	// Wait info array type, used to store ThreadWaitState.
	// The array has a static size and can be accessed without any synchronization.
	// Only the owning thread has write access, other threads can read.
	// Zero values in the array mean that its not initialized. Minor access errors may occur
	// during multi threading access, but only on the last position. This can be ignored.
	// We cannot use std::tr1::array for our WaitInfoArray because it cannot keep track of
	// how many entries are in the array (size() always returns the maximum buffer size).
	typedef HyStaticArray<256> WaitInfoArray;


	// members
private:
	uint32 m_ThreadId;						// id of this thread
	ThreadHandle m_hThread;					// handle of this thread
	BasicEvent* m_hThreadEvent;			// event representing thread state, signaled when stopped

	bool m_bAttached;						// true if attached to running thread, false if created by the thread manager
	HyString m_ThreadName;					// name of this thread

	// user thread procedure and data
	PTHREAD_START_PROCEDURE m_pThreadProc;	// thread start procedure
	PVOID m_pThreadParam;					// thread start parameter

	// synchronization info in the following form
	WaitInfoArray m_WaitObjectInfo;			// info about wait objects in this thread, static size array

	// thread trigger time
	uint32 m_TriggerTime;						// last thread trigger time
	bool m_bIgnoreTrigger;					// true if trigger should be ignored

public:

	inline intx AddRef()			{ return m_RefCounter.AddRef(); }
	inline intx Release()			{ intx count = m_RefCounter.Release(); if(!count) delete(this); return(count); }
	inline intx RefCount() const	{ return m_RefCounter.RefCount(); }

	// default constructor attaches to current thread
	ThreadInfo();

	// set thread start procedure and data, thread procedure will collect infos from new thread
	ThreadInfo(PCTSTR p_ThreadName, PTHREAD_START_PROCEDURE p_pThreadProc, PVOID p_pParam);

	// cleanup thread data
	~ThreadInfo();


	// interface
public:

	// get unique id of thread
	uint32 GetThreadId();

	// get low level thread handle
	ThreadHandle GetThreadHandle();

	// get thread event reference, event is signaled when thread is stopped
	BasicEvent& GetThreadEvent();

	// set name of thread (only possible on current thread, must be synchronized by user)
	// should be avoided and used with care (async GetThreadName could crash!)
	void SetThreadName(PCTSTR p_Name);

	// get name of thread
	HyString GetThreadName();


	// true if thread is still running
	bool IsAlive();

	// true if thread instance is attached or created via library threading interface
	bool IsAttached();

	// true if thread instance represents current thread
	bool IsCurrentThread();


	// thread management interface
public:

	// kill thread, should be avoided!
	void Abort();

	// wait for thread to stop
	void WaitForThreadStop();

	// wait for thread to stop with timeout
	bool WaitForThreadStop(uint32 p_TimeoutMs);

	// Detach a thread from thread management. A thread which is started through CoreFx is not anymore managed by it.
	// Should only be used for threads which live until process exit and are used for OS critical tasks.
	// Only allowed to call in this threads context.
	void DetachThread();


	// internal methods for thread startup
public:

	// start thread instance, only allowed to call by ThreadManager
	void StartThread();

protected:
	// thread startup function, initializes thread instance and calls the overloaded Run method
	void ThreadStartup();


public:
#if defined(HY_PLATFORM_WINDOWS)
	// static windows platform thread procedure
	static DWORD WINAPI ThreadProc(PVOID p_pParam)
	{
		ThreadInfo *pThreadInfo = (ThreadInfo*)p_pParam;
		((ThreadInfo*)p_pParam)->ThreadStartup();
		return 0;
	}
#elif defined(HY_PLATFORM_UNIX)
	// static Unix platform thread procedure
	static void* ThreadProc(PVOID p_pParam)
	{
		ThreadInfo *This = (ThreadInfo*)p_pParam;
		This->ThreadStartup();
		return 0;
	}
#endif


	// wait object information
public:

	// get static array with wait object information about this thread
	WaitInfoArray& GetWaitObjectInfo()
	{
		return m_WaitObjectInfo;
	}

	// thread trigger
public:

	// trigger thread, should be continously called by each thread
	void TriggerThread();

	// get last trigger time
	uint32 GetTriggerTime();

	// get trigger time diff since last trigger
	uint32 GetTriggerTimeDiff();

	// returns true if trigger timeout occured, timeout in milliseconds
	bool IsTriggerTimeout(uint32 p_Timeout);

	// returns true if thread want to ignore the trigger
	bool IgnoreTriggerTimeout();

	// set ignore trigger state for thread
	void SetIgnoreTrigger(bool p_bValue);
};

#endif /* __HyThreadInfo_h__ */
