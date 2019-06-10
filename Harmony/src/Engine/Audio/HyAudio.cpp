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
#include "Diagnostics/Console/HyConsole.h"
#include "Assets/HyAssets.h"
#include "HyEngine.h"

fpCreateHyAudio		HyAudio::sm_fpCreateHyAudio = nullptr;
fpCreateHyAudioBank	HyAudio::sm_fpCreateHyAudioBank = nullptr;
fpCreateHyAudioInst HyAudio::sm_fpCreateHyAudioInst = nullptr;
IHyAudio *			HyAudio::sm_pInternal = nullptr;

HyAudio::HyAudio()
{
#if defined(HY_PLATFORM_WINDOWS)
	//CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
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

			//// Load master banks
			//IHyAudioBank *pMasterBank = CreateAudioBank();
			//IHyAudioBank *pMasterSoundBank = CreateAudioBank();

			//std::string sFilePath = Hy_DataDir() + HYASSETS_AudioDir + "Desktop/Master.bank";
			//pMasterBank->Load(sFilePath);

			//sFilePath = Hy_DataDir() + HYASSETS_AudioDir + "Desktop/Master.strings.bank";
			//pMasterSoundBank->Load(sFilePath);
		}
		else
		{
			HyLogError("A GetProcAddress() has failed in the FMOD module");
			sm_fpCreateHyAudio = nullptr;
			sm_fpCreateHyAudioBank = nullptr;
			FreeLibrary(hModule);
			//CoUninitialize();
		}
	}
#endif

	HyLogInfo("No audio library detected");
	if(sm_pInternal == nullptr)
		sm_pInternal = HY_NEW HyAudio_Null();
}

HyAudio::~HyAudio()
{
	delete sm_pInternal;
	sm_pInternal = nullptr;
}

/*static*/ IHyAudioBank *HyAudio::CreateAudioBank()
{
	if(sm_fpCreateHyAudioBank)
		return sm_fpCreateHyAudioBank(sm_pInternal);

	return HY_NEW HyAudioBank_Null();
}

/*static*/ IHyAudioInst *HyAudio::CreateAudioInst(const char *szPath)
{
	if(sm_fpCreateHyAudioInst)
		return sm_fpCreateHyAudioInst(sm_pInternal, szPath);

	return HY_NEW HyAudioInst_Null();
}
