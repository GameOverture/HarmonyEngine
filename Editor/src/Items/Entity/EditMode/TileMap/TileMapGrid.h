/**************************************************************************
 *	TileMapGrid.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2026 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef TILEMAPGRID_H
#define TILEMAPGRID_H

#include "Global.h"

class TileMapGrid : public HyPrimitive2d
{
protected:
	HyTileMapLayout		m_eLayout;
	glm::vec2			m_vTileSize;
	glm::vec2			m_vDimensions;
	
public:
	TileMapGrid(HyTileMapLayout eLayout, glm::vec2 vTileSize, glm::vec2 vGridSize);
	virtual ~TileMapGrid();

	HyTileMapLayout GetLayout() const;

	void Reset(HyTileMapLayout eLayout, glm::vec2 vTileSize, glm::vec2 vGridSize);

	virtual void OnUpdateUniforms(float fExtrapolatePercent) override;
	virtual bool WriteVertexData(uint32 uiNumInstances, HyVertexBuffer &vertexBufferRef, float fExtrapolatePercent) override;
};

#endif // TILEMAPGRID_H
