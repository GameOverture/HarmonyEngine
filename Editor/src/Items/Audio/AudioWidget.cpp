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
#include "AudioModel.h"
#include "AudioWidget.h"
#include "AudioPlayListModel.h"
#include "PropertiesTreeModel.h"
#include "ManagerWidget.h"

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

	ui->btnAddAudio->setDefaultAction(ui->actionAddAudio);
	ui->btnRemoveAudio->setDefaultAction(ui->actionRemoveAudio);
	ui->btnOrderAudioUp->setDefaultAction(ui->actionOrderAudioUpwards);
	ui->btnOrderAudioDown->setDefaultAction(ui->actionOrderAudioDownwards);

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
	pMenu->addAction(ui->actionAddAudio);
	pMenu->addAction(ui->actionRemoveAudio);
	pMenu->addAction(ui->actionOrderAudioUpwards);
	pMenu->addAction(ui->actionOrderAudioDownwards);
	//pMenu->addSeparator();
}

/*virtual*/ void AudioWidget::OnUpdateActions() /*override*/
{
	int iCurNumAudio = static_cast<AudioStateData *>(GetCurStateData())->GetPlayListModel().rowCount();
	bool bHasSelection = iCurNumAudio > 0 && ui->playListTableView->currentIndex().row() >= 0;

	ui->actionOrderAudioUpwards->setEnabled(ui->playListTableView->currentIndex().row() != 0 && iCurNumAudio > 1);
	ui->actionOrderAudioDownwards->setEnabled(ui->playListTableView->currentIndex().row() != iCurNumAudio - 1 && iCurNumAudio > 1);
	ui->actionRemoveAudio->setEnabled(bHasSelection);

	//////////////////////////////////////////////////////
	AudioPlayListModel &playListModelRef = static_cast<AudioModel *>(m_ItemRef.GetModel())->GetPlayListModel(GetCurStateIndex());
	ui->playListTableView->setModel(&playListModelRef);
	
	//////////////////////////////////////////////////////
	PropertiesTreeModel &propertiesModelRef = static_cast<AudioModel *>(m_ItemRef.GetModel())->GetPropertiesModel(GetCurStateIndex());
	ui->statePropertiesTreeView->setModel(&propertiesModelRef);
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

void AudioWidget::on_actionAddAudio_triggered()
{
	QList<AssetItemData *> selectedAssetsList;
	QList<TreeModelItemData *> selectedFiltersList;
	m_ItemRef.GetProject().GetAudioWidget()->GetSelected(selectedAssetsList, selectedFiltersList);

	QUndoCommand *pCmd = new UndoCmd_LinkStateAssets("Add Audio", m_ItemRef, GetCurStateIndex(), selectedAssetsList);
	GetItem().GetUndoStack()->push(pCmd);
}

void AudioWidget::on_actionRemoveAudio_triggered()
{
}

void AudioWidget::on_actionOrderAudioUpwards_triggered()
{
}

void AudioWidget::on_actionOrderAudioDownwards_triggered()
{
}
