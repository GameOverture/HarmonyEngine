/**************************************************************************
*	HyTexturedQuadData.h
*
*	Harmony Engine
*	Copyright (c) 2013 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Assets/Nodes/HyTexturedQuadData.h"
#include "Renderer/IHyRenderer.h"


HyTexturedQuadData::HyTexturedQuadData(uint32 uiAtlasGrpId, uint32 uiIndexInGroup, HyAssets &assetsRef) :
	IHyNodeData("N/A"),
	m_uiATLAS_GROUP_ID(uiAtlasGrpId),
	m_uiINDEX_IN_GROUP(uiIndexInGroup),
	m_pAtlas(nullptr)
{
	m_pAtlas = assetsRef.GetAtlasUsingGroupId(m_uiATLAS_GROUP_ID, m_uiINDEX_IN_GROUP);
	m_RequiredAtlases.Set(m_pAtlas->GetManifestIndex());
}

HyTexturedQuadData::~HyTexturedQuadData()
{
}

HyFileAtlas *HyTexturedQuadData::GetAtlas() const
{
	return m_pAtlas;
}
