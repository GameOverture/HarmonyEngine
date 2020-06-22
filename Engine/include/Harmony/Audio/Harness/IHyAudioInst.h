/**************************************************************************
*	IHyAudioInst.h
*	
*	Harmony Engine
*	Copyright (c) 2019 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef IHyAudioInst_h__
#define IHyAudioInst_h__

enum HyAudioStop
{
	HYAUDIOSTOP_AllowFadeOut = 0,
	HYAUDIOSTOP_Immediate
};

class IHyAudioInst
{
public:
	IHyAudioInst() = default;
	virtual ~IHyAudioInst(void) = default;

	virtual void OnLoaded() = 0;

	virtual void Start() = 0;
	virtual void Stop(HyAudioStop eStopType = HYAUDIOSTOP_AllowFadeOut) = 0;
	
	virtual bool IsPaused() const = 0;
	virtual void SetPause(bool bPause) = 0;

	virtual float GetVolume(float *fFinalVolumeOut = nullptr) const = 0;
	virtual void SetVolume(float fVolume) = 0;

	virtual float GetPitch(float *fFinalPitchOut = nullptr) const = 0;
	virtual void SetPitch(float fPitch) = 0;

	virtual int GetTimelinePosition() const = 0;
	virtual void SetTimelinePosition(int iPosition) = 0;

	virtual float GetParam(const char *szParam, float *fFinalValueOut = nullptr) const = 0;
	virtual void SetParam(const char *szParam, float fValue) = 0;

	virtual float GetReverb(int iIndex) const = 0;
	virtual void SetReverb(int iIndex, float fLevel) = 0;
};

class HyAudioInst_Null : public IHyAudioInst
{
public:
	HyAudioInst_Null() = default;
	virtual ~HyAudioInst_Null() = default;

	virtual void OnLoaded() override { }

	virtual void Start() override { }
	virtual void Stop(HyAudioStop eStopType = HYAUDIOSTOP_AllowFadeOut) override { }

	virtual bool IsPaused() const override { return false; }
	virtual void SetPause(bool bPause) override { }

	virtual float GetVolume(float *fFinalVolumeOut = nullptr) const override { return 0.0f; }
	virtual void SetVolume(float fVolume) override { }

	virtual float GetPitch(float *fFinalPitchOut = nullptr) const override { return 0.0f; }
	virtual void SetPitch(float fPitch) override { }

	virtual int GetTimelinePosition() const override { return 0; }
	virtual void SetTimelinePosition(int iPosition) override { }

	virtual float GetParam(const char *szParam, float *fFinalValueOut = nullptr) const override { return 0.0f; }
	virtual void SetParam(const char *szParam, float fValue) override { }

	virtual float GetReverb(int iIndex) const override { return 0.0f; }
	virtual void SetReverb(int iIndex, float fLevel) override { }
};

#endif /* IHyAudioInst_h__ */
