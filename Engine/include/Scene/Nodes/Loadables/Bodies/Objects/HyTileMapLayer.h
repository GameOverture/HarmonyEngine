/**************************************************************************
 *	HyTileMapLayer.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2026 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyTileMapLayer_h__
#define HyTileMapLayer_h__

#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Loadables/Bodies/Objects/HyEntity2d.h"
#include "Scene/Nodes/Loadables/Bodies/Drawables/Objects/HyTileMapBatch.h"
#include "Assets/HyAssets.h"
#include "Diagnostics/Console/IHyConsole.h"

class HyTileMapLayer : public HyEntity2d
{
protected:
	glm::ivec2													m_RenderSettings;		// Default both X/Y to '0' is batch rendering. Otherwise significantly changes to individual sorted by the specified axis, allowing other tiles and sprites to sort correctly between tiles

	glm::ivec2													m_vCellDimensions;
	HyTileMapLayout												m_eLayout;

	std::function<glm::ivec2(glm::vec2, const glm::ivec2 &)>	m_fpLocalToCellFunc;
	std::function<glm::vec2(glm::ivec2, const glm::ivec2 &)>	m_fpCellToLocalFunc;

	struct TileChunk
	{
		const glm::ivec2										m_ptCoordinate;	// The chunk coordinate, among other chunks (not local/world space)
		std::vector<HyTileMapBatch *>							m_BatchList;

		TileChunk(glm::ivec2 ptChunkCoord) :
			m_ptCoordinate(ptChunkCoord)
		{ }
		~TileChunk()
		{
			for(HyTileMapBatch *pTileMapBatch : m_BatchList)
				delete pTileMapBatch;
		}

		HyTileMapBatch *AllocateTileMapBatch(HyTileSetHandle hTileSet, HyTileMapLayer *pTileMapLayer)
		{
			HyFileTileSet *pTileSet = sm_pHyAssets->GetTileSet(hTileSet);
			if(pTileSet == nullptr)
			{
				HyLogWarning("HyTileMapLayer::TileChunk::AllocateTileMapBatch was passed an invalid TileSet handle");
				return nullptr;
			}
			HyTileMapBatch *pTileMapBatch = HY_NEW HyTileMapBatch(pTileSet, pTileMapLayer);
			m_BatchList.push_back(pTileMapBatch);

			return pTileMapBatch;
		}
	};
	std::vector<TileChunk>										m_ChunkList;
	float														m_fTotalWidth;
	float														m_fTotalHeight;

public:
	HyTileMapLayer(HyEntity2d *pParent = nullptr);
	HyTileMapLayer(const HyNodePath &nodePath, HyEntity2d *pParent = nullptr);
	HyTileMapLayer(const HyTileMapLayer &copyRef) = delete;
	virtual ~HyTileMapLayer(void);

	const HyTileMapLayer &operator=(HyTileMapLayer &&donor);

	bool WriteCell(HyTileSetHandle hTileSet, uint16_t uiTileId, glm::ivec2 ptCellCoord);

	const glm::ivec2 &GetCellDimensions() const;
	void SetCellDimensions(glm::ivec2 vCellDimensions);

	HyTileMapLayout GetLayout() const;
	void SetLayout(HyTileMapLayout eLayout);

	glm::ivec2 WorldToTile(glm::vec2 ptWorldPos);
	glm::vec2 TileToWorld(glm::ivec2 ptTileCoord);

	virtual void CalcLocalBoundingShape(HyShape2d &shapeOut) override;
	virtual float GetWidth(float fPercent = 1.0f) override;
	virtual float GetHeight(float fPercent = 1.0f) override;

	virtual bool IsLoadDataValid() override;

protected:
	virtual void OnDataAcquired() override;

private: // Hide inherited functionality that doesn't exist for tile maps
	using IHyLoadable::GetState;
	using IHyLoadable::SetState;
	using IHyLoadable::GetNumStates;
	using IHyLoadable::GetName;
	using IHyLoadable::GetPrefix;
	using IHyLoadable2d::Init;
	using IHyLoadable2d::Uninit;
};

#endif /* HyTileMapLayer_h__ */
