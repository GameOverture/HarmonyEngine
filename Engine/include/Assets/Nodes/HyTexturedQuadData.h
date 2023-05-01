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
	HyFileAtlas *				m_pAtlas;

public:
	HyTexturedQuadData(HyFileAtlas *pAtlas);
	virtual ~HyTexturedQuadData();

	HyFileAtlas *GetAtlas() const;
};

#endif /* HyTexturedQuad2dData_h__ */
