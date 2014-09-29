//*****************************************************************************
// Description:  Extended wait object types
//*****************************************************************************

#ifndef TREADINGFX_WAITOBJECTEX_H
#define TREADINGFX_WAITOBJECTEX_H

#include "Threading.h"

//*****************************************************************************
// Description:
//  thread wait event
// Remarks:
//  Implements a wait object for threads. With this implementation it is
//  possible to wait for multiple objects including threads.
//-----------------------------------------------------------------------------
class ThreadWaitEvent : public WaitObject
{
	// members
protected:
	ThreadInfoPtr m_hThread;		// thread instance

	// ctor
public:

	// create thread wait event, thread name will be used as event name
	ThreadWaitEvent(ThreadInfoPtr p_hThread);

	// release handle to thread
	~ThreadWaitEvent();


	// event interface
public:

	// wait for thread stop event with infinite timeout
	bool Wait();

	// wait for thread stop event
	bool Wait(uint32 p_Timeout);


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

	// handle locked externally (WaitList), called to notify object that its locked by this thread!
	virtual void LockedExtern();

	// handle unlocked externally (WaitList), called to notify object that its unlocked by this thread!
	virtual void UnlockedExtern();
};

// define derived shared pointer type
DEFINE_SHARED_PTR(ThreadWaitEvent, ThreadWaitEventPtr)



//*****************************************************************************
// Description:
//  queue wait event
// Remarks:
//  Implements a wait object for generic queues. With this implementation it is
//  possible to wait for multiple objects including queues.
//  The object is signaled if at least one element is in the queue, and
//  non-signaled if the queue is empty.
//  All queue methods are multi threading safe, there is no need for any
//  additional synchronization object.
//  If the queue is signaled, it is possible that a call to Pop() fails,
//  because another thread could already got the element from the queue.
//  The base class CSharedWaitEvent represents the signaled/non-signaled state
//  of the queue.
//-----------------------------------------------------------------------------
template<typename E>
class WaitQueue : public WaitEvent
{
public:
	typedef std::queue<E> container_type;
	typedef typename container_type::value_type value_type;

	// members
protected:

	// data container access synchronization object
	BasicSection m_AccessSync;

	// queue to store elements
	container_type m_Data;


	// ctor
public:

	// create named wait queue
	WaitQueue(PCTSTR p_Name) : WaitEvent(p_Name, true)
	{
	}

	// release handle to thread
	~WaitQueue()
	{
	}


	// event interface
public:

	// push element at the end of the queue, if queue was empty it gets signaled
	void Push(const value_type& p_Element)
	{
		SLock Lock(m_AccessSync);
		m_Data.push(p_Element);
		WaitEvent::Set();
	}

	// remove element from the beginning of the queue, returns true if successful and false if queue is empty
	// the element will be stored in the p_Element parameter which is used by reference.
	// if the last element is removed from the queue, the object is reset to non-signaled.
	bool Pop(value_type& p_Element)
	{
		SLock Lock(m_AccessSync);
		if(m_Data.empty())
		{
			ASSERT_EXPR(WaitEvent::Wait(0) == false);
			return(false);
		}
		p_Element = m_Data.front();
		m_Data.pop();
		if(m_Data.empty())
			WaitEvent::Reset();
		return(true);
	}

	// get number of elements in this queue
	intx GetSize()
	{
		SLock Lock(m_AccessSync);
		return(m_Data.size());
	}
};

#endif // #ifndef TREADINGFX_WAITOBJECTEX_H
