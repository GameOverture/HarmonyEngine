/**************************************************************************
 *	IHyAudio.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2021 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef IHyAudio_h__
#define IHyAudio_h__

#include "Afx/HyStdAfx.h"
#include "Scene/AnimFloats/HyAnimFloat.h"

template<typename NODETYPE, typename ENTTYPE>
class IHyAudio : public NODETYPE
{
	uint32						m_uiCueFlags;
	float						m_fVolume;
	float						m_fPitch;
	
	// Configurable
	HyPlayListMode				m_ePlayListMode;
	int32						m_iPriority;
	int32						m_iLoops;
	uint32						m_uiMaxDistance;

public:
	HyAnimFloat					volume;
	HyAnimFloat					pitch;

public:
	IHyAudio(std::string sPrefix, std::string sName, ENTTYPE *pParent);

	// TODO: copy ctor and move ctor
	virtual ~IHyAudio(void);
	// TODO: assignment operator and move operator

	int32 GetLoops() const;
	void SetLoops(int32 iLoops);
	void PlayOneShot(bool bUseCurrentSettings /*= true*/);
	void Play();
	void Stop();
	void SetPause(bool bPause);

	virtual bool IsLoadDataValid() override;

protected:
	virtual void OnDataAcquired() override;
	virtual void OnLoadedUpdate() override;
};

#endif /* IHyAudio_h__ */
