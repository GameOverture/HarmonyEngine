/**************************************************************************
 *	HyTileMapChunk.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2026 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Loadables/Bodies/Drawables/Objects/HyTileMapChunk.h"
#include "Assets/Nodes/Objects/HyTileMapData.h"
#include "Diagnostics/Console/IHyConsole.h"

//HyTileMapChunk::HyTileMapChunk(HyEntity2d *pParent /*= nullptr*/) :
//	IHyDrawable2d(HYTYPE_TileMap, HyNodePath(), pParent),
//	m_eLayout(HYTILEMAPLAYOUT_Unknown),
//	m_fpLocalToCellFunc(nullptr),
//	m_fpCellToLocalFunc(nullptr),
//	m_fTotalWidth(0.0f),
//	m_fTotalHeight(0.0f)
//{
//}
//
//HyTileMapChunk::HyTileMapChunk(const HyNodePath &nodePath, HyEntity2d *pParent /*= nullptr*/) :
//	IHyDrawable2d(HYTYPE_TileMap, nodePath, pParent),
//	m_eLayout(HYTILEMAPLAYOUT_Unknown),
//	m_fpLocalToCellFunc(nullptr),
//	m_fpCellToLocalFunc(nullptr),
//	m_fTotalWidth(0.0f),
//	m_fTotalHeight(0.0f)
//{
//}
//
//HyTileMapChunk::HyTileMapChunk(const HyTileMapChunk &copyRef) :
//	IHyDrawable2d(copyRef)
//{
//	operator=(copyRef);
//}
//
//HyTileMapChunk::~HyTileMapChunk(void)
//{
//}
//
//const HyTileMapChunk &HyTileMapChunk::operator=(const HyTileMapChunk &rhs)
//{
//	IHyDrawable2d::operator=(rhs);
//	m_fpLocalToCellFunc = rhs.m_fpLocalToCellFunc;
//	m_fpCellToLocalFunc = rhs.m_fpCellToLocalFunc;
//	m_fTotalWidth = rhs.m_fTotalWidth;
//	m_fTotalHeight = rhs.m_fTotalHeight;
//
//	return *this;
//}
//
//bool HyTileMapChunk::WriteCell(std::string sTileSet, glm::ivec2 ptChunkCellCoord, uint16_t uiTileId)
//{
//}
//
///*virtual*/ void HyTileMapChunk::CalcLocalBoundingShape(HyShape2d &shapeOut) /*override*/
//{
//	if(AcquireData() == nullptr)
//	{
//		HyLogDebug("HySprite2d::CalcLocalBoundingShape invoked on null data");
//		return;
//	}
//
//	float fHalfWidth = m_fTotalWidth * 0.5f;
//	float fHalfHeight = m_fTotalHeight * 0.5f);
//
//	const HySpriteFrame *pFrameRef = static_cast<const HySpriteData *>(UncheckedGetData())->GetFrame(m_uiState, m_uiCurFrame);
//	glm::vec2 ptBoxCenter(pFrameRef->vOFFSET.x + fHalfWidth, pFrameRef->vOFFSET.y + fHalfHeight);
//
//	if(fHalfWidth <= HyMath::FloatSlop || fHalfHeight <= HyMath::FloatSlop)
//		return;
//
//	shapeOut.SetAsBox(HyRect(fHalfWidth, fHalfHeight, ptBoxCenter, 0.0f));
//
//
//	//// Make b2AABB by combining each layer
//	//b2AABB aabb = { {0,0}, {0,0} };
//	//HyMath::InvalidateAABB(aabb);
//	//for(int i = 0; i < m_LayerList.size(); ++i)
//	//{
//	//	const Layer &layerRef = m_LayerList[i];
//	//	if(layerRef.m_uiNumVerts > 0)
//	//	{
//	//		std::vector<b2Vec2> pointList;
//	//		for(int i = 0; i < layerRef.m_uiNumVerts; ++i)
//	//			pointList.push_back({ layerRef.m_pVertBuffer[i].x, layerRef.m_pVertBuffer[i].y });
//	//		aabb = b2MakeAABB(pointList.data(), pointList.size(), 0.0f);
//	//	}
//	//}
//	//if(b2IsValidAABB(aabb) == false)
//	//{
//	//	shapeOut.SetAsNothing();
//	//	return;
//	//}
//
//	//b2Vec2 vExtents = b2AABB_Extents(aabb);
//	//b2Vec2 ptCenter = b2AABB_Center(aabb);
//	//shapeOut.SetAsBox(HyRect(vExtents.x, vExtents.y, glm::vec2(ptCenter.x, ptCenter.y), 0.0f));
//}
//
///*virtual*/ float HyTileMapChunk::GetWidth(float fPercent /*= 1.0f*/) /*override*/
//{
//	return m_fTotalWidth * fPercent;
//}
//
///*virtual*/ float HyTileMapChunk::GetHeight(float fPercent /*= 1.0f*/) /*override*/
//{
//	if(AcquireData() == nullptr)
//		return 0.0f;
//
//	const HyTileMapData *pData = static_cast<const HyTileMapData *>(UncheckedGetData());
//	return pData->GetTileMapHeight(m_iTileMapDataIndex) * fPercent;
//}
//
///*virtual*/ bool HyTileMapChunk::IsLoadDataValid() /*override*/
//{
//	return GetLayout() != HYTILEMAPLAYOUT_Unknown;
//}
//
///*virtual*/ void HyTileMapChunk::OnDataAcquired() /*override*/
//{
//	const HyTileMapData *pData = static_cast<const HyTileMapData *>(UncheckedGetData());
//	SetLayout(pData->GetLayout());
//}
//
///*virtual*/ bool HyTileMapChunk::OnIsValidToRender() /*override*/
//{
//	return GetLayout() != HYTILEMAPLAYOUT_Unknown;
//}
//
///*virtual*/ void HyTileMapChunk::OnUpdateUniforms(float fExtrapolatePercent) /*override*/
//{
//	//// TODO: get rid of this check and improve m_ShaderUniforms
//	//if(m_bUpdateShaderUniforms)
//	//{
//	//	glm::mat4 mtx = GetSceneTransform(fExtrapolatePercent);
//
//	//	m_ShaderUniforms.Set("u_transform_mtx", mtx);
//	//	//m_ShaderUniforms.Set("u_color", vTop);
//
//	//	m_bUpdateShaderUniforms = false;
//	//}
//}
//
///*virtual*/ void HyTileMapChunk::PrepRenderStage(uint32 uiStageIndex, HyRenderMode &eRenderModeOut, HyBlendMode &eBlendModeOut, uint32 &uiNumInstancesOut, uint32 &uiNumVerticesPerInstOut, bool &bIsBatchable) /*override*/
//{
//	eRenderModeOut = HYRENDERMODE_Triangles;
//	eBlendModeOut = HYBLENDMODE_Normal;
//
//
//
//	//// TODO: Do stages with circle batching (and its own shader)
//	//uiNumVerticesPerInstOut = 3;
//	//uiNumInstancesOut = 0;
//	//for(int iLayerIndex = 0; iLayerIndex < m_LayerList.size(); ++iLayerIndex)
//	//{
//	//	const Layer &layerRef = m_LayerList[iLayerIndex];
//	//	if(layerRef.m_bVisible && layerRef.m_uiNumVerts > 0 && layerRef.m_pVertBuffer != nullptr)
//	//		uiNumInstancesOut += layerRef.m_uiNumVerts / uiNumVerticesPerInstOut;
//	//}
//	//bIsBatchable = true;
//}
//
///*virtual*/ bool HyTileMapChunk::WriteVertexData(uint32 uiNumInstances, HyVertexBuffer &vertexBufferRef, float fExtrapolatePercent) /*override*/
//{
//	//// TODO: Get rid of top/bot color
//	//glm::vec3 vTop = CalculateTopTint(fExtrapolatePercent);
//	////glm::vec3 vBot = CalculateBotTint(fExtrapolatePercent);
//	//glm::vec4 vTopColor;
//	//vTopColor.x = vTop.x;
//	//vTopColor.y = vTop.y;
//	//vTopColor.z = vTop.z;
//	//vTopColor.a = CalculateAlpha(fExtrapolatePercent);
//	////glm::vec4 vBotColor;
//	////vBotColor.x = vBot.x;
//	////vBotColor.y = vBot.y;
//	////vBotColor.z = vBot.z;
//	////vBotColor.a = vTopColor.a;
//
//	//for(int iLayerIndex = 0; iLayerIndex < m_LayerList.size(); ++iLayerIndex)
//	//{
//	//	Layer &layerRef = m_LayerList[iLayerIndex];
//	//	if(layerRef.m_bVisible && layerRef.m_uiNumVerts > 0 && layerRef.m_pVertBuffer != nullptr)
//	//	{
//	//		glm::vec4 layerColor = vTopColor;
//	//		layerColor.x *= layerRef.m_Color.GetRedF();
//	//		layerColor.y *= layerRef.m_Color.GetGreenF();
//	//		layerColor.z *= layerRef.m_Color.GetBlueF();
//	//		layerColor.a *= layerRef.m_fAlpha;
//	//		for(int iVertIndex = 0; iVertIndex < layerRef.m_uiNumVerts; ++iVertIndex)
//	//		{
//	//			glm::vec2 ptVert = layerRef.m_pVertBuffer[iVertIndex] + layerRef.m_vOffset;
//	//			vertexBufferRef.AppendVertexData(&ptVert, sizeof(glm::vec2));
//	//			vertexBufferRef.AppendVertexData(&layerColor, sizeof(glm::vec4)); // TODO: Cache bot most and top most vertices and use that to mix color between vTopColor and vBotColor for a vertical gradient effect
//	//		}
//	//	}
//	//}
//
//	return true;
//}
