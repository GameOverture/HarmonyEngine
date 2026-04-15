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
	m_CenterGrabPoint.SetVisible(false);
}

/*virtual*/ GfxPrimitiveView::~GfxPrimitiveView()
{
}

/*virtual*/ void GfxPrimitiveView::OnSyncModel(EditModeState eEditModeState, EditModeAction eResult) /*override*/
{
	m_CenterGrabPoint.SetVisible(false);

	if(m_pModel == nullptr)
	{
		m_ShapeView.OnSyncModel(EDITMODE_Off, EDITMODEACTION_None);
		m_ChainView.OnSyncModel(EDITMODE_Off, EDITMODEACTION_None);
		return;
	}

	if(static_cast<GfxPrimLayerModel *>(m_pModel)->IsShapeModel())
	{
		m_ShapeView.OnSyncModel(eEditModeState, eResult);
		m_ChainView.OnSyncModel(EDITMODE_Off, EDITMODEACTION_None);
	}
	else
	{
		m_ShapeView.OnSyncModel(EDITMODE_Off, EDITMODEACTION_None);
		m_ChainView.OnSyncModel(eEditModeState, eResult);
	}
}
