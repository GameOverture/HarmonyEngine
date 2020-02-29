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

EntityWidget::EntityWidget(ProjectItem &itemRef, QWidget *pParent /*= nullptr*/) :
	IWidget(itemRef, pParent),
	ui(new Ui::EntityWidget)
{
	ui->setupUi(this);

	// Remove and re-add the main layout that holds everything. This makes the Qt Designer (.ui) files work with the base class 'IWidget'. Otherwise it jumbles them together.
	layout()->removeItem(ui->verticalLayout);
	layout()->addItem(ui->verticalLayout);

	ui->btnAddChild->setDefaultAction(ui->actionAddSelectedChild);
	ui->btnAddChildPrimitive->setDefaultAction(ui->actionAddPrimitive);
	ui->btnInsertBoundingVolume->setDefaultAction(ui->actionInsertBoundingVolume);
	ui->btnInsertPhysics->setDefaultAction(ui->actionInsertPhysicsBody);

	ui->nodeTree->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	ui->nodeTree->setDragEnabled(true);
	ui->nodeTree->setDropIndicatorShown(true);
	ui->nodeTree->setDragDropMode(QAbstractItemView::InternalMove);

	EntityModel *pEntityModel = static_cast<EntityModel *>(m_ItemRef.GetModel());
	ui->nodeTree->setModel(&pEntityModel->GetChildrenModel());

	on_childrenTree_clicked(QModelIndex());
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
	QList<ExplorerItem *> selectedItems, selectedPrefixes;
	MainWindow::GetExplorer()->GetSelectedItems(selectedItems, selectedPrefixes);
	bool bEnableAddNodeBtn = false;
	EntityNodeTreeModel *pTreeModel = static_cast<EntityNodeTreeModel *>(ui->nodeTree->model());
	for(auto pItem : selectedItems)
	{
		if(pTreeModel->IsItemValid(pItem, true))
		{
			bEnableAddNodeBtn = true;
			break;
		}
	}
	ui->actionAddSelectedChild->setEnabled(bEnableAddNodeBtn);

	bool bFrameIsSelected = true;
	ui->actionAddPrimitive->setEnabled(bFrameIsSelected);
	ui->actionAddScissorBox->setEnabled(bFrameIsSelected);
	ui->actionInsertBoundingVolume->setEnabled(bFrameIsSelected);
	ui->actionInsertPhysicsBody->setEnabled(bFrameIsSelected);
}

/*virtual*/ void EntityWidget::OnFocusState(int iStateIndex, QVariant subState) /*override*/
{
	// Get EntityStateData from 'iStateIndex', and select the correct EntityTreeItem using 'iSubStateIndex' as the key
	ExplorerItem *pSubStateItem = subState.value<ExplorerItem *>();
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
		ui->lblSelectedItemText->setText(pSubStateItem->GetName(false) % " Properties");

		PropertiesTreeModel *pPropertiesModel = static_cast<EntityModel *>(m_ItemRef.GetModel())->GetPropertiesModel(GetCurStateIndex(), pSubStateItem);
		ui->propertyTree->setModel(pPropertiesModel);

		// Expand the top level nodes (the properties' categories)
		QModelIndex rootIndex = ui->propertyTree->rootIndex();
		ui->propertyTree->expand(rootIndex);
		for(int i = 0; i < pPropertiesModel->rowCount(); ++i)
			ui->propertyTree->expand(pPropertiesModel->index(i, 0, rootIndex));
	}
}

ExplorerItem *EntityWidget::GetSelectedNode()
{
	QModelIndexList selectedIndices = ui->nodeTree->selectionModel()->selectedIndexes();
	if(selectedIndices.empty())
		return nullptr;

	return ui->nodeTree->model()->data(selectedIndices[0], Qt::UserRole).value<ExplorerItem *>();
}

void EntityWidget::on_actionAddSelectedChild_triggered()
{
	QList<ExplorerItem *> selectedItems, selectedPrefixes;
	MainWindow::GetExplorer()->GetSelectedItems(selectedItems, selectedPrefixes);
	if(selectedItems.empty())
	{
		HyGuiLog("Currently selected item(s) in Explorer is/are not a ProjectItem. Cannot add node(s) to entity.", LOGTYPE_Error);
		return;
	}

	QList<QVariant> validItemList;
	for(auto pItem : selectedItems)
	{
		EntityNodeTreeModel *pTreeModel = static_cast<EntityNodeTreeModel *>(ui->nodeTree->model());
		if(pTreeModel->IsItemValid(pItem, true) == false)
			continue;
	
		QVariant v;
		v.setValue<ExplorerItem *>(pItem);
		validItemList.push_back(v);
	}

	QUndoCommand *pCmd = new EntityUndoCmd(ENTITYCMD_AddNewChildren, m_ItemRef, validItemList);
	m_ItemRef.GetUndoStack()->push(pCmd);
}

void EntityWidget::on_actionAddPrimitive_triggered()
{
	QUndoCommand *pCmd = new EntityUndoCmd(ENTITYCMD_AddPrimitive, m_ItemRef, QVariantList());
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
	//EntityTreeItem *pTreeItem = static_cast<EntityTreeItem *>(index.internalPointer()); <--- deprecated
	//FocusState(ui->cmbStates->currentIndex(), QVariant(reinterpret_cast<qulonglong>(pTreeItem)));

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
