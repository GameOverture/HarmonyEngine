//*****************************************************************************
// Description:  platform interop includes and definitions
//*****************************************************************************

#ifndef TREADINGFX_PLATFORMINTEROP_H
#define TREADINGFX_PLATFORMINTEROP_H

#include "Afx/HyStdAfx.h"

//*****************************************************************************
//** Platform and sub-platform (API) definitions
//*****************************************************************************

//// API definitions
//#if defined(_WIN32) || defined(_WIN64)
//	#define API_WINDOWS
//#elif defined(__APPLE_CC__)
//	#define HY_PLATFORM_OSX
//#elif defined(__linux) || defined(__linux__) || defined(__gnu_linux__)
//	#define API_LINUX
//#else
//	#define API_UNIX
//#endif


//// platform definitions
//#if !defined(HY_PLATFORM_WINDOWS) && !defined(HY_PLATFORM_UNIX)
//	#if defined(API_WINDOWS)
//		#define HY_PLATFORM_WINDOWS
//		#define PLATFORM_NATIVE
//	#elif defined(API_UNIX) || defined(API_LINUX) || defined(HY_PLATFORM_OSX)
//		#define HY_PLATFORM_UNIX
//		#define PLATFORM_NATIVE
//	#endif
//#endif

//*****************************************************************************
//** Target specific definitions
//*****************************************************************************

// 64 bit environment
#if !defined(API_32BIT) && !defined(API_64BIT)
	#if defined(_WIN64) || defined(__LP64__) || defined(__LP64)
		#define API_64BIT
	#else
		#define API_32BIT
	#endif
#endif

// character default encoding type
#if !defined(HY_UNICODE) && !defined(HY_ANSI)
	#if defined(_UNICODE)
		#define HY_UNICODE
	#else
		#define HY_ANSI
	#endif
#endif


//*****************************************************************************
//** Verify compile configuration settings
//*****************************************************************************

#if (defined(API_32BIT) && defined(API_64BIT)) || (!defined(API_32BIT) && !defined(API_64BIT))
	#error ThreadingFx: Invalid Architecture definition.
#endif

#if (defined(HY_UNICODE) && defined(HY_ANSI)) || (!defined(HY_UNICODE) && !defined(HY_ANSI))
	#error ThreadingFx: Invalid Character Encoding definition.
#endif

//*****************************************************************************
//** include platform specific definitions
//*****************************************************************************

// library base macros
#include "BaseMacro.h"

// define primitive types
//#include "PrimitiveTypeDefs.h"

// win32 native api
#if defined(HY_PLATFORM_WINDOWS) || defined(HY_PLATFORM_GUI)
#include "InteropWin.h"

// unix api
#elif defined(HY_PLATFORM_UNIX)
#include "InteropUnix.h"

#endif

// error handling/exceptions
#include "Error.h"

#endif // #ifndef TREADINGFX_PLATFORMINTEROP_H
