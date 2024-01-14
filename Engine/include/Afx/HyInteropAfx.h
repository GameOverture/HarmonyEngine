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

// TIME ////////////////////////////////////////////////////////////////////
#ifdef HY_USE_GLFW
	typedef double HyTimeStamp;
	#define HyTimeStampDefaultVal 0.0
#elif defined(HY_USE_SDL2)
	typedef uint64_t HyTimeStamp;
	#define HyTimeStampDefaultVal 0
#else
	typedef std::chrono::time_point<std::chrono::high_resolution_clock, std::chrono::duration<double>> HyTimeStamp;
	#define HyTimeStampDefaultVal
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

// NETWORKING //////////////////////////////////////////////////////////////
#if defined(HY_USE_SDL2) && defined(HY_USE_SDL2_NET)
	#ifdef HY_PLATFORM_BROWSER
		#include <SDL2/SDL_net.h>
	#else
		#include "SDL_net.h"
	#endif
	typedef struct _TCPsocket *HyTcpSocket;
	#include "Networking/Sockets/HySdlNet.h"
	typedef HySdlNet HyNetworkInterop;
#elif defined(HY_PLATFORM_WINDOWS)
	#include <winsock2.h>
	#include <ws2tcpip.h>
	#pragma comment(lib, "Ws2_32.lib")
	typedef SOCKET HyTcpSocket;
	#include "Networking/Sockets/HyWinsock.h"
	typedef HyWinsock HyNetworkInterop;
#elif defined(HY_PLATFORM_LINUX)
	#include <errno.h> /* EINPROGRESS, errno */
	#include <sys/types.h> /* timeval */
	#include <sys/socket.h>
	#include <arpa/inet.h>
	#include <fcntl.h>
	#include <unistd.h>
	typedef int HyTcpSocket;
	#include "Networking/Sockets/HyPosix.h"
	typedef HyPosix HyNetworkInterop;
#else
	typedef int HyTcpSocket;
	#include "Networking/Sockets/HyNullSocket.h"
	typedef HyNullSocket HyNetworkInterop;
#endif

#endif /* HyInteropAfx_h__ */

// AUDIO ////////////////////////////////////////////////////////////////////
//#if defined(HY_USE_MINIAUDIO)
//	#include "Audio/miniaudio/HyAudioCore_miniaudio.h"
//	#include "Audio/miniaudio/HySoundBuffer_miniaudio.h"
//	typedef HyAudioCore_miniaudio HyAudioInterop;
//	typedef HySoundBuffer_miniaudio HySoundBufferInterop;
//#elif defined(HY_USE_SDL2)
//	#ifdef HY_PLATFORM_BROWSER
//		#include <SDL2/SDL_mixer.h>
//	#else
//		#include "SDL_mixer.h"
//	#endif
//	#include "Audio/SDL2/HyAudioCore_SDL2.h"
//	#include "Audio/SDL2/HySoundBuffer_SDL2.h"
//	typedef HyAudioCore_SDL2 HyAudioInterop;
//	//typedef HySoundBuffer_SDL2 HySoundBufferInterop;
//#else
//	#include  "Audio/IHyAudioCore.h"
//	typedef HyAudioCore_Null HyAudioInterop;
//	typedef HySoundBuffer_Null HySoundBufferInterop;
//#endif
//
// 
// 
// 
// 
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
