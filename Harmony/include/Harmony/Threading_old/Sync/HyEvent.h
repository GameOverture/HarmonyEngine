/**************************************************************************
 *	HyEvent.h
 *	
 *	Copyright (c) 2013 Overture Games, Inc.
 *	All Rights Reserved.
 *
 *  Permission to use, copy, modify, and distribute this software
 *  is hereby NOT granted.
 *************************************************************************/
#ifndef __HyEvent_h__
#define __HyEvent_h__

#include "Afx/HyThreadAfx.h"

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

#endif /* __HyEvent_h__ */