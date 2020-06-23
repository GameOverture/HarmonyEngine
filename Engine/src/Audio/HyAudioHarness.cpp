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
	#include "Audio/Harness/SDL2/HyAudio_SDL2.h"
	#include "Audio/Harness/SDL2/HyFileAudioGuts_SDL2.h"
	#include "Audio/Harness/SDL2/HyAudioInst_SDL2.h"
#elif defined(HY_PLATFORM_WINDOWS)
	#include <Objbase.h>
#endif

HyAudioHarness::HyAudioHarness(std::string sDataDir) :
	m_fpAllocateHyAudio(nullptr),
	m_fpAllocateHyAudioBank(nullptr),
	m_fpAllocateHyAudioInst(nullptr),
	m_pInternal(nullptr)
{
	sDataDir = HyIO::CleanPath(sDataDir.c_str(), "/", true);

#if defined(HY_USE_SDL2)
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
		m_fpAllocateHyAudioInst = (fpAllocateHyAudioInst)SDL_LoadFunction(hModule, "AllocateHyAudioInst_FMOD");
#elif defined(HY_PLATFORM_WINDOWS)
		m_fpAllocateHyAudio = (fpAllocateHyAudio)::GetProcAddress(hModule, "AllocateHyAudio_FMOD");
		m_fpAllocateHyAudioBank = (fpAllocateHyAudioBank)::GetProcAddress(hModule, "AllocateHyAudioBank_FMOD");
		m_fpAllocateHyAudioInst = (fpAllocateHyAudioInst)::GetProcAddress(hModule, "AllocateHyAudioInst_FMOD");
#endif
		if(m_fpAllocateHyAudio != nullptr && m_fpAllocateHyAudioBank != nullptr && m_fpAllocateHyAudioInst != nullptr)
		{
			HyLogInfo("FMOD audio library detected");
			m_pInternal = m_fpAllocateHyAudio();
		}
		else
		{
			HyLogError("A GetProcAddress() has failed in the FMOD module");
			m_fpAllocateHyAudio = nullptr;
			m_fpAllocateHyAudioBank = nullptr;
			m_fpAllocateHyAudioInst = nullptr;
#if defined(HY_USE_SDL2)
			SDL_UnloadObject(hModule);
#elif defined(HY_PLATFORM_WINDOWS)
			FreeLibrary(hModule);
#endif
		}
	}

#if defined(HY_USE_SDL2)
	// If no audio libraries were dynamically loaded, use SDL2 implementation
	if(m_pInternal == nullptr)
	{
		m_pInternal = HY_NEW HyAudio_SDL2();
		m_fpAllocateHyAudioBank = HyAudio_SDL2::AllocateBank;
		m_fpAllocateHyAudioInst = HyAudio_SDL2::AllocateInst;
	}
#endif
	
	if(m_pInternal == nullptr)
	{
		HyLogWarning("No audio library detected");
		m_pInternal = HY_NEW HyAudio_Null();
	}
}

HyAudioHarness::~HyAudioHarness()
{
	for(auto iter = m_AudioBankMap.begin(); iter != m_AudioBankMap.end(); ++iter)
		delete iter->second;

	delete m_pInternal;
	m_pInternal = nullptr;

#if !defined(HY_USE_SDL2) && defined(HY_PLATFORM_WINDOWS)
	CoUninitialize();
#endif
}

IHyFileAudioGuts *HyAudioHarness::AllocateAudioBank(const jsonxx::Object &bankObjRef)
{
	if(m_fpAllocateHyAudioBank)
		return m_fpAllocateHyAudioBank(m_pInternal, bankObjRef);

	return HY_NEW HyAudioBank_Null();
}

IHyAudioInst *HyAudioHarness::AllocateAudioInst(const jsonxx::Object &instObjRef)
{
	if(m_fpAllocateHyAudioInst)
		return m_fpAllocateHyAudioInst(m_pInternal, instObjRef);

	return HY_NEW HyAudioInst_Null();
}

HyFileAudio *HyAudioHarness::GetAudioBank(const std::string &sBankName)
{
	auto iter = m_AudioBankMap.find(sBankName);
	if(iter != m_AudioBankMap.end())
		return iter->second;

	return nullptr;
}

void HyAudioHarness::Update()
{
	m_pInternal->OnUpdate();
}
