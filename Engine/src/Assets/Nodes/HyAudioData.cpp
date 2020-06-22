/**************************************************************************
 *	HyAudioData.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Assets/Nodes/HyAudioData.h"
#include "Assets/Files/HyFileAudio.h"
#include "Audio/HyAudioHarness.h"

HyAudioData::HyAudioData(const std::string &sPath, const jsonxx::Object &itemObjRef, HyAssets &assetsRef) :
	IHyNodeData(sPath),
	m_AudioRef(assetsRef.GetAudioRef()),
	m_InitObj(itemObjRef)
{
}

HyAudioData::~HyAudioData(void)
{
}

IHyAudioInst *HyAudioData::AllocateNewInstance() const
{
	return m_AudioRef.AllocateAudioInst(m_InitObj);
}
