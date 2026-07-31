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

TileMapGrid::TileMapGrid(HyTileMapLayout eLayout, float fWidth, float fHeight) :
	m_eLayout(eLayout),
	m_vDimensions(fWidth, fHeight)
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

void TileMapGrid::Reset(HyTileMapLayout eLayout, float fWidth, float fHeight)
{
	m_eLayout = eLayout;
	m_vDimensions = { fWidth, fHeight };

	SetAsBox(0, m_vDimensions.x, m_vDimensions.y, 0.0f);
}

/*virtual*/ void TileMapGrid::OnUpdateUniforms(float fExtrapolatePercent) /*override*/
{
	glm::mat4 mtx = HyPrimitive2d::GetSceneTransform(fExtrapolatePercent);

	m_ShaderUniforms.Set("u_mtxTransform", mtx);
	m_ShaderUniforms.Set("uDimensions", m_vDimensions);
	m_ShaderUniforms.Set("uWorldOrigin", pos.Get());
	//m_ShaderUniforms.Set("uTileSize", glm::vec4(HyGlobal::GetEditorColor(EDITORCOLOR_GridColor1).GetAsVec4()));
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
