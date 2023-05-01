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

HyTexturedQuadData::HyTexturedQuadData(HyFileAtlas *pAtlas) :
	IHyNodeData("N/A"),
	m_pAtlas(pAtlas)
{
	m_RequiredAtlases.Set(m_pAtlas->GetManifestIndex());
}

HyTexturedQuadData::~HyTexturedQuadData()
{
}

HyFileAtlas *HyTexturedQuadData::GetAtlas() const
{
	return m_pAtlas;
}
