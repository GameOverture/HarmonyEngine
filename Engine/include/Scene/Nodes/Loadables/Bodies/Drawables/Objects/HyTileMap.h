/**************************************************************************
 *	HyTileMap.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2026 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyTileMap_h__
#define HyTileMap_h__

#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Loadables/Bodies/Drawables/IHyDrawable2d.h"
#include <array>

#define HYTILEMAP_CHUNKDIMENSION 32
#define HYTILEMAP_CHUNKSIZE (HYTILEMAP_CHUNKDIMENSION * HYTILEMAP_CHUNKDIMENSION)

class HyTileMap : public IHyDrawable2d
{
protected:
	uint16_t										m_uiTotalWidth;
	uint16_t										m_uiTotalHeight;

	struct TileChunk
	{
		glm::ivec2									m_vCoordinate;
		std::array<uint32_t, HYTILEMAP_CHUNKSIZE>	m_Tiles;

		HyTextureHandle								m_hTileTexture;
		bool										m_bDirty;
	};
	std::vector<TileChunk>							m_ChunkList;

public:
	HyTileMap(HyEntity2d *pParent = nullptr);
	HyTileMap(const HyTileMap &copyRef);
	virtual ~HyTileMap(void);

	const HyTileMap &operator=(const HyTileMap &rhs);

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

#endif /* HyTileMap_h__ */
