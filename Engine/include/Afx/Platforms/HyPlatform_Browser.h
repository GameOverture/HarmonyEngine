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
#include <emscripten/fetch.h>

#ifdef HY_USE_SDL2
	#include <SDL2/SDL.h>
	#include <SDL2/SDL_opengl.h>
	#include <SDL2/SDL_opengl_glext.h>
#endif

#include <GLES3/gl3.h>
//#include <GL/gl.h>
//#include <GL/glext.h>

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

EM_BOOL OnHtmlResizeCallback(int iEventType, const EmscriptenUiEvent *pUiEvent, void *pUserData);
EM_BOOL OnHtmlFocusCallback(int iEventType, const EmscriptenFocusEvent *pFocusEvent, void *pUserData);
EM_BOOL OnHtmlOrientationChangeCallback(int iEventType, const EmscriptenOrientationChangeEvent *pOrientationChangeEvent, void *pUserData);
EM_BOOL OnHtmlTouchCallback(int iEventType, const EmscriptenTouchEvent *pTouchEvent, void *pUserData);

#endif /* HyPlatform_Browser_h__ */
