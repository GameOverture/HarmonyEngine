/**************************************************************************
 *	HyPrefabData.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyPrefabData_h__
#define HyPrefabData_h__

#include "Afx/HyStdAfx.h"

#include "Assets/Nodes/IHyNodeData.h"
#include "Assets/Loadables/HyAtlas.h"

class HyPrefabData : public IHyNodeData
{
	//const aiScene *		m_pAiScene;

public:
	HyPrefabData(const std::string &sPath);
	virtual ~HyPrefabData(void);
};

#endif /* HyPrefabData_h__ */
