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

#if defined(HY_USE_SDL2)
class HyAudioInst_SDL2 : public IHyAudioInst
{
public:
	HyAudioInst_SDL2(const jsonxx::Object &instObjRef);
	virtual ~HyAudioInst_SDL2();

	virtual void OnLoaded() override;

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
#endif // defined(HY_USE_SDL2)

#endif /* HyAudioInst_SDL2_h__ */
