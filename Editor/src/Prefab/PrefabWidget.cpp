/**************************************************************************
 *	PrefabWidget.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2018 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "PrefabWidget.h"
#include "ui_PrefabWidget.h"

PrefabWidget::PrefabWidget(ProjectItemData &itemRef, QWidget *pParent /*= nullptr*/) :
	IWidget(itemRef, pParent),
	ui(new Ui::PrefabWidget)
{
	ui->setupUi(this);

	ui->propertyTree->setModel(&static_cast<PrefabModel *>(m_ItemRef.GetModel())->GetPropertiesModel());
}

PrefabWidget::~PrefabWidget()
{
	delete ui;
}

/*virtual*/ void PrefabWidget::OnGiveMenuActions(QMenu *pMenu) /*override*/
{
}

/*virtual*/ void PrefabWidget::OnUpdateActions() /*override*/
{
}

/*virtual*/ void PrefabWidget::OnFocusState(int iStateIndex, QVariant subState) /*override*/
{
}
