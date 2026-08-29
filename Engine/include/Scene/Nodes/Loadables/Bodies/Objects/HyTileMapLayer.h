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
#include "Assets/HyAssets.h"

class HyTileMapLayer : public HyEntity2d
{
protected:
	glm::ivec2													m_vCellDimensions;
	HyTileMapLayout												m_eLayout;

	std::function<glm::ivec2(glm::vec2, const glm::ivec2 &)>	m_fpLocalToCellFunc;
	std::function<glm::vec2(glm::ivec2, const glm::ivec2 &)>	m_fpCellToLocalFunc;

	float														m_fTotalWidth;
	float														m_fTotalHeight;
	struct TileChunk
	{
		glm::ivec2												m_vCoordinate;	// The chunk coordinate, among other chunks (not local/world space)
		b2AABB													m_SceneAABB;	// Used to determine if chunk is within a camera frustum

		struct RenderStage
		{
			uint32_t											m_TileIdList[HYASSETS_TileMapChunkSize][HYASSETS_TileMapChunkSize];

			HyTextureHandle										m_hTileTexture;
			bool												m_bDirty;
		};
		std::vector<RenderStage>								m_RenderStageList;
	};
	std::vector<TileChunk>										m_ChunkList;

public:
	HyTileMapLayer(HyEntity2d *pParent = nullptr);
	HyTileMapLayer(const HyNodePath &nodePath, HyEntity2d *pParent = nullptr);
	HyTileMapLayer(const HyTileMapLayer &copyRef) = delete;
	virtual ~HyTileMapLayer(void);

	const HyTileMapLayer &operator=(HyTileMapLayer &&donor);

	bool WriteCell(std::string sTileSet, glm::ivec2 ptCellCoord, uint16_t uiTileId);

	glm::ivec2 GetCellDimensions() const;
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
