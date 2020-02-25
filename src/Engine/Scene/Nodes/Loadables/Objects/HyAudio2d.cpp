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
#include "Audio/HyAudio.h"
#include "Assets/Nodes/HyAudioData.h"

HyAudio2d::HyAudio2d(const char *szPrefix, const char *szName, HyEntity2d *pParent) :
	IHyLoadable2d(HYTYPE_Audio, szPrefix, szName, pParent),
	m_pInternal(nullptr)
{
	SetRegistered(true);
}

/*virtual*/ HyAudio2d::~HyAudio2d(void)
{
	delete m_pInternal;
}

void HyAudio2d::Start()
{
	AcquireData();
	m_pInternal->Start();
}

void HyAudio2d::Stop()
{
	AcquireData();
	m_pInternal->Stop();
}

float HyAudio2d::GetPitch()
{
	AcquireData();
	return m_pInternal->GetPitch();
}

void HyAudio2d::SetPitch(float fPitch)
{
	AcquireData();
	m_pInternal->SetPitch(fPitch);
}

void HyAudio2d::SetReverb(int iIndex, float fLevel)
{
	AcquireData();
	m_pInternal->SetReverb(iIndex, fLevel);
}

/*virtual*/ void HyAudio2d::OnDataAcquired() /*override*/
{
	if(m_pInternal == nullptr)
	{
		const HyAudioData *pData = static_cast<const HyAudioData *>(UncheckedGetData());
		m_pInternal = pData->AllocateNewInstance();
	}
}

/*virtual*/ void HyAudio2d::OnLoaded() /*override*/
{
	AcquireData();
	m_pInternal->OnLoaded();
}
