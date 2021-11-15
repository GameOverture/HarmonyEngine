/**************************************************************************
 *	HyAudioCore_SDL2.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2020 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyAudioCore_SDL2_h__
#define HyAudioCore_SDL2_h__

#include "Afx/HyStdAfx.h"
#include "Audio/IHyAudioCore.h"

class IHyFileAudioImpl;

#if defined(HY_USE_SDL2)
class HyRawSoundBuffer;
class HyFileAudioImpl_SDL2;

#define HYMAX_AUDIOCHANNELS 32

class HyAudioCore_SDL2 : public IHyAudioCore
{
	static HyAudioCore_SDL2 *					sm_pInstance;

	std::vector<std::string>					m_sDeviceList;

	int32										m_iDesiredFrequency;	// 44100 or 48000, etc.
	SDL_AudioFormat								m_uiDesiredFormat;
	int32										m_iDesiredNumChannels;	// 1 mono, 2 stereo, 4 quad, 6 (5.1)
	int32										m_iDesiredSamples;		// Chunk size (4096, 2048, etc) - Specifies a unit of audio data to be used at a time. Must be a power of 2

	std::unordered_map<int32, HyAudioHandle>	m_ChannelMap;

public:
	HyAudioCore_SDL2();
	virtual ~HyAudioCore_SDL2(void);

	const char *GetAudioDriver();

protected:
	virtual IHyFileAudioImpl *OnAllocateAudioBank(HyJsonObj bankObj) override;
	virtual void OnSetSfxVolume(float fGlobalSfxVolume) override;
	virtual void OnSetMusicVolume(float fGlobalMusicVolume) override;

	virtual void OnCue_Play(HyAudioHandle hHandle, PlayInfo &playInfoRef) override;
	virtual void OnCue_Stop(HyAudioHandle hHandle) override;
	virtual void OnCue_Pause(HyAudioHandle hHandle) override;
	virtual void OnCue_Unpause(HyAudioHandle hHandle) override;
	virtual void OnCue_Volume(HyAudioHandle hHandle) override;

private:
	static void OnChannelFinished(int32 iChannel);

	//static int computeChunkLengthMillisec(int chunkSize);
	//static void OnPitchModifer(int iChannel, void *pStream, int iLength, void *pData);
};
#endif // defined(HY_USE_SDL2)

#endif /* HyAudioCore_SDL2_h__ */
