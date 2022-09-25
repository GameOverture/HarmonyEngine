/**************************************************************************
*	HySoundBuffer_SDL2.h
*	
*	Harmony Engine
*	Copyright (c) 2020 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HySoundBuffer_SDL2_h__
#define HySoundBuffer_SDL2_h__

#include "Afx/HyStdAfx.h"
#include "Audio/HyAudioBank.h"
#include "Diagnostics/Console/IHyConsole.h"
#include "Utilities/HyJson.h"

#if defined(HY_USE_SDL2)

#ifdef HY_PLATFORM_BROWSER
	#include <SDL2/SDL_mixer.h>
#else
	#include "SDL_mixer.h"
#endif

class HySoundBuffer_SDL2 : public IHySoundBuffer
{
	union BufferPtr
	{
		Mix_Chunk *				pSfx;
		Mix_Music *				pMusic;
	};
	BufferPtr					m_Buffer;

public:
	HySoundBuffer_SDL2(IHyAudioCore &coreRef, std::string sFileName, int32 iGroupId, bool bIsStreaming);
	virtual ~HySoundBuffer_SDL2();

	Mix_Chunk *GetSfxPtr();
	Mix_Music *GetMusicPtr();

	bool Load(std::string sFilePath) override;
	void Unload() override;
};

#endif // HY_USE_SDL2

#endif /* HySoundBuffer_SDL2_h__ */
