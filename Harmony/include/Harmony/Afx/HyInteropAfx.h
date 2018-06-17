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
#include "Afx/Platforms/HyPlatAfx.h"

#include "Renderer/OpenGL/HyOpenGL.h"
typedef HyOpenGL HyRendererInterop;

#if defined(HY_PLATFORM_GUI)
	#include "Diagnostics/Console/Interop/HyConsole_Gui.h"
	typedef HyConsole_Gui HyConsoleInterop;

	typedef void *HyWindowHandle;

	//#include "Audio/Interop/HyAudio_Win.h"
	//typedef HyAudio_Win HyAudioInterop;
	//typedef IHyAudioInst_Win IHyAudioInstInterop;

	//#include "Memory/Interop/HyMemAPI_Windows.h"
	//typedef HyMemoryAPI_Windows HyMemoryInterop;
#else
	#if defined(HY_PLATFORM_DESKTOP)
		typedef GLFWwindow *HyWindowHandle;
	#endif

	#if defined(HY_PLATFORM_WINDOWS)
		#include "Diagnostics/Console/Interop/HyConsole_Win.h"
		typedef HyConsole_Win HyConsoleInterop;

		#include "Audio/Interop/HyAudio_Win.h"
		typedef HyAudio_Win HyAudioInterop;
		typedef IHyAudioInst_Win IHyAudioInstInterop;

		#include "Memory/Interop/HyMemAPI_Windows.h"
		typedef HyMemoryAPI_Windows HyMemoryInterop;
	#endif
#endif

#endif HyInteropAfx_h__
