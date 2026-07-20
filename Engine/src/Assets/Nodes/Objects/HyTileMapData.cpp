/**************************************************************************
 *	HyTileMapData.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2026 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Assets/Nodes/Objects/HyTileMapData.h"
#include "Assets/Files/HyFileAtlas.h"

HyTileMapData::HyTileMapData(const HyNodePath &nodePath, HyJsonObj itemDataObj, HyAssets &assetsRef) :
	IHyNodeData(nodePath)
{
	HyUvCoord rSubAtlasUVRect;
	uint64 uiCropMask = 0;
	m_pAtlas = assetsRef.GetAtlas(itemDataObj["checksum"].GetUint(), itemDataObj["bankId"].GetUint(), rSubAtlasUVRect, uiCropMask);

	for(TileSet &tileSetRef : m_TileSetList)
	{
		m_RequiredFiles[HYFILE_Atlas].Set(tileSetRef.m_pAtlas->GetManifestIndex());
		m_RequiredFiles[HYFILE_Atlas].Set(tileSetRef.m_pShaderDescriptor->GetManifestIndex());
	}
}

HyTileMapData::~HyTileMapData(void)
{
}

int HyTileMapData::GetNumTileMaps() const
{
	return m_TileMapList.size();
}

float HyTileMapData::GetTileMapWidth(int iTileMapIndex) const
{
	return m_TileMapList[iTileMapIndex].m_fTotalWidth;
}

float HyTileMapData::GetTileMapHeight(int iTileMapIndex) const
{
	return m_TileMapList[iTileMapIndex].m_fTotalHeight;
}
