/**************************************************************************
*	IHyAudioInst.h
*
*	Harmony Engine
*	Copyright (c) 2016 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef __IHyAudioInst_h__
#define __IHyAudioInst_h__

class IHyAudioInst
{
public:
	IHyAudioInst();
	virtual ~IHyAudioInst();

	virtual void Play() = 0;
	virtual void Stop() = 0;

	virtual float GetPitch() = 0;
	virtual void SetPitch(float fPitch) = 0;

	virtual void SetReverb() = 0;
};

#endif /* __IHyAudioInst_h__ */
