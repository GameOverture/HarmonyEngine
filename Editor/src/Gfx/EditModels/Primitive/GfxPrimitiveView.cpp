/**************************************************************************
 *	GfxPrimitiveView.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2026 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "GfxPrimitiveView.h"
#include "GfxPrimitiveModel.h"

GfxPrimitiveView::GfxPrimitiveView(HyEntity2d *pParent /*= nullptr*/) :
	IGfxEditView(pParent),
	m_ShapeView(false, this),
	m_ChainView(this)
{
}

/*virtual*/ GfxPrimitiveView::~GfxPrimitiveView()
{
}

/*virtual*/ void GfxPrimitiveView::SyncColor() /*override*/
{
	if(m_pModel == nullptr)
		return;

	if(static_cast<GfxPrimitiveModel *>(m_pModel)->IsShapeModel())
		m_ShapeView.SyncColor();
	else
		m_ChainView.SyncColor();
}

/*virtual*/ void GfxPrimitiveView::ClearPreview() /*override*/
{
	m_ShapeView.ClearPreview();
	m_ChainView.ClearPreview();
}

/*virtual*/ void GfxPrimitiveView::OnSyncModel(EditModeState eEditModeState, EditModeAction eResult) /*override*/
{
	if(static_cast<GfxPrimitiveModel *>(m_pModel)->IsShapeModel())
		m_ShapeView.OnSyncModel(eEditModeState, eResult);
	else
		m_ChainView.OnSyncModel(eEditModeState, eResult);
}

/*virtual*/ void GfxPrimitiveView::OnSyncPreview(EditModeState eEditModeState, EditModeAction eResult, int iGrabPointIndex, glm::vec2 vDragDelta) /*override*/
{
	if(static_cast<GfxPrimitiveModel *>(m_pModel)->IsShapeModel())
		m_ShapeView.OnSyncPreview(eEditModeState, eResult, iGrabPointIndex, vDragDelta);
	else
		m_ChainView.OnSyncPreview(eEditModeState, eResult, iGrabPointIndex, vDragDelta);
}
