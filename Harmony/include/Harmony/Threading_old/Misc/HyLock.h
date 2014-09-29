/**************************************************************************
 *	HyLock.h
 *	
 *	Copyright (c) 2013 Overture Games, Inc.
 *	All Rights Reserved.
 *
 *  Permission to use, copy, modify, and distribute this software
 *  is hereby NOT granted.
 *************************************************************************/
#ifndef __HyLock_h__
#define __HyLock_h__


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
//  The template parameter sync object must support Lock(), IsLocked() and Unlock()
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

//typedef LockT<BasicMutex> MLock;
typedef LockT<HyCriticalSection> SLock;

#endif /* __HyLock_h__ */
