/**************************************************************************
 *	HyPlatform_Windows.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2012 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyPlatform_Windows_h__
#define HyPlatform_Windows_h__

#define WIN32_LEAN_AND_MEAN 
#include <Windows.h>
#include <Psapi.h>							// Used in HyDiagnostics for memory leak detection

#define HY_MAXWINDOWS 6

// All x86 and x86-64 machines are little-endian.
#define HY_ENDIAN_LITTLE

// This undefines the macros MIN and MAX which are specified in the windows headers. Use the stl versions instead.
#define NOMINMAX

#ifdef HY_DEBUG
	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <crtdbg.h>

	#define HY_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)	// Replace _NORMAL_BLOCK with _CLIENT_BLOCK if you want the allocations to be of _CLIENT_BLOCK type

	#define  HY_SET_CRT_DEBUG_FIELD(a)		_CrtSetDbgFlag((a) | _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG))
	#define  HY_CLEAR_CRT_DEBUG_FIELD(a)	_CrtSetDbgFlag(~(a) & _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG))
#else
	#define HY_NEW new
#endif

// Diagnostics assertion
#if defined(HY_DEBUG)
	#define HyAssert(condition, message) \
		do { \
			if(!(condition)) \
			{	std::stringstream ss; \
				ss << "Assertion (" #condition ") failed.\n\n" << __FILE__ \
				<< "\nLine: " << __LINE__ << "\n\n" << message << std::endl; \
				int iRetVal = MessageBoxA(NULL, ss.str().c_str(), "Harmony Engine Assert!", MB_ICONERROR | MB_ABORTRETRYIGNORE | MB_DEFBUTTON2); \
				switch(iRetVal) { \
				case IDABORT: std::exit(EXIT_FAILURE); \
				case IDRETRY: DebugBreak(); break; \
				case IDIGNORE: break; \
				} \
			} \
		} while (false)

	#define HyError(message) \
		do { \
			std::stringstream ss; \
			ss << "Harmony Error has occured.\n\n" << __FILE__ \
			<< "\nLine: " << __LINE__ << "\n\n" << message << std::endl; \
			MessageBoxA(NULL, ss.str().c_str(), "Harmony Engine Error!", MB_ICONERROR); \
			DebugBreak();/*std::exit(EXIT_FAILURE);*/ \
		} while (false)
#else
	#define HyAssert(condition, message) do { } while (false)
	#define HyError(message) do { } while (false)
#endif

#endif /* HyPlatform_Windows_h__ */
