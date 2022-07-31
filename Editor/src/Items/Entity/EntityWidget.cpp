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
#include "MainWindow.h"

EntityWidget::EntityWidget(ProjectItemData &itemRef, QWidget *pParent /*= nullptr*/) :
	IWidget(itemRef, pParent),
	ui(new Ui::EntityWidget)
{
	ui->setupUi(this);

	ShowStates(false);

	// Remove and re-add the main layout that holds everything. This makes the Qt Designer (.ui) files work with the base class 'IWidget'. Otherwise it jumbles them together.
	layout()->removeItem(ui->verticalLayout);
	layout()->addItem(ui->verticalLayout);

	ui->btnAddChild->setDefaultAction(ui->actionAppendChildren);
	ui->btnAddChildPrimitive->setDefaultAction(ui->actionAddPrimitive);
	ui->btnInsertBoundingVolume->setDefaultAction(ui->actionInsertBoundingVolume);
	ui->btnInsertPhysics->setDefaultAction(ui->actionInsertPhysicsBody);

	ui->nodeTree->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	ui->nodeTree->setDragEnabled(true);
	ui->nodeTree->setDropIndicatorShown(true);
	ui->nodeTree->setDragDropMode(QAbstractItemView::InternalMove);

	EntityModel *pEntityModel = static_cast<EntityModel *>(m_ItemRef.GetModel());
	ui->nodeTree->setModel(&pEntityModel->GetNodeTreeModel());
	pEntityModel->RegisterWidgets(*ui->txtClassName, *ui->cmbEntityType);
}

EntityWidget::~EntityWidget()
{
	delete ui;
}

/*virtual*/ void EntityWidget::OnGiveMenuActions(QMenu *pMenu) /*override*/
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

/*virtual*/ void EntityWidget::OnUpdateActions() /*override*/
{
	ui->nodeTree->expandAll();

	QList<ProjectItemData *> selectedItems; QList<ExplorerItemData *> selectedPrefixes;
	MainWindow::GetExplorerWidget().GetSelected(selectedItems, selectedPrefixes);
	bool bEnableAddNodeBtn = false;
	EntityTreeModel *pTreeModel = static_cast<EntityTreeModel *>(ui->nodeTree->model());
	for(auto pItem : selectedItems)
	{
		if(pTreeModel->IsItemValid(pItem, false))
		{
			bEnableAddNodeBtn = true;
			break;
		}
	}
	ui->actionAppendChildren->setEnabled(bEnableAddNodeBtn);

	bool bFrameIsSelected = true;
	ui->actionAddPrimitive->setEnabled(bFrameIsSelected);
	ui->actionAddScissorBox->setEnabled(bFrameIsSelected);
	ui->actionInsertBoundingVolume->setEnabled(bFrameIsSelected);
	ui->actionInsertPhysicsBody->setEnabled(bFrameIsSelected);

	EntityNodeItem *pSubStateItem = static_cast<EntityModel *>(m_ItemRef.GetModel())->GetNodeTreeModel().data(ui->nodeTree->currentIndex(), Qt::UserRole).value<EntityNodeItem *>();
	if(pSubStateItem == nullptr)
	{
		ui->lblSelectedItemIcon->setVisible(false);
		ui->lblSelectedItemText->setVisible(false);

		ui->propertyTree->setModel(nullptr);
	}
	else
	{
		ui->lblSelectedItemIcon->setVisible(true);
		ui->lblSelectedItemIcon->setPixmap(pSubStateItem->GetIcon(SUBICON_Settings).pixmap(QSize(16, 16)));
		ui->lblSelectedItemText->setVisible(true);
		ui->lblSelectedItemText->setText(pSubStateItem->GetCodeName() % " Properties");

		PropertiesTreeModel &propModelRef = pSubStateItem->GetPropertiesModel();
		ui->propertyTree->setModel(&propModelRef);

		// Expand the top level nodes (the properties' categories)
		QModelIndex rootIndex = ui->propertyTree->rootIndex();
		ui->propertyTree->expand(rootIndex);
		for(int i = 0; i < propModelRef.rowCount(); ++i)
			ui->propertyTree->expand(propModelRef.index(i, 0, rootIndex));

		ui->propertyTree->resizeColumnToContents(0);
	}
}

/*virtual*/ void EntityWidget::OnFocusState(int iStateIndex, QVariant subState) /*override*/
{
	
}

ExplorerItemData *EntityWidget::GetSelectedNode()
{
	QModelIndexList selectedIndices = ui->nodeTree->selectionModel()->selectedIndexes();
	if(selectedIndices.empty())
		return nullptr;

	return ui->nodeTree->model()->data(selectedIndices[0], Qt::UserRole).value<ExplorerItemData *>();
}

void EntityWidget::on_actionAppendChildren_triggered()
{
	QList<ProjectItemData *> selectedItems; QList<ExplorerItemData *> selectedPrefixes;
	MainWindow::GetExplorerWidget().GetSelected(selectedItems, selectedPrefixes);
	if(selectedItems.empty())
	{
		HyGuiLog("Currently selected item(s) in Explorer is/are not a ProjectItemData. Cannot add node(s) to entity.", LOGTYPE_Error);
		return;
	}

	QList<ProjectItemData *> validItemList;
	for(auto pItem : selectedItems)
	{
		EntityTreeModel *pTreeModel = static_cast<EntityTreeModel *>(ui->nodeTree->model());
		if(pTreeModel->IsItemValid(pItem, false) == false)
			continue;

		validItemList.push_back(pItem);
	}
	QUndoCommand *pCmd = new EntityUndoCmd_AddChildren(m_ItemRef, validItemList);
	m_ItemRef.GetUndoStack()->push(pCmd);
}

void EntityWidget::on_actionAddPrimitive_triggered()
{
	//QUndoCommand *pCmd = new EntityUndoCmd(ENTITYCMD_AddPrimitive, m_ItemRef, QVariantList());
	//m_ItemRef.GetUndoStack()->push(pCmd);
}

void EntityWidget::on_actionInsertBoundingVolume_triggered()
{

}

void EntityWidget::on_actionInsertPhysicsBody_triggered()
{

}

void EntityWidget::on_nodeTree_clicked(QModelIndex index)
{
	UpdateActions();
}
