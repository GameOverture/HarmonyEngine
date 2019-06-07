/**************************************************************************
*	HyAudio_FMOD.cpp
*	
*	Harmony Engine
*	Copyright (c) 2019 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "stdafx.h"

#include "HyAudio_FMOD.h"

void ERRCHECK_fn(FMOD_RESULT result, const char *file, int line)
{
	//if(result != FMOD_OK)
	//	HyError(file << "(" << line << "): FMOD error " << result << " - " << FMOD_ErrorString(result));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
__declspec (dllexport) IHyAudio *CreateHyAudio_FMOD()
{
	IHyAudio *pNewAudio = new HyAudio_FMOD();
	return pNewAudio;
}

HyAudio_FMOD::HyAudio_FMOD() :
	m_pSystem(nullptr)
{
	ERRCHECK(Studio::System::create(&m_pSystem));

	// The example Studio project is authored for 5.1 sound, so set up the system output mode to match
	System *pCoreSystem = nullptr;
	ERRCHECK(m_pSystem->getCoreSystem(&pCoreSystem));
	ERRCHECK(pCoreSystem->setSoftwareFormat(0, FMOD_SPEAKERMODE_5POINT1, 0));

	// Initialize
	ERRCHECK(m_pSystem->initialize(1024, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, nullptr));
}

/*virtual*/ HyAudio_FMOD::~HyAudio_FMOD()
{
	ERRCHECK(m_pSystem->release());
}

Studio::System *HyAudio_FMOD::GetSystem() const
{
	return m_pSystem;
}

/*virtual*/ void HyAudio_FMOD::OnUpdate() /*override*/
{
	ERRCHECK(m_pSystem->update());
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
__declspec (dllexport) IHyAudioBank *CreateHyAudioBank_FMOD(IHyAudio *pAudio)
{
	IHyAudioBank *pNewAudio = new HyAudioBank_FMOD(static_cast<HyAudio_FMOD *>(pAudio)->GetSystem());
	return pNewAudio;
}

HyAudioBank_FMOD::HyAudioBank_FMOD(Studio::System *pSystemRef) :
	m_pSystemRef(pSystemRef),
	m_pBank(nullptr)
{
}

/*virtual*/ HyAudioBank_FMOD::~HyAudioBank_FMOD()
{
	ERRCHECK(m_pBank->unload());
}

/*virtual*/ bool HyAudioBank_FMOD::Load(std::string sFilePath) /*override*/
{
	ERRCHECK(m_pSystemRef->loadBankFile(sFilePath.c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &m_pBank));
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
__declspec (dllexport) IHyAudioInst *CreateHyAudioInst_FMOD(IHyAudio *pAudio)
{
	IHyAudioInst *pNewAudio = new HyAudioInst_FMOD(static_cast<HyAudio_FMOD *>(pAudio)->GetSystem());
	return pNewAudio;
}

HyAudioInst_FMOD::HyAudioInst_FMOD(Studio::System *pSystemRef)
{
}

/*virtual*/ HyAudioInst_FMOD::~HyAudioInst_FMOD()
{
}

/*virtual*/ void HyAudioInst_FMOD::Play() /*override*/
{
}

/*virtual*/ void HyAudioInst_FMOD::Stop() /*override*/
{
}

/*virtual*/ float HyAudioInst_FMOD::GetPitch() /*override*/
{
	return 0.0f;
}

/*virtual*/ void HyAudioInst_FMOD::SetPitch(float fPitch) /*override*/
{
}

/*virtual*/ void HyAudioInst_FMOD::SetReverb() /*override*/
{
}
