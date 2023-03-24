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
#include "EntityDraw.h"
#include "ui_EntityWidget.h"
#include "Project.h"
#include "EntityUndoCmds.h"
#include "GlobalUndoCmds.h"
#include "DlgInputName.h"
#include "MainWindow.h"

EntityWidget::EntityWidget(ProjectItemData &itemRef, QWidget *pParent /*= nullptr*/) :
	IWidget(itemRef, pParent),
	ui(new Ui::EntityWidget),
	m_AddShapeActionGroup(this)
{
	ui->setupUi(this);

	ShowStates(false);

	// Remove and re-add the main layout that holds everything. This makes the Qt Designer (.ui) files work with the base class 'IWidget'. Otherwise it jumbles them together.
	layout()->removeItem(ui->verticalLayout);
	layout()->addItem(ui->verticalLayout);

	m_AddShapeActionGroup.addAction(ui->actionAddBoxPrimitive);
	m_AddShapeActionGroup.addAction(ui->actionAddCirclePrimitive);
	m_AddShapeActionGroup.addAction(ui->actionAddPolygonPrimitive);
	m_AddShapeActionGroup.addAction(ui->actionAddSegmentPrimitive);
	m_AddShapeActionGroup.addAction(ui->actionAddLineChainPrimitive);
	m_AddShapeActionGroup.addAction(ui->actionAddLineLoopPrimitive);
	ui->btnAddPrimitiveBox->setDefaultAction(ui->actionAddBoxPrimitive);
	ui->btnAddPrimitiveCircle->setDefaultAction(ui->actionAddCirclePrimitive);
	ui->btnAddPrimitivePolygon->setDefaultAction(ui->actionAddPolygonPrimitive);
	ui->btnAddPrimitiveSegment->setDefaultAction(ui->actionAddSegmentPrimitive);
	ui->btnAddPrimitiveChain->setDefaultAction(ui->actionAddLineChainPrimitive);
	ui->btnAddPrimitiveLoop->setDefaultAction(ui->actionAddLineLoopPrimitive);

	m_AddShapeActionGroup.addAction(ui->actionAddBoxShape);
	m_AddShapeActionGroup.addAction(ui->actionAddCircleShape);
	m_AddShapeActionGroup.addAction(ui->actionAddPolygonShape);
	m_AddShapeActionGroup.addAction(ui->actionAddSegmentShape);
	m_AddShapeActionGroup.addAction(ui->actionAddLineChainShape);
	m_AddShapeActionGroup.addAction(ui->actionAddLineLoopShape);
	ui->btnAddShapeBox->setDefaultAction(ui->actionAddBoxShape);
	ui->btnAddShapeCircle->setDefaultAction(ui->actionAddCircleShape);
	ui->btnAddShapePolygon->setDefaultAction(ui->actionAddPolygonShape);
	ui->btnAddShapeSegment->setDefaultAction(ui->actionAddSegmentShape);
	ui->btnAddShapeChain->setDefaultAction(ui->actionAddLineChainShape);
	ui->btnAddShapeLoop->setDefaultAction(ui->actionAddLineLoopShape);

	m_AddShapeActionGroup.addAction(ui->actionVertexManip);
	ui->btnVertexManip->setDefaultAction(ui->actionVertexManip);

	ui->btnAddChild->setDefaultAction(ui->actionAddChildren);

	ui->btnOrderUp->setDefaultAction(ui->actionOrderChildrenUp);
	ui->btnOrderDown->setDefaultAction(ui->actionOrderChildrenDown);
	ui->btnRemove->setDefaultAction(ui->actionRemoveItems);

	ui->nodeTree->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	ui->nodeTree->setDragEnabled(true);
	ui->nodeTree->setDropIndicatorShown(true);
	ui->nodeTree->setDragDropMode(QAbstractItemView::InternalMove);

	EntityModel *pEntityModel = static_cast<EntityModel *>(m_ItemRef.GetModel());
	ui->nodeTree->setModel(&pEntityModel->GetTreeModel());
	pEntityModel->RegisterWidgets(*ui->cmbEntityType);

	connect(ui->nodeTree->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection &)), this, SLOT(OnTreeSelectionChanged(const QItemSelection &, const QItemSelection &)));
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

	// Query what items are selected in the Explorer Widget
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
	ui->actionAddChildren->setEnabled(bEnableAddNodeBtn);

	// Manage currently selected items in the item tree
	QList<EntityTreeItemData *> selectedItemDataList = GetSelectedItems(true, true);
	if(selectedItemDataList.empty())
	{
		ui->lblSelectedItemIcon->setVisible(false);
		ui->lblSelectedItemText->setVisible(false);

		ui->propertyTree->setModel(nullptr);
	}
	else
	{
		if(selectedItemDataList.size() == 1)
		{
			ui->lblSelectedItemIcon->setVisible(true);
			ui->lblSelectedItemIcon->setPixmap(selectedItemDataList[0]->GetIcon(SUBICON_Settings).pixmap(QSize(16, 16)));
			ui->lblSelectedItemText->setVisible(true);
			ui->lblSelectedItemText->setText(selectedItemDataList[0]->GetCodeName() % " Properties");

			PropertiesTreeModel &propModelRef = selectedItemDataList[0]->GetPropertiesModel();
			ui->propertyTree->setModel(&propModelRef);
		}
		else
		{
			ui->lblSelectedItemIcon->setVisible(false);
			ui->lblSelectedItemText->setVisible(true);
			ui->lblSelectedItemText->setText("Multiple items selected");

			ui->propertyTree->setModel(nullptr);
		}

		// Expand the top level nodes (the properties' categories)
		ui->propertyTree->expandAll();
		ui->propertyTree->resizeColumnToContents(0);
	}

	//if(m_ItemRef.GetDraw())
	//	static_cast<EntityDraw *>(m_ItemRef.GetDraw())->On RefreshSelectedItems();
}

/*virtual*/ void EntityWidget::OnFocusState(int iStateIndex, QVariant subState) /*override*/
{
	EntityTreeModel *pTreeModel = static_cast<EntityTreeModel *>(ui->nodeTree->model());

	// This occurs whenever a property changes within ui->propertyTree
	EntityTreeItemData *pEntityTreeData = reinterpret_cast<EntityTreeItemData *>(subState.toLongLong());
	QModelIndex index = pTreeModel->FindIndex<EntityTreeItemData *>(pEntityTreeData, 0);

	ui->nodeTree->selectionModel()->select(index, QItemSelectionModel::Select);
}

QList<EntityTreeItemData *> EntityWidget::GetSelectedItems(bool bIncludeMainEntity, bool bIncludeShapes)
{
	EntityTreeItemData *pEntityTreeItemData = static_cast<EntityModel *>(m_ItemRef.GetModel())->GetTreeModel().GetEntityTreeItemData();

	QModelIndexList selectedIndices = ui->nodeTree->selectionModel()->selectedIndexes();
	QList<EntityTreeItemData *> selectedItemList;
	for(QModelIndex index : selectedIndices)
	{
		if(index.column() != 0)
			continue;

		EntityTreeItemData *pCurItemData = ui->nodeTree->model()->data(index, Qt::UserRole).value<EntityTreeItemData *>();

		if(pCurItemData == pEntityTreeItemData)
		{
			if(bIncludeMainEntity)
				selectedItemList.push_back(pCurItemData);
		}
		else
		{
			if(pCurItemData->GetType() == ITEM_Shape)
			{
				if(bIncludeShapes)
					selectedItemList.push_back(pCurItemData);
			}
			else
				selectedItemList.push_back(pCurItemData);
		}
	}

	return selectedItemList;
}

void EntityWidget::RequestSelectedItems(QList<QUuid> uuidList)
{
	EntityTreeModel &entityTreeModelRef = static_cast<EntityModel *>(m_ItemRef.GetModel())->GetTreeModel();
	QModelIndexList indexList = entityTreeModelRef.GetAllIndices();
	
	QItemSelection *pItemSelection = new QItemSelection();
	for(QUuid uuid : uuidList)
	{
		for(QModelIndex index : indexList)
		{
			EntityTreeItemData *pCurItemData = ui->nodeTree->model()->data(index, Qt::UserRole).value<EntityTreeItemData *>();
			if(uuid == pCurItemData->GetThisUuid())
				pItemSelection->select(index, index);
		}
	}


	QItemSelectionModel *pSelectionModel = ui->nodeTree->selectionModel();
	pSelectionModel->select(*pItemSelection, QItemSelectionModel::ClearAndSelect);
	delete pItemSelection;
}

void EntityWidget::SetSelectedItems(QList<EntityTreeItemData *> selectedList, QList<EntityTreeItemData *> deselectedList)
{
	EntityTreeModel *pTreeModel = static_cast<EntityTreeModel *>(ui->nodeTree->model());

	QItemSelection *pItemSelection = new QItemSelection();
	for(EntityTreeItemData *pSelectItem : selectedList)
	{
		QModelIndex index = pTreeModel->FindIndex<EntityTreeItemData *>(pSelectItem, 0);
		pItemSelection->select(index, index);
	}

	QItemSelection *pItemDeselection = new QItemSelection();
	for(EntityTreeItemData *pDeselectItem : deselectedList)
	{
		QModelIndex index = pTreeModel->FindIndex<EntityTreeItemData *>(pDeselectItem, 0);
		pItemDeselection->select(index, index);
	}

	QItemSelectionModel *pSelectionModel = ui->nodeTree->selectionModel();
	pSelectionModel->blockSignals(true);
	pSelectionModel->select(*pItemSelection, QItemSelectionModel::Select);
	pSelectionModel->select(*pItemDeselection, QItemSelectionModel::Deselect);
	pSelectionModel->blockSignals(false);
	delete pItemSelection;

	ui->actionVertexManip->setEnabled(selectedList.size() == 1 && (selectedList[0]->GetType() == ITEM_Primitive || selectedList[0]->GetType() == ITEM_Shape));

	ui->nodeTree->repaint();
}

void EntityWidget::DoNewShape(QToolButton *pBtn, QString sStatusMsg, EditorShape eShapeType, bool bAsPrimitive)
{
	pBtn->setChecked(true);

	MainWindow::SetStatus(sStatusMsg, 0);
	EntityDraw *pEntDraw = static_cast<EntityDraw *>(m_ItemRef.GetDraw());
	if(pEntDraw)
		pEntDraw->SetShapeEditDrag(eShapeType, bAsPrimitive);
}

void EntityWidget::OnNewShapeFinished()
{
	for(QAction *pAction : m_AddShapeActionGroup.actions())
		pAction->setChecked(false);

	MainWindow::ClearStatus();
}

void EntityWidget::SetVertexEditMode(bool bEnabled)
{
	ui->btnVertexManip->setChecked(bEnabled);
}

/*virtual*/ void EntityWidget::showEvent(QShowEvent *pEvent) /*override*/
{
	resizeEvent(nullptr);
}

/*virtual*/ void EntityWidget::resizeEvent(QResizeEvent *pEvent) /*override*/
{
	QWidget::resizeEvent(pEvent);

	if(ui->nodeTree == nullptr)
		return;

	// TODO: Use formula to account for device pixels and scaling using QWindow::devicePixelRatio()
	const int iInfoColumnWidth = 130;

	int iTotalWidth = ui->nodeTree->size().width();
	ui->nodeTree->setColumnWidth(0, iTotalWidth - iInfoColumnWidth);
}

//void EntityWidget::ClearAddShape()
//{
//	ui->btnAddPrimitiveBox->setChecked(Qt::Unchecked);
//	ui->btnAddPrimitiveCircle->setChecked(Qt::Unchecked);
//	ui->btnAddPrimitivePolygon->setChecked(Qt::Unchecked);
//	ui->btnAddPrimitiveSegment->setChecked(Qt::Unchecked);
//	ui->btnAddPrimitiveChain->setChecked(Qt::Unchecked);
//	ui->btnAddPrimitiveLoop->setChecked(Qt::Unchecked);
//	ui->btnAddShapeBox->setChecked(Qt::Unchecked);
//	ui->btnAddShapeCircle->setChecked(Qt::Unchecked);
//	ui->btnAddShapePolygon->setChecked(Qt::Unchecked);
//	ui->btnAddShapeSegment->setChecked(Qt::Unchecked);
//	ui->btnAddShapeChain->setChecked(Qt::Unchecked);
//	ui->btnAddShapeLoop->setChecked(Qt::Unchecked);
//
//	MainWindow::ClearStatus();
//	EntityDraw *pEntDraw = static_cast<EntityDraw *>(m_ItemRef.GetDraw());
//	//if(pEntDraw)
//	//	pEntDraw->ClearDrawShape();
//}

void EntityWidget::OnTreeSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
	UpdateActions();

	QList<EntityTreeItemData *> selectedItemDataList;
	QModelIndexList selectedIndices = selected.indexes();
	for(QModelIndex index : selectedIndices)
	{
		if(index.column() != 0)
			continue;

		EntityTreeItemData *pCurItemData = ui->nodeTree->model()->data(index, Qt::UserRole).value<EntityTreeItemData *>();
		if(pCurItemData)
			selectedItemDataList.push_back(pCurItemData);
	}

	QList<EntityTreeItemData *> deselectedItemDataList;
	QModelIndexList deselectedIndices = deselected.indexes();
	for(QModelIndex index : deselectedIndices)
	{
		if(index.column() != 0)
			continue;

		EntityTreeItemData *pCurItemData = ui->nodeTree->model()->data(index, Qt::UserRole).value<EntityTreeItemData *>();
		if(pCurItemData)
			deselectedItemDataList.push_back(pCurItemData);
	}

	m_ItemRef.GetUndoStack()->push(new EntityUndoCmd_SelectionChanged(m_ItemRef, selectedItemDataList, deselectedItemDataList));
}

void EntityWidget::on_actionAddChildren_triggered()
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

void EntityWidget::on_actionAddBoxPrimitive_triggered()
{
	DoNewShape(ui->btnAddPrimitiveBox, "Drawing new primitive box...", SHAPE_Box, true);
}

void EntityWidget::on_actionAddCirclePrimitive_triggered()
{
	DoNewShape(ui->btnAddPrimitiveCircle, "Drawing new primitive circle...", SHAPE_Circle, true);
}

void EntityWidget::on_actionAddPolygonPrimitive_triggered()
{
	DoNewShape(ui->btnAddPrimitivePolygon, "Drawing new primitive polygon...", SHAPE_Polygon, true);
}

void EntityWidget::on_actionAddSegmentPrimitive_triggered()
{
	DoNewShape(ui->btnAddPrimitiveSegment, "Drawing new primitive line segment...", SHAPE_LineSegment, true);
}

void EntityWidget::on_actionAddLineChainPrimitive_triggered()
{
	DoNewShape(ui->btnAddPrimitiveChain, "Drawing new primitive line chain...", SHAPE_LineChain, true);
}

void EntityWidget::on_actionAddLineLoopPrimitive_triggered()
{
	DoNewShape(ui->btnAddPrimitiveLoop, "Drawing new primitive line loop...", SHAPE_LineLoop, true);
}

void EntityWidget::on_actionAddBoxShape_triggered()
{
	DoNewShape(ui->btnAddShapeBox, "Drawing new box...", SHAPE_Box, false);
}

void EntityWidget::on_actionAddCircleShape_triggered()
{
	DoNewShape(ui->btnAddShapeCircle, "Drawing new circle...", SHAPE_Circle, false);
}

void EntityWidget::on_actionAddPolygonShape_triggered()
{
	DoNewShape(ui->btnAddShapePolygon, "Drawing new polygon...", SHAPE_Polygon, false);
}

void EntityWidget::on_actionAddSegmentShape_triggered()
{
	DoNewShape(ui->btnAddShapeSegment, "Drawing new line segment...", SHAPE_LineSegment, false);
}

void EntityWidget::on_actionAddLineChainShape_triggered()
{
	DoNewShape(ui->btnAddShapeChain, "Drawing new line chain...", SHAPE_LineChain, false);
}

void EntityWidget::on_actionAddLineLoopShape_triggered()
{
	DoNewShape(ui->btnAddShapeLoop, "Drawing new line loop...", SHAPE_LineLoop, false);
}

void EntityWidget::on_actionVertexManip_triggered()
{
	EntityModel *pEntityModel = static_cast<EntityModel *>(m_ItemRef.GetModel());

	QUndoCommand *pCmd = new EntityUndoCmd_ToggleVertexManip(m_ItemRef, !pEntityModel->IsVertexExitMode());
	m_ItemRef.GetUndoStack()->push(pCmd);
}

void EntityWidget::on_actionOrderChildrenUp_triggered()
{
	QModelIndexList selectedIndexList = ui->nodeTree->selectionModel()->selectedIndexes();
	std::sort(selectedIndexList.begin(), selectedIndexList.end(), [](const QModelIndex &a, const QModelIndex &b)
		{
			return a.row() < b.row();
		});

	QList<EntityTreeItemData *> selectedItemDataList;
	QList<int> curIndexList;
	QList<int> newIndexList;
	int iDiscardTopIndices = -1;
	for(QModelIndex index : selectedIndexList)
	{
		// Only take selected items that are the first column, and they're children under the root entity tree item
		if(index.column() != 0 || index.parent().row() != 0)
			continue;

		if(iDiscardTopIndices == -1 && index.row() == 0)
			iDiscardTopIndices = 0;
		else if((iDiscardTopIndices + 1) == index.row())
			iDiscardTopIndices = index.row();

		EntityTreeItemData *pEntTreeItemData = ui->nodeTree->model()->data(index, Qt::UserRole).value<EntityTreeItemData *>();
		selectedItemDataList.push_back(pEntTreeItemData);

		int iRow = index.row();
		curIndexList.push_back(iRow);
		newIndexList.push_back(iRow - 1);
	}

	while(iDiscardTopIndices >= 0)
	{
		selectedItemDataList.takeFirst();
		curIndexList.takeFirst();
		newIndexList.takeFirst();

		iDiscardTopIndices--;
	}

	QUndoCommand *pCmd = new EntityUndoCmd_OrderChildren(m_ItemRef, selectedItemDataList, curIndexList, newIndexList, true);
	m_ItemRef.GetUndoStack()->push(pCmd);
}

void EntityWidget::on_actionOrderChildrenDown_triggered()
{
	QModelIndexList selectedIndexList = ui->nodeTree->selectionModel()->selectedIndexes();
	std::sort(selectedIndexList.begin(), selectedIndexList.end(), [](const QModelIndex &a, const QModelIndex &b)
		{
			return a.row() > b.row();
		});

	int iNumChildren = static_cast<EntityModel *>(m_ItemRef.GetModel())->GetTreeModel().GetEntityTreeItem()->GetNumChildren();

	QList<EntityTreeItemData *> selectedItemDataList;
	QList<int> curIndexList;
	QList<int> newIndexList;
	int iDiscardBotIndices = -1;
	for(QModelIndex index : selectedIndexList)
	{
		// Only take selected items that are the first column, and they're children under the root entity tree item
		if(index.column() != 0 || index.parent().row() != 0)
			continue;

		if(iDiscardBotIndices == -1 && index.row() == (iNumChildren - 1))
			iDiscardBotIndices = (iNumChildren - 1);
		else if((iDiscardBotIndices - 1) == index.row())
			iDiscardBotIndices = index.row();

		EntityTreeItemData *pEntTreeItemData = ui->nodeTree->model()->data(index, Qt::UserRole).value<EntityTreeItemData *>();
		selectedItemDataList.push_back(pEntTreeItemData);

		int iRow = index.row();
		curIndexList.push_back(iRow);
		newIndexList.push_back(iRow + 1);
	}

	while(iDiscardBotIndices >= 0 && iDiscardBotIndices < iNumChildren)
	{
		selectedItemDataList.takeFirst();
		curIndexList.takeFirst();
		newIndexList.takeFirst();

		iDiscardBotIndices++;
	}

	QUndoCommand *pCmd = new EntityUndoCmd_OrderChildren(m_ItemRef, selectedItemDataList, curIndexList, newIndexList, false);
	m_ItemRef.GetUndoStack()->push(pCmd);
}

void EntityWidget::on_actionRemoveItems_triggered()
{
	QList<EntityTreeItemData *> poppedItemList = GetSelectedItems(false, true);

	QUndoCommand *pCmd = new EntityUndoCmd_PopItems(m_ItemRef, poppedItemList);
	m_ItemRef.GetUndoStack()->push(pCmd);
}
