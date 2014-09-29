/**************************************************************************
 *	HyEvent.cpp
 *	
 *	Copyright (c) 2013 Overture Games, Inc.
 *	All Rights Reserved.
 *
 *  Permission to use, copy, modify, and distribute this software
 *  is hereby NOT granted.
 *************************************************************************/

#include "HyEvent.h"

BasicEvent::BasicEvent(bool p_bManualReset) : m_bCloseHandle(true)
{
	m_WasOpened = false;
#if defined(HY_PLATFORM_WINDOWS)
	m_hObject = ::CreateEvent(NULL, p_bManualReset, FALSE, NULL);
	HyAssert(m_hObject, "HyEvent::CreateEvent() failed.");

#elif defined(HY_PLATFORM_UNIX)
	// set parameters
	m_bManualReset = p_bManualReset;
	m_bSignaled = false;

	// create mutex
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
	int rc = pthread_mutex_init(&m_hObject.m_hMutex, &attr);
	if(rc != 0)
	{
		pthread_mutexattr_destroy(&attr);
		THROW_UNIX_ERROR(rc);
	}
	pthread_mutexattr_destroy(&attr);

	// create condition
	rc = pthread_cond_init(&m_hObject.m_hCond, 0);
	if(rc != 0)
	{
		pthread_mutex_destroy(&m_hObject.m_hMutex);
		THROW_UNIX_ERROR(rc);
	}
#endif
}

BasicEvent::BasicEvent(bool p_bManualReset, bool p_bInitialState, PCTSTR p_Name) : m_bCloseHandle(true)
{
	m_WasOpened = false;
	if(p_Name == NULL)
	{
#if defined(HY_PLATFORM_WINDOWS)
		EmptySecurityAttributes EmtpyAttrib;
		m_hObject = ::CreateEvent(&EmtpyAttrib.m_SecAttr, p_bManualReset, p_bInitialState, NULL);
		HyAssert(m_hObject, "HyEvent::CreateEvent() failed.");
#elif defined(HY_PLATFORM_UNIX)
		// set parameters
		m_bManualReset = p_bManualReset;
		m_bSignaled = p_bInitialState;

		// create mutex
		pthread_mutexattr_t attr;
		pthread_mutexattr_init(&attr);
		pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
		int rc = pthread_mutex_init(&m_hObject.m_hMutex, &attr);
		if(rc != 0)
		{
			pthread_mutexattr_destroy(&attr);
			THROW_UNIX_ERROR(rc);
		}
		pthread_mutexattr_destroy(&attr);

		// create condition
		rc = pthread_cond_init(&m_hObject.m_hCond, 0);
		if(rc != 0)
		{
			pthread_mutex_destroy(&m_hObject.m_hMutex);
			THROW_UNIX_ERROR(rc);
		}
#endif
	}
	else
	{
		HyStringStream name;
		name << p_Name << (p_bManualReset ? _T("m") : _T("a"));

#if defined(HY_PLATFORM_WINDOWS)
		EmptySecurityAttributes EmtpyAttrib;
		m_hObject = ::CreateEvent(&EmtpyAttrib.m_SecAttr, p_bManualReset, p_bInitialState, name.str().c_str());
		HyAssert(m_hObject, "HyEvent::CreateEvent() failed.");

		m_WasOpened = (GetLastError() == ERROR_ALREADY_EXISTS);
#elif defined(HY_PLATFORM_UNIX)

		m_bManualReset = p_bManualReset;
		m_hObject.m_SysVSemaphore.Create(name.str().c_str(), p_bInitialState ? 0 : 1, m_WasOpened);
#endif
	}
}

BasicEvent::BasicEvent(SyncHandleEvent p_hEvent, bool p_bCloseHandle) : m_bCloseHandle(p_bCloseHandle)
{
	m_WasOpened = false;
#if defined(HY_PLATFORM_WINDOWS)
	HyAssert(p_hEvent, "HyEvent passed NULL SyncHandleEvent");
#endif
	m_hObject = p_hEvent;
#if defined(HY_PLATFORM_WINDOWS)
	HyAssert(m_hObject, "HyEvent out of resources");
#endif
}

BasicEvent::~BasicEvent()
{
	if(m_bCloseHandle)
	{
#if defined(HY_PLATFORM_WINDOWS)
		if(m_hObject != NULL)
			::CloseHandle(m_hObject);
		m_hObject = NULL;
#elif defined(HY_PLATFORM_UNIX)
		if(m_hObject.m_SysVSemaphore.IsValid())
			m_hObject.m_SysVSemaphore.Destroy();
		else
		{
			pthread_mutex_destroy(&m_hObject.m_hMutex);
			pthread_cond_destroy(&m_hObject.m_hCond);
		}
#endif
	}
}

bool BasicEvent::Wait(uint32 p_Timeout)
{
#if defined(HY_PLATFORM_WINDOWS)
	DWORD dwRet = IGNORE_ABANDONED(::WaitForSingleObject(m_hObject, p_Timeout));
	if(dwRet == WAIT_OBJECT_0)
		return(true);
	return(false);
#elif defined(HY_PLATFORM_UNIX)
	if(m_hObject.m_SysVSemaphore.IsValid())
	{
		sembuf ops[2];
		if(p_Timeout == INFINITE)
		{
			const uint32 numOps = m_bManualReset ? 1 : 2;
			// first operation: wait until the event gets signaled (semval == 0)
			ops[0].sem_num = 0;
			ops[0].sem_op = 0;
			ops[0].sem_flg = 0;
			if(!m_bManualReset)
			{
				// second operation - only on manual reset - add 1 to semval (unsignaled state)
				ops[1].sem_num = 0;
				ops[1].sem_op = 1;
				ops[1].sem_flg = 0;
			}

			if(m_hObject.m_SysVSemaphore.DoOp(ops, numOps) != 0)
				THROW_LAST_UNIX_ERROR();

			return true;
		}
		else if(p_Timeout == 0)
		{
			const uint32 numOps = m_bManualReset ? 1 : 2;
			// check if the event is signaled (semval == 0)
			ops[0].sem_num = 0;
			ops[0].sem_op = 0;
			ops[0].sem_flg = IPC_NOWAIT;
			if(!m_bManualReset)
			{
				// second operation - only on manual reset - add 1 to semval (unsignaled state)
				ops[1].sem_num = 0;
				ops[1].sem_op = 1;
				ops[1].sem_flg = 0;
			}

			if(m_hObject.m_SysVSemaphore.DoOp(ops, numOps) == 0)
				return true;
			if(InteropGetLastError() != EAGAIN)
				THROW_LAST_UNIX_ERROR();
			return false;
		}
		else
		{
			timespec delay;
			uint32 timeout = 0;
			const uint32 numOps = m_bManualReset ? 1 : 2;

			delay.tv_sec = 0;
			delay.tv_nsec = 10000000;  // 10 milli sec
			while(timeout < p_Timeout)
			{
				//Call Wait for Zero with IPC_NOWAIT option,so it will be non blocking
				ops[0].sem_num = 0;
				ops[0].sem_op = 0;
				ops[0].sem_flg = IPC_NOWAIT;
				if(!m_bManualReset)
				{
					// second operation - only on manual reset - add 1 to semval
					ops[1].sem_num = 0;
					ops[1].sem_op = 1;
					ops[1].sem_flg = 0;
				}

				int res = m_hObject.m_SysVSemaphore.DoOp(ops, numOps);
				if(res != 0)
				{
					// check whether somebody else has the semaphore
					if(InteropGetLastError() == EAGAIN)
					{
						/* sleep for delay time */
						nanosleep(&delay, NULL);
						timeout += 10;
					}
					else
						THROW_LAST_UNIX_ERROR();
				}
				else
				{
					// semaphore got triggered
					return true;
				}
			}
			return false;
		}		
	}
	else
	{
		if(p_Timeout == INFINITE)
		{
			// try to lock with infinite timeout
			int rc = pthread_mutex_lock(&m_hObject.m_hMutex);
			if(rc != 0)
				THROW_UNIX_ERROR(rc);

			// check signal
			if(m_bSignaled)
			{
				if(!m_bManualReset)
					m_bSignaled = false;
				rc = pthread_mutex_unlock(&m_hObject.m_hMutex);
				if(rc != 0)
					THROW_UNIX_ERROR(rc);
				return(true);
			}

			while(1)
			{
				// mutex will be unlocked, waiting for condition
				rc = pthread_cond_wait(&m_hObject.m_hCond, &m_hObject.m_hMutex);
				if(rc == 0)
				{
					// condition occured and mutex locked!
					// check signal
					if(m_bSignaled)
					{
						if(!m_bManualReset)
							m_bSignaled = false;
						rc = pthread_mutex_unlock(&m_hObject.m_hMutex);
						if(rc != 0)
							THROW_UNIX_ERROR(rc);
						return(true);
					}
					// object not signaled, goto wait state
				}
				else
				{
					pthread_mutex_unlock(&m_hObject.m_hMutex);
					THROW_UNIX_ERROR(rc);
				}
			}
		}
		else if(p_Timeout == 0)
		{
			// try to lock with infinite timeout
			int rc = pthread_mutex_lock(&m_hObject.m_hMutex);
			if(rc != 0)
				THROW_UNIX_ERROR(rc);

			// check signal
			if(m_bSignaled)
			{
				if(!m_bManualReset)
					m_bSignaled = false;
				rc = pthread_mutex_unlock(&m_hObject.m_hMutex);
				if(rc != 0)
					THROW_UNIX_ERROR(rc);
				return(true);
			}

			// not signaled
			rc = pthread_mutex_unlock(&m_hObject.m_hMutex);
			if(rc != 0)
				THROW_UNIX_ERROR(rc);
			return(false);
		}
		else
		{
			// try to lock with infinite timeout
			int rc = pthread_mutex_lock(&m_hObject.m_hMutex);
			if(rc != 0)
				THROW_UNIX_ERROR(rc);

			// check signal
			if(m_bSignaled)
			{
				if(!m_bManualReset)
					m_bSignaled = false;
				rc = pthread_mutex_unlock(&m_hObject.m_hMutex);
				if(rc != 0)
					THROW_UNIX_ERROR(rc);
				return(true);
			}

			// get current time, add timeout and correct overflow
			timeval tp;
			timespec ts;
			if(gettimeofday(&tp, 0) != 0)
			{
				uint32 LastError = InteropGetLastError();
				pthread_mutex_unlock(&m_hObject.m_hMutex);
				THROW_UNIX_ERROR(LastError);
			}
			ts.tv_sec = tp.tv_sec + (p_Timeout / 1000);
			ts.tv_nsec = (tp.tv_usec + ((p_Timeout % 1000) * 1000)) * 1000;
			if(ts.tv_nsec >= 1000000000)
			{
				ts.tv_sec++;
				ts.tv_nsec -= 1000000000;
			}

			// wait for timeout or signal
			while(1)
			{
				// mutex will be unlocked, waiting for condition
				int rc = pthread_cond_timedwait(&m_hObject.m_hCond, &m_hObject.m_hMutex, &ts);
				if(rc == 0 || rc == ETIMEDOUT)
				{
					// check for timeout
					bool bTimeout = (rc == ETIMEDOUT);

					// condition occured and mutex locked!
					// check signal
					if(m_bSignaled)
					{
						if(!m_bManualReset)
							m_bSignaled = false;
						rc = pthread_mutex_unlock(&m_hObject.m_hMutex);
						if(rc != 0)
							THROW_UNIX_ERROR(rc);
						return(true);
					}
					else if(bTimeout)
					{
						// timeout occured, break
						rc = pthread_mutex_unlock(&m_hObject.m_hMutex);
						if(rc != 0)
							THROW_UNIX_ERROR(rc);
						return(false);
					}
					// object not signaled, goto wait state
				}
				else
				{
					pthread_mutex_unlock(&m_hObject.m_hMutex);
					THROW_UNIX_ERROR(rc);
				}
			}
		}
	}
#endif
}

void BasicEvent::Set()
{
#if defined(HY_PLATFORM_WINDOWS)
	HyAssert(SetEvent(m_hObject), "HyEvent::Set() failed at SetEvent()");
#elif defined(HY_PLATFORM_UNIX)
	// lock global timeout mutex
	int rc = pthread_mutex_lock(GetTimeoutMutex());
	if(rc != 0)
		THROW_UNIX_ERROR(rc);

	if(m_hObject.m_SysVSemaphore.IsValid())
	{
		// set the semaphore value to 0 (= signaled)
		sembuf op = {0, -1, IPC_NOWAIT };
		if(m_hObject.m_SysVSemaphore.DoOp(&op, 1) != 0 && InteropGetLastError() != EAGAIN)
		{
			pthread_mutex_unlock(GetTimeoutMutex());
			THROW_LAST_UNIX_ERROR();
		}
	}
	else
	{		
		// lock condition mutex
		rc = pthread_mutex_lock(&m_hObject.m_hMutex);
		if(rc != 0)
		{
			pthread_mutex_unlock(GetTimeoutMutex());
			THROW_UNIX_ERROR(rc);
		}
		// signal condition
		m_bSignaled = true;
		if(m_bManualReset)
		{
			// broadcast for manual reset event, release all threads
			rc = pthread_cond_broadcast(&m_hObject.m_hCond);
			if(rc != 0)
			{
				pthread_mutex_unlock(&m_hObject.m_hMutex);
				pthread_mutex_unlock(GetTimeoutMutex());
				THROW_UNIX_ERROR(rc);
			}
		}
		else
		{
			// signal to one thread for auto reset event, release at least one thread
			rc = pthread_cond_signal(&m_hObject.m_hCond);
			if(rc != 0)
			{
				pthread_mutex_unlock(&m_hObject.m_hMutex);
				pthread_mutex_unlock(GetTimeoutMutex());
				THROW_UNIX_ERROR(rc);
			}
		}
		// unlock condition mutex
		rc = pthread_mutex_unlock(&m_hObject.m_hMutex);
		if(rc != 0)
		{
			pthread_mutex_unlock(GetTimeoutMutex());
			THROW_UNIX_ERROR(rc);
		}
	}

	// broadcast state change
	rc = pthread_cond_broadcast(GetTimeoutCond());
	if(rc != 0)
	{
		pthread_mutex_unlock(GetTimeoutMutex());
		THROW_UNIX_ERROR(rc);
	}
	// unlock global timeout mutex
	rc = pthread_mutex_unlock(GetTimeoutMutex());
	if(rc != 0)
		THROW_UNIX_ERROR(rc);
#endif
}

void BasicEvent::Reset()
{
#if defined(HY_PLATFORM_WINDOWS)
	HyAssert(ResetEvent(m_hObject), "HyEvent::Set() failed at ResetEvent()");
#elif defined(HY_PLATFORM_UNIX)
	if(m_hObject.m_SysVSemaphore.IsValid())
	{
		// look if semaphore is 0 - if yes set to 1 (= not signaled)
		sembuf ops[2] = 
		{
			{0, 0, IPC_NOWAIT },
			{0, 1, 0 }
		};

		if(m_hObject.m_SysVSemaphore.DoOp(ops, 2) != 0 && InteropGetLastError() != EAGAIN)
			THROW_LAST_UNIX_ERROR();
	}
	else
	{
		int rc = pthread_mutex_lock(&m_hObject.m_hMutex);
		if(rc != 0)
			THROW_UNIX_ERROR(rc);
		m_bSignaled = false;
		rc = pthread_mutex_unlock(&m_hObject.m_hMutex);
		if(rc != 0)
			THROW_UNIX_ERROR(rc);
	}
#endif
}
