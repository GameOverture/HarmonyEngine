/**************************************************************************
*	HyFileAudioImpl_SDL2.h
*	
*	Harmony Engine
*	Copyright (c) 2020 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyAudioBank_SDL2_h__
#define HyAudioBank_SDL2_h__

#include "Afx/HyStdAfx.h"
#include "Audio/Harness/IHyFileAudioImpl.h"
#include "Diagnostics/Console/HyConsole.h"

#if defined(HY_USE_SDL2)

#ifdef HY_PLATFORM_BROWSER
	#include <SDL2/SDL_mixer.h>
#else
	#include "SDL_mixer.h"
#endif

class HyRawSoundBuffer
{
	const std::string			m_sFILE_NAME;
	const bool					m_bIS_MUSIC;
	union BufferPtr
	{
		Mix_Chunk *				pSfx;
		Mix_Music *				pMusic;
	};
	BufferPtr					m_Buffer;

public:
	HyRawSoundBuffer(std::string sFileName, bool bIsMusic) :
		m_sFILE_NAME(sFileName),
		m_bIS_MUSIC(bIsMusic)
	{
		m_Buffer.pSfx = nullptr;
	}

	~HyRawSoundBuffer()
	{
		Unload();
	}

	bool IsMusic() const {
		return m_bIS_MUSIC;
	}
	Mix_Chunk *GetSfxPtr() {
		return m_Buffer.pSfx;
	}
	Mix_Music *GetMusicPtr() {
		return m_Buffer.pMusic;
	}

	bool Load(std::string sFilePath) {
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

	void Unload() {
		if(m_Buffer.pSfx != nullptr)
		{
			if(m_bIS_MUSIC)
				Mix_FreeMusic(m_Buffer.pMusic);
			else
				Mix_FreeChunk(m_Buffer.pSfx);

			m_Buffer.pSfx = nullptr;
		}
	}
};

class HyFileAudioImpl_SDL2 : public IHyFileAudioImpl
{
	std::vector<HyRawSoundBuffer *>			m_SoundBuffers;
	std::map<uint32, HyRawSoundBuffer *>	m_ChecksumMap;

public:
	HyFileAudioImpl_SDL2(HyJsonObj bankObj);
	virtual ~HyFileAudioImpl_SDL2();

	virtual bool ContainsAsset(uint32 uiAssetChecksum) override;

	virtual bool Load(std::string sFilePath) override;
	virtual void Unload() override;

	HyRawSoundBuffer *GetBufferInfo(uint32 uiChecksum);
};
#endif // HY_USE_SDL2

#endif /* HyAudioBank_SDL2_h__ */
