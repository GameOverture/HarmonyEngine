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
#include "Assets/Files/HyFileAudio.h"
#include "Diagnostics/Console/HyConsole.h"
#include "Assets/HyAssets.h"
#include "Utilities/HyIO.h"
#include "HyEngine.h"

#if defined(HY_USE_SDL2)
	#include "SDL_mixer.h"

	#include "Audio/Harness/SDL2/HyAudioCore_SDL2.h"
	#include "Audio/Harness/SDL2/HyFileAudioImpl_SDL2.h"
#elif defined(HY_PLATFORM_WINDOWS)
	#include <Objbase.h>
#endif

HyAudioHarness::HyAudioHarness(std::string sDataDir) :
	m_fpAllocateHyAudio(nullptr),
	m_fpAllocateHyAudioBank(nullptr),
	m_pCore(nullptr)
{
	sDataDir = HyIO::CleanPath(sDataDir.c_str(), "/", true);

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
		m_fpAllocateHyAudio = (fpAllocateHyAudio)SDL_LoadFunction(hModule, "AllocateHyAudio_FMOD");
		m_fpAllocateHyAudioBank = (fpAllocateHyAudioBank)SDL_LoadFunction(hModule, "AllocateHyAudioBank_FMOD");
#elif defined(HY_PLATFORM_WINDOWS)
		m_fpAllocateHyAudio = (fpAllocateHyAudio)::GetProcAddress(hModule, "AllocateHyAudio_FMOD");
		m_fpAllocateHyAudioBank = (fpAllocateHyAudioBank)::GetProcAddress(hModule, "AllocateHyAudioBank_FMOD");
#endif
		if(m_fpAllocateHyAudio != nullptr && m_fpAllocateHyAudioBank != nullptr)
		{
			HyLogInfo("FMOD audio library detected");
			m_pCore = m_fpAllocateHyAudio();
		}
		else
		{
			HyLogError("A GetProcAddress() has failed in the FMOD module");
			m_fpAllocateHyAudio = nullptr;
			m_fpAllocateHyAudioBank = nullptr;
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
		m_fpAllocateHyAudioBank = HyAudioCore_SDL2::AllocateBank;
	}
#endif
	
	if(m_pCore == nullptr)
	{
		HyLogWarning("No audio library detected");
		m_pCore = HY_NEW HyAudioCore_Null();
	}
}

HyAudioHarness::~HyAudioHarness()
{
	for(auto iter = m_AudioBankMap.begin(); iter != m_AudioBankMap.end(); ++iter)
		delete iter->second;

	delete m_pCore;
	m_pCore = nullptr;

#if !defined(HY_USE_SDL2) && defined(HY_PLATFORM_WINDOWS)
	CoUninitialize();
#endif
}

IHyFileAudioImpl *HyAudioHarness::AllocateAudioBank(HyJsonObj bankObj)
{
	if(m_fpAllocateHyAudioBank)
		return m_fpAllocateHyAudioBank(m_pCore, bankObj);

	return HY_NEW HyAudioBank_Null();
}

HyFileAudio *HyAudioHarness::GetAudioBank(const std::string &sBankName)
{
	auto iter = m_AudioBankMap.find(sBankName);
	if(iter != m_AudioBankMap.end())
		return iter->second;

	return nullptr;
}

void HyAudioHarness::AppendCue(IHyNode *pNode, IHyAudioCore::CueType eCueType)
{
	m_pCore->AppendCue(pNode, eCueType);
}

void HyAudioHarness::Update()
{
	m_pCore->OnUpdate();
}
