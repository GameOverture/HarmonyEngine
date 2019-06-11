/**************************************************************************
*	HyAudio.cpp
*
*	Harmony Engine
*	Copyright (c) 2019 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Audio/HyAudio.h"
#include "Assets/Files/HyAudioBank.h"
#include "Diagnostics/Console/HyConsole.h"
#include "Assets/HyAssets.h"
#include "HyEngine.h"

#if defined(HY_PLATFORM_WINDOWS)
	#include <Objbase.h>
#endif

HyAudio::HyAudio(std::string sDataDir) : 
	m_fpAllocateHyAudio(nullptr),
	m_fpAllocateHyAudioBank(nullptr),
	m_fpAllocateHyAudioInst(nullptr),
	m_pInternal(nullptr)
{
	sDataDir = HyStr::MakeStringProperPath(sDataDir.c_str(), "/", true);

#if defined(HY_PLATFORM_WINDOWS)
	CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
	HMODULE hModule = LoadLibraryA("HyFMOD.dll");
	if(hModule != nullptr)
	{
		m_fpAllocateHyAudio = (fpAllocateHyAudio)::GetProcAddress(hModule, "AllocateHyAudio_FMOD");
		m_fpAllocateHyAudioBank = (fpAllocateHyAudioBank)::GetProcAddress(hModule, "AllocateHyAudioBank_FMOD");
		m_fpAllocateHyAudioInst = (fpAllocateHyAudioInst)::GetProcAddress(hModule, "AllocateHyAudioInst_FMOD");
		
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
			FreeLibrary(hModule);
		}
	}
#endif

	if(m_pInternal == nullptr)
	{
		HyLogInfo("No audio library detected");
		m_pInternal = HY_NEW HyAudio_Null();
	}

	// Create HyAudioBank objects to represent every sound bank file
	std::string sAudioFileContents;
	HyReadTextFile(std::string(sDataDir + HYASSETS_AudioDir + HYASSETS_AudioFile).c_str(), sAudioFileContents);
	jsonxx::Object audioObj;
	if(audioObj.parse(sAudioFileContents))
	{
		for(auto iter = audioObj.kv_map().begin(); iter != audioObj.kv_map().end(); ++iter)
			m_AudioBankMap[iter->first] = HY_NEW HyAudioBank(sDataDir, iter->first, iter->second->get<jsonxx::Object>(), AllocateAudioBank());
	}
}

HyAudio::~HyAudio()
{
	for(auto iter = m_AudioBankMap.begin(); iter != m_AudioBankMap.end(); ++iter)
		delete iter->second;

	delete m_pInternal;
	m_pInternal = nullptr;

#if defined(HY_PLATFORM_WINDOWS)
	CoUninitialize();
#endif
}

IHyAudioInst *HyAudio::AllocateAudioInst(const char *szPath)
{
	if(m_fpAllocateHyAudioInst)
		return m_fpAllocateHyAudioInst(m_pInternal, szPath);

	return HY_NEW HyAudioInst_Null();
}

HyAudioBank *HyAudio::GetAudioBank(const std::string &sBankName)
{
	auto iter = m_AudioBankMap.find(sBankName);
	if(iter != m_AudioBankMap.end())
		return iter->second;

	return nullptr;
}

void HyAudio::Update()
{
	m_pInternal->OnUpdate();
}

IHyAudioBank *HyAudio::AllocateAudioBank()
{
	if(m_fpAllocateHyAudioBank)
		return m_fpAllocateHyAudioBank(m_pInternal);

	return HY_NEW HyAudioBank_Null();
}
