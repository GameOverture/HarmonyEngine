/**************************************************************************
 *	HyPlatAfx.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2015 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyPlatAfx_h__
#define HyPlatAfx_h__

#if defined(_WIN64) || defined(__LP64__) || defined(__LP64)
	#define HY_PLATFORM_64BIT
#else
	#define HY_PLATFORM_32BIT
#endif

#ifdef HY_PLATFORM_GUI	// Runs on Qt framework
	#define HyAssert(condition, message) do { } while (false)
	#define HyError(message) do { } while (false)
#else
	#if (defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__))
		#define HY_PLATFORM_WINDOWS

		#include "GL/glew.h"
		#include "GL/gl.h"
		#include "GL/glext.h"

		#include "HyPlatform_Windows.h"
	#elif ((defined(__APPLE__) && defined(__MACH__)) || defined(__APPLE_CC__))
		#if ((defined(__APPLE__) && defined(__MACH__))
			#define HY_PLATFORM_OSX
			#include "HyPlatform_OSX.h"
		#endif
		#define HY_PLATFORM_APPLE
	#elif defined(__linux) || defined(__linux__) || defined(__gnu_linux__)
		#define HY_PLATFORM_LINUX
		#include "HyPlatform_Unix.h"
	#else
		#define HY_PLATFORM_UNKNOWN
	#endif

	// Include glfw after required platform headers
	#include <GLFW/glfw3.h>

#endif

#endif HyPlatAfx_h__
