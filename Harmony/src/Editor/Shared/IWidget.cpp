/**************************************************************************
*	IWidget.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2019 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Global.h"
#include "IWidget.h"
#include "ui_IWidget.h"

#include "ProjectItem.h"
#include "IModel.h"
#include "GlobalUndoCmds.h"
#include "DlgInputName.h"
#include "SpriteModels.h"
#include "FontModels.h"
#include "EntityModel.h"
#include "PrefabModel.h"

#include <QUndoCommand>

IWidget::IWidget(ProjectItem &itemRef, QWidget *pParent /*= nullptr*/) :
	QWidget(pParent),
	ui(new Ui::IWidget),
	m_ItemRef(itemRef)
{
	ui->setupUi(this);

	ui->cmbStates->blockSignals(true);
	ui->cmbStates->clear();
	ui->cmbStates->setModel(m_ItemRef.GetModel());
	ui->cmbStates->blockSignals(false);

	ui->btnAddState->setDefaultAction(ui->actionAddState);
	ui->btnRemoveState->setDefaultAction(ui->actionRemoveState);
	ui->btnRenameState->setDefaultAction(ui->actionRenameState);
	ui->btnOrderStateBack->setDefaultAction(ui->actionOrderStateBackwards);
	ui->btnOrderStateForward->setDefaultAction(ui->actionOrderStateForwards);
}

IWidget::~IWidget()
{
    delete ui;
}

ProjectItem &IWidget::GetItem()
{
	return m_ItemRef;
}

void IWidget::UpdateActions()
{
	ui->actionRemoveState->setEnabled(ui->cmbStates->count() > 1);
	ui->actionOrderStateBackwards->setEnabled(ui->cmbStates->currentIndex() != 0);
	ui->actionOrderStateForwards->setEnabled(ui->cmbStates->currentIndex() != (ui->cmbStates->count() - 1));

	OnUpdateActions();
}

void IWidget::FocusState(int iStateIndex, QVariant subState)
{
	if(iStateIndex >= 0)
	{
		ui->cmbStates->blockSignals(true);
		ui->cmbStates->setCurrentIndex(iStateIndex);
		ui->cmbStates->blockSignals(false);

		OnFocusState(iStateIndex, subState);
	}

	UpdateActions();
}

int IWidget::GetCurStateIndex()
{
	return ui->cmbStates->currentIndex();
}

IStateData *IWidget::GetCurStateData()
{
	return m_ItemRef.GetModel()->GetStateData(ui->cmbStates->currentIndex());
}

void IWidget::on_cmbStates_currentIndexChanged(int index)
{
	FocusState(index, -1);
}

void IWidget::on_actionAddState_triggered()
{
	QUndoCommand *pCmd = nullptr;

	switch(m_ItemRef.GetType())
	{
	case ITEM_Sprite:
		pCmd = new UndoCmd_AddState<SpriteStateData>("Add Sprite State", m_ItemRef);
		break;
	case ITEM_Font:
		pCmd = new UndoCmd_AddState<FontStateData>("Add Font State", m_ItemRef);
		break;
	case ITEM_Entity:
		pCmd = new UndoCmd_AddState<EntityStateData>("Add Entity State", m_ItemRef);
		break;
	case ITEM_Prefab:
		pCmd = new UndoCmd_AddState<PrefabStateData>("Add Prefab State", m_ItemRef);
		break;
	default:
		HyGuiLog("Unimplemented item on_actionAddState_triggered(): " % QString::number(m_ItemRef.GetType()), LOGTYPE_Error);
		break;
	}

	if(pCmd)
		m_ItemRef.GetUndoStack()->push(pCmd);
}

void IWidget::on_actionRemoveState_triggered()
{
	QUndoCommand *pCmd = nullptr;

	switch(m_ItemRef.GetType())
	{
	case ITEM_Sprite:
		pCmd = new UndoCmd_RemoveState<SpriteStateData>("Remove Sprite State", m_ItemRef, ui->cmbStates->currentIndex());
		break;
	case ITEM_Font:
		pCmd = new UndoCmd_RemoveState<FontStateData>("Remove Font State", m_ItemRef, ui->cmbStates->currentIndex());
		break;
	case ITEM_Entity:
		pCmd = new UndoCmd_RemoveState<EntityStateData>("Remove Entity State", m_ItemRef, ui->cmbStates->currentIndex());
		break;
	case ITEM_Prefab:
		pCmd = new UndoCmd_RemoveState<PrefabStateData>("Remove Prefab State", m_ItemRef, ui->cmbStates->currentIndex());
		break;
	default:
		HyGuiLog("Unimplemented item on_actionRemoveState_triggered(): " % QString::number(m_ItemRef.GetType()), LOGTYPE_Error);
		break;
	}

	if(pCmd)
		m_ItemRef.GetUndoStack()->push(pCmd);
}

void IWidget::on_actionRenameState_triggered()
{
	DlgInputName *pDlg = new DlgInputName("Rename Font State", GetCurStateData()->GetName());
	if(pDlg->exec() == QDialog::Accepted)
	{
		QUndoCommand *pCmd = new UndoCmd_RenameState("Rename Font State", m_ItemRef, pDlg->GetName(), ui->cmbStates->currentIndex());
		m_ItemRef.GetUndoStack()->push(pCmd);
	}
	delete pDlg;
}

void IWidget::on_actionOrderStateBackwards_triggered()
{
	QUndoCommand *pCmd = new UndoCmd_MoveStateBack("Shift Font State Index <-", m_ItemRef, ui->cmbStates->currentIndex());
	m_ItemRef.GetUndoStack()->push(pCmd);
}

void IWidget::on_actionOrderStateForwards_triggered()
{
	QUndoCommand *pCmd = new UndoCmd_MoveStateForward("Shift Font State Index ->", m_ItemRef, ui->cmbStates->currentIndex());
	m_ItemRef.GetUndoStack()->push(pCmd);
}
