//*****************************************************************************
// Description:  Advanced thread/process synchronization objects
//*****************************************************************************

#ifndef TREADINGFX_WAITOBJECT_H
#define TREADINGFX_WAITOBJECT_H

#include "Utils.h"
#include "BasicSync.h"

//*****************************************************************************
// Description:
//  Base class for shared wait objects (abstract)
// Remarks:
//  Implements base functionality for all wait object implementations.
//  Overrides must manage the m_LockCount and m_LockedByThreadId members!
//-----------------------------------------------------------------------------
class WaitObject
{
	// declare WaitList a friend so it can access LockedExtern/UnlockedExtern
	friend class WaitList;

	// reference counting implementation
	REFERENCE_COUNT_IMPL_MT()

	// members
protected:
	uint32 m_Id;													// unique id
	tstring m_Name;										// name of this sync object
	volatile uint32 m_LockCount;					// active lock count on this object
	volatile uint32 m_LockedByThreadId;		// locked by thread (id) or zero
	bool m_bLockMT;											// when true, this class allows multi threaded locking and unlocking, i.e. lock in T1 and unlock in T2 is allowed

	// hide default ctor
private:
	WaitObject() : m_RefCounter(1) {}

	// ctor
public:

	// Create named wait object
	// - p_bLockMT specifies if the object can be locked and unlocked by a single thread (false) or by multiple threads (true), i.e. semaphores.
	//             When the value is true, for example its possible that T1 locks and T2 unlocks the wait object. In this case no thread lock state is stored.
	WaitObject(PCTSTR p_Name, bool p_bLockMT);

	// virtual destructor
	virtual ~WaitObject();


	// info interface
public:

	// true if there are any locks on this wait object
	bool IsLocked();

	// returns the active lock count on this object
	uint32 GetLockCount();
	
	// thread id of locking thread or zero
	uint32 GetLockedByThreadId();

	// get id of object
	uint32 GetId();

	// get name of object
	tstring GetName();

	// get name and state of this sync object
	tstring GetNameState();

	// get state of sync object, could be overridden
	tstring GetStateInfo();


	// helper methods
protected:

	// should be called by overrides before a lock
	void SetStateWait(uint32 p_Timeout);

	// should be called by overrides after a failed lock
	// function call must follow immediately by SetStateWait function call, no other state is allowed between!
	void RemoveStateWait();

	// should be called by overrides after a lock
	// - p_bUpdateWait: when true a previous wait state is just updated, otherwise a new lock state is added
	void SetStateLock(bool p_bUpdateWait);

	// should be called by overrides before an unlock
	void RemoveStateLock();


	// interface
public:

	// get handle
	virtual SyncHandle GetHandle() = 0;

	// lock handle with infinite timeout
	virtual bool Lock() = 0;

	// lock handle
	virtual bool Lock(uint32 p_Timeout) = 0;

	// unlock handle
	virtual void Unlock() = 0;

private:
	// handle locked externally (WaitList), called to notify object that its locked by this thread!
	virtual void LockedExtern() = 0;

	// handle unlocked externally (WaitList), called to notify object that its unlocked by this thread!
	virtual void UnlockedExtern() = 0;
};

// define derived shared pointer type
DEFINE_SHARED_PTR(WaitObject, WaitObjectPtr)


//*****************************************************************************
// Description:
//  Simple lock class for WaitObject synchronization objects
// Remarks:
//  Simple locking class which free the lock in the destructor.
//  This ensures that a lock will be given free in all cases.
//  This class should be used only in a local context, so the destructor
//  will free the lock in the same local context!
//  Features:
//   - could attach to an already locked wait handle
//   - stores extended locking information per thread
//	 - could detect deadlocks
//	 - could get call stack where the lock occurs
//-----------------------------------------------------------------------------
class WaitLock
{
private:
	WaitObject* m_SyncObject;	// pointer to synchronization object
	bool m_bLocked;					// internal state flag if currently locking or not

	// prevent default constructors
private:
	WaitLock() {}
	WaitLock(const WaitLock&) {}

public:

	//*****************************************************************************
	// Description:
	//  Locks the synchronization object
	// Parameters:
	//  - p_SyncObject         synchronization object for this lock instance
	//-----------------------------------------------------------------------------
	WaitLock(WaitObject* p_SyncObject) : m_SyncObject(p_SyncObject), m_bLocked(false)
	{
		CHECK_EXPR(p_SyncObject);
		m_SyncObject->AddRef();
		Lock();
	}

	//*****************************************************************************
	// Description:
	//  Locks the synchronization object
	// Parameters:
	//  - p_SyncObject         synchronization object for this lock instance
	//-----------------------------------------------------------------------------
	WaitLock(WaitObject& p_SyncObject) : m_SyncObject(&p_SyncObject), m_bLocked(false)
	{
		m_SyncObject->AddRef();
		Lock();
	}

	//*****************************************************************************
	// Description:
	//  Locks the synchronization object
	// Parameters:
	//  - p_SyncObject         synchronization object for this lock instance
	//  - p_bInitialLock       if true lock synchronization object immediately
	//-----------------------------------------------------------------------------
	WaitLock(WaitObject* p_SyncObject, bool p_bInitialLock) : m_SyncObject(p_SyncObject), m_bLocked(false)
	{
		CHECK_EXPR(p_SyncObject);
		m_SyncObject->AddRef();
		if(p_bInitialLock)
			Lock();
	}

	//*****************************************************************************
	// Description:
	//  Locks the synchronization object
	// Parameters:
	//  - p_SyncObject         synchronization object for this lock instance
	//  - p_bInitialLock       if true lock synchronization object immediately
	//-----------------------------------------------------------------------------
	WaitLock(WaitObject& p_SyncObject, bool p_bInitialLock) : m_SyncObject(&p_SyncObject), m_bLocked(false)
	{
		m_SyncObject->AddRef();
		if(p_bInitialLock)
			Lock();
	}

	//*****************************************************************************
	// Description:
	//  Locks the synchronization object
	// Parameters:
	//  - p_SyncObject         synchronization object for this lock instance
	//  - p_bInitialLock       if true lock synchronization object immediately
	//  - p_bAttachWaitHandle  if true attach to wait handle as locked, but do not lock
	//-----------------------------------------------------------------------------
	WaitLock(WaitObject* p_SyncObject, bool p_bInitialLock, bool p_bAttachWaitHandle) : m_SyncObject(p_SyncObject), m_bLocked(false)
	{
		CHECK_EXPR(p_SyncObject);
		m_SyncObject->AddRef();
		if(p_bAttachWaitHandle && p_bInitialLock)
			m_bLocked = true;
		else if(p_bInitialLock)
			Lock();
	}

	//*****************************************************************************
	// Description:
	//  Locks the synchronization object
	// Parameters:
	//  - p_SyncObject         synchronization object for this lock instance
	//  - p_bInitialLock       if true lock synchronization object immediately
	//  - p_bAttachWaitHandle  if true attach to wait handle as locked, but do not lock
	//-----------------------------------------------------------------------------
	WaitLock(WaitObject& p_SyncObject, bool p_bInitialLock, bool p_bAttachWaitHandle) : m_SyncObject(&p_SyncObject), m_bLocked(false)
	{
		m_SyncObject->AddRef();
		if(p_bAttachWaitHandle && p_bInitialLock)
			m_bLocked = true;
		else if(p_bInitialLock)
			Lock();
	}

	//*****************************************************************************
	// Description:
	//  Locks the synchronization object
	// Parameters:
	//  - p_SyncObject         synchronization object for this lock instance
	//-----------------------------------------------------------------------------
	WaitLock(WaitObjectPtr p_SyncObject) : m_SyncObject(p_SyncObject.get()), m_bLocked(false)
	{
		CHECK_EXPR(p_SyncObject != NULL);
		m_SyncObject->AddRef();
		Lock();
	}

	//*****************************************************************************
	// Description:
	//  Locks the synchronization object
	// Parameters:
	//  - p_SyncObject         synchronization object for this lock instance
	//  - p_bInitialLock       if true lock synchronization object immediately
	//-----------------------------------------------------------------------------
	WaitLock(WaitObjectPtr p_SyncObject, bool p_bInitialLock) : m_SyncObject(p_SyncObject.get()), m_bLocked(false)
	{
		CHECK_EXPR(p_SyncObject != NULL);
		m_SyncObject->AddRef();
		if(p_bInitialLock)
			Lock();
	}

	//*****************************************************************************
	// Description:
	//  Locks the synchronization object
	// Parameters:
	//  - p_SyncObject         synchronization object for this lock instance
	//  - p_bInitialLock       if true lock synchronization object immediately
	//  - p_bAttachWaitHandle  if true attach to wait handle as locked, but do not lock
	//-----------------------------------------------------------------------------
	WaitLock(WaitObjectPtr& p_SyncObject, bool p_bInitialLock, bool p_bAttachWaitHandle) : m_SyncObject(p_SyncObject.get()), m_bLocked(false)
	{
		CHECK_EXPR(p_SyncObject != NULL);
		m_SyncObject->AddRef();
		if(p_bAttachWaitHandle && p_bInitialLock)
			m_bLocked = true;
		else if(p_bInitialLock)
			Lock();
	}


	// unlock sync object
	~WaitLock()
	{
		// destructors are not allowed to throw!!!
		try
		{
			Unlock();
		}
		catch(...)
		{
		}
		m_SyncObject->Release();
	}


	// interface
public:

	// lock synchronization object with infinite timeout (if not already locked)
	inline bool Lock()
	{
		if(!IsLocked())
			m_bLocked = m_SyncObject->Lock();
		return(m_bLocked);
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
// Description:
//  list of wait handles
// Remarks:
//  A list of wait handle objects, implementing handle group methods like
//  Win32 function WaitForMultipleObjects.
//  The maximum number of wait objects is 64.
//-----------------------------------------------------------------------------
class WaitList
{
	// enumerations
public:

	// return values for wait methods
	static const int WAIT_OBJECT_0_VALUE = WAIT_OBJECT_0;
	static const int WAIT_TIMEOUT_VALUE = WAIT_TIMEOUT;
	static const int WAIT_FAILED_VALUE = WAIT_FAILED;

	// infinite timeout for waiting operation
	static const uint32 TIMEOUT_INFINITE = INFINITE;

	// maximum allowed number of wait objects
	static const uint32 MAX_WAIT_OBJECTS = MAXIMUM_WAIT_OBJECTS;


	// members
protected:
	typedef std::vector<WaitObject*> WaitObjectPtrArray;
	WaitObjectPtrArray m_WaitHandles;
#if defined(HY_PLATFORM_GUI_WIN)
	typedef std::vector<SyncHandle> SyncHandleArray;
	SyncHandleArray m_RawHandles;
#endif

	// ctor
public:

	// create a wait list with empty list of sync objects
	WaitList();

	// create a wait list and attach 1 sync object
	WaitList(WaitObject* p_WaitHandle1);
	WaitList(WaitObject& p_WaitHandle1);

	// create a wait list and attach 2 sync objects
	WaitList(WaitObject* p_WaitHandle1, WaitObject* p_WaitHandle2);
	WaitList(WaitObject& p_WaitHandle1, WaitObject& p_WaitHandle2);

	// create a wait list and attach 3 sync objects
	WaitList(WaitObject* p_WaitHandle1, WaitObject* p_WaitHandle2, WaitObject* p_WaitHandle3);
	WaitList(WaitObject& p_WaitHandle1, WaitObject& p_WaitHandle2, WaitObject& p_WaitHandle3);

	// create a wait list and attach 4 sync objects
	WaitList(WaitObject* p_WaitHandle1, WaitObject* p_WaitHandle2, WaitObject* p_WaitHandle3, WaitObject* p_WaitHandle4);
	WaitList(WaitObject& p_WaitHandle1, WaitObject& p_WaitHandle2, WaitObject& p_WaitHandle3, WaitObject& p_WaitHandle4);

	// create a wait list and attach 5 sync objects
	WaitList(WaitObject* p_WaitHandle1, WaitObject* p_WaitHandle2, WaitObject* p_WaitHandle3, WaitObject* p_WaitHandle4, WaitObject* p_WaitHandle5);
	WaitList(WaitObject& p_WaitHandle1, WaitObject& p_WaitHandle2, WaitObject& p_WaitHandle3, WaitObject& p_WaitHandle4, WaitObject& p_WaitHandle5);


	// dtor
	~WaitList();

	// interface
public:

	// add handle to list
	void AddHandle(WaitObject* p_WaitHandle);
	void AddHandle(WaitObject& p_WaitHandle);

	// add raw handle to list
	void AddHandle(SyncHandle p_WaitHandle);

	// removes a handle (by index) from the list
	void RemoveHandle(intx p_Idx);

	// removes the given handle from the list
	void RemoveHandle(WaitObject* p_WaitHandle);

	// get wait handle by index
	WaitObject* GetWaitHandle(int p_Idx);

	// get raw wait handle by index
	SyncHandle GetSyncHandle(int p_Idx);


	// synchronization interface
public:

	//-----------------------------------------------------------------------------
	// Description:
	//  wait for any object in this list with infinite timeout (TIMEOUT_INFINITE)
	// Return:
	//  - WAIT_OBJECT_0_VALUE + wait handle index that was triggered
	//  - WAIT_TIMEOUT_VALUE when a timeout occurred
	//  - WAIT_FAILED_VALUE when any other error occurred
	int WaitForMultipleObjects();

	//-----------------------------------------------------------------------------
	// Description:
	//  wait for any object in this list
	// Parameters:
	//  - p_Timeout       timeout in milliseconds waiting for any object
	// Return:
	//  - WAIT_OBJECT_0_VALUE + wait handle index that was triggered
	//  - WAIT_TIMEOUT_VALUE when a timeout occurred
	//  - WAIT_FAILED_VALUE when any other error occurred
	int WaitForMultipleObjects(uint32 p_Timeout);

	//-----------------------------------------------------------------------------
	// Description:
	//  wait for a specific single object with infinite timeout (TIMEOUT_INFINITE)
	// Parameters:
	//  - p_WaitObject    index of wait object to wait to
	// Return:
	//  - true if locked, false on timeout
	bool WaitForSingleObject(int p_WaitObject);

	//-----------------------------------------------------------------------------
	// Description:
	//  wait for a specific single object
	// Parameters:
	//  - p_WaitObject    index of wait object to wait to
	//  - p_Timeout       timeout in milliseconds waiting for any object
	// Return:
	//  - true if locked, false on timeout
	bool WaitForSingleObject(int p_WaitObject, uint32 p_Timeout);


	// helper methods
protected:
	// set thread wait state
	void SetStateWait(uint32 p_Timeout, int p_WaitObject);

	// remove thread wait state
	void RemoveStateWait(int p_WaitObject);
};


//*****************************************************************************
// Description:
//  Mutex synchronization object
// Remarks:
//  Abstracts a mutex synchronization object.
//  Implements the CWaitObject abstract base class with the functionality
//  of a basic mutex object.
//  The object is reference counted - if the last reference to the instance
//  is released the section object gets destroyed.
//-----------------------------------------------------------------------------
class WaitMutex : public WaitObject
{
	// members
protected:
	BasicMutex m_hObject;			// basic mutex object

	// ctor
public:
	// create default mutex, no name and no initial lock
	WaitMutex(PCTSTR p_Name);

	// create named mutex with initial lock
	WaitMutex(PCTSTR p_Name, bool p_bInitialOwner, bool p_bNamed);

	// take handle instance, handle will be closed by this instance
	WaitMutex(PCTSTR p_Name, SyncHandleMutex p_hMutex, bool p_bCloseHandle);

	// release handle to mutex
	~WaitMutex();

	// interface
public:

	// get handle
	virtual SyncHandle GetHandle();

	// lock handle with infinite timeout
	virtual bool Lock();

	// lock handle
	virtual bool Lock(uint32 p_Timeout);

	// unlock handle
	virtual void Unlock();

	// handle locked externally (WaitList), called to notify object that its locked by this thread!
	virtual void LockedExtern();

	// handle unlocked externally (WaitList), called to notify object that its unlocked by this thread!
	virtual void UnlockedExtern();

	// returns if the event was created or opened (for named events)
	bool WasOpened() const { return m_hObject.WasOpened(); }
};

// define derived shared pointer type
DEFINE_SHARED_PTR(WaitMutex, WaitMutexPtr)



//*****************************************************************************
// Description:
//  Semaphore synchronization object
// Remarks:
//  Abstracts a semaphore synchronization object.
//  Implements the CWaitObject abstract base class with the functionality
//  of a basic semaphore object.
//  The object is reference counted - if the last reference to the instance
//  is released the section object gets destroyed.
//-----------------------------------------------------------------------------
class WaitSemaphore : public WaitObject
{
	// members
protected:
	BasicSemaphore m_hObject;			// basic semaphore object

	// ctor
public:
	// create default semaphore without a name
	WaitSemaphore(PCTSTR p_Name, uint32 p_InitialCount);

	// create named semaphore
	WaitSemaphore(PCTSTR p_Name, uint32 p_InitialCount, bool p_bNamed);

	// take handle instance, handle will be closed by this instance
	WaitSemaphore(PCTSTR p_Name, SyncHandleSemaphore p_hMutex, bool p_bCloseHandle);

	// release handle to semaphore
	~WaitSemaphore();

	// interface
public:

	// get handle
	virtual SyncHandle GetHandle();

	// lock handle with infinite timeout
	virtual bool Lock();

	// lock handle
	virtual bool Lock(uint32 p_Timeout);

	// unlock handle
	virtual void Unlock();

	// returns if the event was created or opened (for named events)
	bool WasOpened() const { return m_hObject.WasOpened(); }

private:
	// handle locked externally (WaitList), called to notify object that its locked by this thread!
	virtual void LockedExtern();

	// handle unlocked externally (WaitList), called to notify object that its unlocked by this thread!
	virtual void UnlockedExtern();
};

// define derived shared pointer type
DEFINE_SHARED_PTR(WaitSemaphore, WaitSemaphorePtr)



//*****************************************************************************
// Description:
//  Event synchronization object
// Remarks:
//  Abstracts an event synchronization object.
//  Implements the CWaitObject abstract base class with the functionality
//  of a basic event object.
//  The object is reference counted - if the last reference to the instance
//  is released the section object gets destroyed.
//-----------------------------------------------------------------------------
class WaitEvent : public WaitObject
{
	// members
protected:
	BasicEvent m_hObject;			// basic event object

	// ctor
public:

	// create event with manual or automatic reset
	WaitEvent(PCTSTR p_Name, bool p_bManualReset);

	// create named event with initial lock
	WaitEvent(PCTSTR p_Name, bool p_bManualReset, bool p_bInitialOwner, bool p_bNamed);

	// take handle instance, handle will be closed by this instance
	WaitEvent(PCTSTR p_Name, SyncHandleEvent p_hEvent, bool p_bCloseHandle);

	// release handle to event
	~WaitEvent();


	// event interface
public:

	// wait for event with infinite timeout
	virtual bool Wait();

	// wait for event
	virtual bool Wait(uint32 p_Timeout);

	// signal event
	virtual void Set();

	// reset event
	virtual void Reset();


	// interface
public:

	// get handle
	virtual SyncHandle GetHandle();

	// lock handle with infinite timeout
	virtual bool Lock();

	// lock handle
	virtual bool Lock(uint32 p_Timeout);

	// unlock handle, not supported
	virtual void Unlock();

	// returns if the event was created or opened (for named events)
	bool WasOpened() const { return m_hObject.WasOpened(); }

private:
	// handle locked externally (WaitList), called to notify object that its locked by this thread!
	virtual void LockedExtern();

	// handle unlocked externally (WaitList), called to notify object that its unlocked by this thread!
	virtual void UnlockedExtern();
};

// define derived shared pointer type
DEFINE_SHARED_PTR(WaitEvent, WaitEventPtr)



//*****************************************************************************
// Description:
//  Critical section synchronization object
// Remarks:
//  Abstracts a windows critical section synchronization object.
//  Not usable in combination with WaitList, because critical sections do
//  not support handles!
//  Locking with timeout is not supported, only infinite locks possible.
//  Critical sections are faster than other synchronization objects!
//  The object is reference counted - if the last reference to the instance
//  is released the section object gets destroyed.
//-----------------------------------------------------------------------------
class WaitSection : public WaitObject
{
	// members
protected:
	BasicSection m_hObject;		// basic critical section object

	// ctor
public:

	// create critical section
	WaitSection(PCTSTR p_Name);

	// release handle to critical section
	~WaitSection();


	// interface
public:

	// get handle, throws not supported exception
	virtual SyncHandle GetHandle();

	// lock handle with infinite timeout
	virtual bool Lock();

	// lock handle, only supported with TIMEOUT_INFINITE infinite timeout!
	virtual bool Lock(uint32 p_Timeout);

	// unlock handle
	virtual void Unlock();

private:
	// handle locked externally (WaitList), called to notify object that its locked by this thread!
	// throws not supported exception
	virtual void LockedExtern();

	// handle unlocked externally (WaitList), called to notify object that its unlocked by this thread!
	// throws not supported exception
	virtual void UnlockedExtern();
};

// define derived shared pointer type
DEFINE_SHARED_PTR(WaitSection, WaitSectionPtr)

#endif // #ifndef TREADINGFX_WAITOBJECT_H
