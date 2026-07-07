/**************************************************************************
 *	HyTileSetData.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2026 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyTileSetData_h__
#define HyTileSetData_h__

#include "Afx/HyStdAfx.h"

#include "Assets/Nodes/IHyNodeData.h"
#include "Assets/HyAssets.h"

class HyTileSetData : public IHyNodeData
{
	HyFileAtlas *					m_pAtlas;

public:
	HyTileSetData(const HyNodePath &nodePath, HyJsonObj itemDataObj, HyAssets &assetsRef);
	virtual ~HyTileSetData();

	HyFileAtlas *GetAtlas() const;
};

#endif /* HyTileSetData_h__ */
