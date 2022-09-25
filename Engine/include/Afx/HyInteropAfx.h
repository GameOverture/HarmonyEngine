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
	typedef GLFWcursor *HyMouseCursorPtr;
#elif defined(HY_USE_SDL2)
	typedef SDL_Window *HyWindowInteropPtr;
	typedef SDL_Cursor *HyMouseCursorPtr;
#else
	typedef void *HyWindowInteropPtr;
	typedef void *HyMouseCursorPtr;
#endif

// AUDIO ////////////////////////////////////////////////////////////////////
#if defined(HY_USE_MINIAUDIO)
	#include "Audio/miniaudio/HyAudioCore_miniaudio.h"
	#include "Audio/miniaudio/HySoundBuffer_miniaudio.h"
	typedef HyAudioCore_miniaudio HyAudioInterop;
	typedef HySoundBuffer_miniaudio HySoundBufferInterop;
#elif defined(HY_USE_SDL2)
	#ifdef HY_PLATFORM_BROWSER
		#include <SDL2/SDL_mixer.h>
	#else
		#include "SDL_mixer.h"
	#endif
	#include "Audio/SDL2/HyAudioCore_SDL2.h"
	#include "Audio/SDL2/HySoundBuffer_SDL2.h"
	typedef HyAudioCore_SDL2 HyAudioInterop;
	typedef HySoundBuffer_SDL2 HySoundBufferInterop;
#else
	#include  "Audio/IHyAudioCore.h"
	typedef HyAudioCore_Null HyAudioInterop;
	typedef HySoundBuffer_Null HySoundBufferInterop;
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
#endif

#endif /* HyInteropAfx_h__ */




//
//HyAudioHarness::HyAudioHarness() :
//	m_pCore(nullptr)
//{
//	using fpAllocateHyAudio = IHyAudioCore * (*)();
//
//	fpAllocateHyAudio fpAllocateHyAudioCore = nullptr;
//
//#if defined(HY_USE_SDL2) && !defined(HY_PLATFORM_BROWSER)
//	void *hModule = SDL_LoadObject("HyFMOD");
//#elif defined(HY_PLATFORM_WINDOWS)
//	CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
//	HMODULE hModule = LoadLibraryA("HyFMOD.dll");
//#else
//	void *hModule = nullptr;
//#endif
//	if(hModule != nullptr)
//	{
//#if defined(HY_USE_SDL2)
//		fpAllocateHyAudioCore = (fpAllocateHyAudio)SDL_LoadFunction(hModule, "AllocateHyAudio_FMOD");
//#elif defined(HY_PLATFORM_WINDOWS)
//		fpAllocateHyAudioCore = (fpAllocateHyAudio)::GetProcAddress(hModule, "AllocateHyAudio_FMOD");
//#endif
//		if(fpAllocateHyAudioCore != nullptr)
//		{
//			HyLogInfo("FMOD audio library detected");
//			m_pCore = fpAllocateHyAudioCore();
//		}
//		else
//		{
//			HyLogError("A GetProcAddress() has failed in the FMOD module");
//			fpAllocateHyAudioCore = nullptr;
//#if defined(HY_USE_SDL2)
//			SDL_UnloadObject(hModule);
//#elif defined(HY_PLATFORM_WINDOWS)
//			FreeLibrary(hModule);
//#endif
//		}
//	}
//
//#if defined(HY_USE_SDL2)
//	// If no audio libraries were dynamically loaded, use SDL2 implementation
//	if(m_pCore == nullptr)
//		m_pCore = HY_NEW HyAudioCore_SDL2();
//#endif
//
//	if(m_pCore == nullptr)
//	{
//		HyLogWarning("No audio library detected");
//		m_pCore = HY_NEW HyAudioCore_Null();
//	}
//
//	HyAssert(m_pCore != nullptr, "Audio core was not instantiated");
//}
//
//HyAudioHarness::~HyAudioHarness()
//{
//	delete m_pCore;
//	m_pCore = nullptr;
//
//#if !defined(HY_USE_SDL2) && defined(HY_PLATFORM_WINDOWS)
//	CoUninitialize();
//#endif
//}
