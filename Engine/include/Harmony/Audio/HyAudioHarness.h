/**************************************************************************
*	HyAudioHarness.h
*
*	Harmony Engine
*	Copyright (c) 2019 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyAudioHarness_h__
#define HyAudioHarness_h__

#include "Afx/HyStdAfx.h"
#include "Audio/Harness/IHyAudioCore.h"
#include "Audio/Harness/IHyFileAudioImpl.h"

class HyFileAudio;

class HyAudioHarness
{
	friend class HyEngine;

	IHyAudioCore *							m_pCore;

public:
	HyAudioHarness();
	~HyAudioHarness();

	void SetSfxVolume(float fGlobalSfxVolume);

private:
	IHyAudioCore &GetCore() const;
};

#endif /* HyAudioHarness_h__ */
