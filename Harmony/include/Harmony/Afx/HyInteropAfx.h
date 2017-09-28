/**************************************************************************
 *	HyInteropAfx.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2015 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyInteropAfx_h__
#define HyInteropAfx_h__

#include "Afx/HyStdAfx.h"
#include "Afx/Platforms/HyPlatAfx.h"

#if defined(HY_PLATFORM_GUI)
	#include "Renderer/OpenGL/HyOpenGL.h"
	typedef HyOpenGL HyRendererInterop;

	#include "Diagnostics/Console/Interop/HyConsole_Gui.h"
	typedef HyConsole_Gui HyConsoleInterop;

	#include "Renderer/OpenGL/HyOpenGLShader.h"
	typedef HyOpenGLShader HyShaderInterop;

	typedef void *HyWindowHandle;

	#include "Time/Interop/HyTime_Win.h"
	typedef HyTime_Win HyTimeInterop;
	#include "Audio/Interop/HyAudio_Win.h"
	typedef HyAudio_Win HyAudioInterop;
	typedef IHyAudioInst_Win IHyAudioInstInterop;
	#include "Memory/Interop/HyMemAPI_Windows.h"
	typedef HyMemoryAPI_Windows HyMemoryInterop;
#else
	#if defined(HY_PLATFORM_DESKTOP)
		typedef GLFWwindow *HyWindowHandle;
		#include "Renderer/OpenGL/HyOpenGL.h"
		typedef HyOpenGL HyRendererInterop;
	#endif

	#if defined(HY_PLATFORM_WINDOWS)
		#include "Diagnostics/Console/Interop/HyConsole_Win.h"
		typedef HyConsole_Win HyConsoleInterop;

		#include "Renderer/OpenGL/HyOpenGLShader.h"
		typedef HyOpenGLShader HyShaderInterop;

		#include "Audio/Interop/HyAudio_Win.h"
		typedef HyAudio_Win HyAudioInterop;
		typedef IHyAudioInst_Win IHyAudioInstInterop;

		#include "Time/Interop/HyTime_Win.h"
		typedef HyTime_Win HyTimeInterop;

		#include "Memory/Interop/HyMemAPI_Windows.h"
		typedef HyMemoryAPI_Windows HyMemoryInterop;

	#elif defined(HY_PLATFORM_OSX)
		#include "Renderer/OpenGL/Interop/HyOpenGL_OSX.h"
	#elif defined(HY_PLATFORM_LINUX)
		#include "Renderer/OpenGL/Interop/HyOpenGL_Linux.h"
	#endif

#endif

#endif HyInteropAfx_h__
