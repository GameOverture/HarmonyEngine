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
	struct TileSet
	{
		uint32_t					m_uiFirstGid;

		HyFileAtlas *				m_pAtlas;			// The texture atlas that contains the tile set images
		HyFileAtlas *				m_pShaderDescriptor;// Additional render information for each tile stored as a data texture (GL_RGBA32UI)
															// R - Atlas Index + Bit Flags (ModulateColor, FlipH, FlipV, Transpose, isAnimActive, isAnimLooping, Rand Phase, Ping-Pong, Reverse)
															// G - Texture Origin to visually offset the tile (X and Y packed as two signed 16 bit values)
															// B - Color Tint + Alpha (RGBA packed as four unsigned 8bit integers values)
															// A - Animation info (frameDurationMs and frameCount packed as two unsigned 16 bit values)
	};
	std::vector<TileSet>			m_TileSetList;

	glm::ivec2						m_vChunkSize;
	struct TileMap
	{
		float						m_fTotalWidth;
		float						m_fTotalHeight;

		struct TileChunk
		{
			glm::ivec2				m_vCoordinate;
			std::vector<uint32_t>	m_TileGidList;

			HyTextureHandle			m_hTileTexture;
			bool					m_bDirty;
		};
		std::vector<TileChunk>		m_ChunkList;
	};
	std::vector<TileMap>			m_TileMapList;

public:
	HyTileMapData(const HyNodePath &nodePath, HyJsonObj itemDataObj, HyAssets &assetsRef);
	virtual ~HyTileMapData();

	int GetNumTileMaps() const;

	float GetTileMapWidth(int iTileMapIndex) const;
	float GetTileMapHeight(int iTileMapIndex) const;
};

#endif /* HyTileMapData_h__ */
