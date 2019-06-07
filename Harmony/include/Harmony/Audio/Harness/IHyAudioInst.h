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

class IHyAudioInst
{
public:
	IHyAudioInst() = default;
	virtual ~IHyAudioInst(void) = default;

	virtual void Play() = 0;
	virtual void Stop() = 0;

	virtual float GetPitch() = 0;
	virtual void SetPitch(float fPitch) = 0;

	virtual void SetReverb() = 0;
};

class HyAudioInst_Null : public IHyAudioInst
{
public:
	HyAudioInst_Null() = default;
	virtual ~HyAudioInst_Null() = default;

	virtual void Play() override { }
	virtual void Stop() override { }

	virtual float GetPitch() override { return 0.0f; }
	virtual void SetPitch(float fPitch) override { }

	virtual void SetReverb() override { }
};

#endif /* IHyAudioInst_h__ */
