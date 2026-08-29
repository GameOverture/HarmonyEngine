/**************************************************************************
 *	HyTileMapChunk.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2026 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyTileMapChunk_h__
#define HyTileMapChunk_h__

#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Loadables/Bodies/Drawables/IHyDrawable2d.h"

//class HyTileMapChunk : public IHyDrawable2d
//{
//protected:
//	glm::ivec2												m_vCoordinate;	// The chunk coordinate, among other chunks (not local/world space)
//	b2AABB													m_SceneAABB;	// Used to determine if chunk is within a camera frustum
//
//	struct RenderStage
//	{
//		uint32_t											m_TileIdList[HYASSETS_TileMapChunkSize][HYASSETS_TileMapChunkSize];
//
//		HyTextureHandle										m_hTileTexture;
//		bool												m_bDirty;
//	};
//	std::vector<RenderStage>								m_RenderStageList;
//
//public:
//	HyTileMapChunk(HyEntity2d *pParent = nullptr);
//	HyTileMapChunk(const HyNodePath &nodePath, HyEntity2d *pParent = nullptr);
//	HyTileMapChunk(const HyTileMapChunk &copyRef);
//	virtual ~HyTileMapChunk(void);
//
//	const HyTileMapChunk &operator=(const HyTileMapChunk &rhs);
//
//	bool WriteCell(std::string sTileSet, glm::ivec2 ptChunkCellCoord, uint16_t uiTileId);
//
//	virtual void CalcLocalBoundingShape(HyShape2d &shapeOut) override;
//	virtual float GetWidth(float fPercent = 1.0f) override;
//	virtual float GetHeight(float fPercent = 1.0f) override;
//
//	virtual bool IsLoadDataValid() override;
//
//protected:
//	virtual void OnDataAcquired() override;
//
//	virtual bool OnIsValidToRender() override;
//
//	virtual void OnUpdateUniforms(float fExtrapolatePercent) override;
//
//	virtual void PrepRenderStage(uint32 uiStageIndex, HyRenderMode &eRenderModeOut, HyBlendMode &eBlendModeOut, uint32 &uiNumInstancesOut, uint32 &uiNumVerticesPerInstOut, bool &bIsBatchable) override;
//	virtual bool WriteVertexData(uint32 uiNumInstances, HyVertexBuffer &vertexBufferRef, float fExtrapolatePercent) override;
//
//
//private: // Hide inherited functionality that doesn't exist for tile maps
//	using IHyLoadable::GetState;
//	using IHyLoadable::SetState;
//	using IHyLoadable::GetNumStates;
//	using IHyLoadable::GetName;
//	using IHyLoadable::GetPrefix;
//	using IHyLoadable2d::Init;
//	using IHyLoadable2d::Uninit;
//};

#endif /* HyTileMapChunk_h__ */
