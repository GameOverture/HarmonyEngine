/**************************************************************************
*	IManagerWidget.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2020 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Global.h"
#include "IManagerWidget.h"
#include "ui_IManagerWidget.h"

#include "ProjectItemData.h"
#include "IModel.h"
#include "GlobalUndoCmds.h"
#include "DlgInputName.h"
#include "SpriteModels.h"
#include "TextModel.h"
#include "EntityModel.h"
#include "PrefabModel.h"

#include <QUndoCommand>

IManagerWidget::IManagerWidget(QWidget *pParent /*= nullptr*/) :
	QWidget(pParent),
	ui(new Ui::IManagerWidget)
{
	ui->setupUi(this);
}

IManagerWidget::~IManagerWidget()
{
	delete ui;
}


void IManagerWidget::OnContextMenu(const QPoint &pos)
{
}

void IManagerWidget::on_actionDeleteAssets_triggered()
{
}

void IManagerWidget::on_actionReplaceAssets_triggered()
{
}

void IManagerWidget::on_assetTree_itemSelectionChanged()
{
}

void IManagerWidget::on_actionRename_triggered()
{
}

void IManagerWidget::on_cmbBanks_currentIndexChanged(int index)
{
}

void IManagerWidget::on_actionAddBank_triggered()
{
}

void IManagerWidget::on_actionBankSettings_triggered()
{
}

void IManagerWidget::on_actionRemoveBank_triggered()
{
}

void IManagerWidget::on_actionBankTransfer_triggered(QAction *pAction)
{
}

void IManagerWidget::on_actionImportImages_triggered()
{
}

void IManagerWidget::on_actionImportDirectory_triggered()
{
}

void IManagerWidget::on_actionAddFilter_triggered()
{
}
