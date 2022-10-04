/**************************************************************************
*	HyFileAudio.cpp
*	
*	Harmony Engine
*	Copyright (c) 2020 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Assets/Files/HyFileAudio.h"
#include "HyEngine.h"

HyFileAudio::HyFileAudio(std::string sFilePath, uint32 uiManifestIndex, HyAudioCore &coreRef, HyJsonObj bankObj) :
	IHyFile(sFilePath, HYFILE_AudioBank, uiManifestIndex)
{
	HyJsonArray assetsArray = bankObj["assets"].GetArray();
	for(uint32 i = 0; i < assetsArray.Size(); ++i)
	{
		HyJsonObj assetObj = assetsArray[i].GetObject();
		std::string sSoundFilePath = m_sFILE_NAME + "/";
		sSoundFilePath += assetObj["fileName"].GetString();
		HySoundBuffers *pNewBuffer = HY_NEW HySoundBuffers(coreRef, sSoundFilePath, assetObj["groupId"].GetInt(), assetObj["isStreaming"].GetBool(), assetObj["instanceLimit"].GetInt());

		m_SoundBuffers.push_back(pNewBuffer);
		m_ChecksumMap[assetObj["checksum"].GetUint()] = pNewBuffer;
	}
}

HyFileAudio::~HyFileAudio()
{
	for(uint32 i = 0; i < static_cast<uint32>(m_SoundBuffers.size()); ++i)
		delete m_SoundBuffers[i];
}

bool HyFileAudio::ContainsAsset(uint32 uiAssetChecksum)
{
	return m_ChecksumMap.find(uiAssetChecksum) != m_ChecksumMap.end();
}

HySoundBuffers *HyFileAudio::GetSound(uint32 uiChecksum)
{
	auto iter = m_ChecksumMap.find(uiChecksum);
	if(iter == m_ChecksumMap.end())
		return nullptr;

	return iter->second;
}

/*virtual*/ std::string HyFileAudio::AssetTypeName() /*override*/
{
	return "Audio";
}

/*virtual*/ void HyFileAudio::OnLoadThread() /*override*/
{
	if(GetLoadableState() == HYLOADSTATE_Queued)
	{
		for(uint32 i = 0; i < static_cast<uint32>(m_SoundBuffers.size()); ++i)
			m_SoundBuffers[i]->Load();
	}
	else
	{
		for(uint32 i = 0; i < static_cast<uint32>(m_SoundBuffers.size()); ++i)
			m_SoundBuffers[i]->Unload();
	}
}

/*virtual*/ void HyFileAudio::OnRenderThread(IHyRenderer &rendererRef) /*override*/
{
}
