/**************************************************************************
 *	HyPlatform_OSX.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2012 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyPlatform_OSX_h__
#define HyPlatform_OSX_h__

#include <glad/glad.h>
#define HY_USE_GLAD

#ifdef HY_USE_SDL2
	#include <SDL.h>
#endif

#define HY_ENDIAN_LITTLE // All x86 and x86-64 machines are little-endian.

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

#endif /* HyPlatform_OSX_h__ */
