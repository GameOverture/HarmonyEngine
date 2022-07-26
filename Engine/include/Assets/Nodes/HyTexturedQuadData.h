/**************************************************************************
*	HyTexturedQuadData.h
*
*	Harmony Engine
*	Copyright (c) 2013 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyTexturedQuadData_h__
#define HyTexturedQuadData_h__

#include "Afx/HyStdAfx.h"

#include "Assets/Nodes/IHyNodeData.h"
#include "Assets/HyAssets.h"

class HyTexturedQuadData : public IHyNodeData
{
	const uint32		m_uiATLAS_GROUP_ID;
	const uint32		m_uiINDEX_IN_GROUP;
	HyFileAtlas *		m_pAtlas;

public:
	HyTexturedQuadData(uint32 uiAtlasGrpId, uint32 uiIndexInGroup, HyAssets &assetsRef);
	virtual ~HyTexturedQuadData();

	HyFileAtlas *GetAtlas() const;
};

#endif /* HyTexturedQuad2dData_h__ */
