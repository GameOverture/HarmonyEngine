/**************************************************************************
 *	HyTileMapBatch.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2026 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyTileMapBatch_h__
#define HyTileMapBatch_h__

#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Loadables/Bodies/Drawables/IHyDrawable2d.h"
#include "Assets/HyAssets.h"

#include <array>

class HyFileTileSet;
class HyTileMapLayer;
class IHyRenderer;

class HyTileMapBatch : public IHyDrawable2d
{
	friend class HyTileMapLayer;
	friend class IHyRenderer;
	static IHyRenderer *														sm_pHyRenderer;

protected:
	HyFileTileSet *																m_pTileSet;
	HyTextureHandle																m_hTileMap; // single channel uint16_t texture
	
	bool																		m_bDirty;
	glm::ivec2																	m_vDirtySize;
	glm::ivec2																	m_ptDirtyOffset;

	std::array<uint16_t, HYASSETS_TileMapChunkSize * HYASSETS_TileMapChunkSize>	*m_pTileIdArray; // 1D array layout of row major [HYASSETS_TileMapChunkSize][HYASSETS_TileMapChunkSize];

	HyTileMapBatch(HyFileTileSet *pTileSet, HyTileMapLayer *pParent);
public:
	virtual ~HyTileMapBatch(void);

	HyTileSetHandle GetHandle() const;

	bool WriteTileMapTexel(glm::ivec2 ptChunkCellCoord, uint16_t uiTileId);

	virtual void CalcLocalBoundingShape(HyShape2d &shapeOut) override;
	virtual float GetWidth(float fPercent = 1.0f) override;
	virtual float GetHeight(float fPercent = 1.0f) override;

	virtual bool IsLoadDataValid() override;

protected:
	virtual bool OnIsValidToRender() override;

	virtual void OnUpdateUniforms(float fExtrapolatePercent) override;

	virtual void PrepRenderStage(uint32 uiStageIndex, HyRenderMode &eRenderModeOut, HyBlendMode &eBlendModeOut, uint32 &uiNumInstancesOut, uint32 &uiNumVerticesPerInstOut, bool &bIsBatchable) override;
	virtual bool WriteVertexData(uint32 uiNumInstances, HyVertexBuffer &vertexBufferRef, float fExtrapolatePercent) override;


private: // Hide inherited functionality that doesn't exist for tile maps
	using IHyLoadable::GetState;
	using IHyLoadable::SetState;
	using IHyLoadable::GetNumStates;
	using IHyLoadable::GetName;
	using IHyLoadable::GetPrefix;
	using IHyLoadable2d::Init;
	using IHyLoadable2d::Uninit;
};

#endif /* HyTileMapBatch_h__ */
