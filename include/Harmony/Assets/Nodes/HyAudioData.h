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
#include "Audio/Harness/IHyAudioInst.h"

class HyAudioData : public IHyNodeData
{
	HyAudio &		m_AudioRef;

public:
	HyAudioData(const std::string &sPath, const jsonxx::Object &itemDataObjRef, HyAssets &assetsRef);
	virtual ~HyAudioData(void);

	IHyAudioInst *AllocateNewInstance() const;
};

#endif /* HyAudioData_h__ */
