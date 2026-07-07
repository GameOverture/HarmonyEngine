/**************************************************************************
 *	HyTileSetData.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2026 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Assets/Nodes/Objects/HyTileSetData.h"
#include "Assets/Files/HyFileAtlas.h"

HyTileSetData::HyTileSetData(const HyNodePath &nodePath, HyJsonObj itemDataObj, HyAssets &assetsRef) :
	IHyNodeData(nodePath)
{
	HyUvCoord rSubAtlasUVRect;
	uint64 uiCropMask = 0;
	m_pAtlas = assetsRef.GetAtlas(itemDataObj["checksum"].GetUint(), itemDataObj["bankId"].GetUint(), rSubAtlasUVRect, uiCropMask);

	m_RequiredFiles[HYFILE_Atlas].Set(m_pAtlas->GetManifestIndex());
}

HyTileSetData::~HyTileSetData(void)
{
}

HyFileAtlas *HyTileSetData::GetAtlas() const
{
	return m_pAtlas;
}
