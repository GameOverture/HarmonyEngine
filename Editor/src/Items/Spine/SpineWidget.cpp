/**************************************************************************
*	SpineWidget.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "global.h"
#include "SpineWidget.h"
#include "Project.h"
#include "SpineUndoCmds.h"
#include "SpineDraw.h"

SpineWidget::SpineWidget(ProjectItemData &itemRef, QWidget *parent) :
	IWidget(itemRef, parent)
{
	ui.setupUi(this);

	// Remove and re-add the main layout that holds everything. This makes the Qt Designer (.ui) files work with the base class 'IWidget'. Otherwise it jumbles them together.
	layout()->removeItem(ui.verticalLayout_2);
	layout()->addItem(ui.verticalLayout_2);
}

SpineWidget::~SpineWidget()
{
}

/*virtual*/ void SpineWidget::OnGiveMenuActions(QMenu *pMenu) /*override*/
{
	//pMenu->addAction(ui.actionAddFill);
	//pMenu->addAction(ui.actionAddEdge);
	//pMenu->addAction(ui.actionAddEdgeFill);
	//pMenu->addAction(ui.actionAddInner);
	//pMenu->addAction(ui.actionAddSDF);
	//pMenu->addSeparator();
	//pMenu->addAction(ui.actionOrderLayerUp);
	//pMenu->addAction(ui.actionOrderLayerDown);
	//pMenu->addAction(ui.actionRemoveLayer);
}

/*virtual*/ void SpineWidget::OnUpdateActions() /*override*/
{
}

/*virtual*/ void SpineWidget::OnFocusState(int iStateIndex, QVariant subState) /*override*/
{
	SpineDraw *pDraw = static_cast<SpineDraw *>(m_ItemRef.GetDraw());
}
