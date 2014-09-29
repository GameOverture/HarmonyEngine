/**************************************************************************
 *	HyCriticalSection.h
 *	
 *	Copyright (c) 2013 Overture Games, Inc.
 *	All Rights Reserved.
 *
 *  Permission to use, copy, modify, and distribute this software
 *  is hereby NOT granted.
 *************************************************************************/
#ifndef __HyCriticalSection_h__
#define __HyCriticalSection_h__

#include "Afx/HyThreadAfx.h"

class HyCriticalSection
{
	// members
protected:
	SyncHandleSection m_hObject;

	// ctor
public:

	// create critical section
	HyCriticalSection();

	// release handle to critical section
	~HyCriticalSection();


	// interface
public:

	// lock handle
	inline bool Lock()
	{
#if defined(HY_PLATFORM_WINDOWS)
		::EnterCriticalSection((CRITICAL_SECTION*)&m_hObject);
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
		HyAssert(p_Timeout != INFINITE && p_Timeout != 0, "HyCriticalSection::Lock() was passed an invalid timeout value.");
#if defined(HY_PLATFORM_WINDOWS)
		if(p_Timeout == INFINITE)
			::EnterCriticalSection((CRITICAL_SECTION*)&m_hObject);
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
		::LeaveCriticalSection((CRITICAL_SECTION*)&m_hObject);
#elif defined(HY_PLATFORM_UNIX)
		int rc = pthread_mutex_unlock(&m_hObject);
		if(rc != 0)
			THROW_UNIX_ERROR(rc);
#endif
	}
};

#endif /* __HyCriticalSection_h__ */