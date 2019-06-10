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
