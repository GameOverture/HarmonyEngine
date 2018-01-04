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

// Data types
typedef signed char			char8;
typedef unsigned char		uchar8;
typedef signed char			int8;
typedef unsigned char		uint8;
typedef signed short		int16;
typedef unsigned short		uint16;
typedef signed int			int32;
typedef unsigned int		uint32;
typedef __int64				int64;
typedef unsigned __int64	uint64;

#define HY_MAXWINDOWS 1

// All x86 and x86-64 machines are little-endian.
#define HY_ENDIAN_LITTLE

// Diagnostics assertion
#if defined(HY_DEBUG)
	#define HyAssert(condition, message) do { if((#condition) == false) DebugBreak(); } while (false)
	#define HyError(message) do { DebugBreak(); } while (false)
#else
	#define HyAssert(condition, message) do { } while (false)
	#define HyError(message) do { } while (false)
#endif

#endif /* HyPlatform_Gui_h__ */
