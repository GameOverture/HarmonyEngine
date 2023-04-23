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

#include "ProjectItemData.h"
#include "IModel.h"
#include "GlobalUndoCmds.h"
#include "DlgInputName.h"
#include "SpriteModels.h"
#include "TextModel.h"
#include "EntityModel.h"
#include "PrefabModel.h"
#include "AudioModel.h"

#include <QUndoCommand>

IWidget::IWidget(ProjectItemData &itemRef, QWidget *pParent /*= nullptr*/) :
	QWidget(pParent),
	uiWidget(new Ui::IWidget),
	m_ItemRef(itemRef)
{
	uiWidget->setupUi(this);

	uiWidget->cmbStates->blockSignals(true);
	uiWidget->cmbStates->clear();
	uiWidget->cmbStates->setModel(m_ItemRef.GetModel());
	uiWidget->cmbStates->blockSignals(false);

	uiWidget->btnAddState->setDefaultAction(uiWidget->actionAddState);
	uiWidget->btnRemoveState->setDefaultAction(uiWidget->actionRemoveState);
	uiWidget->btnRenameState->setDefaultAction(uiWidget->actionRenameState);
	uiWidget->btnOrderStateBack->setDefaultAction(uiWidget->actionOrderStateBackwards);
	uiWidget->btnOrderStateForward->setDefaultAction(uiWidget->actionOrderStateForwards);

	connect(uiWidget->cmbStates, SIGNAL(currentIndexChanged(int)), this, SLOT(OnCurrentIndexChanged(int)));
	connect(uiWidget->actionAddState, &QAction::triggered, this, &IWidget::OnAddStateTriggered);
	connect(uiWidget->actionRemoveState, &QAction::triggered, this, &IWidget::OnRemoveStateTriggered);
	connect(uiWidget->actionRenameState, &QAction::triggered, this, &IWidget::OnRenameStateTriggered);
	connect(uiWidget->actionOrderStateBackwards, &QAction::triggered, this, &IWidget::OnOrderStateBackwardsTriggered);
	connect(uiWidget->actionOrderStateForwards, &QAction::triggered, this, &IWidget::OnOrderStateForwardsTriggered);
}

IWidget::~IWidget()
{
    delete uiWidget;
}

ProjectItemData &IWidget::GetItem()
{
	return m_ItemRef;
}

QVBoxLayout *IWidget::GetAboveStatesLayout()
{
	return uiWidget->lytAboveStates;
}

QVBoxLayout *IWidget::GetBelowStatesLayout()
{
	return uiWidget->lytBelowStates;
}

int IWidget::GetCurStateIndex()
{
	return uiWidget->cmbStates->currentIndex();
}

IStateData *IWidget::GetCurStateData()
{
	return m_ItemRef.GetModel()->GetStateData(uiWidget->cmbStates->currentIndex());
}

void IWidget::UpdateActions()
{
	uiWidget->actionRemoveState->setEnabled(uiWidget->cmbStates->count() > 1);
	uiWidget->actionOrderStateBackwards->setEnabled(uiWidget->cmbStates->currentIndex() != 0);
	uiWidget->actionOrderStateForwards->setEnabled(uiWidget->cmbStates->currentIndex() != (uiWidget->cmbStates->count() - 1));

	OnUpdateActions();
}

void IWidget::FocusState(int iStateIndex, QVariant subState)
{
	if(iStateIndex >= 0)
	{
		uiWidget->cmbStates->blockSignals(true);
		uiWidget->cmbStates->setCurrentIndex(iStateIndex);
		uiWidget->cmbStates->blockSignals(false);

		OnFocusState(iStateIndex, subState);
	}

	UpdateActions();
}

void IWidget::ShowStates(bool bShow)
{
	uiWidget->grpStates->setVisible(bShow);
}

void IWidget::OnCurrentIndexChanged(int index)
{
	FocusState(index, -1);
}

void IWidget::OnAddStateTriggered()
{
	QUndoCommand *pCmd = nullptr;

	switch(m_ItemRef.GetType())
	{
	case ITEM_Sprite:
		pCmd = new UndoCmd_AddState<SpriteStateData>("Add Sprite State", m_ItemRef);
		break;
	case ITEM_Text:
		pCmd = new UndoCmd_AddState<TextStateData>("Add Text State", m_ItemRef);
		break;
	case ITEM_Entity:
		// TODO: Ask to copy from other state or create OnDuplicateStateTriggered())
		pCmd = new UndoCmd_AddState<EntityStateData>("Add Entity State", m_ItemRef);
		break;
	case ITEM_Prefab:
		pCmd = new UndoCmd_AddState<PrefabStateData>("Add Prefab State", m_ItemRef);
		break;
	case ITEM_Audio:
		pCmd = new UndoCmd_AddState<AudioStateData>("Add Audio State", m_ItemRef);
		break;
	default:
		HyGuiLog("Unimplemented item on_actionAddState_triggered(): " % QString::number(m_ItemRef.GetType()), LOGTYPE_Error);
		break;
	}

	if(pCmd)
		m_ItemRef.GetUndoStack()->push(pCmd);
}

void IWidget::OnRemoveStateTriggered()
{
	QUndoCommand *pCmd = nullptr;

	switch(m_ItemRef.GetType())
	{
	case ITEM_Sprite:
		pCmd = new UndoCmd_RemoveState<SpriteStateData>("Remove Sprite State", m_ItemRef, uiWidget->cmbStates->currentIndex());
		break;
	case ITEM_Text:
		pCmd = new UndoCmd_RemoveState<TextStateData>("Remove Text State", m_ItemRef, uiWidget->cmbStates->currentIndex());
		break;
	case ITEM_Entity:
		pCmd = new UndoCmd_RemoveState<EntityStateData>("Remove Entity State", m_ItemRef, uiWidget->cmbStates->currentIndex());
		break;
	case ITEM_Prefab:
		pCmd = new UndoCmd_RemoveState<PrefabStateData>("Remove Prefab State", m_ItemRef, uiWidget->cmbStates->currentIndex());
		break;
	case ITEM_Audio:
		pCmd = new UndoCmd_RemoveState<AudioStateData>("Remove Audio State", m_ItemRef, uiWidget->cmbStates->currentIndex());
		break;
	default:
		HyGuiLog("Unimplemented item on_actionRemoveState_triggered(): " % QString::number(m_ItemRef.GetType()), LOGTYPE_Error);
		break;
	}

	if(pCmd)
		m_ItemRef.GetUndoStack()->push(pCmd);
}

void IWidget::OnRenameStateTriggered()
{
	DlgInputName *pDlg = new DlgInputName("Rename State", GetCurStateData()->GetName(), HyGlobal::FreeFormValidator(), nullptr, nullptr);
	if(pDlg->exec() == QDialog::Accepted)
	{
		QUndoCommand *pCmd = new UndoCmd_RenameState("Rename State", m_ItemRef, pDlg->GetName(), uiWidget->cmbStates->currentIndex());
		m_ItemRef.GetUndoStack()->push(pCmd);
	}
	delete pDlg;
}

void IWidget::OnOrderStateBackwardsTriggered()
{
	QUndoCommand *pCmd = new UndoCmd_MoveStateBack("Shift State Index <-", m_ItemRef, uiWidget->cmbStates->currentIndex());
	m_ItemRef.GetUndoStack()->push(pCmd);
}

void IWidget::OnOrderStateForwardsTriggered()
{
	QUndoCommand *pCmd = new UndoCmd_MoveStateForward("Shift State Index ->", m_ItemRef, uiWidget->cmbStates->currentIndex());
	m_ItemRef.GetUndoStack()->push(pCmd);
}
