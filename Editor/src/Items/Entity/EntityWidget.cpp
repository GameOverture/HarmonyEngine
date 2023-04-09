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
	m_AddShapeActionGroup(this),
	m_bAllowSelectionUndoCmd(true)
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

	//m_AddShapeActionGroup.addAction(ui->actionVertexEditMode);
	ui->btnVertexEditMode->setDefaultAction(ui->actionVertexEditMode);

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

	ui->nodeTree->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(ui->nodeTree, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(OnContextMenu(const QPoint &)));

	connect(ui->nodeTree->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)), this, SLOT(OnTreeSelectionChanged(const QItemSelection &, const QItemSelection &)));

	// Initialize what items are selected in the model
	QList<EntityTreeItemData *> childList, shapeList;
	pEntityModel->GetTreeModel().GetTreeItemData(childList, shapeList);
	childList += shapeList;
	QList<EntityTreeItemData *> selectedItemsList;
	for(EntityTreeItemData *pItem : childList)
	{
		if(pItem->IsSelected())
			selectedItemsList.push_back(pItem);
	}
	SetSelectedItems(selectedItemsList, QList<EntityTreeItemData *>());
}

EntityWidget::~EntityWidget()
{
	delete ui;
}

/*virtual*/ void EntityWidget::OnGiveMenuActions(QMenu *pMenu) /*override*/
{
	pMenu->addAction(ui->actionVertexEditMode);
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
	bool bEnableVemMode = false;
	QModelIndexList selectedIndices = ui->nodeTree->selectionModel()->selectedIndexes();
	selectedIndices.erase(std::remove_if(selectedIndices.begin(), selectedIndices.end(),
		[](const QModelIndex &index) {
			return index.column() != 0;
		}),
		selectedIndices.end());

	m_ContextMenu.clear();

	if(selectedIndices.empty())
	{
		ui->actionOrderChildrenUp->setEnabled(false);
		ui->actionOrderChildrenDown->setEnabled(false);
		ui->actionRemoveItems->setEnabled(false);

		ui->actionRenameItem->setEnabled(false);

		ui->actionConvertToArray->setEnabled(false);
		ui->actionUnpackFromArray->setEnabled(false);
		ui->actionPackToArray->setEnabled(false);

		ui->actionCopyEntityItems->setEnabled(false);
		ui->actionPasteEntityItems->setEnabled(false);

		ui->lblSelectedItemIcon->setVisible(false);
		ui->lblSelectedItemText->setVisible(false);

		ui->propertyTree->setModel(nullptr);

		bEnableVemMode = false;
	}
	else
	{
		bool bRootOrBvFolder = false;
		bool bSelectedHaveSameParent = true;
		bool bAllSameType = true;
		bool bAllArrayItems = true;

		QModelIndex parentIndex = selectedIndices.at(0).parent();
		HyGuiItemType eType = ui->nodeTree->model()->data(selectedIndices[0], Qt::UserRole).value<EntityTreeItemData *>()->GetType();
		for(const QModelIndex &index : selectedIndices)
		{
			if(index.parent() != parentIndex)
				bSelectedHaveSameParent = false;

			EntityTreeItemData *pEntItemData = ui->nodeTree->model()->data(index, Qt::UserRole).value<EntityTreeItemData *>();
			if(pEntItemData->GetType() != eType)
				bAllSameType = false;
			if(pEntItemData->GetEntType() != ENTTYPE_ArrayItem)
				bAllArrayItems = false;
			if(pEntItemData->GetEntType() == ENTTYPE_Root || pEntItemData->GetEntType() == ENTTYPE_BvFolder)
				bRootOrBvFolder = true;
		}
		
		if(bSelectedHaveSameParent && bAllSameType && bAllArrayItems)
		{
			ui->actionPackToArray->setEnabled(false);
			ui->actionConvertToArray->setEnabled(false);
			ui->actionUnpackFromArray->setEnabled(true);

			ui->actionUnpackFromArray->setIcon(HyGlobal::ItemIcon(eType, SUBICON_Close));
			m_ContextMenu.addAction(ui->actionUnpackFromArray);
		}
		else if(bSelectedHaveSameParent && bAllSameType && selectedIndices.size() == 1)
		{
			ui->actionPackToArray->setEnabled(false);
			ui->actionConvertToArray->setEnabled(true);
			ui->actionUnpackFromArray->setEnabled(false);

			ui->actionConvertToArray->setIcon(HyGlobal::ItemIcon(eType, SUBICON_New));
			m_ContextMenu.addAction(ui->actionConvertToArray);
		}
		else
		{
			ui->actionPackToArray->setEnabled(bSelectedHaveSameParent && bAllSameType);
			ui->actionConvertToArray->setEnabled(false);
			ui->actionUnpackFromArray->setEnabled(false);

			ui->actionPackToArray->setIcon(HyGlobal::ItemIcon(eType, SUBICON_Open));
			m_ContextMenu.addAction(ui->actionPackToArray);
		}

		m_ContextMenu.addSeparator();

		ui->actionCopyEntityItems->setEnabled(bRootOrBvFolder == false);
		ui->actionPasteEntityItems->setEnabled(bRootOrBvFolder == false);

		m_ContextMenu.addAction(ui->actionCopyEntityItems);
		m_ContextMenu.addAction(ui->actionPasteEntityItems);

		ui->actionOrderChildrenUp->setEnabled(bSelectedHaveSameParent);
		ui->actionOrderChildrenDown->setEnabled(bSelectedHaveSameParent);
		
		ui->actionRemoveItems->setEnabled(bRootOrBvFolder == false);

		if(selectedIndices.size() == 1)
		{
			EntityTreeItemData *pEntTreeItemData = ui->nodeTree->model()->data(selectedIndices[0], Qt::UserRole).value<EntityTreeItemData *>();

			ui->lblSelectedItemIcon->setVisible(true);
			ui->lblSelectedItemIcon->setPixmap(pEntTreeItemData->GetIcon(SUBICON_Settings).pixmap(QSize(16, 16)));
			ui->lblSelectedItemText->setVisible(true);
			ui->lblSelectedItemText->setText(pEntTreeItemData->GetCodeName() % " Properties");

			PropertiesTreeModel &propModelRef = pEntTreeItemData->GetPropertiesModel();
			ui->propertyTree->setModel(&propModelRef);

			bEnableVemMode = (pEntTreeItemData->GetType() == ITEM_Primitive || pEntTreeItemData->GetType() == ITEM_Shape);
		}
		else
		{
			ui->lblSelectedItemIcon->setVisible(false);
			ui->lblSelectedItemText->setVisible(true);
			ui->lblSelectedItemText->setText("Multiple items selected");

			ui->propertyTree->setModel(nullptr);

			bEnableVemMode = false;
		}

		// Expand the top level nodes (the properties' categories)
		ui->propertyTree->expandAll();
		ui->propertyTree->resizeColumnToContents(0);
	}

	if(bEnableVemMode)
		ui->actionVertexEditMode->setEnabled(true);
	else
	{
		ui->actionVertexEditMode->setChecked(false);
		ui->actionVertexEditMode->setEnabled(false);
	}

	ui->nodeTree->repaint();
}

/*virtual*/ void EntityWidget::OnFocusState(int iStateIndex, QVariant subState) /*override*/
{
	EntityTreeModel *pTreeModel = static_cast<EntityTreeModel *>(ui->nodeTree->model());

	// This occurs whenever a property changes within ui->propertyTree
	EntityTreeItemData *pEntityTreeData = reinterpret_cast<EntityTreeItemData *>(subState.toLongLong());
	QModelIndex index = pTreeModel->FindIndex<EntityTreeItemData *>(pEntityTreeData, 0);

	ui->nodeTree->selectionModel()->select(index, QItemSelectionModel::Select);
}

QList<EntityTreeItemData *> EntityWidget::GetSelectedItems(bool bIncludeRootEntity, bool bIncludeBvFolder, bool bIncludeArrayFolders, bool bIncludeShapes)
{
	QModelIndexList selectedIndices = ui->nodeTree->selectionModel()->selectedIndexes();
	QList<EntityTreeItemData *> selectedItemList;
	for(QModelIndex index : selectedIndices)
	{
		if(index.column() != 0)
			continue;

		EntityTreeItemData *pCurItemData = ui->nodeTree->model()->data(index, Qt::UserRole).value<EntityTreeItemData *>();

		switch(pCurItemData->GetEntType())
		{
		case ENTTYPE_Root:
			if(bIncludeRootEntity)
				selectedItemList.push_back(pCurItemData);
			break;

		case ENTTYPE_BvFolder:
			if(bIncludeBvFolder)
				selectedItemList.push_back(pCurItemData);
			break;

		case ENTTYPE_ArrayFolder: {
			if(bIncludeArrayFolders)
				selectedItemList.push_back(pCurItemData);

			QList<TreeModelItemData *> arrayItemDataList = static_cast<EntityModel *>(m_ItemRef.GetModel())->GetTreeModel().GetItemsRecursively(index);
			for(TreeModelItemData *pArrayItemData : arrayItemDataList)
				selectedItemList.push_back(static_cast<EntityTreeItemData *>(pArrayItemData));
			break; }

		case ENTTYPE_Item:
		case ENTTYPE_ArrayItem:
			if(pCurItemData->GetType() == ITEM_Shape)
			{
				if(bIncludeShapes)
					selectedItemList.push_back(pCurItemData);
			}
			else
				selectedItemList.push_back(pCurItemData);
			break;

		default:
			HyGuiLog("EntityWidget::GetSelectedItems - Unknown EntityItemType", LOGTYPE_Error);
			break;
		}
	}

	return selectedItemList;
}

// Will clear and select only what 'uuidList' contains. Will optionally allow the signal callback to push an UndoCmd on the stack for selection
void EntityWidget::RequestSelectedItems(QList<QUuid> uuidList, bool bPushUndoCmd)
{
	m_bAllowSelectionUndoCmd = bPushUndoCmd;

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

// This catches all cases when selection occurs. Does not call signal
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

	UpdateActions();
}

void EntityWidget::CheckShapeAdd(EditorShape eShapeType, bool bAsPrimitive)
{
	switch(eShapeType)
	{
	case SHAPE_Box:
		if(bAsPrimitive)
			ui->btnAddPrimitiveBox->setChecked(true);
		else
			ui->btnAddShapeBox->setChecked(true);
		break;

	case SHAPE_Circle:
		if(bAsPrimitive)
			ui->btnAddPrimitiveCircle->setChecked(true);
		else
			ui->btnAddShapeCircle->setChecked(true);
		break;

	case SHAPE_Polygon:
		if(bAsPrimitive)
			ui->btnAddPrimitivePolygon->setChecked(true);
		else
			ui->btnAddShapePolygon->setChecked(true);
		break;

	case SHAPE_LineSegment:
		if(bAsPrimitive)
			ui->btnAddPrimitiveSegment->setChecked(true);
		else
			ui->btnAddShapeSegment->setChecked(true);
		break;

	case SHAPE_LineChain:
		if(bAsPrimitive)
			ui->btnAddPrimitiveChain->setChecked(true);
		else
			ui->btnAddShapeChain->setChecked(true);
		break;

	case SHAPE_LineLoop:
		if(bAsPrimitive)
			ui->btnAddPrimitiveLoop->setChecked(true);
		else
			ui->btnAddShapeLoop->setChecked(true);
		break;
	}
}

void EntityWidget::CheckVertexEditMode(bool bCheck)
{
	ui->btnVertexEditMode->setChecked(bCheck);
}

void EntityWidget::UncheckAll()
{
	for(QAction *pAction : m_AddShapeActionGroup.actions())
		pAction->setChecked(false);
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

void EntityWidget::OnContextMenu(const QPoint &pos)
{
	



	//HyGuiItemType eAllSameType = ITEM_Unknown;
	//for(EntityTreeItemData *pSelItem : selectedItemList)
	//{
	//	if(eAllSameType == ITEM_Unknown)
	//		eAllSameType = pSelItem->
	//}

	//if(selectedItems.count() == 1)
	//{
	//	switch(selectedItems[0]->GetEntType())
	//	{
	//	case ENTTYPE_Root:
	//	case ENTTYPE_BvFolder:
	//		break;

	//	case ENTTYPE_Item:
	//	case ENTTYPE_ArrayFolder:
	//	case ENTTYPE_ArrayItem:
	//		break;

	//	default:
	//		HyGuiLog("EntityWidget::OnContextMenu - Unknown EntityItemType", LOGTYPE_Error);
	//		break;
	//	}
	//}
	////ui->actionConvertToArray->setText("Open Selected Items");
	//ui->actionConvertToArray->setIcon(HyGlobal::ItemIcon(ITEM_Prefix, SUBICON_None));
	//contextMenu.addAction(ui->actionConvertToArray);
	//
	//if(selectedItems.count() == 1)
	//	contextMenu.addAction(ui->actionRenameItem);





	//if(pContextExplorerItem == nullptr)
	//{
	//	contextMenu.addAction(FINDACTION("actionNewProject"));
	//	contextMenu.addAction(FINDACTION("actionOpenProject"));
	//}
	//else
	//{
	//	switch(pContextExplorerItem->GetType())
	//	{
	//	case ITEM_Project:
	//		if(Harmony::GetProject() != pContextExplorerItem)
	//			contextMenu.addAction(FINDACTION("actionActivateProject"));
	//		else
	//			contextMenu.addMenu(MainWindow::GetNewItemMenu());
	//		contextMenu.addSeparator();
	//		contextMenu.addAction(FINDACTION("actionCloseProject"));
	//		contextMenu.addAction(FINDACTION("actionProjectSettings"));
	//		contextMenu.addSeparator();
	//		contextMenu.addAction(FINDACTION("actionOpenFolderExplorer"));
	//		contextMenu.addSeparator();
	//		contextMenu.addAction(ui->actionPasteItem);
	//		break;
	//	case ITEM_Audio:
	//	case ITEM_Particles:
	//	case ITEM_Text:
	//	case ITEM_Spine:
	//	case ITEM_Sprite:
	//	case ITEM_Source:
	//	case ITEM_Header:
	//	case ITEM_Entity:
	//	case ITEM_Prefab:
	//	case ITEM_Prefix:
	//		if(Harmony::GetProject() != &pContextExplorerItem->GetProject())
	//		{
	//			contextMenu.addAction(FINDACTION("actionActivateProject"));
	//			contextMenu.addSeparator();
	//		}
	//		else
	//		{
	//			if(selectedItems.count() > 0)
	//			{
	//				if(selectedItems.count() > 1)
	//				{
	//					ui->actionOpen->setText("Open Selected Items");
	//					ui->actionOpen->setIcon(HyGlobal::ItemIcon(ITEM_Prefix, SUBICON_None));
	//				}
	//				else
	//				{
	//					ui->actionOpen->setText("Open " % pContextExplorerItem->GetName(false));
	//					ui->actionOpen->setIcon(HyGlobal::ItemIcon(pContextExplorerItem->GetType(), SUBICON_None));
	//				}
	//				contextMenu.addAction(ui->actionOpen);
	//				contextMenu.addSeparator();
	//			}

	//			contextMenu.addMenu(MainWindow::GetNewItemMenu());
	//			contextMenu.addSeparator();
	//		}

	//		contextMenu.addAction(ui->actionRename);
	//		contextMenu.addAction(ui->actionCopyItem);
	//		contextMenu.addAction(ui->actionPasteItem);
	//		contextMenu.addSeparator();
	//		if(selectedItems.count() + selectedPrefixes.count() == 1)
	//		{
	//			ui->actionDeleteItem->setText("Delete " % pContextExplorerItem->GetName(false));
	//			ui->actionDeleteItem->setIcon(HyGlobal::ItemIcon(pContextExplorerItem->GetType(), SUBICON_Delete));
	//		}
	//		else
	//		{
	//			ui->actionDeleteItem->setText("Delete Selected Items");
	//			ui->actionDeleteItem->setIcon(HyGlobal::ItemIcon(ITEM_Prefix, SUBICON_Delete));
	//		}
	//		contextMenu.addAction(ui->actionDeleteItem);
	//		break;

	//	default: {
	//		HyGuiLog("ExplorerWidget::OnContextMenu - Unknown TreeModelItemData type", LOGTYPE_Error);
	//	} break;
	//	}
	//}

	m_ContextMenu.exec(ui->nodeTree->mapToGlobal(pos));
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

	if(m_bAllowSelectionUndoCmd)
		m_ItemRef.GetUndoStack()->push(new EntityUndoCmd_SelectionChanged(m_ItemRef, selectedItemDataList, deselectedItemDataList));
	else
	{
		static_cast<EntityModel *>(m_ItemRef.GetModel())->Cmd_SelectionChanged(selectedItemDataList, deselectedItemDataList);
		m_bAllowSelectionUndoCmd = true; // Only block UndoCmd once
	}
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
	static_cast<EntityModel *>(m_ItemRef.GetModel())->SetShapeEditDrag(SHAPE_Box, true);
}

void EntityWidget::on_actionAddCirclePrimitive_triggered()
{
	static_cast<EntityModel *>(m_ItemRef.GetModel())->SetShapeEditDrag(SHAPE_Circle, true);
}

void EntityWidget::on_actionAddPolygonPrimitive_triggered()
{
	static_cast<EntityModel *>(m_ItemRef.GetModel())->SetShapeEditDrag(SHAPE_Polygon, true);
}

void EntityWidget::on_actionAddSegmentPrimitive_triggered()
{
	static_cast<EntityModel *>(m_ItemRef.GetModel())->SetShapeEditDrag(SHAPE_LineSegment, true);
}

void EntityWidget::on_actionAddLineChainPrimitive_triggered()
{
	static_cast<EntityModel *>(m_ItemRef.GetModel())->SetShapeEditDrag(SHAPE_LineChain, true);
}

void EntityWidget::on_actionAddLineLoopPrimitive_triggered()
{
	static_cast<EntityModel *>(m_ItemRef.GetModel())->SetShapeEditDrag(SHAPE_LineLoop, true);
}

void EntityWidget::on_actionAddBoxShape_triggered()
{
	static_cast<EntityModel *>(m_ItemRef.GetModel())->SetShapeEditDrag(SHAPE_Box, false);
}

void EntityWidget::on_actionAddCircleShape_triggered()
{
	static_cast<EntityModel *>(m_ItemRef.GetModel())->SetShapeEditDrag(SHAPE_Circle, false);
}

void EntityWidget::on_actionAddPolygonShape_triggered()
{
	static_cast<EntityModel *>(m_ItemRef.GetModel())->SetShapeEditDrag(SHAPE_Polygon, false);
}

void EntityWidget::on_actionAddSegmentShape_triggered()
{
	static_cast<EntityModel *>(m_ItemRef.GetModel())->SetShapeEditDrag(SHAPE_LineSegment, false);
}

void EntityWidget::on_actionAddLineChainShape_triggered()
{
	static_cast<EntityModel *>(m_ItemRef.GetModel())->SetShapeEditDrag(SHAPE_LineChain, false);
}

void EntityWidget::on_actionAddLineLoopShape_triggered()
{
	static_cast<EntityModel *>(m_ItemRef.GetModel())->SetShapeEditDrag(SHAPE_LineLoop, false);
}

void EntityWidget::on_actionVertexEditMode_toggled(bool bChecked)
{
	static_cast<EntityModel *>(m_ItemRef.GetModel())->SetShapeEditVemMode(bChecked);
}

void EntityWidget::on_actionOrderChildrenUp_triggered()
{
	QModelIndexList selectedIndexList = ui->nodeTree->selectionModel()->selectedIndexes();
	std::sort(selectedIndexList.begin(), selectedIndexList.end(), [](const QModelIndex &a, const QModelIndex &b)
		{
			if(a.parent() != b.parent())
				HyGuiLog("EntityWidget::on_actionOrderChildrenUp_triggered - selected indices have different parents", LOGTYPE_Error);

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
	if(selectedIndexList.empty())
		return;

	std::sort(selectedIndexList.begin(), selectedIndexList.end(), [](const QModelIndex &a, const QModelIndex &b)
		{
			if(a.parent() != b.parent())
				HyGuiLog("EntityWidget::on_actionOrderChildrenDown_triggered - selected indices have different parents", LOGTYPE_Error);

			return a.row() > b.row();
		});

	int iNumChildren = ui->nodeTree->model()->rowCount(ui->nodeTree->model()->parent(selectedIndexList[0]));
	QList<EntityTreeItemData *> selectedItemDataList;
	QList<int> curIndexList;
	QList<int> newIndexList;
	int iDiscardBotIndices = -1;
	for(QModelIndex index : selectedIndexList)
	{
		// Only take selected items that are the first column
		if(index.column() != 0)
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
	QList<EntityTreeItemData *> poppedItemList = GetSelectedItems(false, false, false, true);

	QUndoCommand *pCmd = new EntityUndoCmd_PopItems(m_ItemRef, poppedItemList);
	m_ItemRef.GetUndoStack()->push(pCmd);
}

void EntityWidget::on_actionUnpackFromArray_triggered()
{
}

void EntityWidget::on_actionConvertToArray_triggered()
{
}

void EntityWidget::on_actionPackToArray_triggered()
{
}

void EntityWidget::on_actionCopyEntityItems_triggered()
{
}

void EntityWidget::on_actionPasteEntityItems_triggered()
{
}
