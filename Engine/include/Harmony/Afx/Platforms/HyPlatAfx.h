/**************************************************************************
 *	HyPlatAfx.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2015 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyPlatAfx_h__
#define HyPlatAfx_h__

#if defined(_WIN64) || defined(__LP64__) || defined(__LP64)
	#define HY_PLATFORM_64BIT
#else
	#define HY_PLATFORM_32BIT
#endif

#if defined(HY_CONFIG_BROWSER)
	#define HY_CONFIG_SINGLETHREAD
	#define HY_PLATFORM_BROWSER
	#include "HyPlatform_Unix.h"
#elif defined(HY_CONFIG_GUI)
	#define HY_PLATFORM_GUI
	#include "HyPlatform_Gui.h"
#elif (defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
	#define HY_PLATFORM_WINDOWS
	#include "HyPlatform_Windows.h"
#elif ((defined(__APPLE__) && defined(__MACH__)) || defined(__APPLE_CC__))
	#define HY_PLATFORM_OSX
	#include "HyPlatform_OSX.h"
#elif defined(__linux) || defined(__linux__) || defined(__gnu_linux__)
	#define HY_PLATFORM_LINUX
	#include "HyPlatform_Unix.h"
#else
	#define HY_PLATFORM_UNKNOWN
#endif

#endif /* HyPlatAfx_h__ */
