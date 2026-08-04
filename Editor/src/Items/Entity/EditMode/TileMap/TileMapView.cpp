/**************************************************************************
 *	TileMapView.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2026 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "TileMapView.h"
#include "TileMapModel.h"
#include "TileData.h"
#include "Assets/Nodes/Objects/HyTexturedQuadData.h"
#include "Assets/Files/HyFileAtlas.h"

BrushPreview::BrushPreview(HyEntity2d *pParent /*= nullptr*/) :
	HyEntity2d(pParent)
{

}

/*virtual*/ BrushPreview::~BrushPreview()
{
	Clear();
}

void BrushPreview::Clear()
{
	for(int i = 0; i < m_TileList.size(); ++i)
		delete m_TileList[i];
	m_TileList.clear();
}

void BrushPreview::Sync(const QMap<QPoint, TileData *> &brushMap, glm::vec2 vGridSize, HyTileMapLayout eLayout)
{
	Clear();
	for(auto iter = brushMap.constBegin(); iter != brushMap.constEnd(); ++iter)
	{
		const QPoint &pt = iter.key();
		TileData *pTileData = iter.value();
		AtlasTileSet *pTileSet = pTileData->GetTileSet();
		QSize sampleRegionSize = pTileSet->GetAtlasRegionSize();
		int iSubAtlasIndex = pTileSet->GetTileSubAtlasIndex(pTileData);
		int iNumCols = NUM_COLS_TILESET(pTileSet->GetNumSubAtlasTiles());

		HyTexturedQuad2d *pTileQuad = new HyTexturedQuad2d(pTileSet->GetChecksum(), pTileSet->GetBankId(), this);

		const HyTexturedQuadData *pTileQuadData = static_cast<const HyTexturedQuadData *>(pTileQuad->AcquireData());
		HyUvCoord uvCoords = pTileQuadData->GetUvCoords();
		int iX = uvCoords.left * pTileQuadData->GetAtlas()->GetWidth();
		iX += (iSubAtlasIndex % iNumCols) * sampleRegionSize.width();
		int iY = uvCoords.top * pTileQuadData->GetAtlas()->GetHeight();
		iY += (iSubAtlasIndex / iNumCols) * sampleRegionSize.height();
		
		pTileQuad->SetUvCoordinates(iX, iY, sampleRegionSize.width(), sampleRegionSize.height());
		pTileQuad->pos.Set(pt.x(), pt.y());
		
		m_TileList.push_back(pTileQuad);
	}
}

TileMapView::TileMapView(HyEntity2d *pParent /*= nullptr*/) :
	IEditModeView(pParent),
	m_MouseHoverGrid(nullptr)
{
}

/*virtual*/ TileMapView::~TileMapView()
{
}

void TileMapView::SyncMouseHoverGrid()
{
	TileMapModel *pModel = static_cast<TileMapModel *>(GetModel());

	const glm::vec2 vQuadDimensions(250.0f, 250.0f);
	m_MouseHoverGrid.Sync(glm::inverse(GetSceneTransform(0.0f)),
						  vQuadDimensions,
						  pModel->GetGridSize(),
						  pModel->GetLayout());

	m_MouseHoverGrid.SetShader(pModel->GetGridShader());

	glm::vec2 ptWorldMousePos;
	if(HyEngine::Input().GetWorldMousePos(ptWorldMousePos))
		m_MouseHoverGrid.pos.Set(ptWorldMousePos);
}

/*virtual*/ void TileMapView::SyncWithModel(EditModeState eEditModeState) /*override*/
{
	if(eEditModeState == EDITMODE_Off)
	{
		m_MouseHoverGrid.SetVisible(false);
		return;
	}

	m_MouseHoverGrid.SetVisible(true);
	SyncMouseHoverGrid();
}
