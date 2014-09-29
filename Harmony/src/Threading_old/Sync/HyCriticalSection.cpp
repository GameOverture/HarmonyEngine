/**************************************************************************
 *	HyCriticalSection.cpp
 *	
 *	Copyright (c) 2013 Overture Games, Inc.
 *	All Rights Reserved.
 *
 *  Permission to use, copy, modify, and distribute this software
 *  is hereby NOT granted.
 *************************************************************************/
#include "HyCriticalSection.h"

HyCriticalSection::HyCriticalSection()
{
#if defined(HY_PLATFORM_WINDOWS)
	::InitializeCriticalSection(&m_hObject);
#elif defined(HY_PLATFORM_UNIX)
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
	int rc = pthread_mutex_init(&m_hObject, &attr);
	if(rc != 0)
	{
		pthread_mutexattr_destroy(&attr);
		THROW_UNIX_ERROR(rc);
	}
	pthread_mutexattr_destroy(&attr);
#endif
}

HyCriticalSection::~HyCriticalSection()
{
#if defined(HY_PLATFORM_WINDOWS)
	::DeleteCriticalSection(&m_hObject);
#elif defined(HY_PLATFORM_UNIX)
	pthread_mutex_destroy(&m_hObject);
#endif
}
