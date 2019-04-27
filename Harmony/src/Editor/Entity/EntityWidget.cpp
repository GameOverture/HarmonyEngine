/**************************************************************************
*	EntityWidget.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Global.h"
#include "EntityWidget.h"
#include "ui_EntityWidget.h"
#include "Project.h"
#include "EntityUndoCmds.h"
#include "GlobalUndoCmds.h"
#include "DlgInputName.h"

EntityWidget::EntityWidget(ProjectItem &itemRef, QWidget *parent) :
	QWidget(parent),
	ui(new Ui::EntityWidget),
	m_ItemRef(itemRef)
{
	ui->setupUi(this);

	ui->btnAddState->setDefaultAction(ui->actionAddState);
	ui->btnRemoveState->setDefaultAction(ui->actionRemoveState);
	ui->btnRenameState->setDefaultAction(ui->actionRenameState);
	ui->btnOrderStateBack->setDefaultAction(ui->actionOrderStateBackwards);
	ui->btnOrderStateForward->setDefaultAction(ui->actionOrderStateForwards);

	ui->btnAddChild->setDefaultAction(ui->actionAddSelectedChild);
	ui->btnAddChildPrimitive->setDefaultAction(ui->actionAddPrimitive);
	ui->btnInsertBoundingVolume->setDefaultAction(ui->actionInsertBoundingVolume);
	ui->btnInsertPhysics->setDefaultAction(ui->actionInsertPhysicsBody);

	ui->cmbStates->clear();
	ui->cmbStates->setModel(m_ItemRef.GetModel());

	ui->childrenTree->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	ui->childrenTree->setDragEnabled(true);
	ui->childrenTree->setDropIndicatorShown(true);
	ui->childrenTree->setDragDropMode(QAbstractItemView::InternalMove);

	EntityModel *pEntityModel = static_cast<EntityModel *>(m_ItemRef.GetModel());
	pEntityModel->SetWidget(ui->childrenTree);

	on_childrenTree_clicked(QModelIndex());

	FocusState(0, QVariant(static_cast<qulonglong>(0)));
}

EntityWidget::~EntityWidget()
{
	delete ui;
}

ProjectItem &EntityWidget::GetItem()
{
	return m_ItemRef;
}

EntityModel *EntityWidget::GetEntityModel()
{
	return static_cast<EntityModel *>(m_ItemRef.GetModel());
}

EntityTreeItem *EntityWidget::GetCurSelectedTreeItem()
{
	QModelIndex curIndex = ui->childrenTree->currentIndex();
	return static_cast<EntityTreeItem *>(curIndex.internalPointer());

	//return static_cast<EntityTreeItem *>(GetEntityModel()->GetTreeModel().index(curIndex.row(), curIndex.column(), curIndex.parent()).internalPointer());
}

void EntityWidget::OnGiveMenuActions(QMenu *pMenu)
{
//    pMenu->addAction(ui->actionAddState);
//    pMenu->addAction(ui->actionRemoveState);
//    pMenu->addAction(ui->actionRenameState);
//    pMenu->addAction(ui->actionOrderStateBackwards);
//    pMenu->addAction(ui->actionOrderStateForwards);
//    pMenu->addSeparator();
//    pMenu->addAction(ui->actionAddLayer);
//    pMenu->addAction(ui->actionRemoveLayer);
//    pMenu->addAction(ui->actionOrderLayerUpwards);
//    pMenu->addAction(ui->actionOrderLayerDownwards);
}

EntityStateData *EntityWidget::GetCurStateData()
{
	return static_cast<EntityStateData *>(GetEntityModel()->GetStateData(ui->cmbStates->currentIndex()));
}

int EntityWidget::GetNumStates() const
{
	return ui->cmbStates->count();
}

void EntityWidget::FocusState(int iStateIndex, QVariant subState)
{
	if(iStateIndex >= 0)
	{
		ui->cmbStates->blockSignals(true);
		ui->cmbStates->setCurrentIndex(iStateIndex);
		ui->cmbStates->blockSignals(false);

		// Get EntityStateData from 'iStateIndex', and select the correct EntityTreeItem using 'iSubStateIndex' as the key
		EntityStateData *pCurStateData = static_cast<EntityStateData *>(static_cast<EntityModel *>(m_ItemRef.GetModel())->GetStateData(iStateIndex));
		EntityTreeItem *pTreeItem = reinterpret_cast<EntityTreeItem *>(subState.toULongLong());
		if(pTreeItem == nullptr)
		{
			ui->lblSelectedItemIcon->setVisible(false);
			ui->lblSelectedItemText->setVisible(false);

			ui->propertyTree->setModel(nullptr);
		}
		else
		{
			ui->lblSelectedItemIcon->setVisible(true);
			ui->lblSelectedItemIcon->setPixmap(pTreeItem->GetProjItem()->GetIcon(SUBICON_Settings).pixmap(QSize(16, 16)));
			ui->lblSelectedItemText->setVisible(true);
			ui->lblSelectedItemText->setText(pTreeItem->GetProjItem()->GetName(false) % " Properties");

			PropertiesTreeModel *pPropertiesModel = GetEntityModel()->GetPropertiesModel(ui->cmbStates->currentIndex(), pTreeItem);
			ui->propertyTree->setModel(pPropertiesModel);

			// Expand the top level nodes (the properties' categories)
			QModelIndex rootIndex = ui->propertyTree->rootIndex();
			ui->propertyTree->expand(rootIndex);
			for(int i = 0; i < pPropertiesModel->rowCount(); ++i)
				ui->propertyTree->expand(pPropertiesModel->index(i, 0, rootIndex));
		}
	}

	UpdateActions();
}

void EntityWidget::UpdateActions()
{
	ui->actionRemoveState->setEnabled(ui->cmbStates->count() > 1);
	ui->actionOrderStateBackwards->setEnabled(ui->cmbStates->currentIndex() != 0);
	ui->actionOrderStateForwards->setEnabled(ui->cmbStates->currentIndex() != (ui->cmbStates->count() - 1));

	ExplorerItem *pExplorerItem = nullptr;//m_ItemRef.GetProject().GetExplorerWidget()->GetCurItemSelected();
	ui->actionAddSelectedChild->setEnabled(pExplorerItem && pExplorerItem->IsProjectItem());

	EntityTreeItem *pSelectedItem = GetCurSelectedTreeItem();
	ProjectItem *pSelectedProjItem = pSelectedItem ? pSelectedItem->GetProjItem() : nullptr;
	bool bFrameIsSelected = pSelectedProjItem && pSelectedProjItem->GetType() == ITEM_Entity;
	ui->actionAddPrimitive->setEnabled(bFrameIsSelected);

	bFrameIsSelected = pSelectedProjItem;
	ui->actionAddScissorBox->setEnabled(bFrameIsSelected);
	ui->actionInsertBoundingVolume->setEnabled(bFrameIsSelected);
	ui->actionInsertPhysicsBody->setEnabled(bFrameIsSelected);
}

void EntityWidget::on_actionAddSelectedChild_triggered()
{
	if(GetCurSelectedTreeItem() == nullptr)
	{
		HyGuiLog("Currently selected entity tree item is nullptr. Cannot add child.", LOGTYPE_Error);
		return;
	}

	ExplorerItem *pExplorerItem = nullptr;//m_ItemRef.GetProject().GetExplorerWidget()->GetCurItemSelected();
	if(pExplorerItem == nullptr || pExplorerItem->IsProjectItem() == false)
	{
		HyGuiLog("Currently selected item in Explorer is not a ProjectItem. Cannot add child to entity.", LOGTYPE_Error);
		return;
	}

	ProjectItem *pItem = static_cast<ProjectItem *>(pExplorerItem);
	EntityTreeModel *pTreeModel = static_cast<EntityTreeModel *>(ui->childrenTree->model());
	if(pItem == pTreeModel->GetRootItem())
	{
		HyGuiLog("Entities cannot add themselves as a child.", LOGTYPE_Info);
		return;
	}

	QUndoCommand *pCmd = new EntityUndoCmd(ENTITYCMD_AddNewChild, m_ItemRef, pItem);
	m_ItemRef.GetUndoStack()->push(pCmd);
}

void EntityWidget::on_actionAddPrimitive_triggered()
{
	if(GetCurSelectedTreeItem() == nullptr)
	{
		HyGuiLog("Currently selected entity tree item is nullptr. Cannot add primitive.", LOGTYPE_Error);
		return;
	}

	QUndoCommand *pCmd = new EntityUndoCmd(ENTITYCMD_AddPrimitive, m_ItemRef, static_cast<EntityModel *>(m_ItemRef.GetModel())->CreateNewPrimitive());
	m_ItemRef.GetUndoStack()->push(pCmd);
}

void EntityWidget::on_actionInsertBoundingVolume_triggered()
{

}

void EntityWidget::on_actionInsertPhysicsBody_triggered()
{

}

void EntityWidget::on_childrenTree_clicked(const QModelIndex &index)
{
	EntityTreeItem *pTreeItem = static_cast<EntityTreeItem *>(index.internalPointer());
	FocusState(ui->cmbStates->currentIndex(), QVariant(reinterpret_cast<qulonglong>(pTreeItem)));

//    ui->toolBox->setVisible(true);
//    ui->toolBoxLine->setVisible(true);

////    pTreeItem->GetData(0).m_PosX;
////    pTreeItem->GetData(0).m_PosY;
////    pTreeItem->GetData(0).m_ScaleX;
////    pTreeItem->GetData(0).m_ScaleY;
////    pTreeItem->GetData(0).m_Rotation;

////    pTreeItem->GetData(0).m_Enabled;
////    pTreeItem->GetData(0).m_UpdateWhilePaused;
////    pTreeItem->GetData(0).m_Tag;
////    pTreeItem->GetData(0).m_DisplayOrder;
//    //ui->sbTransformX-

//    switch(pTreeItem->GetItem()->GetType())
//    {
//    case ITEM_Sprite:
//        ui->stackedWidget->setCurrentIndex(STACKED_Sprite);
//        break;
//    case ITEM_Entity:
//        ui->stackedWidget->setCurrentIndex(STACKED_Entity);
//        break;
//    case ITEM_Font:
//        ui->stackedWidget->setCurrentIndex(STACKED_Font);
//        break;
//    case ITEM_AtlasImage:
//        ui->stackedWidget->setCurrentIndex(STACKED_TexturedQuad);
//        break;
//    case ITEM_Audio:
//    case ITEM_Particles:
//    case ITEM_Spine:
//    case ITEM_Shader:
//    case ITEM_BoundingVolume:
//        ui->stackedWidget->setCurrentIndex(STACKED_BoundingVolume);
//        break;
//    case ITEM_Physics:
//        ui->stackedWidget->setCurrentIndex(STACKED_Physics);
//        break;
//        HyGuiLog("Unsupported Entity childrenTree clicked", LOGTYPE_Error);
//        break;
//    }


}

void EntityWidget::on_actionRenameState_triggered()
{
	DlgInputName *pDlg = new DlgInputName("Rename Entity State", GetCurStateData()->GetName());
	if(pDlg->exec() == QDialog::Accepted)
	{
		QUndoCommand *pCmd = new UndoCmd_RenameState("Rename Entity State", m_ItemRef, pDlg->GetName(), ui->cmbStates->currentIndex());
		m_ItemRef.GetUndoStack()->push(pCmd);
	}

	delete pDlg;
}

void EntityWidget::on_actionAddState_triggered()
{
	QUndoCommand *pCmd = new UndoCmd_AddState<EntityStateData>("Add Entity State", m_ItemRef, nullptr);
	m_ItemRef.GetUndoStack()->push(pCmd);
}

void EntityWidget::on_actionRemoveState_triggered()
{
	QUndoCommand *pCmd = new UndoCmd_RemoveState<EntityStateData>("Remove Entity State", m_ItemRef, ui->cmbStates->currentIndex());
	m_ItemRef.GetUndoStack()->push(pCmd);
}

void EntityWidget::on_actionOrderStateBackwards_triggered()
{
	QUndoCommand *pCmd = new UndoCmd_MoveStateBack("Shift Entity State Index <-", m_ItemRef, ui->cmbStates->currentIndex());
	m_ItemRef.GetUndoStack()->push(pCmd);
}

void EntityWidget::on_actionOrderStateForwards_triggered()
{
	QUndoCommand *pCmd = new UndoCmd_MoveStateForward("Shift Entity State Index ->", m_ItemRef, ui->cmbStates->currentIndex());
	m_ItemRef.GetUndoStack()->push(pCmd);
}
