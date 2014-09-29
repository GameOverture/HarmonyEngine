/**************************************************************************
 *	HyThreadManager.h
 *	
 *	Copyright (c) 2012 Overture Games, Inc.
 *	All Rights Reserved.
 *
 *  Permission to use, copy, modify, and distribute this software
 *  is hereby NOT granted.
 *************************************************************************/
#ifndef __HyThreadManager_h__
#define __HyThreadManager_h__

#include "Afx/HyThreadAfx.h"

#include "HyThreadInfo.h"
#include "Misc/HyTLSEntry.h"

#include "Sync/HyCriticalSection.h"
#include "Sync/HyEvent.h"
#include "Sync/Wait/HyWaitBase.h"

#include <vector>

// shared pointer to thread info
DEFINE_SHARED_PTR(ThreadInfo, ThreadInfoPtr)

class HyThreadManager
{
	// protected data members
protected:

	// TLS entry
	CTLSEntry m_TLSEntry;

	// thread manager sync object
	HyCriticalSection m_SyncObj;

	// mapping from thread id to thread info structure
	typedef std::vector<ThreadInfo*> ThreadInfoVector;
	ThreadInfoVector m_ThreadInfo;

	// unique wait object id counter
	intx m_UniqueWaitObjectId;

	// array of all registered wait objects
	// only the current thread is allowed to write to this array, all other threads
	// are only allowed to read!
	typedef std::vector<WaitObject*> WaitObjectBaseArray;
	WaitObjectBaseArray m_WaitObjects;

#if defined(HY_PLATFORM_UNIX)
public:
	// unix specific global synchronization objects used to support timeout with synchronization objects
	pthread_mutex_t m_hTimeoutMutex;
	pthread_cond_t m_hTimeoutCond;
	pthread_mutex_t m_hInterlockedMutex;
#endif

public:
	~HyThreadManager();

	// returns the one and only thread manager instance
	static HyThreadManager* Get() { static HyThreadManager manager; return &manager; }

private:
	// hide ctor
	HyThreadManager();

	// thread interface
public:

	// start a new thread, use the macro THREAD_START_PROCEDURE to use the thread function as parameter
	// platform independent usage:
	//  static void MyThreadFunc(PVOID p_pParam);
	//  BASE_NSP::Global::GetThreadManager()->BeginThread(_T("My Name"), THREAD_START_PROCEDURE(MyThreadFunc), NULL);
	ThreadInfoPtr BeginThread(LPCTSTR p_ThreadName, PTHREAD_START_PROCEDURE p_pThreadProc, PVOID p_pParam);

	// remove thread info
	void RemoveThreadInfo(uint32 p_ThreadId);

	// remove stopped threads
	void CleanupThreadInfo();

	// get current thread info, will automatically attach to a unknown thread
	// only for temporary usage, safe in ThreadInfo * to store reference!
	ThreadInfo* GetCurrentThreadInfo();

	// takes a snapshot of all thread infos
	void GetAllThreadInfos(std::vector<ThreadInfoPtr>& p_ThreadInfos);


	// wait object interface
public:

	// create unique id for wait objects
	uint32 CreateUniqueWaitObjectId();

	// register wait object
	void AddWaitObject(WaitObject* p_pWaitObject);

	// unregister wait object
	void RemoveWaitObject(WaitObject* p_pWaitObject);


	// true if thread has any wait object information
	bool HasThreadWaitObjectInfo();

	// dump wait object info of this thread
	HyString DumpThreadWaitObjectInfo();

	// dump wait object info per thread (for all threads)
	HyString DumpProcessWaitObjectInfo(bool p_bSkipEmptyThreads);


protected:
	// get thread info, will automatically attach to a unknown thread
	ThreadInfoPtr GetThreadInfo(uint32 p_ThreadId);

	// get wait object
	WaitObject* GetWaitObject(uint32 p_WaitObjectId);

	// dump thread wait object information, this function must be called in locked thread manager state!
	HyString DumpThreadWaitObjectInfo(ThreadInfo* p_pThreadInfo, PCTSTR p_LinePrefix, uint32 p_DumpTimestamp);
};

#endif /* __HyThreadManager_h__ */
