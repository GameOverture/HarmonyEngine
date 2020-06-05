/**************************************************************************
*	HyAudioManager.cpp
*
*	Harmony Engine
*	Copyright (c) 2019 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Audio/HyAudioManager.h"
#include "Assets/Files/HyAudioBank.h"
#include "Diagnostics/Console/HyConsole.h"
#include "Assets/HyAssets.h"
#include "Utilities/HyIO.h"
#include "HyEngine.h"

#if defined(HY_USE_SDL2)
	#include "Audio/Harness/SDL2/HyAudio_SDL2.h"
#elif defined(HY_PLATFORM_WINDOWS)
	#include <Objbase.h>
#endif

HyAudioManager::HyAudioManager(std::string sDataDir) :
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
		m_pInternal = HY_NEW HyAudio_SDL2();
#endif
	
	if(m_pInternal == nullptr)
	{
		HyLogInfo("No audio library detected");
		m_pInternal = HY_NEW HyAudio_Null();
	}

	std::string sAudioFilePath = sDataDir + HYASSETS_AudioDir + HYASSETS_AudioFile;
	if(HyIO::FileExists(sAudioFilePath))
	{
		// Create HyAudioBank objects to represent every sound bank file
		std::string sAudioFileContents;
		HyIO::ReadTextFile(sAudioFilePath.c_str(), sAudioFileContents);
		jsonxx::Object audioObj;
		if(audioObj.parse(sAudioFileContents))
		{
			for(auto iter = audioObj.kv_map().begin(); iter != audioObj.kv_map().end(); ++iter)
				m_AudioBankMap[iter->first] = HY_NEW HyAudioBank(sDataDir, iter->first, iter->second->get<jsonxx::Object>(), AllocateAudioBank());
		}
	}
}

HyAudioManager::~HyAudioManager()
{
	for(auto iter = m_AudioBankMap.begin(); iter != m_AudioBankMap.end(); ++iter)
		delete iter->second;

	delete m_pInternal;
	m_pInternal = nullptr;

#if !defined(HY_USE_SDL2) && defined(HY_PLATFORM_WINDOWS)
	CoUninitialize();
#endif
}

IHyAudioInst *HyAudioManager::AllocateAudioInst(const char *szPath)
{
	if(m_fpAllocateHyAudioInst)
		return m_fpAllocateHyAudioInst(m_pInternal, szPath);

	return HY_NEW HyAudioInst_Null();
}

HyAudioBank *HyAudioManager::GetAudioBank(const std::string &sBankName)
{
	auto iter = m_AudioBankMap.find(sBankName);
	if(iter != m_AudioBankMap.end())
		return iter->second;

	return nullptr;
}

void HyAudioManager::Update()
{
	m_pInternal->OnUpdate();
}

IHyAudioBank *HyAudioManager::AllocateAudioBank()
{
	if(m_fpAllocateHyAudioBank)
		return m_fpAllocateHyAudioBank(m_pInternal);

	return HY_NEW HyAudioBank_Null();
}
