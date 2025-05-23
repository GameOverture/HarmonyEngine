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
#include "AuxDopeSheet.h"
#include "PropertiesTreeMultiModel.h"

#include <QClipboard>
#include <QShortcut>

EntityWidget::EntityWidget(ProjectItemData &itemRef, QWidget *pParent /*= nullptr*/) :
	IWidget(itemRef, pParent),
	ui(new Ui::EntityWidget),
	m_AddShapeActionGroup(this),
	m_pMultiPropModel(nullptr)
{
	ui->setupUi(this);

	// Remove and re-add the main layout that holds everything. This makes the Qt Designer (.ui) files work with the base class 'IWidget'. Otherwise it jumbles them together.
	layout()->removeItem(ui->verticalLayout);
	GetBelowStatesLayout()->addItem(ui->verticalLayout);
	GetAboveStatesLayout()->addWidget(ui->grpNodes);

	ui->btnAddLabel->setDefaultAction(ui->actionAddLabel);
	ui->btnAddRichLabel->setDefaultAction(ui->actionAddRichLabel);
	ui->btnAddButton->setDefaultAction(ui->actionAddButton);
	ui->btnAddRackMeter->setDefaultAction(ui->actionAddRackMeter);
	ui->btnAddBarMeter->setDefaultAction(ui->actionAddBarMeter);
	ui->btnAddCheckBox->setDefaultAction(ui->actionAddCheckBox);
	ui->btnAddRadioButton->setDefaultAction(ui->actionAddRadioButton);
	ui->btnAddTextField->setDefaultAction(ui->actionAddTextField);
	ui->btnAddComboBox->setDefaultAction(ui->actionAddComboBox);
	ui->btnAddSlider->setDefaultAction(ui->actionAddSlider);

	m_AddShapeActionGroup.addAction(ui->actionAddBoxPrimitive);
	m_AddShapeActionGroup.addAction(ui->actionAddCirclePrimitive);
	m_AddShapeActionGroup.addAction(ui->actionAddPolygonPrimitive);
	m_AddShapeActionGroup.addAction(ui->actionAddSegmentPrimitive);
	m_AddShapeActionGroup.addAction(ui->actionAddLineChainPrimitive);
	m_AddShapeActionGroup.addAction(ui->actionAddCapsulePrimitive);
	ui->btnAddPrimitiveBox->setDefaultAction(ui->actionAddBoxPrimitive);
	ui->btnAddPrimitiveCircle->setDefaultAction(ui->actionAddCirclePrimitive);
	ui->btnAddPrimitivePolygon->setDefaultAction(ui->actionAddPolygonPrimitive);
	ui->btnAddPrimitiveSegment->setDefaultAction(ui->actionAddSegmentPrimitive);
	ui->btnAddPrimitiveChain->setDefaultAction(ui->actionAddLineChainPrimitive);
	ui->btnAddPrimitiveCapsule->setDefaultAction(ui->actionAddCapsulePrimitive);

	m_AddShapeActionGroup.addAction(ui->actionAddBoxShape);
	m_AddShapeActionGroup.addAction(ui->actionAddCircleShape);
	m_AddShapeActionGroup.addAction(ui->actionAddPolygonShape);
	m_AddShapeActionGroup.addAction(ui->actionAddSegmentShape);
	m_AddShapeActionGroup.addAction(ui->actionAddLineChainShape);
	m_AddShapeActionGroup.addAction(ui->actionAddCapsuleShape);
	ui->btnAddShapeBox->setDefaultAction(ui->actionAddBoxShape);
	ui->btnAddShapeCircle->setDefaultAction(ui->actionAddCircleShape);
	ui->btnAddShapePolygon->setDefaultAction(ui->actionAddPolygonShape);
	ui->btnAddShapeSegment->setDefaultAction(ui->actionAddSegmentShape);
	ui->btnAddShapeChain->setDefaultAction(ui->actionAddLineChainShape);
	ui->btnAddShapeCapsule->setDefaultAction(ui->actionAddCapsuleShape);

	ui->btnAddChild->setDefaultAction(ui->actionAddChildren);

	ui->btnOrderUp->setDefaultAction(ui->actionOrderChildrenUp);
	ui->btnOrderDown->setDefaultAction(ui->actionOrderChildrenDown);
	ui->btnReplace->setDefaultAction(ui->actionReplaceItems);
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

	m_pPreviewUpdateTimer = new QTimer(this);
	connect(m_pPreviewUpdateTimer, SIGNAL(timeout()), this, SLOT(OnPreviewUpdate()));
	m_pPreviewUpdateTimer->setInterval(1000 / 60);
	m_iPreviewStartingFrame = 0;

	new QShortcut(QKeySequence(Qt::Key_Space), this, SLOT(OnKeySpace()));
	new QShortcut(QKeySequence(Qt::Key_Q), this, SLOT(OnKeyQ()));
	new QShortcut(QKeySequence(Qt::Key_E), this, SLOT(OnKeyE()));
	new QShortcut(QKeySequence(Qt::SHIFT + Qt::Key_Q), this, SLOT(OnKeyShiftQ()));
	new QShortcut(QKeySequence(Qt::SHIFT + Qt::Key_E), this, SLOT(OnKeyShiftE()));
	new QShortcut(QKeySequence(Qt::Key_F), this, SLOT(OnKeyF()));

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

	SetAddStateBtnEnabled(false);

	// Root and BvFolder should always be expanded
	ui->nodeTree->expand(pTreeModel->FindIndex<EntityTreeItemData *>(pTreeModel->GetRootTreeItemData(), 0));
	ui->nodeTree->expand(pTreeModel->FindIndex<EntityTreeItemData *>(pTreeModel->GetBvFolderTreeItemData(), 0));

	// Query what items are selected in the Explorer Widget
	QList<ProjectItemData *> selectedItems; QList<ExplorerItemData *> selectedPrefixes;
	MainWindow::GetExplorerWidget().GetSelected(selectedItems, selectedPrefixes, true);
	bool bEnableAddNodeBtn = false;
	
	ItemType eSelectedSingleItemType = ITEM_Unknown;
	int iNumValidItems = 0;
	for(auto pItem : selectedItems)
	{
		if(pTreeModel->IsItemValid(pItem, false))
		{
			if(eSelectedSingleItemType != ITEM_None)
			{
				if(eSelectedSingleItemType == ITEM_Unknown)
					eSelectedSingleItemType = pItem->GetType();
				else if(eSelectedSingleItemType != pItem->GetType())
					eSelectedSingleItemType = ITEM_None;
			}

			iNumValidItems++;
			bEnableAddNodeBtn = true;
		}
	}
	ui->actionAddChildren->setEnabled(bEnableAddNodeBtn);
	if(iNumValidItems != 1)
		eSelectedSingleItemType = ITEM_None;

	// Manage currently selected items in the item tree
	bool bEnableVemMode = false;
	QModelIndexList selectedIndices = GetSelectedItems();

	if(eSelectedSingleItemType != ITEM_None && eSelectedSingleItemType != ITEM_Unknown)
	{
		for(const QModelIndex &index : selectedIndices)
		{
			EntityTreeItemData *pCurItemData = ui->nodeTree->model()->data(index, Qt::UserRole).value<EntityTreeItemData *>();
			if(eSelectedSingleItemType != pCurItemData->GetType())
			{
				eSelectedSingleItemType = ITEM_None;
				break;
			}
		}
	}
	ui->actionReplaceItems->setEnabled(selectedIndices.empty() == false && eSelectedSingleItemType != ITEM_None && eSelectedSingleItemType != ITEM_Unknown);

	m_ContextMenu.clear();

	if(selectedIndices.empty())
	{
		bEnableVemMode = false;

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

		//// No items selected, so no properties to show
		//ui->propertyTree->setModel(nullptr);
		//ui->lblSelectedItemIcon->setVisible(false);
		//ui->lblSelectedItemText->setVisible(true);
		//ui->lblSelectedItemText->setText("No items selected");
	}
	else
	{
		EntityTreeItemData *pFirstItemData = ui->nodeTree->model()->data(selectedIndices[0], Qt::UserRole).value<EntityTreeItemData *>();
		bEnableVemMode = selectedIndices.size() == 1 && (pFirstItemData->GetType() == ITEM_Primitive || pFirstItemData->GetType() == ITEM_FixtureShape || pFirstItemData->GetType() == ITEM_FixtureChain || pFirstItemData->GetType() == ITEM_Text);

		bool bRootOrBvFolder = false;
		bool bSelectedHaveSameParent = true;
		bool bAllSameType = true;
		bool bAllArrayItems = true;

		QModelIndex parentIndex = selectedIndices.at(0).parent();
		ItemType eType = pFirstItemData->GetType();
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

		if(bRootOrBvFolder == false && selectedIndices.size() == 1 && (eType == ITEM_Primitive || eType == ITEM_FixtureShape)) // NOTE: Chain is supported here
		{
			ui->actionConvertShape->setEnabled(true);
			if(eType == ITEM_Primitive)
			{


				ui->actionConvertShape->setIcon(HyGlobal::ItemIcon(ITEM_FixtureShape, SUBICON_None));
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
		m_ContextMenu.addAction(ui->actionReplaceItems);
		m_ContextMenu.addAction(ui->actionRemoveItems);
	}

	SetExtrapolatedProperties();

	if(bEnableVemMode)
		ui->actionVertexEditMode->setEnabled(true);
	else
	{
		ui->actionVertexEditMode->setChecked(false);
		ui->actionVertexEditMode->setEnabled(false);
	}

	ui->nodeTree->model()->dataChanged(ui->nodeTree->model()->index(0, 0), ui->nodeTree->model()->index(ui->nodeTree->model()->rowCount() - 1, 1));
	ui->nodeTree->repaint();
}

/*virtual*/ void EntityWidget::OnFocusState(int iStateIndex, QVariant subState) /*override*/
{
	// Note: this causes ApplyJsonData() to be invoked twice when UndoCmd's are executed, but it's needed here for when the user switches between states
	EntityDraw *pEntDraw = static_cast<EntityDraw *>(m_ItemRef.GetDraw());
	if(pEntDraw)
		pEntDraw->ApplyJsonData();

	AuxDopeSheet *pAuxDopeSheet = static_cast<AuxDopeSheet *>(MainWindow::GetAuxWidget(AUXTAB_DopeSheet));
	EntityStateData *pEntStateData = static_cast<EntityStateData *>(m_ItemRef.GetModel()->GetStateData(iStateIndex));
	pAuxDopeSheet->SetEntityStateModel(pEntStateData);
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

void EntityWidget::RequestSelectedItemChange(EntityTreeItemData *pTreeItemData, QItemSelectionModel::SelectionFlags flags)
{
	EntityTreeModel &entityTreeModelRef = static_cast<EntityModel *>(m_ItemRef.GetModel())->GetTreeModel();

	QModelIndex index = entityTreeModelRef.FindIndex<EntityTreeItemData *>(pTreeItemData, EntityTreeModel::COLUMN_CodeName);
	ui->nodeTree->selectionModel()->select(index, flags);
	
	index = index.sibling(index.row(), EntityTreeModel::COLUMN_ItemPath); // Using 'index', get the next column over
	flags = QItemSelectionModel::Select;
	ui->nodeTree->selectionModel()->select(index, flags);
}

void EntityWidget::SetExtrapolatedProperties()
{
	if(m_pPreviewUpdateTimer->isActive())
	{
		ui->propertyTree->setModel(nullptr);
		ui->lblSelectedItemIcon->setVisible(false);
		ui->lblSelectedItemText->setVisible(true);
		ui->lblSelectedItemText->setText("Preview Active");
		return;
	}

	// Get selected items only, removing any folders
	QList<EntityTreeItemData *> selectedItemsDataList;
	QModelIndexList selectedIndexList = GetSelectedItems();
	for(const QModelIndex &index : selectedIndexList)
	{
		EntityTreeItemData *pEntItemData = ui->nodeTree->model()->data(index, Qt::UserRole).value<EntityTreeItemData *>();
		if(pEntItemData->GetEntType() == ENTTYPE_Root || pEntItemData->GetEntType() == ENTTYPE_Item || pEntItemData->GetEntType() == ENTTYPE_ArrayItem)
			selectedItemsDataList.push_back(pEntItemData);
	}

	// Set selected items' properties using the extrapolated values from the dope sheet
	if(selectedItemsDataList.empty())
	{
		// No items selected, so no properties to show
		ui->propertyTree->setModel(nullptr);
		ui->lblSelectedItemIcon->setVisible(false);
		ui->lblSelectedItemText->setVisible(true);
		ui->lblSelectedItemText->setText("No items selected");
	}
	else if(selectedItemsDataList.size() == 1)
	{
		PropertiesTreeModel &propModelRef = selectedItemsDataList[0]->GetPropertiesModel();
		EntityDopeSheetScene &entityDopeSheetSceneRef = static_cast<EntityStateData *>(m_ItemRef.GetModel()->GetStateData(GetCurStateIndex()))->GetDopeSheetScene();
		QJsonObject propsObj = entityDopeSheetSceneRef.GetCurrentFrameProperties(selectedItemsDataList[0]);
		propModelRef.ResetValues();
		propModelRef.DeserializeJson(propsObj);
		ui->propertyTree->setModel(&propModelRef);

		ui->lblSelectedItemIcon->setVisible(true);
		ui->lblSelectedItemIcon->setPixmap(selectedItemsDataList[0]->GetIcon(SUBICON_Settings).pixmap(QSize(16, 16)));
		ui->lblSelectedItemText->setVisible(true);

		QString sProp;
		if(entityDopeSheetSceneRef.IsCtor())
			sProp = "Constructor";
		else
			sProp = "Frame " % QString::number(entityDopeSheetSceneRef.GetCurrentFrame());
		ui->lblSelectedItemText->setText(selectedItemsDataList[0]->GetCodeName() % " Properties [" % sProp % "]");
	}
	else // Multiple selection
	{
		EntityDopeSheetScene &entityDopeSheetSceneRef = static_cast<EntityStateData *>(m_ItemRef.GetModel()->GetStateData(GetCurStateIndex()))->GetDopeSheetScene();

		QList<PropertiesTreeModel *> multiModelList;
		for(EntityTreeItemData *pEntItemData : selectedItemsDataList)
		{
			PropertiesTreeModel &propModelRef = pEntItemData->GetPropertiesModel();
			QJsonObject propsObj = entityDopeSheetSceneRef.GetCurrentFrameProperties(pEntItemData);
			propModelRef.ResetValues();
			propModelRef.DeserializeJson(propsObj);

			multiModelList.push_back(&propModelRef);
		}

		ui->propertyTree->setModel(nullptr);
		delete m_pMultiPropModel;
		m_pMultiPropModel = new EntityPropertiesTreeMultiModel(m_ItemRef, -1, 0, multiModelList, this);
		ui->propertyTree->setModel(m_pMultiPropModel);

		ui->lblSelectedItemIcon->setVisible(false);
		ui->lblSelectedItemText->setVisible(true);

		QString sProp;
		if(entityDopeSheetSceneRef.IsCtor())
			sProp = "Constructor";
		else
			sProp = "Frame " % QString::number(entityDopeSheetSceneRef.GetCurrentFrame());
		ui->lblSelectedItemText->setText("Multiple items selected [" % sProp % "]");
	}
	
	ui->propertyTree->resizeColumnToContents(0);
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

	case SHAPE_LineSegment:
		if(bAsPrimitive)
			ui->btnAddPrimitiveSegment->setChecked(true);
		else
			ui->btnAddShapeSegment->setChecked(true);
		break;

	case SHAPE_Polygon:
		if(bAsPrimitive)
			ui->btnAddPrimitivePolygon->setChecked(true);
		else
			ui->btnAddShapePolygon->setChecked(true);
		break;

	case SHAPE_Capsule:
		if(bAsPrimitive)
			ui->btnAddPrimitiveCapsule->setChecked(true);
		else
			ui->btnAddShapeCapsule->setChecked(true);
		break;

	case SHAPE_LineChain:
		if(bAsPrimitive)
			ui->btnAddPrimitiveChain->setChecked(true);
		else
			ui->btnAddShapeChain->setChecked(true);
		break;
	}
}

void EntityWidget::SetAsShapeEditMode(bool bEnableSem)
{
	ui->chkShapeEditMode->setChecked(bEnableSem);
	ui->nodeTree->update();

	if(bEnableSem == false)
		UncheckAll();

	// Check if any selected item is still able to be selected
	QModelIndexList selectedIndexes = GetSelectedItems();
	QList<QUuid> uuidList;
	for(const QModelIndex &index : selectedIndexes)
	{
		EntityTreeItemData *pEntItemData = ui->nodeTree->model()->data(index, Qt::UserRole).value<EntityTreeItemData *>();
		if(pEntItemData->IsSelected())
			uuidList.append(pEntItemData->GetUuid());
	}

	RequestSelectedItems(uuidList);
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

void EntityWidget::StopPreview()
{
	EntityDraw *pEntDraw = static_cast<EntityDraw *>(m_ItemRef.GetDraw());
	if(pEntDraw == nullptr)
		return;

	pEntDraw->ClearBackgroundAction();
	m_pPreviewUpdateTimer->stop();

	// Restore selected items
	RequestSelectedItems(m_PreviewSelectedItemsList);
	m_PreviewSelectedItemsList.clear();
}

void EntityWidget::OnKeySpace()
{
	EntityDraw *pEntDraw = static_cast<EntityDraw *>(m_ItemRef.GetDraw());
	if(pEntDraw == nullptr || pEntDraw->IsActionTransforming())
	{
		HyGuiLog((pEntDraw == nullptr) ? "EntityDraw is nullptr" : "Finish transforming before previewing", LOGTYPE_Normal);
		return;
	}

	if(pEntDraw->GetCurAction() == HYACTION_Previewing)
		StopPreview();
	else if(pEntDraw->SetBackgroundAction(HYACTION_Previewing))
	{
		// Starts Preview:

		// Store currently selected items to be restored after preview
		m_PreviewSelectedItemsList.clear();
		QModelIndexList selectedIndexList = GetSelectedItems();
		for(const QModelIndex &index : selectedIndexList)
		{
			EntityTreeItemData *pEntItemData = ui->nodeTree->model()->data(index, Qt::UserRole).value<EntityTreeItemData *>();
			m_PreviewSelectedItemsList.push_back(pEntItemData->GetThisUuid());
		}

		// Clear selection and start preview update timer
		RequestSelectedItems(QList<QUuid>());
		m_pPreviewUpdateTimer->start();
		m_PreviewElapsedTimer.start();
		m_iPreviewStartingFrame = static_cast<EntityStateData *>(m_ItemRef.GetModel()->GetStateData(GetCurStateIndex()))->GetDopeSheetScene().GetCurrentFrame();
	}
}

void EntityWidget::OnKeyQ()
{
	EntityDopeSheetScene &entityDopeSheetSceneRef = static_cast<EntityStateData *>(m_ItemRef.GetModel()->GetStateData(GetCurStateIndex()))->GetDopeSheetScene();
	entityDopeSheetSceneRef.SetCurrentFrame(0);

	AuxDopeSheet *pAuxDopeSheet = static_cast<AuxDopeSheet *>(MainWindow::GetAuxWidget(AUXTAB_DopeSheet));
	pAuxDopeSheet->EnsureSelectedFrameVisible();
}

void EntityWidget::OnKeyE()
{
	EntityDopeSheetScene &entityDopeSheetSceneRef = static_cast<EntityStateData *>(m_ItemRef.GetModel()->GetStateData(GetCurStateIndex()))->GetDopeSheetScene();
	entityDopeSheetSceneRef.SetCurrentFrame(entityDopeSheetSceneRef.GetFinalFrame());

	AuxDopeSheet *pAuxDopeSheet = static_cast<AuxDopeSheet *>(MainWindow::GetAuxWidget(AUXTAB_DopeSheet));
	pAuxDopeSheet->EnsureSelectedFrameVisible();
}

void EntityWidget::OnKeyShiftQ()
{
	EntityDopeSheetScene &entityDopeSheetSceneRef = static_cast<EntityStateData *>(m_ItemRef.GetModel()->GetStateData(GetCurStateIndex()))->GetDopeSheetScene();
	entityDopeSheetSceneRef.SetCurrentFrame(entityDopeSheetSceneRef.GetCurrentFrame() - 1);

	AuxDopeSheet *pAuxDopeSheet = static_cast<AuxDopeSheet *>(MainWindow::GetAuxWidget(AUXTAB_DopeSheet));
	pAuxDopeSheet->EnsureSelectedFrameVisible();
}

void EntityWidget::OnKeyShiftE()
{
	EntityDopeSheetScene &entityDopeSheetSceneRef = static_cast<EntityStateData *>(m_ItemRef.GetModel()->GetStateData(GetCurStateIndex()))->GetDopeSheetScene();
	entityDopeSheetSceneRef.SetCurrentFrame(entityDopeSheetSceneRef.GetCurrentFrame() + 1);

	AuxDopeSheet *pAuxDopeSheet = static_cast<AuxDopeSheet *>(MainWindow::GetAuxWidget(AUXTAB_DopeSheet));
	pAuxDopeSheet->EnsureSelectedFrameVisible();
}

void EntityWidget::OnKeyF()
{
	static_cast<EntityModel *>(m_ItemRef.GetModel())->ToggleShapeEditMode();
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

		if(pCurItemData->GetEntType() == ENTTYPE_ArrayItem)
		{
			QModelIndex parentIndex = ui->nodeTree->model()->parent(selIndex);
			ui->nodeTree->expand(parentIndex);
		}
		
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
	MainWindow::GetExplorerWidget().GetSelected(selectedItems, selectedPrefixes, true);
	if(selectedItems.empty())
	{
		HyGuiLog("Currently selected item(s) in Explorer is/are not a ProjectItemData. Cannot add to entity.", LOGTYPE_Error);
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

void EntityWidget::on_actionAddLabel_triggered()
{
	QUndoCommand *pCmd = new EntityUndoCmd_AddWidget(m_ItemRef, ITEM_UiLabel);
	m_ItemRef.GetUndoStack()->push(pCmd);
}

void EntityWidget::on_actionAddRichLabel_triggered()
{
	QUndoCommand *pCmd = new EntityUndoCmd_AddWidget(m_ItemRef, ITEM_UiRichLabel);
	m_ItemRef.GetUndoStack()->push(pCmd);
}

void EntityWidget::on_actionAddButton_triggered()
{
	QUndoCommand *pCmd = new EntityUndoCmd_AddWidget(m_ItemRef, ITEM_UiButton);
	m_ItemRef.GetUndoStack()->push(pCmd);
}

void EntityWidget::on_actionAddRackMeter_triggered()
{
	QUndoCommand *pCmd = new EntityUndoCmd_AddWidget(m_ItemRef, ITEM_UiRackMeter);
	m_ItemRef.GetUndoStack()->push(pCmd);
}

void EntityWidget::on_actionAddBarMeter_triggered()
{
	QUndoCommand *pCmd = new EntityUndoCmd_AddWidget(m_ItemRef, ITEM_UiBarMeter);
	m_ItemRef.GetUndoStack()->push(pCmd);
}

void EntityWidget::on_actionAddCheckBox_triggered()
{
	QUndoCommand *pCmd = new EntityUndoCmd_AddWidget(m_ItemRef, ITEM_UiCheckBox);
	m_ItemRef.GetUndoStack()->push(pCmd);
}

void EntityWidget::on_actionAddRadioButton_triggered()
{
	QUndoCommand *pCmd = new EntityUndoCmd_AddWidget(m_ItemRef, ITEM_UiRadioButton);
	m_ItemRef.GetUndoStack()->push(pCmd);
}

void EntityWidget::on_actionAddTextField_triggered()
{
	QUndoCommand *pCmd = new EntityUndoCmd_AddWidget(m_ItemRef, ITEM_UiTextField);
	m_ItemRef.GetUndoStack()->push(pCmd);
}

void EntityWidget::on_actionAddComboBox_triggered()
{
	QUndoCommand *pCmd = new EntityUndoCmd_AddWidget(m_ItemRef, ITEM_UiComboBox);
	m_ItemRef.GetUndoStack()->push(pCmd);
}

void EntityWidget::on_actionAddSlider_triggered()
{
	QUndoCommand *pCmd = new EntityUndoCmd_AddWidget(m_ItemRef, ITEM_UiSlider);
	m_ItemRef.GetUndoStack()->push(pCmd);
}

void EntityWidget::on_actionAddBoxPrimitive_triggered()
{
	static_cast<EntityModel *>(m_ItemRef.GetModel())->ToggleShapeAdd(SHAPE_Box, true);
}

void EntityWidget::on_actionAddCirclePrimitive_triggered()
{
	static_cast<EntityModel *>(m_ItemRef.GetModel())->ToggleShapeAdd(SHAPE_Circle, true);
}

void EntityWidget::on_actionAddPolygonPrimitive_triggered()
{
	static_cast<EntityModel *>(m_ItemRef.GetModel())->ToggleShapeAdd(SHAPE_Polygon, true);
}

void EntityWidget::on_actionAddSegmentPrimitive_triggered()
{
	static_cast<EntityModel *>(m_ItemRef.GetModel())->ToggleShapeAdd(SHAPE_LineSegment, true);
}

void EntityWidget::on_actionAddLineChainPrimitive_triggered()
{
	static_cast<EntityModel *>(m_ItemRef.GetModel())->ToggleShapeAdd(SHAPE_LineChain, true);
}

void EntityWidget::on_actionAddCapsulePrimitive_triggered()
{
	static_cast<EntityModel *>(m_ItemRef.GetModel())->ToggleShapeAdd(SHAPE_Capsule, true);
}

void EntityWidget::on_actionAddBoxShape_triggered()
{
	static_cast<EntityModel *>(m_ItemRef.GetModel())->ToggleShapeAdd(SHAPE_Box, false);
}

void EntityWidget::on_actionAddCircleShape_triggered()
{
	static_cast<EntityModel *>(m_ItemRef.GetModel())->ToggleShapeAdd(SHAPE_Circle, false);
}

void EntityWidget::on_actionAddPolygonShape_triggered()
{
	static_cast<EntityModel *>(m_ItemRef.GetModel())->ToggleShapeAdd(SHAPE_Polygon, false);
}

void EntityWidget::on_actionAddSegmentShape_triggered()
{
	static_cast<EntityModel *>(m_ItemRef.GetModel())->ToggleShapeAdd(SHAPE_LineSegment, false);
}

void EntityWidget::on_actionAddLineChainShape_triggered()
{
	static_cast<EntityModel *>(m_ItemRef.GetModel())->ToggleShapeAdd(SHAPE_LineChain, false);
}

void EntityWidget::on_actionAddCapsuleShape_triggered()
{
	static_cast<EntityModel *>(m_ItemRef.GetModel())->ToggleShapeAdd(SHAPE_Capsule, false);
}

void EntityWidget::on_actionVertexEditMode_toggled(bool bChecked)
{
	static_cast<EntityModel *>(m_ItemRef.GetModel())->SetShapeEditMode(bChecked);
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

void EntityWidget::on_actionReplaceItems_triggered()
{
	EntityTreeModel *pTreeModel = static_cast<EntityTreeModel *>(ui->nodeTree->model());

	QList<ProjectItemData *> selectedExplorerItems; QList<ExplorerItemData *> selectedExplorerPrefixes;
	MainWindow::GetExplorerWidget().GetSelected(selectedExplorerItems, selectedExplorerPrefixes, true);

	ProjectItemData *pReplaceItem = nullptr;
	ItemType eSelectedSingleItemType = ITEM_Unknown;
	int iNumValidItems = 0;
	for(auto pItem : selectedExplorerItems)
	{
		if(pTreeModel->IsItemValid(pItem, false))
		{
			if(eSelectedSingleItemType != ITEM_None)
			{
				if(eSelectedSingleItemType == ITEM_Unknown)
					eSelectedSingleItemType = pItem->GetType();
				else if(eSelectedSingleItemType != pItem->GetType())
					eSelectedSingleItemType = ITEM_None;
			}

			pReplaceItem = pItem;
			iNumValidItems++;
		}
	}
	if(iNumValidItems != 1 || pReplaceItem == nullptr)
		eSelectedSingleItemType = ITEM_None;
	if(eSelectedSingleItemType == ITEM_None || eSelectedSingleItemType == ITEM_Unknown)
	{
		HyGuiLog("EntityWidget::on_actionReplaceItems_triggered() - Only one valid item can be selected to replace the current selection.", LOGTYPE_Error);
		return;
	}

	QList<EntityTreeItemData *> replaceItemList;
	QModelIndexList selectedIndices = GetSelectedItems();
	for(const QModelIndex &index : selectedIndices)
	{
		EntityTreeItemData *pCurItemData = ui->nodeTree->model()->data(index, Qt::UserRole).value<EntityTreeItemData *>();
		if(pCurItemData->GetEntType() == ENTTYPE_Item || pCurItemData->GetEntType() == ENTTYPE_ArrayItem)
		{
			if(eSelectedSingleItemType != pCurItemData->GetType())
			{
				HyGuiLog("EntityWidget::on_actionReplaceItems_triggered() - All selected items must be of the same type to replace them.", LOGTYPE_Error);
				return;
			}

			replaceItemList.push_back(pCurItemData);
		}
	}

	QUndoCommand *pCmd = new EntityUndoCmd_ReplaceItems(*pReplaceItem, replaceItemList);
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
	if(pCurItemData->GetType() != ITEM_Primitive && pCurItemData->GetType() != ITEM_FixtureShape) // NOTE: Chain fixture not supported here
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
		HyGlobal::CodeNameValidator(),
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

	// Determine if the pasted item can merge its key frames data into the current entity's states
	QJsonArray itemArray = pastedObject["itemArray"].toArray();
	for(int i = 0; i < itemArray.size(); ++i)
	{
		QJsonObject itemObj = itemArray[i].toObject();
		QJsonArray stateKeyFramesArray = itemObj["stateKeyFramesArray"].toArray();
		
		bool bIsStatesMatch = true;
		if(m_ItemRef.GetModel()->GetNumStates() != stateKeyFramesArray.size())
			bIsStatesMatch = false;
		for(int iStateIndex = 0; iStateIndex < m_ItemRef.GetModel()->GetNumStates(); ++iStateIndex)
		{
			EntityStateData *pStateData = static_cast<EntityStateData *>(m_ItemRef.GetModel()->GetStateData(iStateIndex));
			if(pStateData->GetName() != stateKeyFramesArray[iStateIndex].toObject()["name"].toString())
				bIsStatesMatch = false;
			else if(static_cast<EntityModel *>(m_ItemRef.GetModel())->GetFramesPerSecond() != stateKeyFramesArray[iStateIndex].toObject()["framesPerSecond"].toInt())
				bIsStatesMatch = false;
		}
		if(bIsStatesMatch == false)
		{
			QJsonObject descObj = itemObj["descObj"].toObject();
			HyGuiLog("Pasted item (" % descObj["codeName"].toString() % ") " % descObj["itemType"].toString() % " has mismatching state data from the current entity. Aborting paste\n\nTODO: Impelement state merging/quantizing", LOGTYPE_Warning);
			return;
		}
	}

	EntityUndoCmd_PasteItems *pCmd = new EntityUndoCmd_PasteItems(m_ItemRef, pastedObject, pArrayFolder);
	m_ItemRef.GetUndoStack()->push(pCmd);
}

void EntityWidget::on_chkSetConstructor_clicked()
{
	static_cast<EntityModel *>(m_ItemRef.GetModel())->SetCtor(ui->chkSetConstructor->isChecked());
}

void EntityWidget::OnPreviewUpdate()
{
	EntityDopeSheetScene &entityDopeSheetSceneRef = static_cast<EntityStateData *>(m_ItemRef.GetModel()->GetStateData(GetCurStateIndex()))->GetDopeSheetScene();

	int iCurFrame = entityDopeSheetSceneRef.GetCurrentFrame();
	if(iCurFrame >= entityDopeSheetSceneRef.GetFinalFrame())
	{
		StopPreview();
		return;
	}

	// Using the elapsed time and 'm_iPreviewStartingFrame', set the current frame
	int iFPS = static_cast<EntityModel *>(m_ItemRef.GetModel())->GetFramesPerSecond();
	qint64 iElapsedMs = m_PreviewElapsedTimer.elapsed();

	int iCurrentFrame = m_iPreviewStartingFrame + (iElapsedMs * iFPS) / 1000;
	if(iCurFrame != iCurrentFrame)
	{
		entityDopeSheetSceneRef.SetCurrentFrame(iCurrentFrame);

		// Have the graphics view scroll while previewing
		AuxDopeSheet *pAuxDopeSheet = static_cast<AuxDopeSheet *>(MainWindow::GetAuxWidget(AUXTAB_DopeSheet));
		pAuxDopeSheet->EnsureSelectedFrameVisible();
	}
}
