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

	//#include "Input/Interop/HyInput_NULL.h"
	//typedef HyInput_NULL HyInputInterop;
	//typedef HyInputMap_NULL HyInputMapInterop;

	#include "Diagnostics/Console/Interop/HyConsole_Gui.h"
	typedef HyConsole_Gui HyConsoleInterop;

	#include "Renderer/OpenGL/HyOpenGLShader.h"
	typedef HyOpenGLShader HyShaderInterop;

	typedef HWND HyRenderSurfaceHandleInterop;

	#include "Time/Interop/HyTime_Win.h"
	typedef HyTime_Win HyTimeInterop;
	#include "Audio/Interop/HyAudio_Win.h"
	typedef HyAudio_Win HyAudioInterop;
	typedef IHyAudioInst_Win IHyAudioInstInterop;
	#include "Memory/Interop/HyMemAPI_Windows.h"
	typedef HyMemoryAPI_Windows HyMemoryInterop;

	typedef MSG HyApiMsgInterop;
#else
	#if defined(HY_PLATFORM_DESKTOP)
		typedef GLFWwindow *HyRenderSurfaceHandleInterop;
		#include "Renderer/OpenGL/HyOpenGL.h"
		typedef HyOpenGL HyRendererInterop;
	#endif

	#if defined(HY_PLATFORM_WINDOWS)
		//#include "Renderer/OpenGL/Interop/HyOpenGL_Win.h"
		//typedef HyOpenGL_Win HyRendererInterop;

		//#include "Input/Interop/HyInput_Gainput.h"
		//#include "Input/Interop/HyInputMap_Gainput.h"
		//typedef HyInput_Gainput HyInputInterop;
		//typedef HyInputMap_Gainput HyInputMapInterop;

		#include "Diagnostics/Console/Interop/HyConsole_Win.h"
		typedef HyConsole_Win HyConsoleInterop;

		typedef MSG HyApiMsgInterop;

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
