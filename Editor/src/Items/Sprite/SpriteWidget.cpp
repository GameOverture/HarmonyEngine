/**************************************************************************
 *	SpriteWidget.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "SpriteWidget.h"
#include "ui_SpriteWidget.h"
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

SpriteWidget::SpriteWidget(ProjectItemData &itemRef, QWidget *pParent) :
	IWidget(itemRef, pParent),
	ui(new Ui::SpriteWidget),
	m_bPlayActive(false),
	m_fElapsedTime(0.0),
	m_bIsBounced(false)
{
	ui->setupUi(this);

	// Remove and re-add the main layout that holds everything. This makes the Qt Designer (.ui) files work with the base class 'IWidget'. Otherwise it jumbles them together.
	layout()->removeItem(ui->verticalLayout);
	GetBelowStatesLayout()->addItem(ui->verticalLayout);
	
	ui->btnAddFrames->setDefaultAction(ui->actionImportFrames);
	ui->btnRemoveFrame->setDefaultAction(ui->actionRemoveFrames);
	ui->btnRemoveAllFrames->setDefaultAction(ui->actionRemoveAllFrames);
	ui->actionApplyToAll->setChecked(true);
	ui->btnApplyToAll->setDefaultAction(ui->actionApplyToAll);
	ui->btnOrderFrameUp->setDefaultAction(ui->actionOrderFrameUpwards);
	ui->btnOrderFrameDown->setDefaultAction(ui->actionOrderFrameDownwards);
	ui->btnPlay->setDefaultAction(ui->actionPlay);
	ui->btnFirstFrame->setDefaultAction(ui->actionFirstFrame);
	ui->btnLastFrame->setDefaultAction(ui->actionLastFrame);

	//ui->framesView->setModel(static_cast<SpriteModel *>(m_ItemRef.GetModel())->GetStateData(0)->pFramesModel);
	ui->framesView->setItemDelegate(new WidgetSpriteDelegate(&m_ItemRef, ui->framesView, this));
	QItemSelectionModel *pSelModel = ui->framesView->selectionModel();
	connect(pSelModel, SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
			this, SLOT(on_framesView_selectionChanged(const QItemSelection &, const QItemSelection &)));

	new QShortcut(QKeySequence(Qt::Key_Space), this, SLOT(on_actionPlay_triggered()));
	new QShortcut(QKeySequence(Qt::Key_Q), this, SLOT(on_actionFirstFrame_triggered()));
	new QShortcut(QKeySequence(Qt::Key_E), this, SLOT(on_actionLastFrame_triggered()));

	new QShortcut(QKeySequence(Qt::SHIFT + Qt::Key_Q), this, SLOT(OnKeyShiftQ()));
	new QShortcut(QKeySequence(Qt::SHIFT + Qt::Key_E), this, SLOT(OnKeyShiftE()));
}

/*virtual*/ SpriteWidget::~SpriteWidget()
{
	delete ui;
}

/*virtual*/ void SpriteWidget::OnGiveMenuActions(QMenu *pMenu) /*override*/
{
	pMenu->addAction(ui->actionImportFrames);
	pMenu->addAction(ui->actionRemoveFrames);
	pMenu->addAction(ui->actionOrderFrameUpwards);
	pMenu->addAction(ui->actionOrderFrameDownwards);
	pMenu->addSeparator();
	pMenu->addAction(ui->actionAlignUp);
	pMenu->addAction(ui->actionAlignUp);
	pMenu->addAction(ui->actionAlignLeft);
	pMenu->addAction(ui->actionAlignDown);
	pMenu->addAction(ui->actionAlignRight);
	pMenu->addAction(ui->actionAlignCenterHorizontal);
	pMenu->addAction(ui->actionAlignCenterVertical);
	pMenu->addAction(ui->actionApplyToAll);
}

/*virtual*/ void SpriteWidget::OnUpdateActions() /*override*/
{
	int iCurNumFrames = static_cast<SpriteStateData *>(GetCurStateData())->GetFramesModel()->rowCount();
	bool bFrameIsSelected = iCurNumFrames > 0 && ui->framesView->currentIndex().row() >= 0;

	ui->actionAlignCenterHorizontal->setEnabled(bFrameIsSelected);
	ui->actionAlignCenterVertical->setEnabled(bFrameIsSelected);
	ui->actionAlignUp->setEnabled(bFrameIsSelected);
	ui->actionAlignDown->setEnabled(bFrameIsSelected);
	ui->actionAlignLeft->setEnabled(bFrameIsSelected);
	ui->actionAlignRight->setEnabled(bFrameIsSelected);

	ui->actionOrderFrameUpwards->setEnabled(ui->framesView->currentIndex().row() != 0 && iCurNumFrames > 1);
	ui->actionOrderFrameDownwards->setEnabled(ui->framesView->currentIndex().row() != iCurNumFrames - 1 && iCurNumFrames > 1);
	ui->actionRemoveFrames->setEnabled(bFrameIsSelected);
}

/*virtual*/ void SpriteWidget::OnFocusState(int iStateIndex, QVariant subState) /*override*/
{
	// Set the model of 'iStateIndex'
	SpriteStateData *pCurStateData = static_cast<SpriteStateData *>(static_cast<SpriteModel *>(m_ItemRef.GetModel())->GetStateData(iStateIndex));
	ui->framesView->setModel(pCurStateData->GetFramesModel());

	// TODO: Determine if connect() leaks if I keep assigning a new asender
	QItemSelectionModel *pSelModel = ui->framesView->selectionModel();
	connect(pSelModel, SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
		this, SLOT(on_framesView_selectionChanged(const QItemSelection &, const QItemSelection &)));

	pCurStateData->GetLoopMapper()->AddCheckBoxMapping(ui->chkLoop);
	pCurStateData->GetReverseMapper()->AddCheckBoxMapping(ui->chkReverse);
	pCurStateData->GetBounceMapper()->AddCheckBoxMapping(ui->chkBounce);

	// iSubStateIndex represents which row to select
	if(subState.toInt() >= 0 && ui->framesView->model()->rowCount() > 0)
	{
		int iRowToSelect = subState.toInt();
		if(iRowToSelect < ui->framesView->model()->rowCount())
			ui->framesView->selectRow(iRowToSelect);
		else
			ui->framesView->selectRow(0);
	}

	if(ui->framesView->currentIndex().isValid() == false && ui->framesView->model()->rowCount() > 0)
		ui->framesView->selectRow(0);
}

bool SpriteWidget::IsPlayingAnim()
{
	return m_bPlayActive;
}

void SpriteWidget::SetSelectedFrame(int iFrameIndex)
{
	ui->framesView->selectRow(iFrameIndex);
}

void SpriteWidget::StopPlayingAnim()
{
	m_bPlayActive = false;
	ui->btnPlay->setIcon(QIcon(":/icons16x16/media-play.png"));
}

void SpriteWidget::GetSpriteInfo(int &iStateIndexOut, int &iFrameIndexOut)
{
	iStateIndexOut = GetCurStateIndex();
	iFrameIndexOut = ui->framesView->currentIndex().row();
}

QPoint SpriteWidget::GetSelectedFrameOffset()
{
	SpriteFramesModel *pSpriteFramesModel = static_cast<SpriteFramesModel *>(ui->framesView->model());
	SpriteFrame *pSpriteFrame = pSpriteFramesModel->GetFrameAt(ui->framesView->currentIndex().row());
	if(pSpriteFrame)
		return pSpriteFrame->m_vOffset;
	
	HyGuiLog("SpriteWidget::GetSelectedFrameOffset() - No frame selected", LOGTYPE_Error);
	return QPoint(0, 0);
}

void SpriteWidget::ApplyTranslate(QPoint ptPosition, bool bApplyAsOffset)
{
	QUndoCommand *pCmd = new SpriteUndoCmd_PositionFrame(ui->framesView, ui->actionApplyToAll->isChecked() ? -1 : ui->framesView->currentIndex().row(), ptPosition, bApplyAsOffset);
	m_ItemRef.GetUndoStack()->push(pCmd);
}

void SpriteWidget::RefreshFrameRateSpinBox()
{
	SpriteFramesModel *pSpriteFramesModel = static_cast<SpriteFramesModel *>(ui->framesView->model());

	ui->sbFrameRate->setEnabled(true);

	ui->sbFrameRate->blockSignals(true);
	SpriteFrame *pSpriteFrame = pSpriteFramesModel->GetFrameAt(ui->framesView->currentIndex().row());
	if(pSpriteFrame)
		ui->sbFrameRate->setValue(pSpriteFrame->m_fDuration);
	else
		ui->sbFrameRate->setValue(0.0f);
	ui->sbFrameRate->blockSignals(false);
}

void SpriteWidget::on_framesView_selectionChanged(const QItemSelection &newSelection, const QItemSelection &oldSelection)
{
	if(m_ItemRef.GetDraw())
		static_cast<SpriteDraw *>(m_ItemRef.GetDraw())->SetFrame(GetCurStateIndex(), ui->framesView->currentIndex().row());

	RefreshFrameRateSpinBox();

	UpdateActions();
}

void SpriteWidget::on_actionAlignLeft_triggered()
{
	SpriteFramesModel *pSpriteFramesModel = static_cast<SpriteFramesModel *>(ui->framesView->model());

	if(pSpriteFramesModel->rowCount() == 0)
		return;

	if(ui->actionApplyToAll->isChecked())
	{
		QList<int> newPosList;
		for(int i = 0; i < pSpriteFramesModel->rowCount(); ++i)
			newPosList.append(0.0f);

		m_ItemRef.GetUndoStack()->push(new SpriteUndoCmd_SetXFrame(ui->framesView, -1, newPosList));
		return;
	}

	QList<int> newPosList;
	newPosList.append(0.0f);
	m_ItemRef.GetUndoStack()->push(new SpriteUndoCmd_SetXFrame(ui->framesView, ui->framesView->currentIndex().row(), newPosList));
}

void SpriteWidget::on_actionAlignRight_triggered()
{
	SpriteFramesModel *pSpriteFramesModel = static_cast<SpriteFramesModel *>(ui->framesView->model());

	if(pSpriteFramesModel->rowCount() == 0)
		return;

	if(ui->actionApplyToAll->isChecked())
	{
		QList<int> newPosList;
		for(int i = 0; i < pSpriteFramesModel->rowCount(); ++i)
			newPosList.append(pSpriteFramesModel->GetFrameAt(i)->m_pFrame->GetSize().width() * -1);

		m_ItemRef.GetUndoStack()->push(new SpriteUndoCmd_SetXFrame(ui->framesView, -1, newPosList));
		return;
	}

	QList<int> newPosList;
	newPosList.append(pSpriteFramesModel->GetFrameAt(ui->framesView->currentIndex().row())->m_pFrame->GetSize().width() * -1);
	m_ItemRef.GetUndoStack()->push(new SpriteUndoCmd_SetXFrame(ui->framesView, ui->framesView->currentIndex().row(), newPosList));
}

void SpriteWidget::on_actionAlignUp_triggered()
{
	SpriteFramesModel *pSpriteFramesModel = static_cast<SpriteFramesModel *>(ui->framesView->model());

	if(pSpriteFramesModel->rowCount() == 0)
		return;

	if(ui->actionApplyToAll->isChecked())
	{
		QList<int> newPosList;
		for(int i = 0; i < pSpriteFramesModel->rowCount(); ++i)
			newPosList.append(pSpriteFramesModel->GetFrameAt(i)->m_pFrame->GetSize().height() * -1);

		m_ItemRef.GetUndoStack()->push(new SpriteUndoCmd_SetYFrame(ui->framesView, -1, newPosList));
		return;
	}

	QList<int> newPosList;
	newPosList.append(pSpriteFramesModel->GetFrameAt(ui->framesView->currentIndex().row())->m_pFrame->GetSize().height() * -1);
	m_ItemRef.GetUndoStack()->push(new SpriteUndoCmd_SetYFrame(ui->framesView, ui->framesView->currentIndex().row(), newPosList));
}

void SpriteWidget::on_actionAlignDown_triggered()
{
	SpriteFramesModel *pSpriteFramesModel = static_cast<SpriteFramesModel *>(ui->framesView->model());

	if(pSpriteFramesModel->rowCount() == 0)
		return;

	if(ui->actionApplyToAll->isChecked())
	{
		QList<int> newPosList;
		for(int i = 0; i < pSpriteFramesModel->rowCount(); ++i)
			newPosList.append(0.0f);

		m_ItemRef.GetUndoStack()->push(new SpriteUndoCmd_SetYFrame(ui->framesView, -1, newPosList));
		return;
	}

	QList<int> newPosList;
	newPosList.append(0.0f);
	m_ItemRef.GetUndoStack()->push(new SpriteUndoCmd_SetYFrame(ui->framesView, ui->framesView->currentIndex().row(), newPosList));
}

void SpriteWidget::on_actionAlignCenterVertical_triggered()
{
	SpriteFramesModel *pSpriteFramesModel = static_cast<SpriteFramesModel *>(ui->framesView->model());

	if(pSpriteFramesModel->rowCount() == 0)
		return;

	if(ui->actionApplyToAll->isChecked())
	{
		QList<int> newPosList;
		for(int i = 0; i < pSpriteFramesModel->rowCount(); ++i)
			newPosList.append(pSpriteFramesModel->GetFrameAt(i)->m_pFrame->GetSize().height() * -0.5f);

		m_ItemRef.GetUndoStack()->push(new SpriteUndoCmd_SetYFrame(ui->framesView, -1, newPosList));
		return;
	}

	QList<int> newPosList;
	newPosList.append(pSpriteFramesModel->GetFrameAt(ui->framesView->currentIndex().row())->m_pFrame->GetSize().height() * -0.5f);
	m_ItemRef.GetUndoStack()->push(new SpriteUndoCmd_SetYFrame(ui->framesView, ui->framesView->currentIndex().row(), newPosList));
}

void SpriteWidget::on_actionAlignCenterHorizontal_triggered()
{
	SpriteFramesModel *pSpriteFramesModel = static_cast<SpriteFramesModel *>(ui->framesView->model());

	if(pSpriteFramesModel->rowCount() == 0)
		return;

	if(ui->actionApplyToAll->isChecked())
	{
		QList<int> newPosList;
		for(int i = 0; i < pSpriteFramesModel->rowCount(); ++i)
			newPosList.append(pSpriteFramesModel->GetFrameAt(i)->m_pFrame->GetSize().width() * -0.5f);

		m_ItemRef.GetUndoStack()->push(new SpriteUndoCmd_SetXFrame(ui->framesView, -1, newPosList));
		return;
	}

	QList<int> newPosList;
	newPosList.append(pSpriteFramesModel->GetFrameAt(ui->framesView->currentIndex().row())->m_pFrame->GetSize().width() * -0.5f);
	m_ItemRef.GetUndoStack()->push(new SpriteUndoCmd_SetXFrame(ui->framesView, ui->framesView->currentIndex().row(), newPosList));
}

void SpriteWidget::on_actionImportFrames_triggered()
{
	QList<IAssetItemData *> selectedAssetsList; QList<TreeModelItemData *> selectedFiltersList;
	m_ItemRef.GetProject().GetAtlasWidget()->GetSelected(selectedAssetsList, selectedFiltersList, true);

	QList<AtlasFrame *> frameList;
	for(IAssetItemData *pAsset : selectedAssetsList)
		frameList.push_back(static_cast<AtlasFrame *>(pAsset));

	QUndoCommand *pCmd = new SpriteUndoCmd_AddFrames(m_ItemRef, GetCurStateIndex(), frameList);
	GetItem().GetUndoStack()->push(pCmd);
}

void SpriteWidget::on_actionRemoveFrames_triggered()
{
	QList<AtlasFrame *> removeList;
	removeList.append(static_cast<SpriteFramesModel *>(ui->framesView->model())->GetFrameAt(ui->framesView->currentIndex().row())->m_pFrame);

	QUndoCommand *pCmd = new SpriteUndoCmd_RemoveFrames(m_ItemRef, GetCurStateIndex(), removeList);
	GetItem().GetUndoStack()->push(pCmd);
}

void SpriteWidget::on_actionRemoveAllFrames_triggered()
{
	QList<AtlasFrame *> removeList;
	for(int i = 0; i < static_cast<SpriteFramesModel *>(ui->framesView->model())->rowCount(); ++i)
		removeList.append(static_cast<SpriteFramesModel *>(ui->framesView->model())->GetFrameAt(i)->m_pFrame);

	QUndoCommand *pCmd = new SpriteUndoCmd_RemoveFrames(m_ItemRef, GetCurStateIndex(), removeList);
	GetItem().GetUndoStack()->push(pCmd);
}

void SpriteWidget::on_actionOrderFrameUpwards_triggered()
{
	QUndoCommand *pCmd = new SpriteUndoCmd_OrderFrame(ui->framesView, ui->framesView->currentIndex().row(), ui->framesView->currentIndex().row() - 1);
	GetItem().GetUndoStack()->push(pCmd);
}

void SpriteWidget::on_actionOrderFrameDownwards_triggered()
{
	QUndoCommand *pCmd = new SpriteUndoCmd_OrderFrame(ui->framesView, ui->framesView->currentIndex().row(), ui->framesView->currentIndex().row() + 1);
	GetItem().GetUndoStack()->push(pCmd);
}

void SpriteWidget::on_actionPlay_triggered()
{
	m_bPlayActive = !m_bPlayActive;

	if(m_bPlayActive)
	{
		ui->btnPlay->setIcon(QIcon(":/icons16x16/media-pause.png"));
		m_bIsBounced = false;
		m_fElapsedTime = 0.0f;

		static_cast<SpriteDraw *>(m_ItemRef.GetDraw())->PlayAnim(ui->framesView->currentIndex().row());
	}
	else
		ui->btnPlay->setIcon(QIcon(":/icons16x16/media-play.png"));
}

void SpriteWidget::on_actionFirstFrame_triggered()
{
	ui->framesView->selectRow(0);
}

void SpriteWidget::on_actionLastFrame_triggered()
{
	ui->framesView->selectRow(static_cast<SpriteFramesModel *>(ui->framesView->model())->rowCount() - 1);
}

void SpriteWidget::OnKeyShiftQ()
{
	int iRowIndex = ui->framesView->currentIndex().row() - 1;
	if(iRowIndex < 0)
		iRowIndex = 0;

	ui->framesView->selectRow(iRowIndex);
}

void SpriteWidget::OnKeyShiftE()
{
	int iRowIndex = ui->framesView->currentIndex().row() + 1;
	if(iRowIndex >= static_cast<SpriteFramesModel *>(ui->framesView->model())->rowCount())
		iRowIndex = static_cast<SpriteFramesModel *>(ui->framesView->model())->rowCount() - 1;
	
	ui->framesView->selectRow(iRowIndex);
}

void SpriteWidget::on_chkReverse_clicked()
{
	QUndoCommand *pCmd = new UndoCmd_CheckBox(ui->chkReverse->text(), GetItem(), static_cast<SpriteStateData *>(GetCurStateData())->GetReverseMapper(), GetCurStateIndex());
	GetItem().GetUndoStack()->push(pCmd);
}

void SpriteWidget::on_chkLoop_clicked()
{
	QUndoCommand *pCmd = new UndoCmd_CheckBox(ui->chkLoop->text(), GetItem(), static_cast<SpriteStateData *>(GetCurStateData())->GetLoopMapper(), GetCurStateIndex());
	GetItem().GetUndoStack()->push(pCmd);
}

void SpriteWidget::on_chkBounce_clicked()
{
	QUndoCommand *pCmd = new UndoCmd_CheckBox(ui->chkBounce->text(), GetItem(), static_cast<SpriteStateData *>(GetCurStateData())->GetBounceMapper(), GetCurStateIndex());
	GetItem().GetUndoStack()->push(pCmd);
}

void SpriteWidget::on_btnHz10_clicked()
{
	QUndoCommand *pCmd = new SpriteUndoCmd_DurationFrame(ui->framesView, -1, 1.0f / 10.0f);
	GetItem().GetUndoStack()->push(pCmd);
}

void SpriteWidget::on_btnHz20_clicked()
{
	QUndoCommand *pCmd = new SpriteUndoCmd_DurationFrame(ui->framesView, -1, 1.0f / 20.0f);
	GetItem().GetUndoStack()->push(pCmd);
}

void SpriteWidget::on_btnHz30_clicked()
{
	QUndoCommand *pCmd = new SpriteUndoCmd_DurationFrame(ui->framesView, -1, 1.0f / 30.0f);
	GetItem().GetUndoStack()->push(pCmd);
}

void SpriteWidget::on_btnHz40_clicked()
{
	QUndoCommand *pCmd = new SpriteUndoCmd_DurationFrame(ui->framesView, -1, 1.0f / 40.0f);
	GetItem().GetUndoStack()->push(pCmd);
}

void SpriteWidget::on_btnHz50_clicked()
{
	QUndoCommand *pCmd = new SpriteUndoCmd_DurationFrame(ui->framesView, -1, 1.0f / 50.0f);
	GetItem().GetUndoStack()->push(pCmd);
}

void SpriteWidget::on_btnHz60_clicked()
{
	QUndoCommand *pCmd = new SpriteUndoCmd_DurationFrame(ui->framesView, -1, 1.0f / 60.0f);
	GetItem().GetUndoStack()->push(pCmd);
}

void SpriteWidget::on_sbFrameRate_valueChanged(double dValue)
{
	QUndoCommand *pCmd = new SpriteUndoCmd_DurationFrame(ui->framesView, -1, dValue);
	GetItem().GetUndoStack()->push(pCmd);
}
