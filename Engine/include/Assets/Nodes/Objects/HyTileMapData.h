/**************************************************************************
 *	HyTileMapData.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2026 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyTileMapData_h__
#define HyTileMapData_h__

#include "Afx/HyStdAfx.h"

#include "Assets/Nodes/IHyNodeData.h"
#include "Assets/HyAssets.h"

//struct TileData	// Tile data required by the CPU
//{
//	int16				m_iDisplayOrder;
//	int16				m_SortOriginY;
//};
//std::vector<TileData>	m_TileDataList;

class HyTileMapData : public IHyNodeData
{
	std::vector<HyFileTileSet *>	m_DescriptorTileSetList;	// All potential tile set descriptor textures
	std::vector<HyFileAtlas *>		m_SubAtlasTileSetList;		// The diffuse (image) textures of the tile sets

	struct TileSet
	{
		uint32_t					m_uiFirstGid;

		HyFileAtlas *				m_pAtlas;				// The texture atlas that contains the tile set images
		HyFileAtlas *				m_pShaderDescriptor;	// Render information for each tile stored as a data texture (GL_RGBA16UI)
																// R - Atlas Index (unsigned 16bits)
																// G - Animation bit flags (isAnimActive, isAnimLooping, Rand Phase, Ping-Pong, Reverse)
																// B - Animation frames duration in milliseconds (unsigned 16bits)
																// A - Animation frame count (unsigned 16bits)

		HyFileAtlas *				m_pShaderDescriptorEx;	// Optional render information for each tile stored as a data texture (GL_RGBA16UI)
																// R - Texture Origin to visually offset the tile in the X-Axis (signed 16bits)
																// G - Texture Origin to visually offset the tile in the Y-Axis (signed 16bits)
																// B - Color Tint Red and Green channels (packed as 2 unsigned 8bit integers)
																// A - Color Tint Blue and Alpha channels (packed as 2 unsigned 8bit integers)
	};
	std::vector<TileSet>			m_TileSetList;

public:
	HyTileMapData(const HyNodePath &nodePath, HyJsonObj itemDataObj, HyAssets &assetsRef);
	virtual ~HyTileMapData();
};

#endif /* HyTileMapData_h__ */
