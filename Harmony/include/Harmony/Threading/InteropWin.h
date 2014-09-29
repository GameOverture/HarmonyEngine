//*****************************************************************************
// Description:  interop defs for win32
//*****************************************************************************

#ifndef TREADINGFX_INTEROPWIN_H
#define TREADINGFX_INTEROPWIN_H

//*****************************************************************************
//** platform includes
//*****************************************************************************

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.

//#if defined(API_64BIT)
//	// Supporting >= WinXP
//	#ifndef WINVER								// Allow use of features specific to Windows 95 and Windows NT 4 or later.
//	#define WINVER 0x0501					// Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
//	#endif
//
//	#ifndef _WIN32_WINNT					// Allow use of features specific to Windows 2000 or later.
//	#define _WIN32_WINNT 0x0600		// Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
//	#endif						
//
//	#ifndef _WIN32_WINDOWS				// Allow use of features specific to Windows 98 or later.
//	#define _WIN32_WINDOWS 0x0500 // Change this to the appropriate value to target Windows Me or later.
//	#endif
//
//	#ifndef _WIN32_IE							// Allow use of features specific to IE 5.0 or later.
//	#define _WIN32_IE 0x0500			// Change this to the appropriate value to target IE 5.0 or later.
//	#endif
//#else
//	// Supporting >= Win2000
//	#ifndef WINVER								// Allow use of features specific to Windows 95 and Windows NT 4 or later.
//	#define WINVER 0x0500					// Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
//	#endif
//
//	#ifndef _WIN32_WINNT					// Allow use of features specific to Windows 2000 or later.
//	#define _WIN32_WINNT 0x0600		// Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
//	#endif						
//
//	#ifndef _WIN32_WINDOWS				// Allow use of features specific to Windows 98 or later.
//	#define _WIN32_WINDOWS 0x0500 // Change this to the appropriate value to target Windows Me or later.
//	#endif
//
//	#ifndef _WIN32_IE							// Allow use of features specific to IE 5.0 or later.
//	#define _WIN32_IE 0x0500			// Change this to the appropriate value to target IE 5.0 or later.
//	#endif
//#endif
//
//#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
//
//// Windows Header Files:
//#include <windows.h>

// C RunTime Header Files
#include <tchar.h>


// SC++L Header Files
#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <queue>
#include <locale>
#include <sstream>

//*****************************************************************************
//** disable warnings
//*****************************************************************************

//#pragma warning(disable:4503) // warning C4503: decorated name length exceeded, name was truncated



//*****************************************************************************
//** common definitions
//*****************************************************************************

// assert for debugging
#define ASSERT_SYSTEM(expr) _ASSERT(expr)


// simple datatypes

typedef HANDLE FileHandle;
typedef HWND WindowHandle;
typedef HANDLE ThreadHandle;
typedef HMODULE ModuleHandle;
typedef LPCVOID SystemAddress;

typedef HANDLE SyncHandle;
typedef HANDLE SyncHandleSemaphore;
typedef HANDLE SyncHandleMutex;
typedef CRITICAL_SECTION SyncHandleSection;
typedef HANDLE SyncHandleEvent;



// helper structure used for win2k system account, filled with all access security properties
struct EmptySecurityAttributes
{
	SECURITY_ATTRIBUTES m_SecAttr;	// security attributes structure
	SECURITY_DESCRIPTOR m_SecDesc;	// security descriptor structure

	EmptySecurityAttributes()
	{
		m_SecAttr.nLength = sizeof(m_SecAttr);
		m_SecAttr.bInheritHandle = TRUE;
		m_SecAttr.lpSecurityDescriptor = (PSECURITY_DESCRIPTOR)&m_SecDesc;
		::InitializeSecurityDescriptor(m_SecAttr.lpSecurityDescriptor, SECURITY_DESCRIPTOR_REVISION);
		::SetSecurityDescriptorDacl(m_SecAttr.lpSecurityDescriptor, TRUE, (PACL)NULL, FALSE);
	}
};


// char types
typedef char ACHAR;
//typedef wchar_t WCHAR;

// string types
typedef char* PASTR;
typedef const char* PCASTR;

// unicode definitions
#ifdef _UNICODE
	typedef std::wstring tstring;
	typedef std::wostringstream tostringstream; 
#else
	typedef std::string tstring;
	typedef std::ostringstream tostringstream;
#endif


//*****************************************************************************
//** interop definitions
//*****************************************************************************

#define InteropExit()						::exit(1);
#define InteropTerminateProcess()			::TerminateProcess(GetCurrentProcess(), 1);
#define InteropSleep(_ms)					::Sleep(_ms)
#define InteropGetCurrentThreadId()			((uint32)::GetCurrentThreadId())
#define InteropGetLastError()				((uint32)::GetLastError())
#define InteropGetTickCount()				((uint32)::GetTickCount())



// 32 bit interlocked increment, return count after operation
#define InteropInterlockedIncrement32(_param)	((int32)InterlockedIncrement((volatile LONG*)&_param))

// 32 bit interlocked decrement, return count after operation
#define InteropInterlockedDecrement32(_param)	((int32)InterlockedDecrement((volatile LONG*)&_param))

// 64 bit interlocked increment, return count after operation
#define InteropInterlockedIncrement64(_param)	((int64)InterlockedIncrement64((volatile LONGLONG*)&_param))

// 64 bit interlocked decrement, return count after operation
#define InteropInterlockedDecrement64(_param)	((int64)InterlockedDecrement64((volatile LONGLONG*)&_param))

#if defined(API_32BIT)
	#define InteropInterlockedIncrement(_param) InteropInterlockedIncrement32(_param)
	#define InteropInterlockedDecrement(_param) InteropInterlockedDecrement32(_param)
#else
	#define InteropInterlockedIncrement(_param) InteropInterlockedIncrement64(_param)
	#define InteropInterlockedDecrement(_param) InteropInterlockedDecrement64(_param)
#endif


#endif // #ifndef TREADINGFX_INTEROPWIN_H

