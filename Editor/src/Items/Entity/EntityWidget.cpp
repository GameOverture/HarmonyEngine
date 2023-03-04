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
	ui(new Ui::EntityWidget)
{
	ui->setupUi(this);

	m_MenuAddPrimitive.addAction(ui->actionAddBoxPrimitive);
	m_MenuAddPrimitive.addAction(ui->actionAddCirclePrimitive);
	m_MenuAddPrimitive.addAction(ui->actionAddPolygonPrimitive);
	m_MenuAddPrimitive.addAction(ui->actionAddSegmentPrimitive);
	m_MenuAddPrimitive.addAction(ui->actionAddLineChainPrimitive);
	m_MenuAddPrimitive.addAction(ui->actionAddLineLoopPrimitive);
	ui->btnAddChildPrimitive->setMenu(&m_MenuAddPrimitive);
	ui->btnAddChildPrimitive->setDefaultAction(ui->actionAddBoxPrimitive);
	connect(ui->btnAddChildPrimitive, SIGNAL(triggered(QAction *)),
		ui->btnAddChildPrimitive, SLOT(setDefaultAction(QAction *)));

	ui->lblAddingPrim->setVisible(false);

	m_MenuAddShape.addAction(ui->actionAddBoxShape);
	m_MenuAddShape.addAction(ui->actionAddCircleShape);
	m_MenuAddShape.addAction(ui->actionAddPolygonShape);
	m_MenuAddShape.addAction(ui->actionAddSegmentShape);
	m_MenuAddShape.addAction(ui->actionAddLineChainShape);
	m_MenuAddShape.addAction(ui->actionAddLineLoopShape);
	ui->btnAddShape->setMenu(&m_MenuAddShape);
	ui->btnAddShape->setDefaultAction(ui->actionAddBoxShape);
	connect(ui->btnAddShape, SIGNAL(triggered(QAction *)),
		ui->btnAddShape, SLOT(setDefaultAction(QAction *)));

	ui->lblAddingShape->setVisible(false);

	ShowStates(false);

	// Remove and re-add the main layout that holds everything. This makes the Qt Designer (.ui) files work with the base class 'IWidget'. Otherwise it jumbles them together.
	layout()->removeItem(ui->verticalLayout);
	layout()->addItem(ui->verticalLayout);

	ui->btnAddChild->setDefaultAction(ui->actionAddChildren);
	//ui->btnAddChildPrimitive->setDefaultAction(ui->actionAddPrimitive);
	//ui->btnAddShape->setDefaultAction(ui->actionAddShape);

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

void EntityWidget::ResetLabels()
{
	ui->lblAddingPrim->setVisible(false);
	ui->lblAddingShape->setVisible(false);
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

	ui->nodeTree->repaint();
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
	MainWindow::SetStatus("Drawing new primitive box...", 0);
	ui->lblAddingPrim->setText("...Placing Box");
	ui->lblAddingPrim->setVisible(true);

	EntityDraw *pEntDraw = static_cast<EntityDraw *>(m_ItemRef.GetDraw());
	if(pEntDraw)
		pEntDraw->SetDrawShape(SHAPE_Box, true);
}

void EntityWidget::on_actionAddCirclePrimitive_triggered()
{
	MainWindow::SetStatus("Drawing new primitive circle...", 0);
	ui->lblAddingPrim->setText("...Placing Circle");
	ui->lblAddingPrim->setVisible(true);

	EntityDraw *pEntDraw = static_cast<EntityDraw *>(m_ItemRef.GetDraw());
	if(pEntDraw)
		pEntDraw->SetDrawShape(SHAPE_Circle, true);
}

void EntityWidget::on_actionAddPolygonPrimitive_triggered()
{
	MainWindow::SetStatus("Drawing new primitive polygon...", 0);
	ui->lblAddingPrim->setText("...Placing Polygon");
	ui->lblAddingPrim->setVisible(true);

	EntityDraw *pEntDraw = static_cast<EntityDraw *>(m_ItemRef.GetDraw());
	if(pEntDraw)
		pEntDraw->SetDrawShape(SHAPE_Polygon, true);
}

void EntityWidget::on_actionAddSegmentPrimitive_triggered()
{
	MainWindow::SetStatus("Drawing new primitive line segment...", 0);
	ui->lblAddingPrim->setText("...Placing Line Segment");
	ui->lblAddingPrim->setVisible(true);

	EntityDraw *pEntDraw = static_cast<EntityDraw *>(m_ItemRef.GetDraw());
	if(pEntDraw)
		pEntDraw->SetDrawShape(SHAPE_Segment, true);
}

void EntityWidget::on_actionAddLineChainPrimitive_triggered()
{
	MainWindow::SetStatus("Drawing new primitive line chain...", 0);
	ui->lblAddingPrim->setText("...Placing Line Chain");
	ui->lblAddingPrim->setVisible(true);

	EntityDraw *pEntDraw = static_cast<EntityDraw *>(m_ItemRef.GetDraw());
	if(pEntDraw)
		pEntDraw->SetDrawShape(SHAPE_LineChain, true);
}

void EntityWidget::on_actionAddLineLoopPrimitive_triggered()
{
	MainWindow::SetStatus("Drawing new primitive line loop...", 0);
	ui->lblAddingPrim->setText("...Placing Line Loop");
	ui->lblAddingPrim->setVisible(true);

	EntityDraw *pEntDraw = static_cast<EntityDraw *>(m_ItemRef.GetDraw());
	if(pEntDraw)
		pEntDraw->SetDrawShape(SHAPE_LineLoop, true);
}

void EntityWidget::on_actionAddBoxShape_triggered()
{
}

void EntityWidget::on_actionAddCircleShape_triggered()
{
}

void EntityWidget::on_actionAddPolygonShape_triggered()
{
}

void EntityWidget::on_actionAddSegmentShape_triggered()
{
}

void EntityWidget::on_actionAddLineChainShape_triggered()
{
}

void EntityWidget::on_actionAddLineLoopShape_triggered()
{
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
}
