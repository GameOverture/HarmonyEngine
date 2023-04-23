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
#include "AudioUndoCmd.h"
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
	GetBelowStatesLayout()->addItem(ui->verticalLayout);

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
	//ui->playListTableView->setItemDelegate(new WidgetSpriteDelegate(&m_ItemRef, ui->framesView, this));

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
	AudioPlayListModel &playListModelRef = static_cast<AudioModel *>(m_ItemRef.GetModel())->GetPlayListModel(GetCurStateIndex());
	ui->playListTableView->setModel(&playListModelRef);

	// TODO: Determine if connect() leaks if I keep assigning a new sender
	QItemSelectionModel *pSelModel = ui->playListTableView->selectionModel();
	connect(pSelModel, SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
			this, SLOT(on_playListTableView_selectionChanged(const QItemSelection &, const QItemSelection &)));
	
	//////////////////////////////////////////////////////

	int iCurNumAudio = static_cast<AudioStateData *>(GetCurStateData())->GetPlayListModel().rowCount();
	bool bHasSelection = iCurNumAudio > 0 && ui->playListTableView->currentIndex().row() >= 0;

	ui->actionOrderAudioUpwards->setEnabled(ui->playListTableView->currentIndex().row() != 0 && iCurNumAudio > 1);
	ui->actionOrderAudioDownwards->setEnabled(ui->playListTableView->currentIndex().row() != iCurNumAudio - 1 && iCurNumAudio > 1);
	ui->actionRemoveAudio->setEnabled(bHasSelection);

	//////////////////////////////////////////////////////
	PropertiesTreeModel &propertiesModelRef = static_cast<AudioModel *>(m_ItemRef.GetModel())->GetPropertiesModel(GetCurStateIndex());
	ui->statePropertiesTreeView->setModel(&propertiesModelRef);
	ui->statePropertiesTreeView->resizeColumnToContents(0);
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

	// iSubStateIndex represents which row to select
	if(subState.toInt() >= 0 && ui->playListTableView->model()->rowCount() > 0)
	{
		int iRowToSelect = subState.toInt();
		if(iRowToSelect < ui->playListTableView->model()->rowCount())
			ui->playListTableView->selectRow(iRowToSelect);
		else
			ui->playListTableView->selectRow(0);
	}
}

QTableView *AudioWidget::GetPlayListTableView()
{
	return ui->playListTableView;
}

void AudioWidget::on_playListTableView_selectionChanged(const QItemSelection &newSelection, const QItemSelection &oldSelection)
{
	UpdateActions();
}

void AudioWidget::on_actionAddAudio_triggered()
{
	QList<IAssetItemData *> selectedAssetsList;
	QList<TreeModelItemData *> selectedFiltersList;
	m_ItemRef.GetProject().GetAudioWidget()->GetSelected(selectedAssetsList, selectedFiltersList);

	QList<AudioAsset *> audioList;
	for(IAssetItemData *pAsset : selectedAssetsList)
		audioList.push_back(static_cast<AudioAsset *>(pAsset));

	QUndoCommand *pCmd = new AudioUndoCmd_AddAssets(m_ItemRef, GetCurStateIndex(), audioList);
	GetItem().GetUndoStack()->push(pCmd);
}

void AudioWidget::on_actionRemoveAudio_triggered()
{
	AudioPlayListModel *pPlayListModel = static_cast<AudioPlayListModel *>(ui->playListTableView->model());

	QList<AudioAsset *> removeList;
	removeList.append(pPlayListModel->GetAudioAssetAt(ui->playListTableView->currentIndex().row())->GetAudioAsset());

	QUndoCommand *pCmd = new AudioUndoCmd_RemoveAssets(m_ItemRef, GetCurStateIndex(), removeList);
	GetItem().GetUndoStack()->push(pCmd);
}

void AudioWidget::on_actionOrderAudioUpwards_triggered()
{
	QVariantList paramList;
	paramList.append(ui->playListTableView->currentIndex().row());
	paramList.append(ui->playListTableView->currentIndex().row() - 1);
	QUndoCommand *pCmd = new AudioUndoCmd(AUDIOCMD_OrderAudio, m_ItemRef, paramList);
	GetItem().GetUndoStack()->push(pCmd);
}

void AudioWidget::on_actionOrderAudioDownwards_triggered()
{
	QVariantList paramList;
	paramList.append(ui->playListTableView->currentIndex().row());
	paramList.append(ui->playListTableView->currentIndex().row() + 1);
	QUndoCommand *pCmd = new AudioUndoCmd(AUDIOCMD_OrderAudio, m_ItemRef, paramList);
	GetItem().GetUndoStack()->push(pCmd);
}
