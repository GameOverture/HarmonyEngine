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

TileMapView::TileMapView(HyEntity2d *pParent /*= nullptr*/) :
	IEditModeView(pParent),
	m_pMouseHoverGrid(nullptr)
{
}

/*virtual*/ TileMapView::~TileMapView()
{
}

/*virtual*/ void TileMapView::SyncWithModel(EditModeState eEditModeState) /*override*/
{
	TileMapModel *pModel = static_cast<TileMapModel *>(GetModel());

	const float fMouseHoverGridWidth = 500.0f;
	const float fMouseHoverGridHeight = 500.0f;
	if(m_pMouseHoverGrid == nullptr || pModel->GetLayout() != m_pMouseHoverGrid->GetLayout())
	{
		if(m_pMouseHoverGrid == nullptr)
		{
			m_pMouseHoverGrid = new TileMapGrid(pModel->GetLayout(), glm::vec2(pModel->GetTileSize()), glm::vec2(fMouseHoverGridWidth, fMouseHoverGridHeight));

			if(pModel->GetLayout() == HYTILEMAPLAYOUT_HexagonFlatTop || pModel->GetLayout() == HYTILEMAPLAYOUT_HexagonPointTop)
				m_pMouseHoverGrid->SetShader(pModel->GetHexShader());
			else
				m_pMouseHoverGrid->SetShader(pModel->GetSquareShader());
		}
		else
			m_pMouseHoverGrid->Reset(pModel->GetLayout(), glm::vec2(pModel->GetTileSize()), glm::vec2(fMouseHoverGridWidth, fMouseHoverGridHeight));
	}
}
