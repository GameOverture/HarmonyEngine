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
	HyAudioHarness &		m_AudioRef;

	enum PlayListMode
	{
		PLAYLIST_Unknown = -1,
		PLAYLIST_Shuffle = 0,
		PLAYLIST_Weighted,
		PLAYLIST_SequentialLocal,
		PLAYLIST_SequentialGlobal,
	};
	PlayListMode			m_ePlayListMode;
	std::vector<uint32>		m_SoundChecksumList;

public:
	HyAudioData(const std::string &sPath, HyJsonObj itemDataObj, HyAssets &assetsRef);
	virtual ~HyAudioData(void);

	uint32 GetSound(IHyNode *pAudioNode) const;
};

#endif /* HyAudioData_h__ */
