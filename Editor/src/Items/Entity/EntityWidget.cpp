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
#include "EntityItemMimeData.h"
#include "EntityUndoCmds.h"
#include "GlobalUndoCmds.h"
#include "DlgInputName.h"
#include "DlgInputNumber.h"
#include "MainWindow.h"

#include <QClipboard>

EntityWidget::EntityWidget(ProjectItemData &itemRef, QWidget *pParent /*= nullptr*/) :
	IWidget(itemRef, pParent),
	ui(new Ui::EntityWidget),
	m_AddShapeActionGroup(this)
{
	ui->setupUi(this);

	// Remove and re-add the main layout that holds everything. This makes the Qt Designer (.ui) files work with the base class 'IWidget'. Otherwise it jumbles them together.
	layout()->removeItem(ui->verticalLayout);
	GetBelowStatesLayout()->addItem(ui->verticalLayout);
	GetAboveStatesLayout()->addWidget(ui->grpNodes);

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

	ui->nodeTree->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(ui->nodeTree, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(OnContextMenu(const QPoint &)));

	connect(ui->nodeTree->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)), this, SLOT(OnTreeSelectionChanged(const QItemSelection &, const QItemSelection &)));

	connect(ui->nodeTree, SIGNAL(collapsed(const QModelIndex &)), this, SLOT(OnCollapsedNode(const QModelIndex &)));

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

	QItemSelection *pItemSelection = new QItemSelection();
	EntityTreeModel *pTreeModel = static_cast<EntityTreeModel *>(ui->nodeTree->model());
	for(EntityTreeItemData *pSelectItem : selectedItemsList)
	{
		QModelIndex index = pTreeModel->FindIndex<EntityTreeItemData *>(pSelectItem, 0);
		pItemSelection->select(index, index);
	}

	QItemSelectionModel *pSelectionModel = ui->nodeTree->selectionModel();
	pSelectionModel->blockSignals(true);
	pSelectionModel->select(*pItemSelection, QItemSelectionModel::Select);
	pSelectionModel->blockSignals(false);
	delete pItemSelection;

	UpdateActions();
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
	EntityTreeModel *pTreeModel = static_cast<EntityTreeModel *>(ui->nodeTree->model());

	// Root and BvFolder should always be expanded
	ui->nodeTree->expand(pTreeModel->FindIndex<EntityTreeItemData *>(pTreeModel->GetRootTreeItemData(), 0));
	ui->nodeTree->expand(pTreeModel->FindIndex<EntityTreeItemData *>(pTreeModel->GetBvFolderTreeItemData(), 0));

	// Query what items are selected in the Explorer Widget
	QList<ProjectItemData *> selectedItems; QList<ExplorerItemData *> selectedPrefixes;
	MainWindow::GetExplorerWidget().GetSelected(selectedItems, selectedPrefixes);
	bool bEnableAddNodeBtn = false;
	
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
	QModelIndexList selectedIndices = GetSelectedItems();

	m_ContextMenu.clear();

	if(selectedIndices.empty())
	{
		ui->actionOrderChildrenUp->setEnabled(false);
		ui->actionOrderChildrenDown->setEnabled(false);
		ui->actionRemoveItems->setEnabled(false);

		ui->actionConvertShape->setEnabled(false);
		ui->actionRenameItem->setEnabled(false);

		ui->actionDuplicateToArray->setEnabled(false);
		ui->actionUnpackFromArray->setEnabled(false);
		ui->actionPackToArray->setEnabled(false);

		ui->actionCutEntityItems->setEnabled(false);
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
		ItemType eType = ui->nodeTree->model()->data(selectedIndices[0], Qt::UserRole).value<EntityTreeItemData *>()->GetType();
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

		ui->actionOrderChildrenUp->setEnabled(bSelectedHaveSameParent);
		ui->actionOrderChildrenDown->setEnabled(bSelectedHaveSameParent);

		if(bRootOrBvFolder == false && selectedIndices.size() == 1 && (eType == ITEM_Primitive || eType == ITEM_BoundingVolume))
		{
			ui->actionConvertShape->setEnabled(true);
			if(eType == ITEM_Primitive)
			{
				ui->actionConvertShape->setIcon(HyGlobal::ItemIcon(ITEM_BoundingVolume, SUBICON_None));
				ui->actionConvertShape->setText("Convert Shape to Bounding Volume");
			}
			else
			{
				ui->actionConvertShape->setIcon(HyGlobal::ItemIcon(ITEM_Primitive, SUBICON_None));
				ui->actionConvertShape->setText("Convert Shape to Primitive");
			}
		}
		else
		{
			ui->actionConvertShape->setEnabled(false);
			ui->actionConvertShape->setText("Convert Shape");
		}
		ui->actionRenameItem->setEnabled(bRootOrBvFolder == false && selectedIndices.size() == 1);
		ui->actionCutEntityItems->setEnabled(bRootOrBvFolder == false);
		ui->actionCopyEntityItems->setEnabled(bRootOrBvFolder == false);

		QClipboard *pClipboard = QGuiApplication::clipboard();
		const QMimeData *pMimeData = pClipboard->mimeData();
		ui->actionPasteEntityItems->setEnabled(pMimeData->hasFormat(HyGlobal::MimeTypeString(MIMETYPE_EntityItems)));

		ui->actionRemoveItems->setEnabled(bRootOrBvFolder == false);
		
		if(bSelectedHaveSameParent && bAllSameType && bAllArrayItems)
		{
			ui->actionPackToArray->setEnabled(false);
			ui->actionDuplicateToArray->setEnabled(false);
			ui->actionUnpackFromArray->setEnabled(true);

			ui->actionUnpackFromArray->setIcon(HyGlobal::ItemIcon(eType, SUBICON_Close));
			m_ContextMenu.addAction(ui->actionUnpackFromArray);
		}
		else if(bSelectedHaveSameParent && bAllSameType && selectedIndices.size() == 1)
		{
			ui->actionPackToArray->setEnabled(false);
			ui->actionDuplicateToArray->setEnabled(true);
			ui->actionUnpackFromArray->setEnabled(false);

			ui->actionDuplicateToArray->setIcon(HyGlobal::ItemIcon(eType, SUBICON_New));
			m_ContextMenu.addAction(ui->actionDuplicateToArray);
		}
		else
		{
			ui->actionPackToArray->setEnabled(bSelectedHaveSameParent && bAllSameType && bAllArrayItems == false);
			ui->actionDuplicateToArray->setEnabled(false);
			ui->actionUnpackFromArray->setEnabled(false);

			ui->actionPackToArray->setIcon(HyGlobal::ItemIcon(eType, SUBICON_Open));
			m_ContextMenu.addAction(ui->actionPackToArray);
		}

		if(ui->actionConvertShape->isEnabled())
			m_ContextMenu.addAction(ui->actionConvertShape);
		m_ContextMenu.addAction(ui->actionOrderChildrenUp);
		m_ContextMenu.addAction(ui->actionOrderChildrenDown);
		m_ContextMenu.addSeparator();
		m_ContextMenu.addAction(ui->actionRenameItem);
		m_ContextMenu.addAction(ui->actionCutEntityItems);
		m_ContextMenu.addAction(ui->actionCopyEntityItems);
		m_ContextMenu.addAction(ui->actionPasteEntityItems);
		m_ContextMenu.addSeparator();
		m_ContextMenu.addAction(ui->actionRemoveItems);

		if(selectedIndices.size() == 1)
		{
			EntityTreeItemData *pEntTreeItemData = ui->nodeTree->model()->data(selectedIndices[0], Qt::UserRole).value<EntityTreeItemData *>();

			ui->lblSelectedItemIcon->setVisible(true);
			ui->lblSelectedItemIcon->setPixmap(pEntTreeItemData->GetIcon(SUBICON_Settings).pixmap(QSize(16, 16)));
			ui->lblSelectedItemText->setVisible(true);
			ui->lblSelectedItemText->setText(pEntTreeItemData->GetCodeName() % " Properties");

			PropertiesTreeModel &propModelRef = pEntTreeItemData->GetPropertiesModel(GetCurStateIndex());
			ui->propertyTree->setModel(&propModelRef);

			bEnableVemMode = (pEntTreeItemData->GetType() == ITEM_Primitive || pEntTreeItemData->GetType() == ITEM_BoundingVolume);
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
}

QModelIndexList EntityWidget::GetSelectedItems()
{
	QModelIndexList indexList = ui->nodeTree->selectionModel()->selectedIndexes();

	indexList.erase(std::remove_if(indexList.begin(), indexList.end(),
		[](const QModelIndex &index) {
			return index.column() != 0;
		}),
		indexList.end());

	return indexList;
}

// Will clear and select only what 'uuidList' contains
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

void EntityWidget::CheckShapeAddBtn(EditorShape eShapeType, bool bAsPrimitive)
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

void EntityWidget::OnContextMenu(const QPoint &pos)
{
	if(m_ContextMenu.isEmpty() == false)
		m_ContextMenu.exec(ui->nodeTree->mapToGlobal(pos));
}

void EntityWidget::OnTreeSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
	QList<EntityTreeItemData *> selectedItemDataList;
	QModelIndexList selectedIndices = selected.indexes();
	for(QModelIndex selIndex : selectedIndices)
	{
		if(selIndex.column() != 0)
			continue;

		EntityTreeItemData *pCurItemData = ui->nodeTree->model()->data(selIndex, Qt::UserRole).value<EntityTreeItemData *>();
		if(pCurItemData == nullptr)
			continue;
		
		selectedItemDataList.push_back(pCurItemData);

		// Below selects all array items when their folder is selected - TODO: disabled until move up/down is handled differently
		//if(pCurItemData->GetEntType() == ENTTYPE_ArrayFolder)
		//{
		//	QAbstractItemModel *pModel = ui->nodeTree->model();
		//	int numRows = pModel->rowCount(selIndex);
		//	for(int row = 0; row < numRows; ++row)
		//	{
		//		QModelIndex childIndex = pModel->index(row, 0, selIndex);
		//		if(childIndex.isValid())
		//		{
		//			ui->nodeTree->selectionModel()->select(childIndex, QItemSelectionModel::Select);
		//			selectedItemDataList.push_back(ui->nodeTree->model()->data(childIndex, Qt::UserRole).value<EntityTreeItemData *>());
		//		}
		//	}
		//}
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

	static_cast<EntityModel *>(m_ItemRef.GetModel())->Cmd_SelectionChanged(selectedItemDataList, deselectedItemDataList);

	UpdateActions();
}

void EntityWidget::OnCollapsedNode(const QModelIndex &indexRef)
{
	EntityTreeItemData *pTreeItemData = ui->nodeTree->model()->data(indexRef, Qt::UserRole).value<EntityTreeItemData *>();

	// Prevent Root or BvFolder from collapsing
	if(pTreeItemData->GetEntType() == ENTTYPE_Root || pTreeItemData->GetEntType() == ENTTYPE_BvFolder)
		ui->nodeTree->expand(indexRef);
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
	QModelIndexList selectedIndexList = GetSelectedItems();
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
		//// Only take selected items that are the first column, and they're children under the root entity tree item
		//if(index.column() != 0 || index.parent().row() != 0)
		//	continue;

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
	QModelIndexList selectedIndexList = GetSelectedItems();
	if(selectedIndexList.empty())
		return;

	std::sort(selectedIndexList.begin(), selectedIndexList.end(), [](const QModelIndex &a, const QModelIndex &b)
		{
			return a.row() > b.row();
		});

	int iNumChildren = ui->nodeTree->model()->rowCount(ui->nodeTree->model()->parent(selectedIndexList[0]));
	QList<EntityTreeItemData *> selectedItemDataList;
	QList<int> curIndexList;
	QList<int> newIndexList;
	int iDiscardBotIndices = -1;
	for(QModelIndex index : selectedIndexList)
	{
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
	QModelIndexList selectedIndices = GetSelectedItems();

	// Sort indices so when removing the items, the other items' indices aren't affected (descending row index)
	std::sort(selectedIndices.begin(), selectedIndices.end(), [](const QModelIndex &a, const QModelIndex &b)
		{
			return a.row() > b.row();
		});

	QList<EntityTreeItemData *> poppedItemList;
	for(QModelIndex index : selectedIndices)
	{
		EntityTreeItemData *pCurItemData = ui->nodeTree->model()->data(index, Qt::UserRole).value<EntityTreeItemData *>();
		if(pCurItemData->GetEntType() == ENTTYPE_Item || pCurItemData->GetEntType() == ENTTYPE_ArrayItem)
			poppedItemList.push_back(pCurItemData);
	}

	QUndoCommand *pCmd = new EntityUndoCmd_PopItems(m_ItemRef, poppedItemList);
	m_ItemRef.GetUndoStack()->push(pCmd);
}

void EntityWidget::on_actionConvertShape_triggered()
{
	QModelIndexList selectedIndexList = GetSelectedItems();

	if(selectedIndexList.size() != 1)
	{
		HyGuiLog("EntityWidget::on_actionConvertShape_triggered was invoked with improper selection size", LOGTYPE_Error);
		return;
	}
	EntityTreeItemData *pCurItemData = ui->nodeTree->model()->data(selectedIndexList[0], Qt::UserRole).value<EntityTreeItemData *>();
	if(pCurItemData->GetType() != ITEM_Primitive && pCurItemData->GetType() != ITEM_BoundingVolume)
	{
		HyGuiLog("EntityWidget::on_actionConvertShape_triggered was invoked with improper selection type", LOGTYPE_Error);
		return;
	}

	QUndoCommand *pCmd = new EntityUndoCmd_ConvertShape(m_ItemRef, pCurItemData);
	m_ItemRef.GetUndoStack()->push(pCmd);
}

void EntityWidget::on_actionRenameItem_triggered()
{
	QModelIndexList selectedIndexList = GetSelectedItems();

	if(selectedIndexList.size() != 1)
	{
		HyGuiLog("EntityWidget::on_actionRenameItem_triggered was invoked with improper selection size", LOGTYPE_Error);
		return;
	}

	EntityTreeItemData *pEntTreeItemData = ui->nodeTree->model()->data(selectedIndexList[0], Qt::UserRole).value<EntityTreeItemData *>();

	DlgInputName dlg("Rename " % pEntTreeItemData->GetText(),
		pEntTreeItemData->GetCodeName(),
		HyGlobal::FileNameValidator(),
		[&](QString sTest) -> QString {
			QString sGenName = static_cast<EntityModel *>(m_ItemRef.GetModel())->GenerateCodeName(sTest);
			if(sGenName == sTest)
				return QString();
			else
				return "Name already exists";
		}, nullptr);

	if(dlg.exec() == QDialog::Accepted)
	{
		QUndoCommand *pCmd = new EntityUndoCmd_RenameItem(m_ItemRef, pEntTreeItemData, dlg.GetName());
		m_ItemRef.GetUndoStack()->push(pCmd);
	}
}

void EntityWidget::on_actionUnpackFromArray_triggered()
{

}

void EntityWidget::on_actionPackToArray_triggered()
{
	QModelIndexList selectedIndexList = GetSelectedItems();

	// The selected items are going to be removed, so they need to be sorted in row descending order
	std::sort(selectedIndexList.begin(), selectedIndexList.end(), [](const QModelIndex &a, const QModelIndex &b)
		{
			return a.row() > b.row();
		});

	QList<EntityTreeItemData *>selectedTreeItemDataList;
	for(QModelIndex index : selectedIndexList)
	{
		EntityTreeItemData *pTreeItemData = ui->nodeTree->model()->data(index, Qt::UserRole).value<EntityTreeItemData *>();
		if(pTreeItemData->GetEntType() == ENTTYPE_Item)
			selectedTreeItemDataList.push_back(pTreeItemData);
	}

	// Move the 'primary' selected item to the front of the list
	QModelIndex primaryIndex = ui->nodeTree->currentIndex();
	QString sArrayName = ui->nodeTree->model()->data(primaryIndex, Qt::UserRole).value<EntityTreeItemData *>()->GetCodeName();

	QUndoCommand *pCmd = new EntityUndoCmd_PackToArray(m_ItemRef, selectedTreeItemDataList, sArrayName, primaryIndex.row());
	m_ItemRef.GetUndoStack()->push(pCmd);
}

void EntityWidget::on_actionDuplicateToArray_triggered()
{
	QModelIndex index = ui->nodeTree->currentIndex();
	EntityTreeItemData *pEntTreeItemData = ui->nodeTree->model()->data(index, Qt::UserRole).value<EntityTreeItemData *>();
	
	QString sDlgTitle = "Duplicate " % pEntTreeItemData->GetCodeName() % " into array of " % HyGlobal::ItemName(pEntTreeItemData->GetType(), false) % " elements";
	DlgInputNumber dlg(sDlgTitle, "Size", HyGlobal::ItemIcon(pEntTreeItemData->GetType(), SUBICON_New), 1, 1, 0x0FFFFFFF, nullptr, nullptr);
	if(dlg.exec() == QDialog::Accepted)
	{
		QUndoCommand *pCmd = new EntityUndoCmd_DuplicateToArray(m_ItemRef, pEntTreeItemData, dlg.GetValue());
		m_ItemRef.GetUndoStack()->push(pCmd);
	}
}

void EntityWidget::on_actionCutEntityItems_triggered()
{
	on_actionCopyEntityItems_triggered();
	on_actionRemoveItems_triggered();
}

void EntityWidget::on_actionCopyEntityItems_triggered()
{
	QModelIndexList selectedIndexList = GetSelectedItems();

	QList<EntityTreeItemData *>selectedTreeItemDataList;
	for(QModelIndex index : selectedIndexList)
	{
		EntityTreeItemData *pTreeItemData = ui->nodeTree->model()->data(index, Qt::UserRole).value<EntityTreeItemData *>();
		if(pTreeItemData->GetEntType() == ENTTYPE_Item || pTreeItemData->GetEntType() == ENTTYPE_ArrayItem)
			selectedTreeItemDataList.push_back(pTreeItemData);
	}

	QClipboard *pClipboard = QGuiApplication::clipboard();
	EntityItemMimeData *pMimeData = new EntityItemMimeData(m_ItemRef, selectedTreeItemDataList);
	pClipboard->setMimeData(pMimeData);
}

void EntityWidget::on_actionPasteEntityItems_triggered()
{
	QClipboard *pClipboard = QGuiApplication::clipboard();
	const QMimeData *pMimeData = pClipboard->mimeData();

	if(pMimeData->hasFormat("application/json") == false &&
		pMimeData->hasFormat(HyGlobal::MimeTypeString(MIMETYPE_EntityItems)) == false)
	{
		HyGuiLog("EntityWidget::on_actionPasteEntityItems_triggered - Wrong mime data type", LOGTYPE_Error);
		return;
	}

	QByteArray jsonData = pMimeData->data(HyGlobal::MimeTypeString(MIMETYPE_EntityItems));
	QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonData);
	QJsonObject pastedObject = jsonDocument.object();

	// Error check for correct project
	if(m_ItemRef.GetProject().GetAbsPath().compare(pastedObject["project"].toString(), Qt::CaseInsensitive) != 0)
	{
		HyGuiLog("Pasted entity items originate from a different project", LOGTYPE_Warning);
		return;
	}

	// Determine if the paste is going to be into an ArrayFolder or not
	EntityTreeItemData *pArrayFolder = nullptr;
	QModelIndex index = ui->nodeTree->currentIndex();
	EntityTreeItemData *pEntTreeItemData = ui->nodeTree->model()->data(index, Qt::UserRole).value<EntityTreeItemData *>();
	if(pEntTreeItemData->GetEntType() == ENTTYPE_ArrayFolder)
	{
		pArrayFolder = pEntTreeItemData;

		// Error check that all items on the clipboard match the type of the ArrayFolder before continuing with the paste
		QJsonArray itemArray = pastedObject["itemArray"].toArray();
		for(int i = 0; i < itemArray.size(); ++i)
		{
			QJsonObject itemObj = itemArray[i].toObject();
			QJsonObject descObj = itemObj["descObj"].toObject();
			if(HyGlobal::GetTypeFromString(descObj["itemType"].toString()) != pArrayFolder->GetType())
			{
				HyGuiLog("Pasted entity item " % descObj["codeName"].toString() % " has a mismatching type of " % descObj["itemType"].toString() % " and cannot be inserted into the array.", LOGTYPE_Warning);
				return;
			}
		}
	}

	EntityUndoCmd_PasteItems *pCmd = new EntityUndoCmd_PasteItems(m_ItemRef, pastedObject, pArrayFolder);
	m_ItemRef.GetUndoStack()->push(pCmd);
}
