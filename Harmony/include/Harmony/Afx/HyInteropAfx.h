/**************************************************************************
 *	HyInteropAfx.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2015 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyInteropAfx_h__
#define __HyInteropAfx_h__

#include "Afx/Platforms/HyPlatAfx.h"

#if defined(HY_PLATFORM_WINDOWS)
	
	#if defined(HY_PLATFORM_GUI)
		#include "Renderer/OpenGL/HyOpenGL.h"
		typedef HyOpenGL HyRendererInterop;

		#include "Input/Interop/HyInput_NULL.h"
		typedef HyInput_NULL HyInputInterop;
		typedef HyInputMap_NULL HyInputMapInterop;
	#else
		#include "Renderer/OpenGL/Interop/HyOpenGL_Win.h"
		typedef HyOpenGL_Win HyRendererInterop;

		#include "Input/Interop/HyInput_Gainput.h"
		#include "Input/Interop/HyInputMap_Gainput.h"
		typedef HyInput_Gainput HyInputInterop;
		typedef HyInputMap_Gainput HyInputMapInterop;
	#endif

	#include "Renderer/OpenGL/HyOpenGLShaderSrc.h"
	typedef HyOpenGLShader HyShaderInterop;

	#include "Audio/Interop/HyAudio_Win.h"
	typedef HyAudio_Win HyAudioInterop;
	typedef IHyAudioInst_Win IHyAudioInstInterop;

	#include "Time/Interop/HyTime_Win.h"
	typedef HyTime_Win HyTimeInterop;

	#include "Memory/Interop/HyMemAPI_Windows.h"
	typedef HyMemoryAPI_Windows HyMemoryInterop;

#elif defined(HY_PLATFORM_OSX) && !defined(HY_PLATFORM_GUI)
	#include "Renderer/OpenGL/Interop/HyOpenGL_OSX.h"
#elif defined(HY_PLATFORM_LINUX) && !defined(HY_PLATFORM_GUI)
	#include "Renderer/OpenGL/Interop/HyOpenGL_Linux.h"
#elif defined(HY_PLATFORM_GUI)
	#include "Renderer/OpenGL/HyOpenGL.h"
#endif

#endif __HyInteropAfx_h__