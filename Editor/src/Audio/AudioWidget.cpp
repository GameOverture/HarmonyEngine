/**************************************************************************
*	AudioWidget.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2019 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Global.h"
#include "AudioWidget.h"
#include "ui_AudioWidget.h"
#include "GlobalUndoCmds.h"

#include <QLayout>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QAction>

AudioWidget::AudioWidget(ProjectItemData &itemRef, QWidget *pParent) :
	IWidget(itemRef, pParent),
	ui(new Ui::AudioWidget)
{
	ui->setupUi(this);

	// Remove and re-add the main layout that holds everything. This makes the Qt Designer (.ui) files work with the base class 'IWidget'. Otherwise it jumbles them together.
	layout()->removeItem(ui->verticalLayout);
	layout()->addItem(ui->verticalLayout);



	//ui->btnAddFrames->setDefaultAction(ui->actionImportFrames);
	//ui->btnRemoveFrame->setDefaultAction(ui->actionRemoveFrames);
	//ui->btnOrderFrameUp->setDefaultAction(ui->actionOrderFrameUpwards);
	//ui->btnOrderFrameDown->setDefaultAction(ui->actionOrderFrameDownwards);
	//ui->btnPlay->setDefaultAction(ui->actionPlay);
	//ui->btnFirstFrame->setDefaultAction(ui->actionFirstFrame);
	//ui->btnLastFrame->setDefaultAction(ui->actionLastFrame);

	////ui->framesView->setModel(static_cast<SpriteModel *>(m_ItemRef.GetModel())->GetStateData(0)->pFramesModel);
	//ui->framesView->setItemDelegate(new WidgetSpriteDelegate(&m_ItemRef, ui->framesView, this));
	//QItemSelectionModel *pSelModel = ui->framesView->selectionModel();
	//connect(pSelModel, SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
	//	this, SLOT(on_framesView_selectionChanged(const QItemSelection &, const QItemSelection &)));

	//FocusState(0, -1);
}

AudioWidget::~AudioWidget()
{
	delete ui;
}

/*virtual*/ void AudioWidget::OnGiveMenuActions(QMenu *pMenu) /*override*/
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

/*virtual*/ void AudioWidget::OnUpdateActions() /*override*/
{
	//int iCurNumFrames = static_cast<SpriteStateData *>(GetCurStateData())->GetFramesModel()->rowCount();
	//bool bFrameIsSelected = iCurNumFrames > 0 && ui->framesView->currentIndex().row() >= 0;

	//ui->actionAlignCenterHorizontal->setEnabled(bFrameIsSelected);
	//ui->actionAlignCenterVertical->setEnabled(bFrameIsSelected);
	//ui->actionAlignUp->setEnabled(bFrameIsSelected);
	//ui->actionAlignDown->setEnabled(bFrameIsSelected);
	//ui->actionAlignLeft->setEnabled(bFrameIsSelected);
	//ui->actionAlignRight->setEnabled(bFrameIsSelected);

	//ui->actionOrderFrameUpwards->setEnabled(ui->framesView->currentIndex().row() != 0 && iCurNumFrames > 1);
	//ui->actionOrderFrameDownwards->setEnabled(ui->framesView->currentIndex().row() != iCurNumFrames - 1 && iCurNumFrames > 1);
	//ui->actionRemoveFrames->setEnabled(bFrameIsSelected);
}

/*virtual*/ void AudioWidget::OnFocusState(int iStateIndex, QVariant subState) /*override*/
{
	//// Set the model of 'iStateIndex'
	//SpriteStateData *pCurStateData = static_cast<SpriteStateData *>(static_cast<SpriteModel *>(m_ItemRef.GetModel())->GetStateData(iStateIndex));
	//ui->framesView->setModel(pCurStateData->GetFramesModel());

	//QItemSelectionModel *pSelModel = ui->framesView->selectionModel();
	//connect(pSelModel, SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
	//	this, SLOT(on_framesView_selectionChanged(const QItemSelection &, const QItemSelection &)));

	//pCurStateData->GetLoopMapper()->AddCheckBoxMapping(ui->chkLoop);
	//pCurStateData->GetReverseMapper()->AddCheckBoxMapping(ui->chkReverse);
	//pCurStateData->GetBounceMapper()->AddCheckBoxMapping(ui->chkBounce);

	//// iSubStateIndex represents which row to select
	//if(subState.toInt() >= 0 && ui->framesView->model()->rowCount() > 0)
	//{
	//	int iRowToSelect = subState.toInt();
	//	if(iRowToSelect < ui->framesView->model()->rowCount())
	//		ui->framesView->selectRow(iRowToSelect);
	//	else
	//		ui->framesView->selectRow(0);
	//}
}
