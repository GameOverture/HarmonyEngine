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
#include "Audio/HyAudioManager.h"

HyAudioData::HyAudioData(const std::string &sPath, const jsonxx::Object &itemObjRef, HyAssets &assetsRef) :
	IHyNodeData(sPath),
	m_AudioRef(assetsRef.GetAudioRef())
{
	const jsonxx::Array &assetsArray = itemObjRef.get<jsonxx::Array>("assets");

	for(uint32 i = 0; i < assetsArray.size(); ++i)
	{
		jsonxx::Object assetObj = assetsArray.get<jsonxx::Object>(i);
		HyFileAudio *pAudio = assetsRef.GetAudioFile(static_cast<uint32>(assetObj.get<jsonxx::Number>("checksum")));
		m_RequiredAudio.Set(pAudio->GetManifestIndex());
	}

	//m_pAudioBank = m_AudioRef.GetAudioBank(itemDataObjRef.get<jsonxx::String>("bank"));
}

HyAudioData::~HyAudioData(void)
{
}

IHyAudioInst *HyAudioData::AllocateNewInstance() const
{
	return m_AudioRef.AllocateAudioInst(GetPath().c_str());
}
