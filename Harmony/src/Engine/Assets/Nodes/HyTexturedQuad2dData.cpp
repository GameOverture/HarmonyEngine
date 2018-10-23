/**************************************************************************
*	HyTexturedQuad2dData.h
*
*	Harmony Engine
*	Copyright (c) 2013 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Assets/Nodes/HyTexturedQuad2dData.h"
#include "Renderer/IHyRenderer.h"


HyTexturedQuad2dData::HyTexturedQuad2dData(uint32 uiAtlasGrpId, uint32 uiIndexInGroup, HyAssets &assetsRef) :	IHyNodeData("N/A"),
																												m_uiATLAS_GROUP_ID(uiAtlasGrpId),
																												m_uiINDEX_IN_GROUP(uiIndexInGroup),
																												m_pAtlas(nullptr)
{
	m_pAtlas = assetsRef.GetAtlasUsingGroupId(m_uiATLAS_GROUP_ID, m_uiINDEX_IN_GROUP);
	m_RequiredAtlasIndices.Set(m_pAtlas->GetMasterIndex());
}

HyTexturedQuad2dData::~HyTexturedQuad2dData()
{
}

HyAtlas *HyTexturedQuad2dData::GetAtlas() const
{
	return m_pAtlas;
}
