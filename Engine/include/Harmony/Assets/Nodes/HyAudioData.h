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

class HyAudioData : public IHyNodeData
{
	HyAudioHarness &		m_AudioRef;
	const jsonxx::Object	m_InitObj;

	enum CueType
	{
		CUETYPE_Unknown = -1,
		CUETYPE_Single = 0
	};
	CueType					m_eCueType;
	std::vector<uint32>		m_SoundChecksumList;

public:
	HyAudioData(const std::string &sPath, const jsonxx::Object &itemDataObjRef, HyAssets &assetsRef);
	virtual ~HyAudioData(void);

	uint32 GetSound() const;
};

#endif /* HyAudioData_h__ */
