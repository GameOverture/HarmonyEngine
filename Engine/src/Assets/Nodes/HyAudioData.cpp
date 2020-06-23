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
	jsonxx::Object sdl2Obj = m_InitObj.get<jsonxx::Object>("SDL2");

	jsonxx::Array assetsArray = sdl2Obj.get<jsonxx::Array>("assets");
	for(uint32 i = 0; i < assetsArray.size(); ++i)
	{
		IHyFile *pAudioFile = assetsRef.GetFileWithAsset(HYFILE_AudioBank, static_cast<uint32>(assetsArray.get<jsonxx::Number>(i)));
		m_RequiredAudio.Set(pAudioFile->GetManifestIndex());
	}
}

HyAudioData::~HyAudioData(void)
{
}

IHyAudioInst *HyAudioData::AllocateNewInstance() const
{
	return m_AudioRef.AllocateAudioInst(m_InitObj);
}
