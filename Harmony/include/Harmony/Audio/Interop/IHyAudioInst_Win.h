/**************************************************************************
*	IHyAudioInst_Win.h
*
*	Harmony Engine
*	Copyright (c) 2016 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef IHyAudioInst_Win_h__
#define IHyAudioInst_Win_h__

#include "Audio/IHyAudioInst.h"

class IHyAudioInst_Win : public IHyAudioInst
{
public:
	IHyAudioInst_Win();
	virtual ~IHyAudioInst_Win();

	virtual void Play();
	virtual void Stop();

	virtual float GetPitch();
	virtual void SetPitch(float fPitch);

	virtual void SetReverb();
};

#endif /* IHyAudioInst_Win_h__ */
