/**************************************************************************
 *	HyAudioData.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyAudioData_h__
#define HyAudioData_h__

#include "Afx/HyStdAfx.h"
#include "Assets/Nodes/IHyNodeData.h"
#include "Assets/HyAssets.h"
#include "Scene/Nodes/IHyNode.h"

class HyAudioData : public IHyNodeData
{
	class AudioState
	{
	public:
		std::vector<uint32>		m_PlayList;	// Stores checksums

		HyPlayListMode			m_ePlayListMode;
		float					m_fVolume;
		float					m_fPitch;
		int32					m_iPriority;
		int32					m_iLoops;
		uint32					m_uiMaxDistance;
	};
	AudioState *				m_pAudioStates;

public:
	HyAudioData(const std::string &sPath, HyJsonObj itemDataObj, HyAssets &assetsRef);
	virtual ~HyAudioData(void);

	HyPlayListMode GetPlayListMode(uint32 uiStateIndex) const;
	int32 GetPriority(uint32 uiStateIndex) const;
	int32 GetLoops(uint32 uiStateIndex) const;
	uint32 GetMaxDistance(uint32 uiStateIndex) const;
	float GetVolume(uint32 uiStateIndex) const;
	float GetPitch(uint32 uiStateIndex) const;

	uint32 GetSound(IHyNode *pAudioNode) const;
};

#endif /* HyAudioData_h__ */
