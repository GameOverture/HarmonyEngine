/**************************************************************************
*	HyAudioBank.cpp
*	
*	Harmony Engine
*	Copyright (c) 2019 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyInteropAfx.h"
#include "Audio/HyAudioBank.h"

HyAudioBank::HyAudioBank(IHyAudioCore &coreRef, HyJsonObj bankObj)
{
	HyJsonArray assetsArray = bankObj["assets"].GetArray();
	for(uint32 i = 0; i < assetsArray.Size(); ++i)
	{
		HyJsonObj assetObj = assetsArray[i].GetObject();
		IHySoundBuffer *pNewBuffer = HY_NEW HySoundBufferInterop(coreRef, assetObj["fileName"].GetString(), assetObj["groupId"].GetInt(), assetObj["isStreaming"].GetBool(), assetObj["instanceLimit"].GetInt());

		m_SoundBuffers.push_back(pNewBuffer);
		m_ChecksumMap[assetObj["checksum"].GetUint()] = pNewBuffer;
	}
}

/*virtual*/ HyAudioBank::~HyAudioBank()
{
	Unload();

	for(uint32 i = 0; i < static_cast<uint32>(m_SoundBuffers.size()); ++i)
		delete m_SoundBuffers[i];
}

/*virtual*/ bool HyAudioBank::ContainsSound(uint32 uiAssetChecksum)
{
	return m_ChecksumMap.find(uiAssetChecksum) != m_ChecksumMap.end();
}

IHySoundBuffer *HyAudioBank::GetSound(uint32 uiChecksum)
{
	auto iter = m_ChecksumMap.find(uiChecksum);
	if(iter == m_ChecksumMap.end())
		return nullptr;

	return iter->second;
}

/*virtual*/ bool HyAudioBank::Load(std::string sFilePath)
{
	bool bAllLoaded = true;
	for(uint32 i = 0; i < static_cast<uint32>(m_SoundBuffers.size()); ++i)
	{
		if(m_SoundBuffers[i]->Load(sFilePath) == false)
			bAllLoaded = false;
	}

	return bAllLoaded;
}

/*virtual*/ void HyAudioBank::Unload()
{
	for(uint32 i = 0; i < m_SoundBuffers.size(); ++i)
		m_SoundBuffers[i]->Unload();
}
