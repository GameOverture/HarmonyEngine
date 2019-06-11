/**************************************************************************
 *	HyAudio2d.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyAudio2d_h__
#define HyAudio2d_h__

#include "Afx/HyInteropAfx.h"
#include "Assets/Nodes/HyAudioData.h"
#include "Scene/Nodes/Loadables/IHyLoadable2d.h"
#include "Audio/Harness/IHyAudioInst.h"

class HyAudio2d : public IHyLoadable2d
{
	IHyAudioInst *		m_pInternal;

public:
	HyAudio2d(const char *szPrefix, const char *szName, HyEntity2d *pParent);
	virtual ~HyAudio2d(void);

	void Start();
	void Stop();

	float GetPitch();
	void SetPitch(float fPitch);

	void SetReverb(int iIndex, float fLevel);

protected:
	virtual void OnDataAcquired() override;
	virtual void OnLoaded() override;
};

#endif /* HyAudio2d_h__ */
