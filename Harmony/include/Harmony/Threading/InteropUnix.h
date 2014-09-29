//*****************************************************************************
// Description:  interop defs for unix
//*****************************************************************************

#ifndef TREADINGFX_INTEROPUNIX_H
#define TREADINGFX_INTEROPUNIX_H


//*****************************************************************************
//** platform includes
//*****************************************************************************

// If defined enables high resolution real-time clock functions.
// Otherwise low-resolution standard system timers are used to
// provide similar functionallity.
// Must link librt.a library to enable real-time clock.
//#define USE_UNIX_LIBRT

// define to use extended mutex functions
#ifndef _GNU_SOURCE
	#define _GNU_SOURCE
#endif

// C RunTime Header Files
#include <stdio.h>
#include <wchar.h>
#include <errno.h>
#include <assert.h>
#include <stdint.h>
#if defined(API_LINUX)
#include <sys/syscall.h>
#endif

// SC++L Header Files
#include <string>
#include <tr1/memory>
#include <vector>
#include <queue>
#include <locale>
#include <sstream>
#include <algorithm>

// Unix platform includes
#include <pthread.h>
#include <semaphore.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <sys/sem.h>
#include <fcntl.h>


#if !defined(USE_UNIX_LIBRT)
#include <sys/times.h>
#endif


//*****************************************************************************
//** common definitions
//*****************************************************************************

// assert for debugging
#define ASSERT_SYSTEM(expr) assert(expr)

// timeout for thread synchronization
#define INFINITE (0xFFFFFFFF)

#define WAIT_OBJECT_0 (0)
#define WAIT_TIMEOUT (258)
#define WAIT_FAILED (-1)
#define MAXIMUM_WAIT_OBJECTS (64)

// Define default semaphore flags
// Usually this is SEM_UNDO, except for Mac where we don't use SEM_UNDO to avoid
// the limit of 10 semaphores that are supported, using the SEM_UNDO flag on this platform.
#if defined(API_LINUX)
#define DEFAULT_SEM_FLAGS SEM_UNDO
#elif defined(HY_PLATFORM_OSX)
#define DEFAULT_SEM_FLAGS 0
#else
#define DEFAULT_SEM_FLAGS SEM_UNDO
#endif


// simple datatypes
typedef unsigned int DWORD;
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long long DWORD64;

typedef pthread_t ThreadHandle;

typedef int SyncHandle;
typedef pthread_mutex_t SyncHandleSection;

#define PVOID void*

#define TRUE	(1)
#define FALSE	(0)

#define UNREFERENCED_PARAMETER(P)
#define MAX_PATH 300

#ifndef STRUNCATE
#define STRUNCATE       80
#endif

// unicode definitions
#ifdef _UNICODE
	#define _T(x)				__WT(x)
	#define _TEXT(x)    __WT(x)
	typedef std::wstring tstring;
	typedef std::wostringstream tostringstream; 
#else
	#define _T(x)				__AT(x)
	#define _TEXT(x)    __AT(x)
	typedef std::string tstring;
	typedef std::ostringstream tostringstream;
#endif

// char types
typedef char ACHAR;
typedef wchar_t WCHAR;

// string types
typedef char* PSTR;
typedef const char* PCSTR;
typedef char* PASTR;
typedef const char* PCASTR;
typedef wchar_t* PWSTR;
typedef const wchar_t* PCWSTR;

#ifdef _UNICODE
	typedef WCHAR TCHAR;
	typedef PWSTR PTSTR;
	typedef PCWSTR PCTSTR;
	typedef PCWSTR LPCWSTR;
#else
	typedef ACHAR TCHAR;
	typedef PASTR PTSTR;
	typedef PCASTR PCTSTR;
	typedef PCASTR LPCASTR;
#endif

typedef PTSTR LPTSTR;
typedef PCTSTR LPCTSTR;

typedef PSTR LPSTR;
typedef PCSTR LPCSTR;

typedef PASTR LPASTR;
typedef PCASTR LPCASTR;

typedef PWSTR LPWSTR;
typedef PCWSTR LPCWSTR;



//*****************************************************************************
//** interop definitions
//*****************************************************************************

#define InteropExit()						exit(1);
#define InteropTerminateProcess()			kill(getpid(), SIGKILL);
#define InteropSleep(_ms)					::UnixInternal::UnixSleep(_ms)
#define InteropGetCurrentThreadId()			::UnixInternal::UnixGetCurrentThreadId()
#define InteropGetLastError()				((uint32)errno)
#define InteropGetTickCount()				((uint32)::UnixInternal::UnixGetTickCount())


// we use GCC built in atomic memory access functions here ...
// 32 bit interlocked increment, return count after operation
#define InteropInterlockedIncrement32(_param)	((int32)__sync_fetch_and_add((volatile int32*)&_param, 1))

// 32 bit interlocked decrement, return count after operation
#define InteropInterlockedDecrement32(_param)	((int32)__sync_fetch_and_sub((volatile int32*)&_param, 1))

// 64 bit interlocked increment, return count after operation
#define InteropInterlockedIncrement64(_param)	((int64)__sync_fetch_and_add((volatile int64*)&_param, 1))

// 64 bit interlocked decrement, return count after operation
#define InteropInterlockedDecrement64(_param)	((int64)__sync_fetch_and_sub((volatile int64*)&_param, 1))


#if defined(API_32BIT)
	#define InteropInterlockedIncrement(_param) InteropInterlockedIncrement32(_param)
	#define InteropInterlockedDecrement(_param) InteropInterlockedDecrement32(_param)
#else
	#define InteropInterlockedIncrement(_param) InteropInterlockedIncrement64(_param)
	#define InteropInterlockedDecrement(_param) InteropInterlockedDecrement64(_param)
#endif

//*****************************************************************************
//** Unix interop functions
//*****************************************************************************

namespace UnixInternal
{
void UnixSleep(uint32 p_Milliseconds);
uint32 UnixGetTickCount();
void UnixDebugBreak();
uint32 UnixGetCurrentThreadId();

class SysVSemaphore
{
	private:
		// semaphore identifier
		int m_Semaphore;

		// flag indicating whether the semaphore is private or not
		bool m_IsPrivate;

	public:
		SysVSemaphore();

		// creates a private semaphore
		void Create(uint32 p_InitialValue);

		// creates a named semaphore
		void Create(LPCTSTR p_Name, uint32 p_InitialValue, bool& p_WasOpened);

		// frees the semaphore
		void Destroy();

		// performs the given operations on the semaphore
		int DoOp(sembuf* p_Ops, uint32 p_NumOps);

		// Locks the semaphore. If blocking is true and the lock can't be acquired the function will only
		// return after it is able to acquire the lock.
		int Lock(bool p_Blocking);

		// unlocks the semaphore
		int Unlock();

		// returns true if the semaphore is valid and initialized
		bool IsValid() { return (m_Semaphore != -1); }

	private:
		// helper method for creating named semaphore ids
		template<class CharType> uint32 Hash32(const CharType* p_Str) const;

		// maximum number of object references to a single semaphore set (used to keep track of objects referencing a certain semaphore set)
		static const int m_MaxSemObjectRefs = 10000;

		// declaration of the various semaphore operations
		static struct sembuf m_OpBeginCreate[2];
		static struct sembuf m_OpEndCreate[2];
		static struct sembuf m_OpBeginClose[3];
		static struct sembuf m_OpEndClose[1];
		static struct sembuf m_OpLock[1];
		static struct sembuf m_OpLockNonBlocking[1];
		static struct sembuf m_OpUnlock[1];
};

template<class CharType>
uint32 SysVSemaphore::Hash32(const CharType* p_Str) const
{
	uint32 nHash = 0;
	uint32 iChar = 0;
	while(p_Str[iChar])
	{
		nHash = (nHash << 5) + nHash + (uint32)p_Str[iChar];
		iChar++;
	}
	return(nHash);
}

}

struct SyncHandleEvent
{
	UnixInternal::SysVSemaphore m_SysVSemaphore;
	pthread_mutex_t m_hMutex;
	pthread_cond_t m_hCond;

	inline operator bool () { return(true); }
	inline SyncHandleEvent& operator = (const SyncHandleEvent &p_Value) { m_SysVSemaphore = p_Value.m_SysVSemaphore; m_hMutex = p_Value.m_hMutex; m_hCond = p_Value.m_hCond; return (*this); }
};

struct SyncHandleSemaphore
{
	UnixInternal::SysVSemaphore m_SysVSemaphore;
	sem_t m_PosixSemaphore;

	inline SyncHandleSemaphore& operator=(const SyncHandleSemaphore &p_Value) { m_SysVSemaphore = p_Value.m_SysVSemaphore; m_PosixSemaphore = p_Value.m_PosixSemaphore; return (*this); }
};

struct SyncHandleMutex
{
	UnixInternal::SysVSemaphore m_SysVSemaphore;
	pthread_mutex_t m_Mutex;

	inline SyncHandleMutex& operator=(const SyncHandleMutex &p_Value) { m_SysVSemaphore = p_Value.m_SysVSemaphore; m_Mutex = p_Value.m_Mutex; return (*this); }
};

#endif // #ifndef TREADINGFX_INTEROPUNIX_H

