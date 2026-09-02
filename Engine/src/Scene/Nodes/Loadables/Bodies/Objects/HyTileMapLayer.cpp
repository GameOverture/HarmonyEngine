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
#include "Scene/Nodes/Loadables/Bodies/Objects/HyTileMapLayer.h"
#include "Assets/Nodes/Objects/HyTileMapData.h"
#include "Scene/Physics/Fixtures/HyShape2d.h"

HyTileMapLayer::HyTileMapLayer(HyEntity2d *pParent /*= nullptr*/) :
	HyEntity2d(pParent),
	m_RenderSettings(0, 0),
	m_vCellDimensions(0, 0),
	m_eLayout(HYTILEMAPLAYOUT_Unknown),
	m_fpLocalToCellFunc(nullptr),
	m_fpCellToLocalFunc(nullptr),
	m_fTotalWidth(0.0f),
	m_fTotalHeight(0.0f)
{
}

HyTileMapLayer::HyTileMapLayer(const HyNodePath &nodePath, HyEntity2d *pParent /*= nullptr*/) :
	HyEntity2d(pParent),
	m_RenderSettings(0, 0),
	m_vCellDimensions(0, 0),
	m_eLayout(HYTILEMAPLAYOUT_Unknown),
	m_fpLocalToCellFunc(nullptr),
	m_fpCellToLocalFunc(nullptr),
	m_fTotalWidth(0.0f),
	m_fTotalHeight(0.0f)
{
	m_NodePath = nodePath;
}

HyTileMapLayer::~HyTileMapLayer(void)
{
}

const HyTileMapLayer &HyTileMapLayer::operator=(HyTileMapLayer &&donor)
{
	HyEntity2d::operator=(std::move(donor));

	m_RenderSettings = donor.m_RenderSettings;
	m_vCellDimensions = donor.m_vCellDimensions;
	m_eLayout = donor.m_eLayout;
	m_fpLocalToCellFunc = donor.m_fpLocalToCellFunc;
	m_fpCellToLocalFunc = donor.m_fpCellToLocalFunc;
	m_fTotalWidth = donor.m_fTotalWidth;
	m_fTotalHeight = donor.m_fTotalHeight;

	return *this;
}

bool HyTileMapLayer::WriteCell(HyTileSetHandle hTileSet, uint16_t uiTileId, glm::ivec2 ptCellCoord)
{
	// Determine chunk coordinate (can reuse TileMapPointToCell_Square function for this purpose)
	glm::ivec2 ptChunkCoord = HyMath::TileMapPointToCell_Square(ptCellCoord, glm::ivec2(HYASSETS_TileMapChunkSize, HYASSETS_TileMapChunkSize));

	// Try to find existing chunk/HyTileMapBatch to WriteTileMapTexel, otherwise add new chunk/HyTileMapBatch then WriteTileMapTexel
	for(TileChunk &tileChunkRef : m_ChunkList)
	{
		if(tileChunkRef.m_ptCoordinate == ptChunkCoord)
		{
			for(HyTileMapBatch *pTileMapBatch : tileChunkRef.m_BatchList)
			{
				if(pTileMapBatch->GetHandle() == hTileSet)
					return pTileMapBatch->WriteTileMapTexel(ptCellCoord % HYASSETS_TileMapChunkSize, uiTileId);
			}

			// 'HyTileMapBatch' not found within chunk, allocating new HyTileMapBatch
			HyTileMapBatch *pTileMapBatch = tileChunkRef.AllocateTileMapBatch(hTileSet, this);
			if(pTileMapBatch == nullptr)
				return false;

			return pTileMapBatch->WriteTileMapTexel(ptCellCoord % HYASSETS_TileMapChunkSize, uiTileId);
		}
	}

	// Chunk not found, add both new chunk and HyTileMapBatch
	m_ChunkList.emplace_back(ptChunkCoord);
	HyTileMapBatch *pTileMapBatch = m_ChunkList.back().AllocateTileMapBatch(hTileSet, this);
	if(pTileMapBatch == nullptr)
		return false;

	return pTileMapBatch->WriteTileMapTexel(ptCellCoord % HYASSETS_TileMapChunkSize, uiTileId);
}

const glm::ivec2 &HyTileMapLayer::GetCellDimensions() const
{
	return m_vCellDimensions;
}

void HyTileMapLayer::SetCellDimensions(glm::ivec2 vCellDimensions)
{
	m_vCellDimensions = vCellDimensions;
}

HyTileMapLayout HyTileMapLayer::GetLayout() const
{
	return m_eLayout;
}

void HyTileMapLayer::SetLayout(HyTileMapLayout eLayout)
{
	m_eLayout = eLayout;
	switch(m_eLayout)
	{
	case HYTILEMAPLAYOUT_Square:
		m_fpLocalToCellFunc = HyMath::TileMapPointToCell_Square;
		m_fpCellToLocalFunc = HyMath::TileMapCellToPoint_Square;
		break;
	case HYTILEMAPLAYOUT_HalfOffsetSquare:
		m_fpLocalToCellFunc = HyMath::TileMapPointToCell_HalfSquare;
		m_fpCellToLocalFunc = HyMath::TileMapCellToPoint_HalfSquare;
		break;
	case HYTILEMAPLAYOUT_Isometric:
		m_fpLocalToCellFunc = HyMath::TileMapPointToCell_Isometric;
		m_fpCellToLocalFunc = HyMath::TileMapCellToPoint_Isometric;
		break;
	case HYTILEMAPLAYOUT_IsometricStaggerX:
		m_fpLocalToCellFunc = HyMath::TileMapPointToCell_IsometricStaggerX;
		m_fpCellToLocalFunc = HyMath::TileMapCellToPoint_IsometricStaggerX;
		break;
	case HYTILEMAPLAYOUT_IsometricStaggerY:
		m_fpLocalToCellFunc = HyMath::TileMapPointToCell_IsometricStaggerY;
		m_fpCellToLocalFunc = HyMath::TileMapCellToPoint_IsometricStaggerY;
		break;
	case HYTILEMAPLAYOUT_HexagonFlatTop:
		m_fpLocalToCellFunc = HyMath::TileMapPointToCell_HexagonFlatTop;
		m_fpCellToLocalFunc = HyMath::TileMapCellToPoint_HexagonFlatTop;
		break;
	case HYTILEMAPLAYOUT_HexagonPointTop:
		m_fpLocalToCellFunc = HyMath::TileMapPointToCell_HexagonPointTop;
		m_fpCellToLocalFunc = HyMath::TileMapCellToPoint_HexagonPointTop;
		break;
	default:
		HyLogError("HyTileMapLayer::SetLayout() - Unknown tile map layout");
		break;
	}
}

glm::ivec2 HyTileMapLayer::WorldToTile(glm::vec2 ptWorldPos)
{
	glm::vec2 ptLocal = glm::inverse(GetSceneTransform(0.0f)) * glm::vec4(ptWorldPos, 0.0f, 1.0f);
	return m_fpLocalToCellFunc(ptLocal, m_vCellDimensions);
}

glm::vec2 HyTileMapLayer::TileToWorld(glm::ivec2 ptTileCoord)
{
	glm::vec2 ptLocal = m_fpCellToLocalFunc(ptTileCoord, m_vCellDimensions);
	return GetSceneTransform(0.0f) * glm::vec4(ptLocal, 0.0f, 1.0f);
}

/*virtual*/ void HyTileMapLayer::CalcLocalBoundingShape(HyShape2d &shapeOut) /*override*/
{
	if(AcquireData() == nullptr)
	{
		HyLogDebug("HyTileMapLayer::CalcLocalBoundingShape invoked on null data");
		return;
	}

	float fHalfWidth = GetWidth(0.5f);
	float fHalfHeight = GetHeight(0.5f);
	if(fHalfWidth <= HyMath::FloatSlop || fHalfHeight <= HyMath::FloatSlop)
		return;

	glm::vec2 ptBoxCenter(fHalfWidth, fHalfHeight);
	shapeOut.SetAsBox(HyRect(fHalfWidth, fHalfHeight, ptBoxCenter, 0.0f));


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
	return m_fTotalWidth * fPercent;
}

/*virtual*/ float HyTileMapLayer::GetHeight(float fPercent /*= 1.0f*/) /*override*/
{
	return m_fTotalHeight * fPercent;
}

/*virtual*/ bool HyTileMapLayer::IsLoadDataValid() /*override*/
{
	return GetLayout() != HYTILEMAPLAYOUT_Unknown;
}

/*virtual*/ void HyTileMapLayer::OnDataAcquired() /*override*/
{
	// TILETODO:
	//const HyTileMapData *pData = static_cast<const HyTileMapData *>(UncheckedGetData());
	//SetLayout(pData->GetLayout());
}
