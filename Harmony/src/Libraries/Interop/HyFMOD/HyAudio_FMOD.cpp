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

	//System *pCoreSystem = nullptr;
	//ERRCHECK(m_pSystem->getCoreSystem(&pCoreSystem));
	//ERRCHECK(pCoreSystem->setSoftwareFormat(0, FMOD_SPEAKERMODE_5POINT1, 0));

	ERRCHECK(m_pSystem->initialize(1024, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, nullptr));
	ERRCHECK(m_pSystem->loadBankFile("Master Bank.bank", FMOD_STUDIO_LOAD_BANK_NORMAL, &m_pMasterBank));
	ERRCHECK(m_pSystem->loadBankFile("Master Bank.strings.bank", FMOD_STUDIO_LOAD_BANK_NORMAL, &m_pMasterStringsBank));
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
__declspec (dllexport) IHyAudioInst *CreateHyAudioInst_FMOD(IHyAudio *pAudio, const char *szPath)
{
	IHyAudioInst *pNewAudio = new HyAudioInst_FMOD(static_cast<HyAudio_FMOD *>(pAudio)->GetSystem(), szPath);
	return pNewAudio;
}

HyAudioInst_FMOD::HyAudioInst_FMOD(Studio::System *pSystemRef, const char *szPath) :
	m_pSystemRef(pSystemRef),
	m_pDesc(nullptr),
	m_pInst(nullptr)
{
	ERRCHECK(m_pSystemRef->getEvent("event:/Weapons/Explosion", &m_pDesc));
	ERRCHECK(m_pDesc->createInstance(&m_pInst));
}

/*virtual*/ HyAudioInst_FMOD::~HyAudioInst_FMOD()
{
}

/*virtual*/ void HyAudioInst_FMOD::PlayOneShot() /*override*/
{
	Studio::EventInstance *pOneShotInstance = nullptr;
	ERRCHECK(m_pDesc->createInstance(&pOneShotInstance));
	ERRCHECK(pOneShotInstance->start());

	// Release will clean up the instance when it completes
	ERRCHECK(pOneShotInstance->release());
}

/*virtual*/ void HyAudioInst_FMOD::Start() /*override*/
{
	ERRCHECK(m_pInst->start());
}

/*virtual*/ void HyAudioInst_FMOD::Stop(HyAudioStop eStopType /*= HYAUDIOSTOP_AllowFadeOut*/) /*override*/
{	
	ERRCHECK(m_pInst->stop(eStopType == HYAUDIOSTOP_AllowFadeOut ? FMOD_STUDIO_STOP_ALLOWFADEOUT : FMOD_STUDIO_STOP_IMMEDIATE));
}

/*virtual*/ bool HyAudioInst_FMOD::IsPaused() const /*override*/
{
	bool bIsPaused = false;
	ERRCHECK(m_pInst->getPaused(&bIsPaused));
	return bIsPaused;
}

/*virtual*/ void HyAudioInst_FMOD::SetPause(bool bPause) /*override*/
{
	ERRCHECK(m_pInst->setPaused(bPause));
}

/*virtual*/ float HyAudioInst_FMOD::GetVolume(float *fFinalVolumeOut /*= nullptr*/) const /*override*/
{
	float fVolume = 0.0f;
	ERRCHECK(m_pInst->getVolume(&fVolume, fFinalVolumeOut));
	return fVolume;
}

/*virtual*/ void HyAudioInst_FMOD::SetVolume(float fVolume) /*override*/
{
	ERRCHECK(m_pInst->setVolume(fVolume));
}

/*virtual*/ float HyAudioInst_FMOD::GetPitch(float *fFinalPitchOut /*= nullptr*/) const /*override*/
{
	float fPitch = 0.0f;
	ERRCHECK(m_pInst->getPitch(&fPitch, fFinalPitchOut));
	return fPitch;
}

/*virtual*/ void HyAudioInst_FMOD::SetPitch(float fPitch) /*override*/
{
	ERRCHECK(m_pInst->setPitch(fPitch));
}

/*virtual*/ int HyAudioInst_FMOD::GetTimelinePosition() const /*override*/
{
	int iPosition = 0;
	ERRCHECK(m_pInst->getTimelinePosition(&iPosition));
	return iPosition;
}

/*virtual*/ void HyAudioInst_FMOD::SetTimelinePosition(int iPosition) /*override*/
{
	ERRCHECK(m_pInst->setTimelinePosition(iPosition));
}

/*virtual*/ float HyAudioInst_FMOD::GetParam(const char *szParam, float *fFinalValueOut /*= nullptr*/) const /*override*/
{
	float fValue = 0.0f;
	ERRCHECK(m_pInst->getParameterByName(szParam, &fValue, fFinalValueOut));
	return fValue;
}

/*virtual*/ void HyAudioInst_FMOD::SetParam(const char *szParam, float fValue) /*override*/
{
	ERRCHECK(m_pInst->setParameterByName(szParam, fValue));
}

/*virtual*/ float HyAudioInst_FMOD::GetReverb(int iIndex) const /*override*/
{
	float fReverb = 0.0f;
	ERRCHECK(m_pInst->getReverbLevel(iIndex, &fReverb));
	return fReverb;
}

/*virtual*/ void HyAudioInst_FMOD::SetReverb(int iIndex, float fLevel) /*override*/
{
	ERRCHECK(m_pInst->setReverbLevel(iIndex, fLevel));
}
