/**************************************************************************
 *	TileMapWidget.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2024 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "TileMapWidget.h"
#include "ui_TileMapWidget.h"
#include "GlobalUndoCmds.h"
#include "SpriteUndoCmds.h"
#include "DlgInputName.h"
#include "SpriteDraw.h"
#include "ManagerWidget.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QAction>
#include <QShortcut>

TileMapWidget::TileMapWidget(ProjectItemData &itemRef, QWidget *pParent) :
	IWidget(itemRef, pParent),
	ui(new Ui::TileMapWidget)
{
	ui->setupUi(this);

	// Remove and re-add the main layout that holds everything. This makes the Qt Designer (.ui) files work with the base class 'IWidget'. Otherwise it jumbles them together.
	//layout()->removeItem(ui->verticalLayout);
	//GetBelowStatesLayout()->addItem(ui->verticalLayout);
	//
	//ui->btnAddFrames->setDefaultAction(ui->actionImportFrames);
	//ui->btnRemoveFrame->setDefaultAction(ui->actionRemoveFrames);
	//ui->btnRemoveAllFrames->setDefaultAction(ui->actionRemoveAllFrames);
	//ui->actionApplyToAll->setChecked(true);
	//ui->btnApplyToAll->setDefaultAction(ui->actionApplyToAll);
	//ui->btnOrderFrameUp->setDefaultAction(ui->actionOrderFrameUpwards);
	//ui->btnOrderFrameDown->setDefaultAction(ui->actionOrderFrameDownwards);
	//ui->btnPlay->setDefaultAction(ui->actionPlay);
	//ui->btnFirstFrame->setDefaultAction(ui->actionFirstFrame);
	//ui->btnLastFrame->setDefaultAction(ui->actionLastFrame);

	//new QShortcut(QKeySequence(Qt::Key_Space), this, SLOT(on_actionPlay_triggered()));
	//new QShortcut(QKeySequence(Qt::Key_Q), this, SLOT(on_actionFirstFrame_triggered()));
	//new QShortcut(QKeySequence(Qt::Key_E), this, SLOT(on_actionLastFrame_triggered()));

	//new QShortcut(QKeySequence(Qt::SHIFT + Qt::Key_Q), this, SLOT(OnKeyShiftQ()));
	//new QShortcut(QKeySequence(Qt::SHIFT + Qt::Key_E), this, SLOT(OnKeyShiftE()));
}

/*virtual*/ TileMapWidget::~TileMapWidget()
{
	delete ui;
}

/*virtual*/ void TileMapWidget::OnGiveMenuActions(QMenu *pMenu) /*override*/
{
	//pMenu->addAction(ui->actionImportFrames);
	//pMenu->addAction(ui->actionRemoveFrames);
	//pMenu->addAction(ui->actionOrderFrameUpwards);
	//pMenu->addAction(ui->actionOrderFrameDownwards);
	//pMenu->addSeparator();
	//pMenu->addAction(ui->actionAlignUp);
	//pMenu->addAction(ui->actionAlignUp);
	//pMenu->addAction(ui->actionAlignLeft);
	//pMenu->addAction(ui->actionAlignDown);
	//pMenu->addAction(ui->actionAlignRight);
	//pMenu->addAction(ui->actionAlignCenterHorizontal);
	//pMenu->addAction(ui->actionAlignCenterVertical);
	//pMenu->addAction(ui->actionApplyToAll);
}

/*virtual*/ void TileMapWidget::OnUpdateActions() /*override*/
{
}

/*virtual*/ void TileMapWidget::OnFocusState(int iStateIndex, QVariant subState) /*override*/
{
}
