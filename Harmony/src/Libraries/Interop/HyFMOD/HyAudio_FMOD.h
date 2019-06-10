/**************************************************************************
*	HyAudio_FMOD.h
*	
*	Harmony Engine
*	Copyright (c) 2019 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "stdafx.h"

#include "Harmony/Audio/Harness/IHyAudio.h"
#include "Harmony/Audio/Harness/IHyAudioBank.h"
#include "Harmony/Audio/Harness/IHyAudioInst.h"

#include "3rdParty/FMOD/fmod_studio.hpp"
#include "3rdParty/FMOD/fmod.hpp"
#include "3rdParty/FMOD/fmod_errors.h"

using namespace FMOD;

void ERRCHECK_fn(FMOD_RESULT result, const char *file, int line);
#define ERRCHECK(_result) ERRCHECK_fn(_result, __FILE__, __LINE__)

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class HyAudio_FMOD : public IHyAudio
{
	Studio::System *			m_pSystem;

public:
	HyAudio_FMOD();
	virtual ~HyAudio_FMOD();

	Studio::System *GetSystem() const;

	virtual void OnUpdate() override;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class HyAudioBank_FMOD : public IHyAudioBank
{
	Studio::System *			m_pSystemRef;
	Studio::Bank *				m_pBank;

public:
	HyAudioBank_FMOD(Studio::System *pSystemRef);
	virtual ~HyAudioBank_FMOD();

	virtual bool Load(std::string sFilePath) override;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class HyAudioInst_FMOD : public IHyAudioInst
{
	Studio::System *			m_pSystemRef;
	Studio::EventDescription *	m_pDesc;
	Studio::EventInstance *		m_pInst;

public:
	HyAudioInst_FMOD(Studio::System *pSystemRef, const char *szPath);
	virtual ~HyAudioInst_FMOD();

	virtual void PlayOneShot() override;
	virtual void Start() override;
	virtual void Stop(HyAudioStop eStopType = HYAUDIOSTOP_AllowFadeOut) override;

	virtual bool IsPaused() const override;
	virtual void SetPause(bool bPause) override;

	virtual float GetVolume(float *fFinalVolumeOut = nullptr) const override;
	virtual void SetVolume(float fVolume) override;

	virtual float GetPitch(float *fFinalPitchOut = nullptr) const override;
	virtual void SetPitch(float fPitch) override;

	virtual int GetTimelinePosition() const override;
	virtual void SetTimelinePosition(int iPosition) override;

	virtual float GetParam(const char *szParam, float *fFinalValueOut = nullptr) const override;
	virtual void SetParam(const char *szParam, float fValue) override;

	virtual float GetReverb(int iIndex) const override;
	virtual void SetReverb(int iIndex, float fLevel) override;
};
