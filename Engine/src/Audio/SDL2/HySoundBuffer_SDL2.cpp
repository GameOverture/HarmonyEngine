/**************************************************************************
*	HySoundBuffer_SDL2.h
*
*	Harmony Engine
*	Copyright (c) 2020 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Audio/SDL2/HySoundBuffer_SDL2.h"
#include "Diagnostics/Console/IHyConsole.h"
#include "Utilities/HyIO.h"

#if defined(HY_USE_SDL2)

HySoundBuffer_SDL2::HySoundBuffer_SDL2(IHyAudioCore &coreRef, std::string sFileName, bool bIsMusic) :
	IHySoundBuffer(coreRef, sFileName, bIsMusic)
{
	m_Buffer.pSfx = nullptr;
}

/*virtual*/ HySoundBuffer_SDL2::~HySoundBuffer_SDL2()
{
	Unload();
}

Mix_Chunk *HySoundBuffer_SDL2::GetSfxPtr()
{
	return m_Buffer.pSfx;
}

Mix_Music *HySoundBuffer_SDL2::GetMusicPtr()
{
	return m_Buffer.pMusic;
}

/*virtual*/ bool HySoundBuffer_SDL2::Load(std::string sFilePath) /*override*/
{
	std::string s = sFilePath;
	s += "/";
	s += m_sFILE_NAME;

	if(m_bIS_MUSIC)
	{
		m_Buffer.pMusic = Mix_LoadMUS(s.c_str());
		if(m_Buffer.pMusic == nullptr)
		{
			HyLogError("Load music failed on: " << s.c_str() << " Error: " << SDL_GetError());
			return false;
		}
	}
	else
	{
		m_Buffer.pSfx = Mix_LoadWAV(s.c_str()); // This can load more than just WAV formats.
		if(m_Buffer.pSfx == nullptr)
		{
			HyLogError("Load sound failed on: " << s.c_str() << " Error: " << SDL_GetError());
			return false;
		}
	}

	return true;
}

/*virtual*/ void HySoundBuffer_SDL2::Unload() /*override*/
{
	if(m_Buffer.pSfx != nullptr)
	{
		if(m_bIS_MUSIC)
			Mix_FreeMusic(m_Buffer.pMusic);
		else
			Mix_FreeChunk(m_Buffer.pSfx);

		m_Buffer.pSfx = nullptr;
	}
}

#endif // defined(HY_USE_SDL2)
