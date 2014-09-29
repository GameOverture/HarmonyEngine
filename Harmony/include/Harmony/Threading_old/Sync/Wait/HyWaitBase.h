/**************************************************************************
 *	HyWaitBase.h
 *	
 *	Copyright (c) 2013 Overture Games, Inc.
 *	All Rights Reserved.
 *
 *  Permission to use, copy, modify, and distribute this software
 *  is hereby NOT granted.
 *************************************************************************/
#ifndef __HyWaitBase_h__
#define __HyWaitBase_h__

#include "Afx/HyThreadAfx.h"
#include "Utilities/HyRefCounters.h"

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

class WaitObject
{
	// declare WaitList a friend so it can access LockedExtern/UnlockedExtern
	friend class WaitList;

	// reference counting implementation
	HyRefCountMT m_RefCounter;//REFERENCE_COUNT_IMPL_MT()

		// members
protected:
	uint32 m_Id;													// unique id
	HyString m_Name;										// name of this sync object
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
	HyString GetName();

	// get name and state of this sync object
	HyString GetNameState();

	// get state of sync object, could be overridden
	HyString GetStateInfo();


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

#endif /* __HyWaitBase_h__ */