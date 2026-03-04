/**************************************************************************
 *	ContainerEditView.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2026 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "ContainerEditView.h"
#include "GfxShapeModel.h"
#include "GfxGrabPointView.h"

ContainerEditView::ContainerEditView(bool bIsFixture, HyEntity2d *pParent /*= nullptr*/) :
	IGfxEditView(pParent)
{
}

/*virtual*/ ContainerEditView::~ContainerEditView()
{
}

/*virtual*/ void ContainerEditView::SyncColor() /*override*/
{
	if(m_pModel == nullptr)
		return;
}

/*virtual*/ void ContainerEditView::ClearPreview() /*override*/
{
}

/*virtual*/ void ContainerEditView::OnSyncModel(EditModeState eEditModeState, EditModeAction eResult) /*override*/
{
}

/*virtual*/ void ContainerEditView::OnSyncPreview(EditModeState eEditModeState, EditModeAction eEditModeAction, int iGrabPointIndex, glm::vec2 vDragDelta) /*override*/
{
}
