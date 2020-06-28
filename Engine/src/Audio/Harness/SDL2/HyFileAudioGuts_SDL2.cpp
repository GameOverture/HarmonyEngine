/**************************************************************************
*	HyFileAudioGuts_SDL2.h
*
*	Harmony Engine
*	Copyright (c) 2020 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Audio/Harness/SDL2/HyFileAudioGuts_SDL2.h"
#include "Diagnostics/Console/HyConsole.h"
#include "Utilities/HyIO.h"

#if defined(HY_USE_SDL2)

HyFileAudioGuts_SDL2::HyFileAudioGuts_SDL2(const jsonxx::Object &bankObjRef)
{
	const jsonxx::Array &assetsArray = bankObjRef.get<jsonxx::Array>("assets");
	for(uint32 i = 0; i < assetsArray.size(); ++i)
	{
		jsonxx::Object assetObj = assetsArray.get<jsonxx::Object>(i);

		HyRawSoundBuffer *pNewBuffer = HY_NEW HyRawSoundBuffer(assetObj.get<jsonxx::String>("fileName"));
		m_SoundBuffers.push_back(pNewBuffer);
		m_ChecksumMap[static_cast<uint32>(assetObj.get<jsonxx::Number>("checksum"))] = pNewBuffer;
	}
}

/*virtual*/ HyFileAudioGuts_SDL2::~HyFileAudioGuts_SDL2()
{
	Unload();

	for(uint32 i = 0; i < static_cast<uint32>(m_SoundBuffers.size()); ++i)
		delete m_SoundBuffers[i];
}

/*virtual*/ bool HyFileAudioGuts_SDL2::ContainsAsset(uint32 uiAssetChecksum) /*override*/
{
	return m_ChecksumMap.find(uiAssetChecksum) != m_ChecksumMap.end();
}

/*virtual*/ bool HyFileAudioGuts_SDL2::Load(std::string sFilePath) /*override*/
{
	bool bAllLoaded = true;
	for(uint32 i = 0; i < static_cast<uint32>(m_SoundBuffers.size()); ++i)
	{
		if(m_SoundBuffers[i]->Load(sFilePath) == false)
			bAllLoaded = false;
	}

	return bAllLoaded;
}

/*virtual*/ void HyFileAudioGuts_SDL2::Unload() /*override*/
{
	for(uint32 i = 0; i < m_SoundBuffers.size(); ++i)
		m_SoundBuffers[i]->Unload();
}

HyRawSoundBuffer *HyFileAudioGuts_SDL2::GetBufferInfo(uint32 uiChecksum)
{
	auto iter = m_ChecksumMap.find(uiChecksum);
	if(iter == m_ChecksumMap.end())
		return nullptr;

	return iter->second;
}

#endif // defined(HY_USE_SDL2)
