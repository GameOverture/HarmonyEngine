/**************************************************************************
 *	HyPlatform_Browser.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2020 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyPlatform_Browser_h__
#define HyPlatform_Browser_h__

#include <emscripten.h>
#include <emscripten/html5.h>

#define HY_USE_SDL2
#include <SDL2/SDL.h>

#include <GLES3/gl3.h>

//#define HY_USE_GLEW

#define HY_MAXWINDOWS 1
#define HY_ENDIAN_LITTLE

#define HY_NEW new

// Diagnostics assertion
#if defined(HY_DEBUG)
	#define HyAssert(condition, message) \
		do { \
			if(!(condition)) { \
				std::cerr << "Assertion `" #condition "` failed in " << __FILE__ \
				<< " line " << __LINE__ << ": " << message << std::endl; \
				std::terminate(); \
			} \
		} while (false)
	#define HyError(message) abort();
#else
	#define HyAssert(condition, message) do { } while (false)
	#define HyError(message) do { } while (false)
#endif


#endif /* HyPlatform_Browser_h__ */
