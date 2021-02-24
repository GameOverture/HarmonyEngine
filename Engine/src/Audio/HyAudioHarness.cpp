/**************************************************************************
*	HyAudioHarness.cpp
*
*	Harmony Engine
*	Copyright (c) 2019 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Audio/HyAudioHarness.h"
#include "Diagnostics/Console/IHyConsole.h"
#include "Assets/HyAssets.h"
#include "Utilities/HyIO.h"
#include "HyEngine.h"

#if defined(HY_USE_SDL2)
	#ifdef HY_PLATFORM_BROWSER
		#include <SDL2/SDL_mixer.h>
	#else
		#include "SDL_mixer.h"
	#endif

	#include "Audio/Harness/SDL2/HyAudioCore_SDL2.h"
	#include "Audio/Harness/SDL2/HyFileAudioImpl_SDL2.h"
#elif defined(HY_PLATFORM_WINDOWS)
	#include <Objbase.h>
#endif

HyAudioHarness::HyAudioHarness() :
	m_pCore(nullptr)
{
	using fpAllocateHyAudio					= IHyAudioCore *(*)();

	fpAllocateHyAudio fpAllocateHyAudioCore;

#if defined(HY_USE_SDL2) && !defined(HY_PLATFORM_BROWSER)
	void *hModule = SDL_LoadObject("HyFMOD");
#elif defined(HY_PLATFORM_WINDOWS)
	CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
	HMODULE hModule = LoadLibraryA("HyFMOD.dll");
#else
	void *hModule = nullptr;
#endif
	if(hModule != nullptr)
	{
#if defined(HY_USE_SDL2)
		fpAllocateHyAudioCore = (fpAllocateHyAudio)SDL_LoadFunction(hModule, "AllocateHyAudio_FMOD");
#elif defined(HY_PLATFORM_WINDOWS)
		fpAllocateHyAudioCore = (fpAllocateHyAudio)::GetProcAddress(hModule, "AllocateHyAudio_FMOD");
#endif
		if(fpAllocateHyAudioCore != nullptr)
		{
			HyLogInfo("FMOD audio library detected");
			m_pCore = fpAllocateHyAudioCore();
		}
		else
		{
			HyLogError("A GetProcAddress() has failed in the FMOD module");
			fpAllocateHyAudioCore = nullptr;
#if defined(HY_USE_SDL2)
			SDL_UnloadObject(hModule);
#elif defined(HY_PLATFORM_WINDOWS)
			FreeLibrary(hModule);
#endif
		}
	}

#if defined(HY_USE_SDL2)
	// If no audio libraries were dynamically loaded, use SDL2 implementation
	if(m_pCore == nullptr)
	{
		m_pCore = HY_NEW HyAudioCore_SDL2();
	}
#endif
	
	if(m_pCore == nullptr)
	{
		HyLogWarning("No audio library detected");
		m_pCore = HY_NEW HyAudioCore_Null();
	}

	HyAssert(m_pCore != nullptr, "Audio core was not instantiated");
}

HyAudioHarness::~HyAudioHarness()
{
	delete m_pCore;
	m_pCore = nullptr;

#if !defined(HY_USE_SDL2) && defined(HY_PLATFORM_WINDOWS)
	CoUninitialize();
#endif
}

void HyAudioHarness::SetSfxVolume(float fGlobalSfxVolume)
{
	m_pCore->SetSfxVolume(fGlobalSfxVolume);
}

IHyAudioCore &HyAudioHarness::GetCore() const
{
	return *m_pCore;
}
