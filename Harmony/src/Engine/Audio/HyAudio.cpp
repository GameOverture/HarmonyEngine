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

fpCreateHyAudio		HyAudio::sm_fpCreateHyAudio = nullptr;
fpCreateHyAudioBank	HyAudio::sm_fpCreateHyAudioBank = nullptr;
fpCreateHyAudioInst HyAudio::sm_fpCreateHyAudioInst = nullptr;
IHyAudio *			HyAudio::sm_pInternal = nullptr;

HyAudio::HyAudio(std::string sDataDir)
{
	sDataDir = HyStr::MakeStringProperPath(sDataDir.c_str(), "/", true);

#if defined(HY_PLATFORM_WINDOWS)
	CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
	HMODULE hModule = LoadLibraryA("HyFMOD.dll");
	if(hModule != nullptr)
	{
		sm_fpCreateHyAudio = (fpCreateHyAudio)::GetProcAddress(hModule, "CreateHyAudio_FMOD");
		sm_fpCreateHyAudioBank = (fpCreateHyAudioBank)::GetProcAddress(hModule, "CreateHyAudioBank_FMOD");
		sm_fpCreateHyAudioInst = (fpCreateHyAudioInst)::GetProcAddress(hModule, "CreateHyAudioInst_FMOD");
		
		if(sm_fpCreateHyAudio != nullptr && sm_fpCreateHyAudioBank != nullptr && sm_fpCreateHyAudioInst != nullptr)
		{
			HyLogInfo("FMOD audio library detected");
			sm_pInternal = sm_fpCreateHyAudio();
		}
		else
		{
			HyLogError("A GetProcAddress() has failed in the FMOD module");
			sm_fpCreateHyAudio = nullptr;
			sm_fpCreateHyAudioBank = nullptr;
			sm_fpCreateHyAudioInst = nullptr;
			FreeLibrary(hModule);
		}
	}
#endif
	if(sm_pInternal == nullptr)
	{
		HyLogInfo("No audio library detected");
		sm_pInternal = HY_NEW HyAudio_Null();
	}

	// Create HyAudioBank objects to represent every sound bank file
	std::string sAudioFileContents;
	HyReadTextFile(std::string(sDataDir + HYASSETS_AudioDir + HYASSETS_AudioFile).c_str(), sAudioFileContents);
	jsonxx::Object audioObj;
	if(audioObj.parse(sAudioFileContents))
	{
		for(auto iter = audioObj.kv_map().begin(); iter != audioObj.kv_map().end(); ++iter)
			m_AudioBankMap[iter->first] = HY_NEW HyAudioBank(sDataDir, iter->first, iter->second->get<jsonxx::Object>());
	}
}

HyAudio::~HyAudio()
{
	for(auto iter = m_AudioBankMap.begin(); iter != m_AudioBankMap.end(); ++iter)
		delete iter->second;

	delete sm_pInternal;
	sm_pInternal = nullptr;

#if defined(HY_PLATFORM_WINDOWS)
	CoUninitialize();
#endif
}

/*static*/ IHyAudioBank *HyAudio::AllocateAudioBank()
{
	if(sm_fpCreateHyAudioBank)
		return sm_fpCreateHyAudioBank(sm_pInternal);

	return HY_NEW HyAudioBank_Null();
}

/*static*/ IHyAudioInst *HyAudio::AllocateAudioInst(const char *szPath)
{
	if(sm_fpCreateHyAudioInst)
		return sm_fpCreateHyAudioInst(sm_pInternal, szPath);

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
	sm_pInternal->OnUpdate();
}
