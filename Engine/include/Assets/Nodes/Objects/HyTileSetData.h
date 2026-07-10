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
	HyFileAtlas *			m_pAtlas;			// The texture atlas that contains the tile set images
	HyFileAtlas *			m_pShaderDescriptor;// Additional render information for each tile stored as a data texture (GL_RGBA32UI)
													// R - Atlas Index + Bit Flags (ModulateColor, FlipH, FlipV, Transpose, isAnimActive, isAnimLooping, Rand Phase, Ping-Pong, Reverse)
													// G - Texture Origin to visually offset the tile (X and Y packed as two signed 16 bit values)
													// B - Color Tint + Alpha (RGBA packed as four unsigned 8bit integers values)
													// A - Animation info (frameDurationMs and frameCount packed as two unsigned 16 bit values)

	struct TileData	// Tile data required by the CPU
	{
		int16				m_iDisplayOrder;
		int16				m_SortOriginY;
	};
	std::vector<TileData>	m_TileDataList;

public:
	HyTileSetData(const HyNodePath &nodePath, HyJsonObj itemDataObj, HyAssets &assetsRef);
	virtual ~HyTileSetData();

	HyFileAtlas *GetAtlas() const;
};

#endif /* HyTileSetData_h__ */
