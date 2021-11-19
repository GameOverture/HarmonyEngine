/**************************************************************************
*	SpineDraw.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Global.h"
#include "SpineDraw.h"
#include "ProjectItemData.h"
#include "SpineModel.h"
#include "Harmony.h"
#include "HarmonyWidget.h"

SpineDraw::SpineDraw(ProjectItemData *pProjItem, const FileDataPair &initFileDataRef) :
	IDraw(pProjItem, initFileDataRef)
{
	m_Spine.Init("", "+GuiPreview", this);
}

SpineDraw::~SpineDraw()
{
}

/*virtual*/ void SpineDraw::OnApplyJsonData(HyJsonObj itemDataObj) /*override*/
{
}

/*virtual*/ void SpineDraw::OnShow() /*override*/
{
	SetVisible(true);
}
 
/*virtual*/ void SpineDraw::OnHide() /*override*/
{
	SetVisible(false, true);
}

/*virtual*/ void SpineDraw::OnResizeRenderer() /*override*/
{
}
