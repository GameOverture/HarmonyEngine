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
#include "Audio/HyAudio.h"

HyAudioData::HyAudioData(const std::string &sPath, const jsonxx::Value &dataValueRef, HyAssets &assetsRef) :
	IHyNodeData(sPath),
	m_AudioRef(assetsRef.GetAudioRef())
{
	const jsonxx::Object &audioObj = dataValueRef.get<jsonxx::Object>();
	m_pAudioBank = m_AudioRef.GetAudioBank(audioObj.get<jsonxx::String>("bank"));
}

HyAudioData::~HyAudioData(void)
{
}

IHyAudioInst *HyAudioData::AllocateNewInstance() const
{
	return m_AudioRef.AllocateAudioInst(GetPath().c_str());
}
