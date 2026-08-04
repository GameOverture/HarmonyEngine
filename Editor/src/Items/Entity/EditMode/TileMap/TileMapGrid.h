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
	glm::mat4			m_InverseTileMapLayerTransform;

	glm::vec2			m_vDimensions;

	glm::vec2			m_vGridSize;
	HyTileMapLayout		m_eLayout;
	HyTileMapStagger	m_eStagger;
	
public:
	TileMapGrid(HyEntity2d *pParent = nullptr);
	virtual ~TileMapGrid();

	void Sync(glm::mat4 inverseTileMapLayerTransform, glm::vec2 vDimensions, glm::vec2 vGridSize, HyTileMapLayout eLayout, HyTileMapStagger eStagger);

	virtual void OnUpdateUniforms(float fExtrapolatePercent) override;
	virtual bool WriteVertexData(uint32 uiNumInstances, HyVertexBuffer &vertexBufferRef, float fExtrapolatePercent) override;
};

#endif // TILEMAPGRID_H
