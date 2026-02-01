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

/*virtual*/ void GfxPrimitiveView::RefreshColor() /*override*/
{
	if(m_pModel == nullptr)
		return;

	if(static_cast<GfxPrimitiveModel *>(m_pModel)->IsShapeModel())
		m_ShapeView.RefreshColor();
	else
		m_ChainView.RefreshColor();
}

/*virtual*/ void GfxPrimitiveView::DoRefreshView(ShapeMouseMoveResult eResult, bool bMouseDown) /*override*/
{
	if(static_cast<GfxPrimitiveModel *>(m_pModel)->IsShapeModel())
		m_ShapeView.DoRefreshView(eResult, bMouseDown);
	else
		m_ChainView.DoRefreshView(eResult, bMouseDown);
}
