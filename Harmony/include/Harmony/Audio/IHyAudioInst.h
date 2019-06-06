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

#endif /* IHyAudioInst_h__ */
