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

/*virtual*/ void ContainerEditView::OnSyncModel(EditModeState eEditModeState, EditModeAction eResult) /*override*/
{
}
