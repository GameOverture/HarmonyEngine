/**************************************************************************
 *	HyTileMapLayer.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2026 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Loadables/Bodies/Drawables/Objects/HyTileMapLayer.h"
#include "Assets/Nodes/Objects/HyTileMapData.h"

HyTileMapLayer::HyTileMapLayer(const HyNodePath &nodePath, HyEntity2d *pParent /*= nullptr*/) :
	IHyDrawable2d(HYTYPE_TileMap, nodePath, pParent),
	m_iTileMapDataIndex(0)
{
}

HyTileMapLayer::HyTileMapLayer(const HyTileMapLayer &copyRef) :
	IHyDrawable2d(copyRef)
{
	operator=(copyRef);
}

HyTileMapLayer::~HyTileMapLayer(void)
{
}

const HyTileMapLayer &HyTileMapLayer::operator=(const HyTileMapLayer &rhs)
{
	IHyDrawable2d::operator=(rhs);
	m_iTileMapDataIndex = rhs.m_iTileMapDataIndex;

	return *this;
}

/*virtual*/ void HyTileMapLayer::CalcLocalBoundingShape(HyShape2d &shapeOut) /*override*/
{
	//// Make b2AABB by combining each layer
	//b2AABB aabb = { {0,0}, {0,0} };
	//HyMath::InvalidateAABB(aabb);
	//for(int i = 0; i < m_LayerList.size(); ++i)
	//{
	//	const Layer &layerRef = m_LayerList[i];
	//	if(layerRef.m_uiNumVerts > 0)
	//	{
	//		std::vector<b2Vec2> pointList;
	//		for(int i = 0; i < layerRef.m_uiNumVerts; ++i)
	//			pointList.push_back({ layerRef.m_pVertBuffer[i].x, layerRef.m_pVertBuffer[i].y });
	//		aabb = b2MakeAABB(pointList.data(), pointList.size(), 0.0f);
	//	}
	//}
	//if(b2IsValidAABB(aabb) == false)
	//{
	//	shapeOut.SetAsNothing();
	//	return;
	//}

	//b2Vec2 vExtents = b2AABB_Extents(aabb);
	//b2Vec2 ptCenter = b2AABB_Center(aabb);
	//shapeOut.SetAsBox(HyRect(vExtents.x, vExtents.y, glm::vec2(ptCenter.x, ptCenter.y), 0.0f));
}

/*virtual*/ float HyTileMapLayer::GetWidth(float fPercent /*= 1.0f*/) /*override*/
{
	if(AcquireData() == nullptr || m_iTileMapDataIndex >= static_cast<const HyTileMapData *>(UncheckedGetData())->GetNumTileMaps() || m_iTileMapDataIndex < 0)
		return 0.0f;

	const HyTileMapData *pData = static_cast<const HyTileMapData *>(UncheckedGetData());
	return pData->GetTileMapWidth(m_iTileMapDataIndex) * fPercent;
}

/*virtual*/ float HyTileMapLayer::GetHeight(float fPercent /*= 1.0f*/) /*override*/
{
	if(AcquireData() == nullptr || m_iTileMapDataIndex >= static_cast<const HyTileMapData *>(UncheckedGetData())->GetNumTileMaps() || m_iTileMapDataIndex < 0)
		return 0.0f;

	const HyTileMapData *pData = static_cast<const HyTileMapData *>(UncheckedGetData());
	return pData->GetTileMapHeight(m_iTileMapDataIndex) * fPercent;
}

/*virtual*/ bool HyTileMapLayer::IsLoadDataValid() /*override*/
{
	return true;
}

/*virtual*/ bool HyTileMapLayer::OnIsValidToRender() /*override*/
{
	return true;
}

/*virtual*/ void HyTileMapLayer::OnUpdateUniforms(float fExtrapolatePercent) /*override*/
{
	//// TODO: get rid of this check and improve m_ShaderUniforms
	//if(m_bUpdateShaderUniforms)
	//{
	//	glm::mat4 mtx = GetSceneTransform(fExtrapolatePercent);

	//	m_ShaderUniforms.Set("u_mtxTransform", mtx);
	//	//m_ShaderUniforms.Set("u_vColor", vTop);

	//	m_bUpdateShaderUniforms = false;
	//}
}

/*virtual*/ void HyTileMapLayer::PrepRenderStage(uint32 uiStageIndex, HyRenderMode &eRenderModeOut, HyBlendMode &eBlendModeOut, uint32 &uiNumInstancesOut, uint32 &uiNumVerticesPerInstOut, bool &bIsBatchable) /*override*/
{
	//eRenderModeOut = HYRENDERMODE_Triangles;
	//eBlendModeOut = HYBLENDMODE_Normal;

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

/*virtual*/ bool HyTileMapLayer::WriteVertexData(uint32 uiNumInstances, HyVertexBuffer &vertexBufferRef, float fExtrapolatePercent) /*override*/
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
