
#include "PlatformInterop.h"
#include "BasicSync.h"

#if defined(HY_PLATFORM_UNIX)

// get global timeout mutex
pthread_mutex_t* GetTimeoutMutex()
{
	return(&ThreadManager::Get()->m_hTimeoutMutex);
}

// get global timeout condition
pthread_cond_t* GetTimeoutCond()
{
	return(&ThreadManager::Get()->m_hTimeoutCond);
}


#endif



//*****************************************************************************
//** BasicSemaphore implementation
//*****************************************************************************

BasicSemaphore::BasicSemaphore(uint32 p_InitialCount) : m_bCloseHandle(true)
{
	ASSERT_EXPR(p_InitialCount <= SEM_MAX_COUNT);
	m_WasOpened = false;
#if defined(HY_PLATFORM_WINDOWS)
	m_hObject = ::CreateSemaphore(NULL, p_InitialCount, SEM_MAX_COUNT, NULL);
	if(m_hObject == NULL)
		THROW_LAST_WIN_ERROR();
#elif defined(HY_PLATFORM_UNIX)
		if(sem_init(&m_hObject.m_PosixSemaphore, 0, p_InitialCount) == -1)
			THROW_LAST_UNIX_ERROR();
#endif
}

BasicSemaphore::BasicSemaphore(uint32 p_InitialCount, PCTSTR p_Name) : m_bCloseHandle(true)
{
	m_WasOpened = false;
	if(p_Name == NULL)
	{
#if defined(HY_PLATFORM_WINDOWS)
		m_hObject = ::CreateSemaphore(NULL, p_InitialCount, SEM_MAX_COUNT, NULL);
		if(m_hObject == NULL)
			THROW_LAST_WIN_ERROR();
#elif defined(HY_PLATFORM_UNIX)
	if(sem_init(&m_hObject.m_PosixSemaphore, 0, p_InitialCount) == -1)
		THROW_LAST_UNIX_ERROR();
#endif
	}
	else
	{
#if defined(HY_PLATFORM_WINDOWS)
		EmptySecurityAttributes EmtpyAttrib;
		m_hObject = ::CreateSemaphore(&EmtpyAttrib.m_SecAttr, p_InitialCount, SEM_MAX_COUNT, p_Name);
		if(m_hObject == NULL)
			THROW_LAST_WIN_ERROR();
		else
			m_WasOpened = (GetLastError() == ERROR_ALREADY_EXISTS);
#elif defined(HY_PLATFORM_UNIX)
		m_hObject.m_SysVSemaphore.Create(p_Name, p_InitialCount, m_WasOpened);
#endif
	}
}

BasicSemaphore::BasicSemaphore(SyncHandleSemaphore p_hSemaphore, bool p_bCloseHandle) : m_bCloseHandle(p_bCloseHandle)
{
	m_WasOpened = false;
#if defined(HY_PLATFORM_WINDOWS)
	CHECK_EXPR(p_hSemaphore);
#endif
	m_hObject = p_hSemaphore;
#if defined(HY_PLATFORM_WINDOWS)
	if(m_hObject == NULL)
		THROW_ERROR_OUTOFRESOURCES();
#endif
}

BasicSemaphore::~BasicSemaphore()
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
			sem_destroy(&m_hObject.m_PosixSemaphore);
#endif
	}
}

bool BasicSemaphore::Lock(uint32 p_Timeout)
{
#if defined(HY_PLATFORM_WINDOWS)
	DWORD dwRet = IGNORE_ABANDONED(::WaitForSingleObject(m_hObject, p_Timeout));
	if(dwRet == WAIT_OBJECT_0)
		return(true);
	return(false);
#elif defined(HY_PLATFORM_UNIX)
	if(m_hObject.m_SysVSemaphore.IsValid())
	{
		if(p_Timeout == INFINITE)
		{
			if(m_hObject.m_SysVSemaphore.Lock(true) != 0)
				THROW_LAST_UNIX_ERROR();
			return true;
		}
		else if(p_Timeout == 0)
		{
			// only try to lock non blocking
			if(m_hObject.m_SysVSemaphore.Lock(false) == 0)
				return(true);
			else if(InteropGetLastError() != EAGAIN)
				THROW_LAST_UNIX_ERROR();
			return(false);
		}
		else
		{
			timespec delay;
			uint32 timeout = 0;

			delay.tv_sec = 0;
			delay.tv_nsec = 10000000;  // wait for 10 milli sec
			while(timeout < p_Timeout)
			{
				//Call Wait for Zero with IPC_NOWAIT option,so it will be non blocking
				if(m_hObject.m_SysVSemaphore.Lock(false) < 0)
				{
					// check whether somebody else has the semaphore
					if(InteropGetLastError() == EAGAIN)
					{
						/* sleep for delay time */
						nanosleep(&delay, NULL);
						timeout+=10;
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
			int res;
			do
			{
				res = sem_wait(&m_hObject.m_PosixSemaphore);				
			}
			while(res == -1 && InteropGetLastError() == EINTR);
			if(res != 0)
				THROW_LAST_UNIX_ERROR();
			return true;
		}
		else if(p_Timeout == 0)
		{
			// only try to lock non blocking
			if(sem_trywait(&m_hObject.m_PosixSemaphore) == 0)
				return(true);
			else if(InteropGetLastError() != EAGAIN)
				THROW_LAST_UNIX_ERROR();
			return(false);
		}
		else
		{
			// only try to lock non blocking
			if(sem_trywait(&m_hObject.m_PosixSemaphore) == 0)
				return(true);
			else if(InteropGetLastError() != EAGAIN)
				THROW_LAST_UNIX_ERROR();

			// try to lock with timeout
			int rc = pthread_mutex_lock(GetTimeoutMutex());
			if(rc != 0)
				THROW_UNIX_ERROR(rc);

			// only try to lock non blocking (2nd time is necessary otherwise race condition is possible!)
			if(sem_trywait(&m_hObject.m_PosixSemaphore) == 0)
			{
				rc = pthread_mutex_unlock(GetTimeoutMutex());
				if(rc != 0)
					THROW_UNIX_ERROR(rc);
				return(true);
			}
			else if(InteropGetLastError() != EAGAIN)
				THROW_LAST_UNIX_ERROR();

			// get current time, add timeout and correct overflow
			timeval tp;
			timespec ts;
			if(gettimeofday(&tp, 0) != 0)
			{
				uint32 LastError = InteropGetLastError();
				pthread_mutex_unlock(GetTimeoutMutex());
				THROW_UNIX_ERROR(LastError);
			}
			ts.tv_sec = tp.tv_sec + (p_Timeout / 1000);
			ts.tv_nsec = (tp.tv_usec + ((p_Timeout % 1000) * 1000)) * 1000;
			if(ts.tv_nsec >= 1000000000)
			{
				ts.tv_sec++;
				ts.tv_nsec -= 1000000000;
			}

			// wait for timeout or change broadcast
			while(1)
			{
				// mutex will be unlocked, waiting for condition
				rc = pthread_cond_timedwait(GetTimeoutCond(), GetTimeoutMutex(), &ts);
				if(rc == 0 || rc == ETIMEDOUT)
				{
					// check for timeout
					bool bTimeout = (rc == ETIMEDOUT);

					// condition occured and mutex locked!
					// only try to lock non blocking
					if(sem_trywait(&m_hObject.m_PosixSemaphore) == 0)
					{
						rc = pthread_mutex_unlock(GetTimeoutMutex());
						if(rc != 0)
							THROW_UNIX_ERROR(rc);
						return(true);
					}
					else if(InteropGetLastError() != EAGAIN)
						THROW_LAST_UNIX_ERROR();
					else if(bTimeout)
					{
						// timeout occured, break
						rc = pthread_mutex_unlock(GetTimeoutMutex());
						if(rc != 0)
							THROW_UNIX_ERROR(rc);
						return(false);
					}
					// object can not be locked, goto wait state
				}
				else
				{
					pthread_mutex_unlock(GetTimeoutMutex());
					THROW_UNIX_ERROR(rc);
				}
			}
		}
	}
	return false;	// keeps gcc happy
#endif
}

void BasicSemaphore::Unlock()
{
#if defined(HY_PLATFORM_WINDOWS)
	::ReleaseSemaphore(m_hObject, 1, NULL);
#elif defined(HY_PLATFORM_UNIX)
	// lock global timeout mutex
	int rc = pthread_mutex_lock(GetTimeoutMutex());
	if(rc != 0)
		THROW_UNIX_ERROR(rc);

	if(m_hObject.m_SysVSemaphore.IsValid())
	{
		// release semaphore
		if(m_hObject.m_SysVSemaphore.Unlock() < 0)
		{
			pthread_mutex_unlock(GetTimeoutMutex());
			THROW_LAST_UNIX_ERROR();
		}
	}
	else
	{
		// release semaphore
		sem_post(&m_hObject.m_PosixSemaphore);
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



//*****************************************************************************
//** BasicMutex implementation
//*****************************************************************************

BasicMutex::BasicMutex() : m_bCloseHandle(true)
{
	m_WasOpened = false;
#if defined(HY_PLATFORM_WINDOWS)
	m_hObject = ::CreateMutex(NULL, FALSE, NULL);
	if(m_hObject == NULL)
		THROW_LAST_WIN_ERROR();
#elif defined(HY_PLATFORM_UNIX)
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
	int rc = pthread_mutex_init(&m_hObject.m_Mutex, &attr);
	if(rc != 0)
	{
		pthread_mutexattr_destroy(&attr);
		THROW_UNIX_ERROR(rc);
	}
	pthread_mutexattr_destroy(&attr);
#endif
}

BasicMutex::BasicMutex(bool p_bInitialOwner, PCTSTR p_Name) : m_bCloseHandle(true)
{
	m_WasOpened = false;
	if(p_Name == NULL)
	{
#if defined(HY_PLATFORM_WINDOWS)
		m_hObject = ::CreateMutex(NULL, p_bInitialOwner, NULL);
		if(m_hObject == NULL)
			THROW_LAST_WIN_ERROR();
#elif defined(HY_PLATFORM_UNIX)
		pthread_mutexattr_t attr;
		pthread_mutexattr_init(&attr);
		pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
		int rc = pthread_mutex_init(&m_hObject.m_Mutex, &attr);
		if(rc != 0)
		{
			pthread_mutexattr_destroy(&attr);
			THROW_UNIX_ERROR(rc);
		}
		pthread_mutexattr_destroy(&attr);
		if(p_bInitialOwner)
		{
			rc = pthread_mutex_lock(&m_hObject.m_Mutex);
			if(rc != 0)
				THROW_UNIX_ERROR(rc);
		}
#endif
	}
	else
	{
#if defined(HY_PLATFORM_WINDOWS)
		EmptySecurityAttributes EmtpyAttrib;
		m_hObject = ::CreateMutex(&EmtpyAttrib.m_SecAttr, p_bInitialOwner, p_Name);
		if(m_hObject == NULL)
			THROW_LAST_WIN_ERROR();
		else
			m_WasOpened = (GetLastError() == ERROR_ALREADY_EXISTS);
#elif defined(HY_PLATFORM_UNIX)
		m_hObject.m_SysVSemaphore.Create(p_Name, p_bInitialOwner ?  0 : 1, m_WasOpened);
#endif
	}
}

BasicMutex::BasicMutex(SyncHandleMutex p_hMutex, bool p_bCloseHandle) : m_bCloseHandle(p_bCloseHandle)
{
	m_WasOpened = false;
#if defined(HY_PLATFORM_WINDOWS)
	CHECK_EXPR(p_hMutex);
#endif
	m_hObject = p_hMutex;
#if defined(HY_PLATFORM_WINDOWS)
	if(m_hObject == NULL)
		THROW_ERROR_OUTOFRESOURCES();
#endif
}

BasicMutex::~BasicMutex()
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
			pthread_mutex_destroy(&m_hObject.m_Mutex);		
#endif
	}
}

bool BasicMutex::Lock(uint32 p_Timeout)
{
#if defined(HY_PLATFORM_WINDOWS)
	DWORD dwRet = IGNORE_ABANDONED(::WaitForSingleObject(m_hObject, p_Timeout));
	if(dwRet == WAIT_OBJECT_0)
		return(true);
	return(false);
#elif defined(HY_PLATFORM_UNIX)
	if(m_hObject.m_SysVSemaphore.IsValid())
	{
		if(p_Timeout == INFINITE)
		{
			if(m_hObject.m_SysVSemaphore.Lock(true) != 0)
				THROW_LAST_UNIX_ERROR();
			return true;
		}
		else if(p_Timeout == 0)
		{
			// only try to lock non blocking
			if(m_hObject.m_SysVSemaphore.Lock(false) == 0)
				return(true);
			else if(InteropGetLastError() != EAGAIN)
				THROW_LAST_UNIX_ERROR();
			return(false);
		}
		else
		{
			timespec delay;
			uint32 timeout = 0;

			delay.tv_sec = 0;
			delay.tv_nsec = 10000000;  // 10 milli sec
			while(timeout < p_Timeout)
			{
				//Call Wait for Zero with IPC_NOWAIT option,so it will be non blocking
				if(m_hObject.m_SysVSemaphore.Lock(false) < 0)
				{
					// check whether somebody else has the semaphore
					if(InteropGetLastError() == EAGAIN)
					{
						/* sleep for delay time */
						nanosleep(&delay, NULL);
						timeout+=10;
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
			int rc = pthread_mutex_lock(&m_hObject.m_Mutex);
			if(rc != 0)
				THROW_UNIX_ERROR(rc);
			return true;
		}
		else if(p_Timeout == 0)
		{
			// only try to lock non blocking
			int rc = pthread_mutex_trylock(&m_hObject.m_Mutex);
			if(rc == 0)
				return(true);
			else if(rc != EBUSY)
				THROW_UNIX_ERROR(rc);
			return(false);
		}
		else
		{
			// only try to lock non blocking
			int rc = pthread_mutex_trylock(&m_hObject.m_Mutex);
			if(rc == 0)
				return(true);
			else if(rc != EBUSY)
				THROW_UNIX_ERROR(rc);

			// try to lock with timeout
			rc = pthread_mutex_lock(GetTimeoutMutex());
			if(rc != 0)
				THROW_UNIX_ERROR(rc);

			// only try to lock non blocking (2nd time is necessary otherwise race condition is possible!)
			rc = pthread_mutex_trylock(&m_hObject.m_Mutex);
			if(rc == 0)
			{
				rc = pthread_mutex_unlock(GetTimeoutMutex());
				if(rc != 0)
					THROW_UNIX_ERROR(rc);
				return(true);
			}
			else if(rc != EBUSY)
				THROW_UNIX_ERROR(rc);

			// get current time, add timeout and correct overflow
			timeval tp;
			timespec ts;
			if(gettimeofday(&tp, 0) != 0)
			{
				uint32 LastError = InteropGetLastError();
				pthread_mutex_unlock(GetTimeoutMutex());
				THROW_UNIX_ERROR(LastError);
			}
			ts.tv_sec = tp.tv_sec + (p_Timeout / 1000);
			ts.tv_nsec = (tp.tv_usec + ((p_Timeout % 1000) * 1000)) * 1000;
			if(ts.tv_nsec >= 1000000000)
			{
				ts.tv_sec++;
				ts.tv_nsec -= 1000000000;
			}

			// wait for timeout or change broadcast
			while(1)
			{
				// mutex will be unlocked, waiting for condition
				rc = pthread_cond_timedwait(GetTimeoutCond(), GetTimeoutMutex(), &ts);
				if(rc == 0 || rc == ETIMEDOUT)
				{
					// check for timeout
					bool bTimeout = (rc == ETIMEDOUT);

					// condition occured and mutex locked!
					// only try to lock non blocking
					rc = pthread_mutex_trylock(&m_hObject.m_Mutex);
					if(rc == 0)
					{
						rc = pthread_mutex_unlock(GetTimeoutMutex());
						if(rc != 0)
							THROW_UNIX_ERROR(rc);
						return(true);
					}
					else if(rc != EBUSY)
						THROW_LAST_UNIX_ERROR();
					else if(bTimeout)
					{
						// timeout occured, break
						rc = pthread_mutex_unlock(GetTimeoutMutex());
						if(rc != 0)
							THROW_UNIX_ERROR(rc);
						return(false);
					}
					// object can not be locked, goto wait state
				}
				else
				{
					pthread_mutex_unlock(GetTimeoutMutex());
					THROW_UNIX_ERROR(rc);
				}
			}
		}
	}
	return false;	// keeps gcc happy
#endif
}

void BasicMutex::Unlock()
{
#if defined(HY_PLATFORM_WINDOWS)
	::ReleaseMutex(m_hObject);
#elif defined(HY_PLATFORM_UNIX)
	{
		// lock global timeout mutex
		int rc = pthread_mutex_lock(GetTimeoutMutex());
		if(rc != 0)
			THROW_UNIX_ERROR(rc);

		// unlock mutex
		if(m_hObject.m_SysVSemaphore.IsValid())
		{
			if(m_hObject.m_SysVSemaphore.Unlock() < 0)
			{
				pthread_mutex_unlock(GetTimeoutMutex());
				THROW_LAST_UNIX_ERROR();
			}
		}
		else
		{
			rc = pthread_mutex_unlock(&m_hObject.m_Mutex);
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
	}
#endif
}



//*****************************************************************************
//** BasicEvent implementation
//*****************************************************************************

BasicEvent::BasicEvent(bool p_bManualReset) : m_bCloseHandle(true)
{
	m_WasOpened = false;
#if defined(HY_PLATFORM_WINDOWS)
	m_hObject = ::CreateEvent(NULL, p_bManualReset, FALSE, NULL);
	if(!m_hObject)
		THROW_LAST_WIN_ERROR();
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
		if(!m_hObject)
			THROW_LAST_WIN_ERROR();
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
		tostringstream name;
		name << p_Name << (p_bManualReset ? _T("m") : _T("a"));

#if defined(HY_PLATFORM_WINDOWS)
		EmptySecurityAttributes EmtpyAttrib;
		m_hObject = ::CreateEvent(&EmtpyAttrib.m_SecAttr, p_bManualReset, p_bInitialState, name.str().c_str());
		if(!m_hObject)
			THROW_LAST_WIN_ERROR();
		else
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
	CHECK_EXPR(p_hEvent);
#endif
	m_hObject = p_hEvent;
#if defined(HY_PLATFORM_WINDOWS)
	if(!m_hObject)
		THROW_ERROR_OUTOFRESOURCES();
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
	if(SetEvent(m_hObject) == FALSE)
		THROW_ERROR_LASTERROR();
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
	if(ResetEvent(m_hObject) == FALSE)
		THROW_ERROR_LASTERROR();
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



//*****************************************************************************
//** BasicSection implementation
//*****************************************************************************

BasicSection::BasicSection()
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

BasicSection::~BasicSection()
{
#if defined(HY_PLATFORM_WINDOWS)
	::DeleteCriticalSection(&m_hObject);
#elif defined(HY_PLATFORM_UNIX)
	pthread_mutex_destroy(&m_hObject);
#endif
}

