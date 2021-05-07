/**************************************************************************
 *	HyInteropAfx.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2015 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyInteropAfx_h__
#define HyInteropAfx_h__

#include "Afx/HyStdAfx.h"

// RENDERING ///////////////////////////////////////////////////////////////
#include "Renderer/OpenGL/HyOpenGL.h"
typedef HyOpenGL HyRendererInterop;

// WINDOWING ///////////////////////////////////////////////////////////////
#if defined(HY_USE_GLFW)
	typedef GLFWwindow *HyWindowInteropPtr;
#elif defined(HY_USE_SDL2)
	typedef SDL_Window *HyWindowInteropPtr;
#else
	typedef void *HyWindowInteropPtr;
#endif

// NETWORKING //////////////////////////////////////////////////////////////
#ifdef HY_USE_SDL2_NET
	struct _TCPsocket;
	typedef struct _TCPsocket *HyTcpSocket;
#else
	typedef int HyTcpSocket;
#endif

// DEBUG CONSOLE ///////////////////////////////////////////////////////////
#if defined(HY_PLATFORM_GUI)
	// TODO: Ideally send Harmony log output to the editor's output window
	#include "Diagnostics/Console/Interop/HyConsole_Std.h"
	typedef HyConsole_Std HyConsoleInterop;
#elif defined(HY_PLATFORM_WINDOWS)
	#include "Diagnostics/Console/Interop/HyConsole_Win.h"
	typedef HyConsole_Win HyConsoleInterop;
#else
	#include "Diagnostics/Console/Interop/HyConsole_Std.h"
	typedef HyConsole_Std HyConsoleInterop;
#endif

// SPINE RUNTIME ////////////////////////////////////////////////////////////
#ifdef HY_USE_SPINE
	#include "spine/spine.h"
#else
	namespace spine
	{
		typedef void Skeleton;
		typedef void Atlas;
		typedef void SkeletonData;
		typedef void AnimationStateData;
	}
#endif

#endif /* HyInteropAfx_h__ */
