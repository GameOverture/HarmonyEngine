/**************************************************************************
*	HyAudioInst_SDL2.h
*	
*	Harmony Engine
*	Copyright (c) 2020 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyAudioInst_SDL2_h__
#define HyAudioInst_SDL2_h__

#include "Audio/Harness/IHyAudioInst.h"

class HyAudioInst_SDL2 : public IHyAudioInst
{
	uint32_t length;
	uint32_t lengthTrue;
	uint8_t * bufferTrue;
	uint8_t * buffer;
	uint8_t loop;
	uint8_t fade;
	uint8_t free;
	uint8_t volume;

	SDL_AudioSpec audio;

	HyAudioInst_SDL2 *		m_pNext;

public:
	HyAudioInst_SDL2();
	virtual ~HyAudioInst_SDL2();

	virtual void OnLoaded() override;

	virtual void PlayOneShot() override;
	virtual void Start() override;
	virtual void Stop(HyAudioStop eStopType = HYAUDIOSTOP_AllowFadeOut) override;

	virtual bool IsPaused() const override;
	virtual void SetPause(bool bPause) override;

	virtual float GetVolume(float *fFinalVolumeOut = nullptr) const override;
	virtual void SetVolume(float fVolume) override;

	virtual float GetPitch(float *fFinalPitchOut = nullptr) const override;
	virtual void SetPitch(float fPitch) override;

	virtual int GetTimelinePosition() const override;
	virtual void SetTimelinePosition(int iPosition) override;

	virtual float GetParam(const char *szParam, float *fFinalValueOut = nullptr) const override;
	virtual void SetParam(const char *szParam, float fValue) override;

	virtual float GetReverb(int iIndex) const override;
	virtual void SetReverb(int iIndex, float fLevel) override;
};

#endif /* HyAudioInst_SDL2_h__ */
