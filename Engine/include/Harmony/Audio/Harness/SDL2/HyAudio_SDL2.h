/**************************************************************************
 *	HyAudio_SDL2.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2020 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyAudio_SDL2_h__
#define HyAudio_SDL2_h__

#include "Afx/HyStdAfx.h"
#include "Audio/Harness/IHyAudio.h"

class IHyAudioBank;
class IHyAudioInst;

#if defined(HY_USE_SDL2)
class HyAudio_SDL2 : public IHyAudio
{
	std::vector<std::string>	m_sDeviceList;

	SDL_AudioDeviceID			m_hDevice;
	SDL_AudioSpec				m_DesiredSpec;

	struct Buffer
	{
		uint32_t				length;
		uint32_t				lengthTrue;
		uint8_t *				bufferTrue;
		uint8_t *				buffer;
		uint8_t					loop;
		uint8_t					fade;
		uint8_t					free;
		uint8_t					volume;

		SDL_AudioSpec			audio;

		Buffer *				m_pNext;
	};
	Buffer						m_Buffer;
	uint32						m_uiSoundCount;

public:
	HyAudio_SDL2();
	virtual ~HyAudio_SDL2(void);

	const char *GetAudioDriver();

	virtual void OnUpdate() override;

	static IHyAudioBank *AllocateBank(IHyAudio *pAudio);
	static IHyAudioInst *AllocateInst(IHyAudio *pAudio, const char *szPath);

private:
	static void OnCallback(void *pUserData, uint8_t *pStream, int32 iLen);
};
#endif // defined(HY_USE_SDL2)

#endif /* HyAudio_SDL2_h__ */
