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
	Studio::System *		m_pSystem;

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
	Studio::System *		m_pSystemRef;
	Studio::Bank *			m_pBank;

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
	HyAudioInst_FMOD(Studio::System *pSystemRef);
	virtual ~HyAudioInst_FMOD();

	virtual void Play() override;
	virtual void Stop() override;

	virtual float GetPitch() override;
	virtual void SetPitch(float fPitch) override;

	virtual void SetReverb() override;
};
