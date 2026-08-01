/**************************************************************************
 *	TileMapGrid.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2026 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "TileMapGrid.h"

const float g_fGridWidth = 500.0f;
const glm::vec2		m_vDIMENSIONS;

TileMapGrid::TileMapGrid(HyTileMapLayout eLayout, glm::vec2 vTileSize, glm::vec2 vGridSize) :
	m_eLayout(eLayout),
	m_vTileSize(vTileSize),
	m_vDimensions(vGridSize)
{
	SetAsBox(0, m_vDimensions.x, m_vDimensions.y, 0.0f);
	
}

/*virtual*/ TileMapGrid::~TileMapGrid()
{
}

HyTileMapLayout TileMapGrid::GetLayout() const
{
	return m_eLayout;
}

void TileMapGrid::Reset(HyTileMapLayout eLayout, glm::vec2 vTileSize, glm::vec2 vGridSize)
{
	m_eLayout = eLayout;
	m_vTileSize = vTileSize;
	m_vDimensions = vGridSize;

	SetAsBox(0, m_vDimensions.x, m_vDimensions.y, 0.0f);
}

/*virtual*/ void TileMapGrid::OnUpdateUniforms(float fExtrapolatePercent) /*override*/
{
	glm::mat4 mtx = HyPrimitive2d::GetSceneTransform(fExtrapolatePercent);

	m_ShaderUniforms.Set("u_transform_mtx", mtx);
	m_ShaderUniforms.Set("u_dimensions", m_vDimensions);
	m_ShaderUniforms.Set("u_world_origin", pos.Get());
	m_ShaderUniforms.Set("u_tile_size", m_vTileSize);
}

/*virtual*/ bool TileMapGrid::WriteVertexData(uint32 uiNumInstances, HyVertexBuffer &vertexBufferRef, float fExtrapolatePercent) /*override*/
{
	HyAssert(GetNumVerts(0) == 6, "TileMapGrid::OnWriteDrawBufferData is trying to draw a primitive that's not a quad");

	for(int i = 0; i < 6; ++i)
	{
		vertexBufferRef.AppendVertexData(&m_LayerList[0].m_pVertBuffer[i], sizeof(glm::vec2));

		glm::vec2 vUV;
		switch(i)
		{
		case 0:
		case 5:
			vUV.x = 0.0f;
			vUV.y = 0.0f;
			break;

		case 1:
			vUV.x = 0.0f;
			vUV.y = 1.0f;
			break;

		case 2:
		case 3:
			vUV.x = 1.0f;
			vUV.y = 1.0f;
			break;

		case 4:
			vUV.x = 1.0f;
			vUV.y = 0.0f;
			break;
		}

		vertexBufferRef.AppendVertexData(&vUV, sizeof(glm::vec2));
	}

	return true;
}
