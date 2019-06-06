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

HyAudio2d::HyAudio2d(const char *szPrefix, const char *szName, HyEntity2d *pParent) :
	IHyLoadable2d(HYTYPE_Audio, szPrefix, szName, pParent)
{
}

/*virtual*/ HyAudio2d::~HyAudio2d(void)
{
}

void HyAudio2d::Play()
{
	m_pInternal->Play();
}

void HyAudio2d::Stop()
{
	m_pInternal->Stop();
}

float HyAudio2d::GetPitch()
{
	return m_pInternal->GetPitch();
}

void HyAudio2d::SetPitch(float fPitch)
{
	m_pInternal->SetPitch(fPitch);
}

void HyAudio2d::SetReverb()
{
	m_pInternal->SetReverb();
}
