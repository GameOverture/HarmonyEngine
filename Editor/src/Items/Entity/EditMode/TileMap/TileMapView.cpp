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
	m_MouseHoverGrid(nullptr)
{
}

/*virtual*/ TileMapView::~TileMapView()
{
}

/*virtual*/ void TileMapView::SyncWithModel(EditModeState eEditModeState) /*override*/
{
	if(eEditModeState == EDITMODE_Off)
	{
		m_MouseHoverGrid.SetVisible(false);
		return;
	}

	TileMapModel *pModel = static_cast<TileMapModel *>(GetModel());

	const glm::vec2 vQuadDimensions(250.0f, 250.0f);
	m_MouseHoverGrid.Reset(glm::inverse(GetSceneTransform(0.0f)),
							vQuadDimensions,
							pModel->GetGridSize(),
							pModel->GetLayout(),
							pModel->GetStaggerIndex());

	if(pModel->GetLayout() == HYTILEMAPLAYOUT_HexagonFlatTop || pModel->GetLayout() == HYTILEMAPLAYOUT_HexagonPointTop)
		m_MouseHoverGrid.SetShader(pModel->GetHexShader());
	else
		m_MouseHoverGrid.SetShader(pModel->GetSquareShader());

	glm::vec2 ptWorldMousePos;
	if(HyEngine::Input().GetWorldMousePos(ptWorldMousePos))
		m_MouseHoverGrid.pos.Set(ptWorldMousePos);

	m_MouseHoverGrid.SetVisible(true);
}
