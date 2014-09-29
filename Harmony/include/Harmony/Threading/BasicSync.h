//*****************************************************************************
// Description:  basic library synchronization object, thin system api layer
//*****************************************************************************

#ifndef THREADINGFX_BASICSYNC_H
#define THREADINGFX_BASICSYNC_H

#include "PlatformInterop.h"

// ignore abandoned bit, ~STATUS_ABANDONED_WAIT_0 = ((DWORD)0xFFFFFF7F)
#define WAIT_ABANDONED_IGNORE			(~(STATUS_ABANDONED_WAIT_0))
#define IGNORE_ABANDONED(_Result) (_Result & WAIT_ABANDONED_IGNORE)


//*****************************************************************************
//** Unix synchronization concept:
//** Unix does not support synchronization timeouts natively.
//** There are two possible ways to support it.
//** 1. Polling since timeout occured or object lock with trylock.
//** 2. Create global condition variable, set condition when any
//**    synchronization object get signaled. When waiting for timeout, wait on
//**    this global condition with timeout. On condition, trylock object. If
//**    failed continue waiting on condition. Otherwise object is locked.
//** For standard synchronization objects, model 2 is implemented because
//** it is more efficient that polling.
//** INFO:
//** There is no support for named synchronization objects on Unix!
//** It is possible to use System V Semaphores, must this is not implemented
//** in the standard synchronization objects.
//*****************************************************************************
#if defined(HY_PLATFORM_UNIX)

// get global timeout mutex
pthread_mutex_t* GetTimeoutMutex();

// get global timeout condition
pthread_cond_t* GetTimeoutCond();

#endif



//*****************************************************************************
// Description:
//  basic semaphore
// Remarks:
//  Basic implementation is only a thin wrapper to the native api, without
//  any virtual methods or anythings else what could produce failures!
//  Abstracts a system semaphore synchronization object.
//  See semaphore concepts for details.
//-----------------------------------------------------------------------------
class BasicSemaphore
{
private:
	// maximum semaphore count (16383)
	static const uint32 SEM_MAX_COUNT = 0x3FFF;

	// members
protected:
	SyncHandleSemaphore m_hObject;
	bool m_bCloseHandle;
	bool m_WasOpened;

	// ctor
public:
	// create default semaphore with initial count, must not exceed SEM_MAX_COUNT
	BasicSemaphore(uint32 p_InitialCount);

	// create named semaphore with initial count, must not exceed SEM_MAX_COUNT
	BasicSemaphore(uint32 p_InitialCount, PCTSTR p_Name);

	// take handle instance, handle will be closed by this instance
	BasicSemaphore(SyncHandleSemaphore p_hSemaphore, bool p_bCloseHandle);

	// release handle to semaphore
	~BasicSemaphore();


	// interface
public:

	// lock semaphore with infinite timeout
	inline bool Lock()
	{
		return(Lock(INFINITE));
	}

	// lock semaphore with specified timeout
	bool Lock(uint32 p_Timeout);

	// unlock semaphore
	void Unlock();

	// get raw handle
	inline SyncHandleSemaphore GetHandle()
	{
		return(m_hObject);
	}

	// returns if the semaphore was created or opened (for named semaphores)
	inline bool WasOpened() const { return m_WasOpened; }
};



//*****************************************************************************
// Description:
//  basic mutex
// Remarks:
//  Basic implementation is only a thin wrapper to the native api, without
//  any virtual methods or anythings else what could produce failures!
//  Abstracts a system mutex synchronization object.
//  See mutex concepts for details.
//-----------------------------------------------------------------------------
class BasicMutex
{
	// members
protected:
	SyncHandleMutex m_hObject;
	bool m_bCloseHandle;
	bool m_WasOpened;

	// ctor
public:
	// create default mutex, no name and no initial lock
	BasicMutex();

	// create named mutex with initial lock
	BasicMutex(bool p_bInitialOwner, PCTSTR p_Name);

	// take handle instance, handle will be closed by this instance
	BasicMutex(SyncHandleMutex p_hMutex, bool p_bCloseHandle);

	// release handle to mutex
	~BasicMutex();


	// interface
public:

	// lock mutex with infinite timeout
	inline bool Lock()
	{
		return(Lock(INFINITE));
	}

	// lock mutex with specified timeout
	bool Lock(uint32 p_Timeout);

	// unlock mutex
	void Unlock();

	// get raw handle
	inline SyncHandleMutex GetHandle()
	{
		return(m_hObject);
	}

	// returns if the mutex was created or opened (for named mutexes)
	inline bool WasOpened() const { return m_WasOpened; }
};



//*****************************************************************************
// Description:
//  basic event
// Remarks:
//  Basic implementation is only a thin wrapper to the native api, without
//  any virtual methods or anythings else what could produce failures!
//  Abstracts a system event synchronization object.
//  See event concepts for details.
//-----------------------------------------------------------------------------
class BasicEvent
{
	// members
protected:
	SyncHandleEvent m_hObject;
	bool m_bCloseHandle;
	bool m_WasOpened;

#if defined(HY_PLATFORM_UNIX)
	bool m_bManualReset;
	bool m_bSignaled;
#endif

	// ctor
public:

	// create event with manual or automatic reset
	BasicEvent(bool p_bManualReset);

	// create named event with initial lock
	BasicEvent(bool p_bManualReset, bool p_bInitialState, PCTSTR p_Name);

	// take handle instance, handle will be closed by this instance
	BasicEvent(SyncHandleEvent p_hEvent, bool p_bCloseHandle);

	// release handle to event
	~BasicEvent();


	// event interface
public:

	// wait for event with infinite timeout
	inline bool Wait()
	{
		return(Wait(INFINITE));
	}

	// wait for event with timeout
	bool Wait(uint32 p_Timeout);

	// signal event
	void Set();

	// reset event
	void Reset();

	// get raw handle
	inline SyncHandleEvent GetHandle()
	{
		return(m_hObject);
	}

	// returns if the event was created or opened (for named events)
	inline bool WasOpened() const { return m_WasOpened; }
};



//*****************************************************************************
// Description:
//  basic critical section
// Remarks:
//  Basic implementation is only a thin wrapper to the native api, without
//  any virtual methods or anythings else what could produce failures!
//  Abstracts a system critical section synchronization object.
//  Locking with timeout is not supported, only infinite locks possible.
//  Critical sections are faster than other synchronization objects!
//-----------------------------------------------------------------------------
class BasicSection
{
	// members
protected:
	SyncHandleSection m_hObject;

	// ctor
public:

	// create critical section
	BasicSection();

	// release handle to critical section
	~BasicSection();


	// interface
public:

	// lock handle
	inline bool Lock()
	{
#if defined(HY_PLATFORM_WINDOWS)
		::EnterCriticalSection(&m_hObject);
		return(true);
#elif defined(HY_PLATFORM_UNIX)
		// try to lock with infinite timeout
		int rc = pthread_mutex_lock(&m_hObject);
		if(rc != 0)
			THROW_UNIX_ERROR(rc);
		return true;
#endif
	}

	// lock mutex with specified timeout
	bool Lock(uint32 p_Timeout)
	{
		CHECK_EXPR(p_Timeout != INFINITE && p_Timeout != 0);
#if defined(HY_PLATFORM_WINDOWS)
		if(p_Timeout == INFINITE)
			::EnterCriticalSection(&m_hObject);
		else if(::TryEnterCriticalSection(&m_hObject) == FALSE)
			return false;
		return(true);
#elif defined(HY_PLATFORM_UNIX)
		if(p_Timeout == INFINITE)
		{
			// try to lock with infinite timeout
			int rc = pthread_mutex_lock(&m_hObject);
			if(rc != 0)
				THROW_UNIX_ERROR(rc);
			return true;
		}
		else
		{
			// only try to lock non blocking
			int rc = pthread_mutex_trylock(&m_hObject);
			if(rc == 0)
				return(true);
			else if(rc != EBUSY)
				THROW_UNIX_ERROR(rc);
			return(false);
		}
#endif
	}

	// unlock handle
	inline void Unlock()
	{
#if defined(HY_PLATFORM_WINDOWS)
		::LeaveCriticalSection(&m_hObject);
#elif defined(HY_PLATFORM_UNIX)
		int rc = pthread_mutex_unlock(&m_hObject);
		if(rc != 0)
			THROW_UNIX_ERROR(rc);
#endif
	}
};


//*****************************************************************************
// Description:
//  simple sync object locking class template
// Remarks:
//  Simple locking class which free´s the lock in the destructor.
//  This ensures that a lock will be given free in all cases.
//  This class should be used only in a local context, so the destructor
//  will free the lock in the same local context!
//  This class does no reporting or logging. It should be used in a small save
//  context and when high speed is needed.
//  The template parameter sync object must support Lock() and Unlock()
//  methods.
//-----------------------------------------------------------------------------
template<class T>
class LockT
{
private:
	T* m_SyncObject;				// pointer to synchronization object
	bool m_bLocked;									// internal state flag if currently locking or not

	// prevent default constructors
private:
	LockT() {}
	LockT(const LockT&) {}

	// ctor
public:

	//*****************************************************************************
	// Description:
	//  Locks the synchronization object
	// Parameters:
	//  - p_SyncObject         synchronization object for this lock instance
	//-----------------------------------------------------------------------------
	LockT(T& p_SyncObject) : m_SyncObject(&p_SyncObject), m_bLocked(false)
	{
		Lock();
	}

	//*****************************************************************************
	// Description:
	//  Locks the synchronization object
	// Parameters:
	//  - p_SyncObject         synchronization object for this lock instance
	//  - p_bInitialLock       if true lock synchronization object immediately
	//-----------------------------------------------------------------------------
	LockT(T& p_SyncObject, bool p_bInitialLock) : m_SyncObject(&p_SyncObject), m_bLocked(false)
	{
		if(p_bInitialLock)
			Lock();
	}

	//*****************************************************************************
	// Description:
	//  Locks the synchronization object
	// Parameters:
	//  - p_SyncObject         synchronization object for this lock instance
	//  - p_bInitialLock       if true lock synchronization object immediately
	//  - p_bAttachWaitHandle  if true attach to wait handle as locked, but dont lock
	//-----------------------------------------------------------------------------
	LockT(T& p_SyncObject, bool p_bInitialLock, bool p_bAttachWaitHandle) : m_SyncObject(&p_SyncObject), m_bLocked(false)
	{
		if(p_bAttachWaitHandle && p_bInitialLock)
			m_bLocked = true;
		else if(p_bInitialLock)
			Lock();
	}

	// unlock sync object
	~LockT()
	{
		// destructor must not throw!
		try
		{
			Unlock();
		}
		catch(...)
		{
		}
	}


	// interface
public:

	// lock synchronization object (if not already locked)
	inline void Lock()
	{
		if(!IsLocked())
		{
			m_SyncObject->Lock();
			m_bLocked = true;
		}
	}

	// lock synchronization object (if not already locked)
	inline bool Lock(uint32 p_Timeout)
	{
		if(!IsLocked())
			m_bLocked = m_SyncObject->Lock(p_Timeout);
		return(m_bLocked);
	}

	// unlock synchronization object (if currently locked)
	inline void Unlock()
	{
		if(IsLocked())
		{
			m_bLocked = false;
			m_SyncObject->Unlock();
		}
	}

	// returns whether this instance locks the synchronization object or not
	inline bool IsLocked()
	{
		return(m_bLocked);
	}
};


//*****************************************************************************
//** type definitions
//*****************************************************************************

typedef LockT<BasicMutex> MLock;
typedef LockT<BasicSection> SLock;

#endif // #ifndef THREADINGFX_BASICSYNC_H
