/**************************************************************************
*	HyAudio_Win.h
*
*	Harmony Engine
*	Copyright (c) 2016 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef __HyAudio_Win_h__
#define __HyAudio_Win_h__

#include "Afx/HyStdAfx.h"
#include "Audio/IHyAudio.h"
#include "IHyAudioInst_Win.h"

struct IXAudio2;
struct IXAudio2MasteringVoice;
struct IXAudio2SourceVoice;
struct IXAudio2SubmixVoice;
struct IUnknown;

class HyAudio_Win : public IHyAudio
{
	// XAudio2
#if ( _WIN32_WINNT < 0x0602 /*_WIN32_WINNT_WIN8*/)
	HMODULE					m_hXAudioDLL;
#endif

	IXAudio2 *				m_pXAudio2;
	IXAudio2MasteringVoice *m_pMasterVoice;

	DWORD					m_uiChannelMask;
	DWORD					m_uiSamplesPerSecond;
	uint32					m_uiNumChannels;

	enum eEffectType
	{
		EFFECT_MasterVolume = 0,
		EFFECT_Reverb,

		NUMEFFECTS
	};

	struct Effect
	{
		IUnknown *				pEffect;
		uint32					uiNumOutputChannels;

		IXAudio2SubmixVoice *	pSubmixVoice;
	};

	Effect						m_Effects[NUMEFFECTS];

public:
	HyAudio_Win(vector<HyWindow *> &windowListRef);
	virtual ~HyAudio_Win();

private:
	void CreateEffect();
};

#endif /* __HyAudio_Win_h__ */
