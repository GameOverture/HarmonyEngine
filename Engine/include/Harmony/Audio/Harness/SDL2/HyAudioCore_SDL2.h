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
#include "Audio/Harness/IHyAudioCore.h"

#include <unordered_map>

class IHyNode;
class IHyFileAudioImpl;

#if defined(HY_USE_SDL2)
class HyRawSoundBuffer;
class HyFileAudioImpl_SDL2;

#define HYMAX_AUDIOCHANNELS 32

class HyAudioCore_SDL2 : public IHyAudioCore
{
	static HyAudioCore_SDL2 *				sm_pInstance;

	std::vector<std::string>				m_sDeviceList;

	int32									m_iDesiredFrequency;	// 44100 or 48000, etc.
	SDL_AudioFormat							m_uiDesiredFormat;
	int32									m_iDesiredNumChannels;	// 1 mono, 2 stereo, 4 quad, 6 (5.1)
	int32									m_iDesiredSamples;		// Chunk size (4096, 2048, etc) - Specifies a unit of audio data to be used at a time. Must be a power of 2

	std::vector<HyFileAudioImpl_SDL2 *>		m_AudioFileList;

	float									m_fGlobalSfxVolume;
	float									m_fGlobalMusicVolume;

	// These are kept in sync
	std::unordered_map<IHyNode *, int32>	m_NodeMap;
	std::unordered_map<int32, IHyNode *>	m_ChannelMap;

public:
	HyAudioCore_SDL2();
	virtual ~HyAudioCore_SDL2(void);

	const char *GetAudioDriver();

	virtual void SetSfxVolume(float fGlobalSfxVolume) override;
	virtual void SetMusicVolume(float fGlobalMusicVolume) override;
	virtual IHyFileAudioImpl *AllocateAudioBank(HyJsonObj bankObj) override;

	virtual void OnUpdate() override;

private:
	template<typename NODETYPE>
	void Play(CueType ePlayType, NODETYPE *pAudioNode);

	template<typename NODETYPE>
	void Modify(NODETYPE *pAudioNode);

	static void OnChannelFinished(int32 iChannel);

	static int computeChunkLengthMillisec(int chunkSize);
	static void OnPitchModifer(int iChannel, void *pStream, int iLength, void *pData);
};
#endif // defined(HY_USE_SDL2)

#endif /* HyAudioCore_SDL2_h__ */
