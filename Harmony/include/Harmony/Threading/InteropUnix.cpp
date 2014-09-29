#include "PlatformInterop.h"

#if defined(HY_PLATFORM_UNIX)

#include "InteropUnix.h"

namespace UnixInternal {


void UnixSleep(uint32 p_Milliseconds)
{
	timespec SleepTime, RemainingTime;
	SleepTime.tv_sec = p_Milliseconds / 1000;
	SleepTime.tv_nsec = (p_Milliseconds % 1000) * 1000000;
	while(nanosleep(&SleepTime, &RemainingTime) == -1)
	{
		if(InteropGetLastError() != EINTR)
			THROW_LAST_UNIX_ERROR();
		SleepTime = RemainingTime;
	}
}

uint32 UnixGetTickCount()
{
#if defined(USE_UNIX_LIBRT)
	timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return(ts.tv_sec * 1000) + (ts.tv_nsec / 1000000);
#else
	static int tickPerSec = (int)sysconf(_SC_CLK_TCK);
	tms t;
	if(tickPerSec > 1000)
		return times(&t) / (tickPerSec / 1000);
	else
		return times(&t) * (1000 / tickPerSec);
#endif
}

void UnixDebugBreak()
{
	asm("int $3");
}

uint32 UnixGetCurrentThreadId()
{
#if defined(HY_PLATFORM_OSX)
	// on Mac platforms we use the non-portable pthreads function pthread_threadid_np
	uint64 tid;
	if(pthread_threadid_np(pthread_self(), &tid) < 0)
	{
		// if the function fails we just use the thread_t value of pthread_self
		tid = (uint64)(size_t)pthread_self();
	}
	return (uint32)tid; 
#elif defined(API_LINUX)
	// try to get TID
	int tid = syscall(SYS_gettid);
	if(tid < 0)
	{
		// If gettid fails (which almost guarantees that the return value is ENOSYS), we call 'getpid'. 
		// On LinuxThreads, each thread has a unique process ID that can serve as a thread identifier.
		tid = syscall(SYS_getpid);
		if(tid < 0)
		{
			// if getpid also fails (very very unlikely) we just use the thread_t value of pthread_self
			tid = (int)(size_t)pthread_self();
		}
	}
	return (uint32)tid;
#else
	UnixDebugBreak();
#endif
}

struct sembuf SysVSemaphore::m_OpBeginCreate[2] = 
{
	{ 2, 0, 0 },					// wait for [2] (lock) to equal 0
	{ 2, 1, DEFAULT_SEM_FLAGS }		// then increment [2] to 1 - this locks it
};

struct sembuf SysVSemaphore::m_OpEndCreate[2] = 
{
	{ 1, -1, DEFAULT_SEM_FLAGS },	// decrement [1] (object ref counter) with undo on exit
	{ 2, -1, DEFAULT_SEM_FLAGS }	// then decrement [2] (lock) back to 0
};

struct sembuf SysVSemaphore::m_OpBeginClose[3] = 
{
	{ 2, 0, 0 },				  // wait for [2] (lock) to equal to 0
	{ 2, 1, DEFAULT_SEM_FLAGS },  // then increment [2] to 1 - this locks it
	{ 1, 1, DEFAULT_SEM_FLAGS }   // then increment [1] (object ref counter)
};

struct sembuf SysVSemaphore::m_OpEndClose[1] = 
{
	{ 2, -1, DEFAULT_SEM_FLAGS }  // decrement [2] (lock) back to 0
};

struct sembuf SysVSemaphore::m_OpLock[1] = 
{
	{ 0, -1, DEFAULT_SEM_FLAGS }  // decrement [0] (lock) back to 0
};

struct sembuf SysVSemaphore::m_OpLockNonBlocking[1] = 
{
	{ 0, -1, DEFAULT_SEM_FLAGS | IPC_NOWAIT } // decrement [0] (lock) back to 0 without blocking the thread 
};

struct sembuf SysVSemaphore::m_OpUnlock[1] = 
{
	{ 0, 1, DEFAULT_SEM_FLAGS }  // increment [0] (unlock)
};


SysVSemaphore::SysVSemaphore() : m_Semaphore(-1)
{
}

void SysVSemaphore::Create(uint32 p_InitialValue)
{
	m_IsPrivate = true;

	// Create the semaphore.
	// We use only 1 private semaphore for each created semaphore object which represents the actual semaphore value
	if((m_Semaphore = semget(IPC_PRIVATE, 1, IPC_CREAT | S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)) == -1)
		THROW_LAST_UNIX_ERROR();

	// initialize semaphore value
	if(semctl(m_Semaphore, 0, SETVAL, p_InitialValue) < 0)
		THROW_LAST_UNIX_ERROR();
}

void SysVSemaphore::Create(LPCTSTR p_Name, uint32 p_InitialValue, bool& p_WasOpened)
{
	m_IsPrivate = false;

	// convert the name into a hash value we can use as the semaphore key
	uint32 hash = Hash32(p_Name);
	if(hash == IPC_PRIVATE)
		hash = Hash32(_T("EmptyName"));

	bool createSuccessfuly = false;	
	do
	{
		// Create the semaphore.
		// We use a set of 3 semaphores for each created semaphore object:
		//  - Semaphore 0 is the actual semaphore value
		//  - Semaphore 1 counts the number of objects associated with the semaphore set
		//  - Semaphore 2 synchronizes creation/deletion of the semaphore object
		if((m_Semaphore = semget(hash, 3, IPC_CREAT | S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)) == -1)
			THROW_LAST_UNIX_ERROR();

		// lock the newly created semaphore set
		// There is a little race condition here. The semaphore set could have been deleted between the previous semget
		// call and this semop call. In case we get EINVAL or EIDRM back from semop we will try to recreate the semaphore set again
		// to handle this scenario.
		if(semop(m_Semaphore, m_OpBeginCreate, sizeof(m_OpBeginCreate) / sizeof(sembuf)) < 0)
		{
			if(errno != EINVAL && errno != EIDRM)
				THROW_LAST_UNIX_ERROR();
		}
		else
			createSuccessfuly = true;
	}
	while(!createSuccessfuly);

	// We now query the value of semaphore 2. If it's still 0 no object has yet initialized the semaphore set.
	int semval;
	if((semval = semctl(m_Semaphore, 1, GETVAL, 0)) < 0)
		THROW_LAST_UNIX_ERROR();
	if(semval == 0)
	{
		// initialize semaphore value
		if(semctl(m_Semaphore, 0, SETVAL, p_InitialValue) < 0)
			THROW_LAST_UNIX_ERROR();

		// init semaphore object references counter
		if(semctl(m_Semaphore, 1, SETVAL, m_MaxSemObjectRefs) < 0)
			THROW_LAST_UNIX_ERROR();

		p_WasOpened = false;
	}
	else
		p_WasOpened = true;

	// End semaphore creation:
	//  - Decrement object reference counter semaphore
	//  - Release creation/deletion lock
	if(semop(m_Semaphore, m_OpEndCreate, sizeof(m_OpEndCreate) / sizeof(sembuf)) < 0)
		THROW_LAST_UNIX_ERROR();
}

void SysVSemaphore::Destroy()
{
	// if the semaphore is private we just delete it
	if(m_IsPrivate)
	{
		// remove the semaphore
		if(semctl(m_Semaphore, 0, IPC_RMID) < 0)
		{
			int err = errno;
			//TRACE(_T("Failed to perform delete operation for SysV semaphore (errno: ") << err << _T(")"));
		}
		return;
	}

	if(semop(m_Semaphore, m_OpBeginClose, sizeof(m_OpBeginClose) / sizeof(sembuf)) < 0)
	{
		int err = errno;
		//TRACE(_T("Failed to perform close begin operation for SysV semaphore (errno: ") << err << _T(")"));
		return;
	}

	// read object reference counter
	int semval;
	if((semval = semctl(m_Semaphore, 1, GETVAL, 0)) < 0)
	{
		int err = errno;
		//TRACE(_T("Failed to GETVAL for object reference semaphore (errno: ") << err << _T(")"));
		return;
	}

	// If we reached the maximum object refs value we can destroy the semaphore set as the last reference
	// is just released.
	if(semval == m_MaxSemObjectRefs)
	{
		// remove the semaphore
		if(semctl(m_Semaphore, 0, IPC_RMID) < 0)
		{
			int err = errno;
			//TRACE(_T("Failed to perform delete operation for SysV semaphore (errno: ") << err << _T(")"));
		}
	}
	else if(semval < m_MaxSemObjectRefs)
	{
		// free create/delete lock on the semaphore set
		if(semop(m_Semaphore, m_OpEndClose, sizeof(m_OpEndClose) / sizeof(sembuf)) < 0)
		{
			int err = errno;
			//TRACE(_T("Failed to perform close end operation for SysV semaphore (errno: ") << err << _T(")"));
		}
	}
	//else
	//	TRACE(_T("Invalid object reference value: ") << semval);
}

int SysVSemaphore::DoOp(sembuf* p_Ops, uint32 p_NumOps)
{
	return semop(m_Semaphore, p_Ops, p_NumOps);
}

int SysVSemaphore::Lock(bool p_Blocking)
{
	if(p_Blocking)
		return semop(m_Semaphore, m_OpLock, 1);
	else
		return semop(m_Semaphore, m_OpLockNonBlocking, 1);
}

int SysVSemaphore::Unlock()
{
	return semop(m_Semaphore, m_OpUnlock, 1);
}

} // namespace UnixInternal {

#endif
