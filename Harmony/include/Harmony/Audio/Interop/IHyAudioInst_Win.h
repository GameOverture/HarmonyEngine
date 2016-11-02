/**************************************************************************
*	IHyAudioInst_Win.h
*
*	Harmony Engine
*	Copyright (c) 2016 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef __IHyAudioInst_Win_h__
#define __IHyAudioInst_Win_h__

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

#endif /* __IHyAudioInst_Win_h__ */
