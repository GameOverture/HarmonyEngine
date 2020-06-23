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

class IHyFileAudioGuts;
class IHyAudioInst;

#if defined(HY_USE_SDL2)
class HyAudioInst_SDL2;
class HyFileAudioGuts_SDL2;

class HyAudio_SDL2 : public IHyAudio
{
	std::vector<std::string>			m_sDeviceList;

	SDL_AudioDeviceID					m_hDevice;
	SDL_AudioSpec						m_DesiredSpec;

	std::vector<HyFileAudioGuts_SDL2 *>	m_AudioFileList;

	struct Play
	{
		HyAudioInst_SDL2 *				m_pInst;
		uint8_t *						m_pBufferPos;
		uint32							m_uiRemainingBytes;
		SDL_AudioSpec					m_AudioSpec;
	};
	std::vector<Play>					m_PlayList;

public:
	HyAudio_SDL2();
	virtual ~HyAudio_SDL2(void);

	const char *GetAudioDriver();

	virtual void OnUpdate() override;

	void QueueInst(HyAudioInst_SDL2 *pInst, uint32 uiSoundChecksum);

	static IHyFileAudioGuts *AllocateBank(IHyAudio *pAudio, const jsonxx::Object &bankObjRef);
	static IHyAudioInst *AllocateInst(IHyAudio *pAudio, const jsonxx::Object &instObjRef);

private:
	static void OnCallback(void *pUserData, uint8_t *pStream, int32 iLen);
};
#endif // defined(HY_USE_SDL2)

#endif /* HyAudio_SDL2_h__ */
