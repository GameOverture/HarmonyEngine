/**************************************************************************
*	HyAudioBank_SDL2.h
*
*	Harmony Engine
*	Copyright (c) 2020 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Audio/Harness/SDL2/HyAudioBank_SDL2.h"
#include "Diagnostics/Console/HyConsole.h"
#include "Utilities/HyIO.h"

#if defined(HY_USE_SDL2)

HyAudioBank_SDL2::HyAudioBank_SDL2(const jsonxx::Object &bankObjRef)
{
	const jsonxx::Array &assetsArray = bankObjRef.get<jsonxx::Array>("assets");
	for(uint32 i = 0; i < assetsArray.size(); ++i)
	{
		jsonxx::Object assetObj = assetsArray.get<jsonxx::Object>(i);
		m_SoundBuffers.emplace_back(assetObj.get<jsonxx::String>("fileName"));
		m_ChecksumMap[static_cast<uint32>(assetObj.get<jsonxx::Number>("checksum"))] = &m_SoundBuffers.back();
	}
}

/*virtual*/ HyAudioBank_SDL2::~HyAudioBank_SDL2()
{
}

/*virtual*/ bool HyAudioBank_SDL2::Load(std::string sFilePath) /*override*/
{
	for(uint32 i = 0; i < static_cast<uint32>(m_SoundBuffers.size()); ++i)
	{
		std::string s = sFilePath;
		s += "/";
		s += m_SoundBuffers[i].m_sFileName;

		if(SDL_LoadWAV(s.c_str(),
					   &m_SoundBuffers[i].m_Spec,
					   &m_SoundBuffers[i].m_pBuffer,
					   &m_SoundBuffers[i].m_uiBufferSize) == nullptr)
		{
			HyLogError("HyAudioBank_SDL2::Load SDL_LoadWAV failed: " << SDL_GetError());
			return false;
		}
	}

	return true;
}

/*virtual*/ void HyAudioBank_SDL2::Unload() /*override*/
{
	for(uint32 i = 0; i < m_SoundBuffers.size(); ++i)
		SDL_FreeWAV(m_SoundBuffers[i].m_pBuffer);

	m_SoundBuffers.clear();
}

#endif // defined(HY_USE_SDL2)
