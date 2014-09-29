//*****************************************************************************
// Description:  Platform independent threading class
//*****************************************************************************

#ifndef TREADINGFX_THREADING_H
#define TREADINGFX_THREADING_H

#include "TLSEntry.h"
#include "Threading.h"
#include "Utils.h"
#include "BasicSync.h"
#include "WaitObject.h"

// thread info forward definition
class ThreadInfo;

// shared pointer to thread info
DEFINE_SHARED_PTR(ThreadInfo, ThreadInfoPtr)

//*****************************************************************************
// Description:
//  Thread function type used to run a thread
// Parameters:
//  - p_pParam     Custom user parameter
//
// void ThreadProc(PVOID p_pParam);

// typedef for native thread function pointer
typedef void (*PTHREAD_START_PROCEDURE)(void*);

// macro to create a thread start procedure pointer
#define THREAD_START_PROCEDURE(_THREAD_START_PROCEDURE) _THREAD_START_PROCEDURE


enum ThreadWaitState
{
	// thread wait object states
	STATE_NOTHING = 0,			// no state, to be ignored
	STATE_WAIT = 1,				// a synchronization object is waiting to be signaled
	STATE_LOCK = 2,				// a synchronization object is locked

	// offset to access the state array
	OFFSET_STATE = 0,			// Offset to the wait object thread state value
	OFFSET_TIMESTAMP = 1,		// Offset to the timestamp (tick count)
	OFFSET_TIMEOUT = 2,			// Offset to the wait timeout or undefined on lock (previous wait value)
	OFFSET_OBJECT_COUNT = 3,	// Offset to the following wait object id count
	OFFSET_OBJECTID_0 = 4,		// Offset to the first wait object identifier
	OFFSET_STATE_SIZE = 5,		// Offset to the state size in elements, the number of values stored in a single state (normally 6)

	// number of static attributes
	STATIC_ELEMENT_COUNT = 5	// Number of static elements used for one entry (without object ids)
};


//*****************************************************************************
// Description:
//  thread description
// Remarks:
//  Describes a thread. Should be created with the thread by ThreadManager,
//  but could also be attached to a running thread.
//  There could be multiple instances per thread, if multiple process core
//  objects exist.
//  The class is non virtual, so it could be created in any module without
//  any module dependency problems of virtual classes (crash when
//  module unloads).
//  This class could hold an instance to a IServiceThread instance, which
//  could be overloaded and customized.
//  This instance could be NULL, but also set to any instance by the user.
//  Be careful with module problems when using the service thread interface!
//-----------------------------------------------------------------------------
class ThreadInfo
{
	// reference counting implementation
	REFERENCE_COUNT_IMPL_MT()

	// public types
public:

	// Wait info array type, used to store ThreadWaitState.
	// The array has a static size and can be accessed without any synchronization.
	// Only the owning thread has write access, other threads can read.
	// Zero values in the array mean that its not initialized. Minor access errors may occur
	// during multi threading access, but only on the last position. This can be ignored.
	// We cannot use std::tr1::array for our WaitInfoArray because it cannot keep track of
	// how many entries are in the array (size() always returns the maximum buffer size).
	typedef StaticUIntArray<256> WaitInfoArray;


	// members
private:
	uint32 m_ThreadId;						// id of this thread
	ThreadHandle m_hThread;					// handle of this thread
	BasicEvent* m_hThreadEvent;			// event representing thread state, signaled when stopped

	bool m_bAttached;						// true if attached to running thread, false if created by the thread manager
	tstring m_ThreadName;					// name of this thread

	// user thread procedure and data
	PTHREAD_START_PROCEDURE m_pThreadProc;	// thread start procedure
	PVOID m_pThreadParam;					// thread start parameter

	// synchronization info in the following form
	WaitInfoArray m_WaitObjectInfo;			// info about wait objects in this thread, static size array

	// thread trigger time
	uint32 m_TriggerTime;						// last thread trigger time
	bool m_bIgnoreTrigger;					// true if trigger should be ignored

	// ctor
public:

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
	tstring GetThreadName();


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
		//ThreadInfo *pThreadInfo = (ThreadInfo*)p_pParam;
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




//*****************************************************************************
// Description:
//  Manages threads and thread info instances
// Remarks:
//  Stores thread info objects identified by thread id.
//  Instance is accessible via process core.
//  CAUTION:
//   Information about threads not opened with ThreadManager will be stored
//   until the program exits!
//   Open threads always with BeginThread !!
//-----------------------------------------------------------------------------
class ThreadManager
{
	// protected data members
protected:

	// TLS entry
	CTLSEntry m_TLSEntry;

	// thread manager sync object
	BasicSection m_SyncObj;

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
	~ThreadManager();

	// returns the one and only thread manager instance
	static ThreadManager* Get() { static ThreadManager manager; return &manager; }

private:
	// hide ctor
	ThreadManager();

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
	// only for temporary usage, safe in ThreadInfoPtr to store reference!
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
	tstring DumpThreadWaitObjectInfo();

	// dump wait object info per thread (for all threads)
	tstring DumpProcessWaitObjectInfo(bool p_bSkipEmptyThreads);


protected:
	// get thread info, will automatically attach to a unknown thread
	ThreadInfoPtr GetThreadInfo(uint32 p_ThreadId);

	// get wait object
	WaitObject* GetWaitObject(uint32 p_WaitObjectId);

	// dump thread wait object information, this function must be called in locked thread manager state!
	tstring DumpThreadWaitObjectInfo(ThreadInfo* p_pThreadInfo, PCTSTR p_LinePrefix, uint32 p_DumpTimestamp);
};

#endif // #ifndef TREADINGFX_THREADING_H
