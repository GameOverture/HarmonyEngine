/**************************************************************************
 *	HyAudio2d.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Loadables/Objects/HyAudio2d.h"
#include "Audio/HyAudioHarness.h"
#include "Assets/Nodes/HyAudioData.h"

IHyAudioCore *HyAudio2d::sm_pAudioCore = nullptr;

HyAudio2d::HyAudio2d(std::string sPrefix /*= ""*/, std::string sName /*= ""*/, HyEntity2d *pParent /*= nullptr*/) :
	IHyLoadable2d(HYTYPE_Audio, sPrefix, sName, pParent),
	m_uiCueFlags(0),
	m_fVolume(1.0f),
	m_fPitch(1.0f),
	volume(m_fVolume, *this, 0),
	pitch(m_fPitch, *this, 0)
{
}

/*virtual*/ HyAudio2d::~HyAudio2d(void)
{
}

void HyAudio2d::PlayOneShot(bool bUseCurrentSettings /*= true*/)
{
	if(bUseCurrentSettings)
		m_uiCueFlags |= (1 << IHyAudioCore::CUETYPE_PlayOneShot);
	else
		m_uiCueFlags |= (1 << IHyAudioCore::CUETYPE_PlayOneShotDefault);
}

void HyAudio2d::Start()
{
	m_uiCueFlags &= ~(1 << IHyAudioCore::CUETYPE_Stop);
	m_uiCueFlags |= (1 << IHyAudioCore::CUETYPE_Start);
}

void HyAudio2d::Stop()
{
	m_uiCueFlags &= ~(1 << IHyAudioCore::CUETYPE_Start);
	m_uiCueFlags |= (1 << IHyAudioCore::CUETYPE_Stop);
}

void HyAudio2d::SetPause(bool bPause)
{
	if(bPause)
	{
		m_uiCueFlags &= ~(1 << IHyAudioCore::CUETYPE_Unpause);
		m_uiCueFlags |= (1 << IHyAudioCore::CUETYPE_Pause);
	}
	else
	{
		m_uiCueFlags &= ~(1 << IHyAudioCore::CUETYPE_Pause);
		m_uiCueFlags |= (1 << IHyAudioCore::CUETYPE_Unpause);
	}
}

/*virtual*/ void HyAudio2d::OnLoadedUpdate() /*override*/
{
	if(IsDirty(DIRTY_Audio))
	{
		m_uiCueFlags |= (1 << IHyAudioCore::CUETYPE_Attributes);
		ClearDirty(DIRTY_Audio);
	}

	if(m_uiCueFlags)
	{
		for(uint32 i = 0; i < IHyAudioCore::NUM_CUETYPES; ++i)
		{
			if(0 != (m_uiCueFlags & (1 << i)))
				sm_pAudioCore->AppendCue(this, static_cast<IHyAudioCore::CueType>(i));
		}

		m_uiCueFlags = 0;
	}
}
