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

		Buffer *pNewBuffer = HY_NEW Buffer(assetObj.get<jsonxx::String>("fileName"));
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
	for(uint32 i = 0; i < static_cast<uint32>(m_SoundBuffers.size()); ++i)
	{
		std::string s = sFilePath;
		s += "/";
		s += m_SoundBuffers[i]->m_sFileName;

		if(SDL_LoadWAV(s.c_str(),
					   &m_SoundBuffers[i]->m_Spec,
					   &m_SoundBuffers[i]->m_pBuffer,
					   &m_SoundBuffers[i]->m_uiBufferSize) == nullptr)
		{
			HyLogError("HyFileAudioGuts_SDL2::Load SDL_LoadWAV failed: " << SDL_GetError());
			return false;
		}
	}

	return true;
}

/*virtual*/ void HyFileAudioGuts_SDL2::Unload() /*override*/
{
	for(uint32 i = 0; i < m_SoundBuffers.size(); ++i)
		SDL_FreeWAV(m_SoundBuffers[i]->m_pBuffer);

	m_SoundBuffers.clear();
}

bool HyFileAudioGuts_SDL2::GetBufferInfo(uint32 uiChecksum, uint8_t *&pBufferOut, uint32 &uiSizeOut, SDL_AudioSpec &audioSpecOut)
{
	auto iter = m_ChecksumMap.find(uiChecksum);
	if(iter == m_ChecksumMap.end())
		return false;

	pBufferOut = iter->second->m_pBuffer;
	uiSizeOut = iter->second->m_uiBufferSize;
	audioSpecOut = iter->second->m_Spec;

	return true;
}

#endif // defined(HY_USE_SDL2)
