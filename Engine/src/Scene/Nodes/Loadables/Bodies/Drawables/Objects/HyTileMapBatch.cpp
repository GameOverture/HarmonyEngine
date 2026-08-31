/**************************************************************************
 *	HyTileMapBatch.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2026 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Loadables/Bodies/Drawables/Objects/HyTileMapBatch.h"
#include "Scene/Nodes/Loadables/Bodies/Objects/HyTileMapLayer.h"
#include "Scene/Physics/Fixtures/HyShape2d.h"
#include "Assets/HyAssets.h"
#include "Assets/Files/HyFileTileSet.h"
#include "Diagnostics/Console/IHyConsole.h"

HyTileMapBatch::HyTileMapBatch(HyFileTileSet *pTileSet, HyTileMapLayer *pParent) :
	IHyDrawable2d(HYTYPE_TileMapBatch, HyNodePath(), pParent),
	m_pTileSet(pTileSet),
	m_hTileMap(HY_UNUSED_HANDLE),
	m_pTileIdList(HY_NEW uint16_t[HYASSETS_TileMapChunkSize * HYASSETS_TileMapChunkSize]),
	m_bDirty(false)
{
	HyAssert(m_pTileSet, "HyTileMapBatch ctor - null HyFileTileSet was passed in");
}

HyTileMapBatch::~HyTileMapBatch(void)
{
	delete[] m_pTileIdList;
}

HyTileSetHandle HyTileMapBatch::GetHandle() const
{
	return m_pTileSet->GetHandle();
}

bool HyTileMapBatch::WriteTileMapTexture(glm::ivec2 ptChunkCellCoord, uint16_t uiTileId)
{
	// TILETODO: Write texel data to tile ID texture
	return false;
}

/*virtual*/ void HyTileMapBatch::CalcLocalBoundingShape(HyShape2d &shapeOut) /*override*/
{
	float fHalfWidth = GetWidth(0.5f);
	float fHalfHeight = GetHeight(0.5f);
	if(fHalfWidth <= HyMath::FloatSlop || fHalfHeight <= HyMath::FloatSlop)
		return;

	glm::vec2 ptBoxCenter(fHalfWidth, fHalfHeight);
	shapeOut.SetAsBox(HyRect(fHalfWidth, fHalfHeight, ptBoxCenter, 0.0f));
}

/*virtual*/ float HyTileMapBatch::GetWidth(float fPercent /*= 1.0f*/) /*override*/
{
	return static_cast<HyTileMapLayer *>(m_pParent)->GetCellDimensions().x * HYASSETS_TileMapChunkSize * fPercent;
}

/*virtual*/ float HyTileMapBatch::GetHeight(float fPercent /*= 1.0f*/) /*override*/
{
	return static_cast<HyTileMapLayer *>(m_pParent)->GetCellDimensions().y * HYASSETS_TileMapChunkSize * fPercent;
}

/*virtual*/ bool HyTileMapBatch::IsLoadDataValid() /*override*/
{
	return true;
}

/*virtual*/ void HyTileMapBatch::OnDataAcquired() /*override*/
{
}

/*virtual*/ bool HyTileMapBatch::OnIsValidToRender() /*override*/
{
	return true;
}

/*virtual*/ void HyTileMapBatch::OnUpdateUniforms(float fExtrapolatePercent) /*override*/
{
	//// TODO: get rid of this check and improve m_ShaderUniforms
	//if(m_bUpdateShaderUniforms)
	//{
	//	glm::mat4 mtx = GetSceneTransform(fExtrapolatePercent);

	//	m_ShaderUniforms.Set("u_transform_mtx", mtx);
	//	//m_ShaderUniforms.Set("u_color", vTop);

	//	m_bUpdateShaderUniforms = false;
	//}
}

/*virtual*/ void HyTileMapBatch::PrepRenderStage(uint32 uiStageIndex, HyRenderMode &eRenderModeOut, HyBlendMode &eBlendModeOut, uint32 &uiNumInstancesOut, uint32 &uiNumVerticesPerInstOut, bool &bIsBatchable) /*override*/
{
	eRenderModeOut = HYRENDERMODE_Triangles;
	eBlendModeOut = HYBLENDMODE_Normal;



	//// TODO: Do stages with circle batching (and its own shader)
	//uiNumVerticesPerInstOut = 3;
	//uiNumInstancesOut = 0;
	//for(int iLayerIndex = 0; iLayerIndex < m_LayerList.size(); ++iLayerIndex)
	//{
	//	const Layer &layerRef = m_LayerList[iLayerIndex];
	//	if(layerRef.m_bVisible && layerRef.m_uiNumVerts > 0 && layerRef.m_pVertBuffer != nullptr)
	//		uiNumInstancesOut += layerRef.m_uiNumVerts / uiNumVerticesPerInstOut;
	//}
	//bIsBatchable = true;
}

/*virtual*/ bool HyTileMapBatch::WriteVertexData(uint32 uiNumInstances, HyVertexBuffer &vertexBufferRef, float fExtrapolatePercent) /*override*/
{
	//// TODO: Get rid of top/bot color
	//glm::vec3 vTop = CalculateTopTint(fExtrapolatePercent);
	////glm::vec3 vBot = CalculateBotTint(fExtrapolatePercent);
	//glm::vec4 vTopColor;
	//vTopColor.x = vTop.x;
	//vTopColor.y = vTop.y;
	//vTopColor.z = vTop.z;
	//vTopColor.a = CalculateAlpha(fExtrapolatePercent);
	////glm::vec4 vBotColor;
	////vBotColor.x = vBot.x;
	////vBotColor.y = vBot.y;
	////vBotColor.z = vBot.z;
	////vBotColor.a = vTopColor.a;

	//for(int iLayerIndex = 0; iLayerIndex < m_LayerList.size(); ++iLayerIndex)
	//{
	//	Layer &layerRef = m_LayerList[iLayerIndex];
	//	if(layerRef.m_bVisible && layerRef.m_uiNumVerts > 0 && layerRef.m_pVertBuffer != nullptr)
	//	{
	//		glm::vec4 layerColor = vTopColor;
	//		layerColor.x *= layerRef.m_Color.GetRedF();
	//		layerColor.y *= layerRef.m_Color.GetGreenF();
	//		layerColor.z *= layerRef.m_Color.GetBlueF();
	//		layerColor.a *= layerRef.m_fAlpha;
	//		for(int iVertIndex = 0; iVertIndex < layerRef.m_uiNumVerts; ++iVertIndex)
	//		{
	//			glm::vec2 ptVert = layerRef.m_pVertBuffer[iVertIndex] + layerRef.m_vOffset;
	//			vertexBufferRef.AppendVertexData(&ptVert, sizeof(glm::vec2));
	//			vertexBufferRef.AppendVertexData(&layerColor, sizeof(glm::vec4)); // TODO: Cache bot most and top most vertices and use that to mix color between vTopColor and vBotColor for a vertical gradient effect
	//		}
	//	}
	//}

	return true;
}
