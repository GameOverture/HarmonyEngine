/**************************************************************************
*	HyPlatform_Gui.h
*	
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyPlatform_Gui_h__
#define HyPlatform_Gui_h__

#define HY_MAXWINDOWS 1

// All x86 and x86-64 machines are little-endian.
#define HY_ENDIAN_LITTLE

#define HY_NEW new

// Diagnostics assertion
#if defined(HY_DEBUG)
	#define HyAssert(condition, message) do { if((#condition) == false) int i = 0/*DebugBreak()*/; } while (false)
	#define HyError(message) do { int i = 0/*DebugBreak()*/; } while (false)
#else
	#define HyAssert(condition, message) do { } while (false)
	#define HyError(message) do { } while (false)
#endif

#if (defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
	#define HY_PLATFORM_GUI_WIN
#endif

#endif /* HyPlatform_Gui_h__ */
