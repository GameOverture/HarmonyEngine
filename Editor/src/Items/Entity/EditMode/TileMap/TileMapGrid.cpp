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

TileMapGrid::TileMapGrid(HyEntity2d *pParent /*= nullptr*/) :
	HyPrimitive2d(pParent),
	m_InverseTileMapLayerTransform(1.0f),
	m_vDimensions(0.0f, 0.0f),
	m_vGridSize(0.0f, 0.0f),
	m_eLayout(HYTILEMAPLAYOUT_Unknown),
	m_eStagger(HYTILEMAPSTAGGER_Unknown)
{
}

/*virtual*/ TileMapGrid::~TileMapGrid()
{
}

void TileMapGrid::Reset(glm::mat4 inverseTileMapLayerTransform, glm::vec2 vDimensions, glm::vec2 vGridSize, HyTileMapLayout eLayout, HyTileMapStagger eStagger)
{
	m_InverseTileMapLayerTransform = inverseTileMapLayerTransform;

	if(m_vDimensions != vDimensions)
	{
		m_vDimensions = vDimensions;
		SetAsBox(0, m_vDimensions.x, m_vDimensions.y, 0.0f);
	}

	m_vGridSize = vGridSize;
	m_eLayout = eLayout;
	m_eStagger = eStagger;
}

/*virtual*/ void TileMapGrid::OnUpdateUniforms(float fExtrapolatePercent) /*override*/
{
	// Vertex shader uniform
	m_ShaderUniforms.Set("u_transform_mtx", GetSceneTransform(fExtrapolatePercent));

	// Fragment shader uniforms
	m_ShaderUniforms.Set("u_inv_tilemap", m_InverseTileMapLayerTransform);
	glm::vec2 ptPos = pos.Get();
	m_ShaderUniforms.Set("u_position", pos.Get());
	m_ShaderUniforms.Set("u_dimensions", m_vDimensions);
	m_ShaderUniforms.Set("u_grid_size", m_vGridSize);
	m_ShaderUniforms.Set("u_grid_color", HyGlobal::GetEditorColor(EDITORCOLOR_TileMapGrid).GetAsVec3());
	glm::vec4 highlightColor = HyGlobal::GetEditorColor(EDITORCOLOR_TileMapHighlight).GetAsVec4();
	highlightColor.w = 0.5f;
	m_ShaderUniforms.Set("u_hover_color", highlightColor);
	m_ShaderUniforms.Set("u_line_width", 1.0f);
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
