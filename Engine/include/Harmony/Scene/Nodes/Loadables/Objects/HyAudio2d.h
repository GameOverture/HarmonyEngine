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
	HyAudio2d(std::string sPrefix = "", std::string sName = "", HyEntity2d *pParent = nullptr);
	// TODO: copy ctor and move ctor
	virtual ~HyAudio2d(void);

	// TODO: assignment operator and move operator

	void Start();
	void Stop();

	HyAudioCtrl GetState() const;

	bool IsPaused();
	void SetPause(bool bPause);

	float GetPitch();
	void SetPitch(float fPitch);

	void SetReverb(int iIndex, float fLevel);

protected:
	virtual void OnDataAcquired() override;
	virtual void OnLoaded() override;
};

#endif /* HyAudio2d_h__ */
