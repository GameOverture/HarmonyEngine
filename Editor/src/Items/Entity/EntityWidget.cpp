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
	m_pMultiPropModel(nullptr)
{
	ui->setupUi(this);

	// Remove and re-add the main layout that holds everything. This makes the Qt Designer (.ui) files work with the base class 'IWidget'. Otherwise it jumbles them together.
	layout()->removeItem(ui->lytAboveStates);
	layout()->removeItem(ui->lytBelowStates);
	GetBelowStatesLayout()->addItem(ui->lytBelowStates);
	GetAboveStatesLayout()->addItem(ui->lytAboveStates);

	//GetAboveStatesLayout()->addWidget(ui->grpNodes);

	for(int i = 0; i < NUM_ENTBASECLASSTYPES; ++i)
		ui->cmbBaseClass->addItem(ENTITYBASECLASSTYPE_STRINGS[i], i);
	ui->cmbBaseClass->setCurrentIndex(static_cast<EntityModel *>(m_ItemRef.GetModel())->GetBaseClassType());

	ui->btnAddChild->setDefaultAction(ui->actionAddChildren);
	
	ui->btnAddPrimitive->setDefaultAction(ui->actionAddPrimitive);
	ui->btnAddPrimBox->setDefaultAction(ui->actionAddPrimBox);
	ui->btnAddPrimCircle->setDefaultAction(ui->actionAddPrimCircle);
	ui->btnAddPrimLineSegment->setDefaultAction(ui->actionAddPrimLineSegment);
	ui->btnAddPrimPolygon->setDefaultAction(ui->actionAddPrimPolygon);
	ui->btnAddPrimCapsule->setDefaultAction(ui->actionAddPrimCapsule);
	ui->btnAddPrimLineChain->setDefaultAction(ui->actionAddPrimLineChain);

	ui->btnAddFixtureShape->setDefaultAction(ui->actionAddShape);
	ui->btnAddFixtureChain->setDefaultAction(ui->actionAddChain);

	ui->btnAddLayoutHorz->setDefaultAction(ui->actionAddLayoutHorz);
	ui->btnAddLayoutVert->setDefaultAction(ui->actionAddLayoutVert);
	ui->btnAddSpacer->setDefaultAction(ui->actionAddSpacer);
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
	QList<EntityTreeItemData *> childList, shapeList, layoutList;
	pEntityModel->GetTreeModel().GetTreeItemData(childList, shapeList, layoutList);
	childList += shapeList;
	childList += layoutList;
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
	new QShortcut(QKeySequence(Qt::SHIFT | Qt::Key_Q), this, SLOT(OnKeyShiftQ()));
	new QShortcut(QKeySequence(Qt::SHIFT | Qt::Key_E), this, SLOT(OnKeyShiftE()));
	new QShortcut(QKeySequence(Qt::Key_F), this, SLOT(OnKeyF()));

	UpdateActions();
}

EntityWidget::~EntityWidget()
{
	delete ui;
}

/*virtual*/ void EntityWidget::OnGiveMenuActions(QMenu *pMenu) /*override*/
{
	pMenu->addAction(ui->actionEditMode);
}

/*virtual*/ void EntityWidget::OnUpdateActions() /*override*/
{
	ui->cmbBaseClass->setCurrentIndex(static_cast<EntityModel *>(m_ItemRef.GetModel())->GetBaseClassType());

	EntityTreeModel *pTreeModel = static_cast<EntityTreeModel *>(ui->nodeTree->model());

	SetAddStateBtnEnabled(false);

	// Root and BvFolder should always be expanded
	ui->nodeTree->expand(pTreeModel->FindIndex<EntityTreeItemData *>(pTreeModel->GetRootTreeItemData(), 0));
	ui->nodeTree->expand(pTreeModel->FindIndex<EntityTreeItemData *>(pTreeModel->GetFixtureFolderTreeItemData(), 0));

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
	bool bAllowEditMode = false;
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
		bAllowEditMode = false;

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

		//// No items selected, so no properties to show
		//ui->propertyTree->setModel(nullptr);
		//ui->lblSelectedItemIcon->setVisible(false);
		//ui->lblSelectedItemText->setVisible(true);
		//ui->lblSelectedItemText->setText("No items selected");
	}
	else
	{
		EntityTreeItemData *pFirstItemData = ui->nodeTree->model()->data(selectedIndices[0], Qt::UserRole).value<EntityTreeItemData *>();
		
		bAllowEditMode = selectedIndices.size() == 1 && pFirstItemData->IsEditable();

		bool bRootOrBvFolder = false;
		bool bSelectedHaveSameParent = true;
		bool bAllSameType = true;
		bool bAllArrayItems = true;
		bool bFusedItemSelected = false;

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
			if(pEntItemData->GetEntType() == ENTTYPE_Root || pEntItemData->GetEntType() == ENTTYPE_FixtureFolder)
				bRootOrBvFolder = true;
			if(pEntItemData->GetEntType() == ENTTYPE_FusedItem)
				bFusedItemSelected = true;
		}

		ui->actionOrderChildrenUp->setEnabled(bSelectedHaveSameParent && bRootOrBvFolder == false && bFusedItemSelected == false);
		ui->actionOrderChildrenDown->setEnabled(bSelectedHaveSameParent && bRootOrBvFolder == false && bFusedItemSelected == false);

		bool bPrimNodeSelected = (selectedIndices.size() == 1 && eType == ITEM_PrimNode);
		ui->actionAddPrimBox->setEnabled(bPrimNodeSelected);
		ui->actionAddPrimCircle->setEnabled(bPrimNodeSelected);
		ui->actionAddPrimLineSegment->setEnabled(bPrimNodeSelected);
		ui->actionAddPrimPolygon->setEnabled(bPrimNodeSelected);
		ui->actionAddPrimCapsule->setEnabled(bPrimNodeSelected);
		ui->actionAddPrimLineChain->setEnabled(bPrimNodeSelected);

		bool bGuiLayoutSelected = (selectedIndices.size() == 1 && eType == ITEM_UiLayout);
		ui->actionAddLayoutHorz->setEnabled(bGuiLayoutSelected);
		ui->actionAddLayoutVert->setEnabled(bGuiLayoutSelected);
		ui->actionAddSpacer->setEnabled(bGuiLayoutSelected);

		if(bRootOrBvFolder == false && selectedIndices.size() == 1 && (eType == ITEM_PrimLayer || HyGlobal::IsItemType_Fixture(eType))) // NOTE: Chain is supported here
		{
			ui->actionConvertShape->setEnabled(true);
			if(eType == ITEM_PrimLayer)
			{
				ui->actionConvertShape->setIcon(HyGlobal::ItemIcon(ITEM_ShapeFixture, SUBICON_None));
				ui->actionConvertShape->setText("Convert Primitive to Fixture");
			}
			else
			{
				ui->actionConvertShape->setIcon(HyGlobal::ItemIcon(ITEM_PrimNode, SUBICON_None));
				ui->actionConvertShape->setText("Convert Fixture to Primitive");
			}
		}
		else
		{
			ui->actionConvertShape->setEnabled(false);
			ui->actionConvertShape->setText("Convert -");
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

	if(bAllowEditMode)
	{
		ui->chkEditMode->setEnabled(true);
		ui->actionEditMode->setEnabled(true);
	}
	else
	{
		ui->actionEditMode->setEnabled(false);
		ui->actionEditMode->setChecked(false);

		ui->chkEditMode->setEnabled(false);
		ui->chkEditMode->setChecked(false);
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

QModelIndexList EntityWidget::GetSelectedItems() const
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

QUuid EntityWidget::FindLayoutItemFromSelected() const
{
	if(static_cast<EntityModel *>(m_ItemRef.GetModel())->GetBaseClassType() != ENTBASECLASS_HyGui)
		return QUuid();

	QModelIndexList selectedIndices = GetSelectedItems();
	if(selectedIndices.isEmpty())
		return QUuid();

	EntityTreeItemData *pCurItemData = ui->nodeTree->model()->data(selectedIndices[0], Qt::UserRole).value<EntityTreeItemData *>();
	return static_cast<EntityModel *>(m_ItemRef.GetModel())->GetTreeModel().FindGuiLayoutUuid(pCurItemData);
}

void EntityWidget::ExpandAllGuiLayouts()
{
	if(static_cast<EntityModel *>(m_ItemRef.GetModel())->GetBaseClassType() != ENTBASECLASS_HyGui)
		return;

	EntityTreeModel &entityTreeModelRef = static_cast<EntityModel *>(m_ItemRef.GetModel())->GetTreeModel();
	QModelIndexList indexList = entityTreeModelRef.GetAllIndices();
	for(const QModelIndex &index : indexList)
	{
		EntityTreeItemData *pCurItemData = ui->nodeTree->model()->data(index, Qt::UserRole).value<EntityTreeItemData *>();
		if(pCurItemData->GetType() == ITEM_UiLayout)
			ui->nodeTree->expand(index);
	}
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

	EntityDopeSheetScene &entityDopeSheetSceneRef = static_cast<EntityStateData *>(m_ItemRef.GetModel()->GetStateData(GetCurStateIndex()))->GetDopeSheetScene();
	ui->chkSetConstructor->setChecked(entityDopeSheetSceneRef.GetCurrentFrame() == -1);

	// Get selected items only, removing any folders
	QList<EntityTreeItemData *> selectedItemsDataList;
	QModelIndexList selectedIndexList = GetSelectedItems();
	for(const QModelIndex &index : selectedIndexList)
	{
		EntityTreeItemData *pEntItemData = ui->nodeTree->model()->data(index, Qt::UserRole).value<EntityTreeItemData *>();
		if(pEntItemData->GetEntType() == ENTTYPE_Root || pEntItemData->GetEntType() == ENTTYPE_FusedItem || pEntItemData->GetEntType() == ENTTYPE_Item || pEntItemData->GetEntType() == ENTTYPE_ArrayItem)
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
		QJsonObject propsObj = entityDopeSheetSceneRef.GetCurrentFrameProperties(selectedItemsDataList[0]);
		propModelRef.ResetValues();
		propModelRef.DeserializeJson(propsObj);
		ui->propertyTree->setModel(&propModelRef);
		
		// Hide/Show the root-specific properties based on which base class is chosen
		if(selectedItemsDataList[0]->GetEntType() == ENTTYPE_Root)
		{
			EntityBaseClassType eCurBaseClassType = static_cast<EntityModel *>(m_ItemRef.GetModel())->GetBaseClassType();
			for(int iBaseClassType = 0; iBaseClassType < NUM_ENTBASECLASSTYPES; ++iBaseClassType)
			{
				int iBaseCategoryIndex = static_cast<PropertiesTreeModel *>(ui->propertyTree->model())->FindCategoryIndex(ENTITYBASECLASSCATEGORY_STRINGS[iBaseClassType]);
				if(iBaseCategoryIndex >= 0)
					ui->propertyTree->setRowHidden(iBaseCategoryIndex, QModelIndex(), eCurBaseClassType != iBaseClassType);
			}
		}

		ui->lblSelectedItemIcon->setVisible(true);
		ui->lblSelectedItemIcon->setPixmap(selectedItemsDataList[0]->GetIcon(SUBICON_Settings).pixmap(QSize(16, 16)));
		ui->lblSelectedItemText->setVisible(true);

		QString sProp;
		if(entityDopeSheetSceneRef.GetCurrentFrame() == -1)
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
		if(entityDopeSheetSceneRef.GetCurrentFrame() == -1)
			sProp = "Constructor";
		else
			sProp = "Frame " % QString::number(entityDopeSheetSceneRef.GetCurrentFrame());
		ui->lblSelectedItemText->setText("Multiple items selected [" % sProp % "]");
	}
	
	ui->propertyTree->resizeColumnToContents(0);
}

bool EntityWidget::IsEditMode() const
{
	return ui->actionEditMode->isChecked();
}

void EntityWidget::SetEditMode(EntityTreeItemData *pItemToEdit)
{
	if(pItemToEdit)
	{
		if(IsEditMode())
		{

		}

		RequestSelectedItems(QList<QUuid>() << pItemToEdit->GetThisUuid());

		// Update EntityDraw with latest selection via ApplyJsonData()
		EntityDraw *pEntityDraw = static_cast<EntityDraw *>(m_ItemRef.GetDraw());
		if(pEntityDraw == nullptr)
		{
			HyGuiLog("EntityWidget::SetEditMode() - pEntityDraw is nullptr", LOGTYPE_Error);
			ui->actionEditMode->setChecked(false);
			return;
		}
		pEntityDraw->ApplyJsonData();

		ui->actionEditMode->setChecked(true);
	}
	else
		ui->actionEditMode->setChecked(false);
}

/*virtual*/ void EntityWidget::showEvent(QShowEvent *pEvent) /*override*/
{
	resizeEvent(nullptr);
	ExpandAllGuiLayouts();
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
	if(ui->actionEditMode->isEnabled())
		ui->actionEditMode->toggle();
}

void EntityWidget::on_cmbBaseClass_activated(int iIndex)
{
	if(static_cast<EntityModel *>(GetItem().GetModel())->GetBaseClassType() == static_cast<EntityBaseClassType>(iIndex))
		return;

	EntityUndoCmd_BaseClass *pCmd = new EntityUndoCmd_BaseClass(GetItem(), static_cast<EntityBaseClassType>(iIndex));
	m_ItemRef.GetUndoStack()->push(pCmd);
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

	// Prevent Root or FixtureFolder from collapsing
	if(pTreeItemData->GetEntType() == ENTTYPE_Root || pTreeItemData->GetEntType() == ENTTYPE_FixtureFolder)
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

void EntityWidget::on_actionAddPrimitive_triggered()
{
	QUndoCommand *pCmd = new EntityUndoCmd_AddPrimitive(m_ItemRef);
	m_ItemRef.GetUndoStack()->push(pCmd);
}

void EntityWidget::on_actionAddPrimBox_triggered()
{
}

void EntityWidget::on_actionAddPrimCircle_triggered()
{
}

void EntityWidget::on_actionAddPrimLineSegment_triggered()
{
}

void EntityWidget::on_actionAddPrimPolygon_triggered()
{
}

void EntityWidget::on_actionAddPrimCapsule_triggered()
{
}

void EntityWidget::on_actionAddPrimLineChain_triggered()
{
}

void EntityWidget::on_actionAddShape_triggered()
{
	QUndoCommand *pCmd = new EntityUndoCmd_AddFixture(m_ItemRef, true);
	m_ItemRef.GetUndoStack()->push(pCmd);
}

void EntityWidget::on_actionAddChain_triggered()
{
	QUndoCommand *pCmd = new EntityUndoCmd_AddFixture(m_ItemRef, false);
	m_ItemRef.GetUndoStack()->push(pCmd);
}

void EntityWidget::on_actionAddLayoutHorz_triggered()
{
	QUuid uuidLayoutParent = FindLayoutItemFromSelected();
	QUndoCommand *pCmd = new EntityUndoCmd_AddGuiItem(m_ItemRef, ITEM_UiLayout, uuidLayoutParent);
	m_ItemRef.GetUndoStack()->push(pCmd);
}

void EntityWidget::on_actionAddLayoutVert_triggered()
{
	QUuid uuidLayoutParent = FindLayoutItemFromSelected();
	EntityUndoCmd_AddGuiItem *pCmd = new EntityUndoCmd_AddGuiItem(m_ItemRef, ITEM_UiLayout, uuidLayoutParent);
	m_ItemRef.GetUndoStack()->push(pCmd);

	static_cast<EntityStateData *>(m_ItemRef.GetModel()->GetStateData(GetCurStateIndex()))->GetDopeSheetScene().SetKeyFrameProperty(pCmd->GetGuiTreeItemData(), -1, "Layout", "Orientation", HyGlobal::OrientationName(HYORIENT_Vertical), true);
}

void EntityWidget::on_actionAddSpacer_triggered()
{
	QUuid uuidLayoutParent = FindLayoutItemFromSelected();
	QUndoCommand *pCmd = new EntityUndoCmd_AddGuiItem(m_ItemRef, ITEM_UiSpacer, uuidLayoutParent);
	m_ItemRef.GetUndoStack()->push(pCmd);
}

void EntityWidget::on_actionAddLabel_triggered()
{
	QUuid uuidLayoutParent = FindLayoutItemFromSelected();
	QUndoCommand *pCmd = new EntityUndoCmd_AddGuiItem(m_ItemRef, ITEM_UiLabel, uuidLayoutParent);
	m_ItemRef.GetUndoStack()->push(pCmd);
}

void EntityWidget::on_actionAddRichLabel_triggered()
{
	QUuid uuidLayoutParent = FindLayoutItemFromSelected();
	QUndoCommand *pCmd = new EntityUndoCmd_AddGuiItem(m_ItemRef, ITEM_UiRichLabel, uuidLayoutParent);
	m_ItemRef.GetUndoStack()->push(pCmd);
}

void EntityWidget::on_actionAddButton_triggered()
{
	QUuid uuidLayoutParent = FindLayoutItemFromSelected();
	QUndoCommand *pCmd = new EntityUndoCmd_AddGuiItem(m_ItemRef, ITEM_UiButton, uuidLayoutParent);
	m_ItemRef.GetUndoStack()->push(pCmd);
}

void EntityWidget::on_actionAddRackMeter_triggered()
{
	QUuid uuidLayoutParent = FindLayoutItemFromSelected();
	QUndoCommand *pCmd = new EntityUndoCmd_AddGuiItem(m_ItemRef, ITEM_UiRackMeter, uuidLayoutParent);
	m_ItemRef.GetUndoStack()->push(pCmd);
}

void EntityWidget::on_actionAddBarMeter_triggered()
{
	QUuid uuidLayoutParent = FindLayoutItemFromSelected();
	QUndoCommand *pCmd = new EntityUndoCmd_AddGuiItem(m_ItemRef, ITEM_UiBarMeter, uuidLayoutParent);
	m_ItemRef.GetUndoStack()->push(pCmd);
}

void EntityWidget::on_actionAddCheckBox_triggered()
{
	QUuid uuidLayoutParent = FindLayoutItemFromSelected();
	QUndoCommand *pCmd = new EntityUndoCmd_AddGuiItem(m_ItemRef, ITEM_UiCheckBox, uuidLayoutParent);
	m_ItemRef.GetUndoStack()->push(pCmd);
}

void EntityWidget::on_actionAddRadioButton_triggered()
{
	QUuid uuidLayoutParent = FindLayoutItemFromSelected();
	QUndoCommand *pCmd = new EntityUndoCmd_AddGuiItem(m_ItemRef, ITEM_UiRadioButton, uuidLayoutParent);
	m_ItemRef.GetUndoStack()->push(pCmd);
}

void EntityWidget::on_actionAddTextField_triggered()
{
	QUuid uuidLayoutParent = FindLayoutItemFromSelected();
	QUndoCommand *pCmd = new EntityUndoCmd_AddGuiItem(m_ItemRef, ITEM_UiTextField, uuidLayoutParent);
	m_ItemRef.GetUndoStack()->push(pCmd);
}

void EntityWidget::on_actionAddComboBox_triggered()
{
	QUuid uuidLayoutParent = FindLayoutItemFromSelected();
	QUndoCommand *pCmd = new EntityUndoCmd_AddGuiItem(m_ItemRef, ITEM_UiComboBox, uuidLayoutParent);
	m_ItemRef.GetUndoStack()->push(pCmd);
}

void EntityWidget::on_actionAddSlider_triggered()
{
	QUuid uuidLayoutParent = FindLayoutItemFromSelected();
	QUndoCommand *pCmd = new EntityUndoCmd_AddGuiItem(m_ItemRef, ITEM_UiSlider, uuidLayoutParent);
	m_ItemRef.GetUndoStack()->push(pCmd);
}

void EntityWidget::on_actionOrderChildrenUp_triggered()
{
	QModelIndexList selectedIndexList = GetSelectedItems();
	if(selectedIndexList.empty())
		return;

	// NOTE: All selected items will have the same parent and no root/fusedItem/fixtureFolder is selected.
	//       ALSO if the selected items are GUI items, it is ensured that they don't span past an unselected layout sibling
	std::sort(selectedIndexList.begin(), selectedIndexList.end(),
		[](const QModelIndex &a, const QModelIndex &b)
		{
			return a.row() < b.row();
		});

	QModelIndex parentIndex = static_cast<EntityTreeModel *>(ui->nodeTree->model())->parent(selectedIndexList[0]);
	QModelIndex grandParentIndex = static_cast<EntityTreeModel *>(ui->nodeTree->model())->parent(parentIndex);
	EntityTreeItemData *pCurParentItem = ui->nodeTree->model()->data(parentIndex, Qt::UserRole).value<EntityTreeItemData *>();
	EntityTreeItemData *pCurGrandParentItem = ui->nodeTree->model()->data(grandParentIndex, Qt::UserRole).value<EntityTreeItemData *>();
	if(pCurParentItem == nullptr || (pCurParentItem->GetEntType() != ENTTYPE_Root && pCurGrandParentItem == nullptr))
	{
		HyGuiLog("EntityWidget - Parent item is nullptr. Cannot reorder children.", LOGTYPE_Error);
		return;
	}

	EntityTreeItemData *pParentSwapDestination = nullptr;	// If 'bParentSwapAllowed' allowed (down below), this is what the new parent should be
	int iParentSwapDestinationIndex = -1;					// If 'bParentSwapAllowed' allowed (down below), this is the index/row under the new parent that the items should be moved to
	int iTOP_INDEX = 0;										// The top index indicates where the uppermost index items are allowed to be moved up to. If 'bParentSwapAllowed', going past this index will move the items to 'pParentSwapDestination', otherwise they will clump at the top and be blocked from moving up any further.

	if(pCurParentItem->GetEntType() == ENTTYPE_Root)
	{
		EntityTreeItemData *pFusedItem = static_cast<EntityModel *>(m_ItemRef.GetModel())->GetFusedItem();
		if(pFusedItem)
		{
			pParentSwapDestination = pFusedItem;

			QModelIndex destinationParentIndex = static_cast<EntityTreeModel *>(ui->nodeTree->model())->FindIndex<EntityTreeItemData *>(pParentSwapDestination, 0);
			iParentSwapDestinationIndex = ui->nodeTree->model()->rowCount(destinationParentIndex);

			if(pFusedItem->IsFixtureItem() == false)
				iTOP_INDEX = 1;
		}
	}
	else if(pCurParentItem->GetEntType() == ENTTYPE_FixtureFolder)
	{
		EntityTreeItemData *pFusedItem = static_cast<EntityModel *>(m_ItemRef.GetModel())->GetFusedItem();
		if(pFusedItem && pFusedItem->IsFixtureItem())
			iTOP_INDEX = 1;
	}
	else if(pCurParentItem->GetType() == ITEM_UiLayout) // If the selected items are within the GUI Layout heirarchy
	{
		// Find the next sibling ITEM_UiLayout item above the selected items, and set the top index to be that sibling if it exists.
		QAbstractItemModel *pModel = ui->nodeTree->model();
		int numSiblings = pModel->rowCount(parentIndex);
		for(int row = selectedIndexList[0].row() - 1; row >= 0; --row)
		{
			QModelIndex siblingIndex = pModel->index(row, 0, parentIndex);
			EntityTreeItemData *pSiblingItemData = ui->nodeTree->model()->data(siblingIndex, Qt::UserRole).value<EntityTreeItemData *>();
			if(pSiblingItemData && pSiblingItemData->GetType() == ITEM_UiLayout)
			{
				pParentSwapDestination = pSiblingItemData;

				QModelIndex destinationParentIndex = static_cast<EntityTreeModel *>(ui->nodeTree->model())->FindIndex<EntityTreeItemData *>(pParentSwapDestination, 0);
				iParentSwapDestinationIndex = ui->nodeTree->model()->rowCount(destinationParentIndex);
					
				iTOP_INDEX = row + 1;
				break;
			}
		}
		if(pParentSwapDestination == nullptr) // Indicates that there are no sibling layouts above the selected items, so the items can be moved up to be above the parent layout (becoming siblings with the parent)
		{
			if(pCurGrandParentItem && pCurGrandParentItem->GetEntType() != ENTTYPE_Root)
			{
				pParentSwapDestination = pCurGrandParentItem;
				iParentSwapDestinationIndex = parentIndex.row();
			}
		}
	}

	int iDiscardTopIndices = -1;
	bool bAllGuiItems = true;
	QList<EntityTreeItemData *> selectedItemDataList;
	QList<int> curIndexList;
	QList<int> newIndexList;
	for(const QModelIndex &index : selectedIndexList)
	{
		if(iDiscardTopIndices == -1 && index.row() == iTOP_INDEX)
			iDiscardTopIndices = iTOP_INDEX;
		else if((iDiscardTopIndices + 1) == index.row())
			iDiscardTopIndices = index.row();

		EntityTreeItemData *pEntTreeItemData = ui->nodeTree->model()->data(index, Qt::UserRole).value<EntityTreeItemData *>();
		if(pEntTreeItemData == nullptr)
		{
			HyGuiLog("EntityWidget OrderUp - One of the selected items is nullptr. Cannot reorder children.", LOGTYPE_Error);
			return;
		}
		selectedItemDataList.push_back(pEntTreeItemData);
		if(pEntTreeItemData->IsWidgetItem() == false && pEntTreeItemData->IsLayoutItem() == false && pEntTreeItemData->GetEntType() != ENTTYPE_ArrayFolder)
			bAllGuiItems = false;

		int iRow = index.row();
		curIndexList.push_back(iRow);
		newIndexList.push_back(iRow - 1);
	}

	bool bParentSwapAllowed = static_cast<EntityModel *>(m_ItemRef.GetModel())->GetBaseClassType() == ENTBASECLASS_HyGui &&
							  bAllGuiItems &&
							  pCurParentItem->GetEntType() != ENTTYPE_ArrayFolder &&
							  pParentSwapDestination != nullptr &&
							  (pCurParentItem->GetEntType() == ENTTYPE_Root || pCurParentItem->GetType() == ITEM_UiLayout);

	EntityTreeItemData *pDestinationParent = pCurParentItem;
	if(iDiscardTopIndices >= iTOP_INDEX)
	{
		if(bParentSwapAllowed && pParentSwapDestination)
		{
			pDestinationParent = pParentSwapDestination;
		
			// Update the indices of the items being moved to be their new indices under the new parent
			for(int i = 0; i < newIndexList.size(); ++i)
				newIndexList[i] = iParentSwapDestinationIndex + i;
		}
		else // Clump at the top and block from moving up any further
		{
			while(iDiscardTopIndices >= iTOP_INDEX)
			{
				selectedItemDataList.takeFirst();
				curIndexList.takeFirst();
				newIndexList.takeFirst();

				iDiscardTopIndices--;
			}
		}
	}

	if(selectedItemDataList.empty() == false)
	{
		QUndoCommand *pCmd = new EntityUndoCmd_MoveChildren(m_ItemRef, selectedItemDataList, pCurParentItem, curIndexList, pDestinationParent, newIndexList, true);
		m_ItemRef.GetUndoStack()->push(pCmd);
	}
}

void EntityWidget::on_actionOrderChildrenDown_triggered()
{
	QModelIndexList selectedIndexList = GetSelectedItems();
	if(selectedIndexList.empty())
		return;

	// NOTE: All selected items will have the same parent and no root/fusedItem/fixtureFolder is selected.
	//       ALSO if the selected items are GUI items, it is ensured that they don't span past an unselected layout sibling
	std::sort(selectedIndexList.begin(), selectedIndexList.end(),
		[](const QModelIndex &a, const QModelIndex &b)
		{
			return a.row() > b.row();
		});

	QModelIndex parentIndex = static_cast<EntityTreeModel *>(ui->nodeTree->model())->parent(selectedIndexList[0]);
	QModelIndex grandParentIndex = static_cast<EntityTreeModel *>(ui->nodeTree->model())->parent(parentIndex);
	EntityTreeItemData *pCurParentItem = ui->nodeTree->model()->data(parentIndex, Qt::UserRole).value<EntityTreeItemData *>();
	EntityTreeItemData *pCurGrandParentItem = ui->nodeTree->model()->data(grandParentIndex, Qt::UserRole).value<EntityTreeItemData *>();
	if(pCurParentItem == nullptr || (pCurParentItem->GetEntType() != ENTTYPE_Root && pCurGrandParentItem == nullptr))
	{
		HyGuiLog("EntityWidget - Parent item is nullptr. Cannot reorder children.", LOGTYPE_Error);
		return;
	}

	EntityTreeItemData *pParentSwapDestination = nullptr;				// If 'bParentSwapAllowed' allowed (down below), this is what the new parent should be
	int iParentSwapDestinationIndex = -1;								// If 'bParentSwapAllowed' allowed (down below), this is the index/row under the new parent that the items should be moved to
	int iBOT_INDEX = ui->nodeTree->model()->rowCount(parentIndex) - 1;	// The bot index indicates where the lowermost index items are allowed to be moved down to. If 'bParentSwapAllowed', going past this index will move the items to 'pParentSwapDestination', otherwise they will clump at the bottom and be blocked from moving down any further.

	if(pCurParentItem->GetType() == ITEM_UiLayout) // If the selected items are within the GUI Layout heirarchy
	{
		// Find the next sibling ITEM_UiLayout item below the selected items, and set the bot index to be that sibling if it exists.
		QAbstractItemModel *pModel = ui->nodeTree->model();
		int numSiblings = ui->nodeTree->model()->rowCount(parentIndex);
		for(int row = selectedIndexList[0].row() + 1; row < numSiblings; ++row)
		{
			QModelIndex siblingIndex = pModel->index(row, 0, parentIndex);
			EntityTreeItemData *pSiblingItemData = ui->nodeTree->model()->data(siblingIndex, Qt::UserRole).value<EntityTreeItemData *>();
			if(pSiblingItemData && pSiblingItemData->GetType() == ITEM_UiLayout)
			{
				pParentSwapDestination = pSiblingItemData;

				QModelIndex destinationParentIndex = static_cast<EntityTreeModel *>(ui->nodeTree->model())->FindIndex<EntityTreeItemData *>(pParentSwapDestination, 0);
				iParentSwapDestinationIndex = 0;
					
				iBOT_INDEX = row - 1;
				break;
			}
		}
		if(pParentSwapDestination == nullptr) // Indicates that there are no sibling layouts below the selected items, so the items can be moved down to be below the parent layout (becoming siblings with the parent). Works going from root layout -> root entity too.
		{
			pParentSwapDestination = pCurGrandParentItem;
			iParentSwapDestinationIndex = parentIndex.row() + 1;
		}
	}

	int iDiscardBotIndices = -1;
	bool bAllGuiItems = true;
	QList<EntityTreeItemData *> selectedItemDataList;
	QList<int> curIndexList;
	QList<int> newIndexList;
	for(const QModelIndex &index : selectedIndexList)
	{
		if(iDiscardBotIndices == -1 && index.row() == iBOT_INDEX)
			iDiscardBotIndices = iBOT_INDEX;
		else if((iDiscardBotIndices - 1) == index.row())
			iDiscardBotIndices = index.row();

		EntityTreeItemData *pEntTreeItemData = ui->nodeTree->model()->data(index, Qt::UserRole).value<EntityTreeItemData *>();
		if(pEntTreeItemData == nullptr)
		{
			HyGuiLog("EntityWidget OrderDown - One of the selected items is nullptr. Cannot reorder children.", LOGTYPE_Error);
			return;
		}
		selectedItemDataList.push_back(pEntTreeItemData);
		if(pEntTreeItemData->IsWidgetItem() == false && pEntTreeItemData->IsLayoutItem() == false && pEntTreeItemData->GetEntType() != ENTTYPE_ArrayFolder)
			bAllGuiItems = false;

		int iRow = index.row();
		curIndexList.push_back(iRow);
		newIndexList.push_back(iRow + 1);
	}

	bool bParentSwapAllowed = static_cast<EntityModel *>(m_ItemRef.GetModel())->GetBaseClassType() == ENTBASECLASS_HyGui &&
							  bAllGuiItems &&
							  pCurParentItem->GetEntType() != ENTTYPE_ArrayFolder &&
							  pParentSwapDestination != nullptr &&
							  pCurParentItem->GetType() == ITEM_UiLayout;

	EntityTreeItemData *pDestinationParent = pCurParentItem;
	if(iDiscardBotIndices >= 0 && iDiscardBotIndices <= iBOT_INDEX)
	{
		if(bParentSwapAllowed && pParentSwapDestination)
		{
			pDestinationParent = pParentSwapDestination;
		
			// Update the indices of the items being moved to be their new indices under the new parent
			for(int i = 0; i < newIndexList.size(); ++i)
				newIndexList[i] = iParentSwapDestinationIndex;
		}
		else // Clump at the bottom and block from moving down any further
		{
			while(iDiscardBotIndices >= 0 && iDiscardBotIndices <= iBOT_INDEX)
			{
				selectedItemDataList.takeFirst();
				curIndexList.takeFirst();
				newIndexList.takeFirst();

				iDiscardBotIndices++;
			}
		}
	}

	if(selectedItemDataList.empty() == false)
	{
		QUndoCommand *pCmd = new EntityUndoCmd_MoveChildren(m_ItemRef, selectedItemDataList, pCurParentItem, curIndexList, pDestinationParent, newIndexList, false);
		m_ItemRef.GetUndoStack()->push(pCmd);
	}
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
	//QModelIndexList selectedIndexList = GetSelectedItems();

	//if(selectedIndexList.size() != 1)
	//{
	//	HyGuiLog("EntityWidget::on_actionConvertShape_triggered was invoked with improper selection size", LOGTYPE_Error);
	//	return;
	//}
	//EntityTreeItemData *pCurItemData = ui->nodeTree->model()->data(selectedIndexList[0], Qt::UserRole).value<EntityTreeItemData *>();
	//if(pCurItemData->GetType() != ITEM_Primitive && pCurItemData->GetType() != ITEM_FixtureShape) // NOTE: Chain fixture not supported here
	//{
	//	HyGuiLog("EntityWidget::on_actionConvertShape_triggered was invoked with improper selection type", LOGTYPE_Error);
	//	return;
	//}

	//QUndoCommand *pCmd = new EntityUndoCmd_ConvertShape(m_ItemRef, pCurItemData);
	//m_ItemRef.GetUndoStack()->push(pCmd);
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
	DlgInputNumber dlg(sDlgTitle, "Size", HyGlobal::ItemIcon(pEntTreeItemData->GetType(), SUBICON_New), 1, 1, 0x0FFFFFFF, nullptr);
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

void EntityWidget::on_actionEditMode_toggled(bool bChecked)
{
	EntityDraw *pEntityDraw = static_cast<EntityDraw *>(m_ItemRef.GetDraw());
	if(pEntityDraw == nullptr)
	{
		HyGuiLog("EntityModel::SetShapeEditMode() - pEntityDraw is nullptr", LOGTYPE_Error);
		return;
	}

	ui->chkEditMode->setChecked(bChecked);

	if(bChecked == false)
	{
		pEntityDraw->OnSetEditMode(false);
		return;
	}

	QModelIndexList selectedIndices = GetSelectedItems();
	if(selectedIndices.size() != 1)
	{
		HyGuiLog("Cannot enter Edit Mode with multiple or no items selected.", LOGTYPE_Error);
		ui->actionEditMode->setChecked(false);
		ui->chkEditMode->setChecked(false);
		return;
	}

	EntityTreeItemData *pFirstItemData = ui->nodeTree->model()->data(selectedIndices[0], Qt::UserRole).value<EntityTreeItemData *>();
	if(pFirstItemData->IsEditable() == false)
	{
		HyGuiLog("Selected item is not editable. Cannot enter Edit Mode.", LOGTYPE_Error);
		ui->actionEditMode->setChecked(false);
		ui->chkEditMode->setChecked(false);
		return;
	}
	
	pEntityDraw->OnSetEditMode(true);

	UpdateActions();
	pEntityDraw->ApplyJsonData();
}

void EntityWidget::on_chkEditMode_clicked()
{
	on_actionEditMode_toggled(ui->chkEditMode->isChecked());
}

void EntityWidget::on_chkSetConstructor_clicked()
{
	if(ui->chkSetConstructor->isChecked())
	{
		m_iPreviewStartingFrame = static_cast<EntityStateData *>(m_ItemRef.GetModel()->GetStateData(GetCurStateIndex()))->GetDopeSheetScene().GetCurrentFrame();
		if(m_iPreviewStartingFrame == -1)
			m_iPreviewStartingFrame = 0;

		static_cast<EntityStateData *>(m_ItemRef.GetModel()->GetStateData(GetCurStateIndex()))->GetDopeSheetScene().SetCurrentFrame(-1);
	}
	else
		static_cast<EntityStateData *>(m_ItemRef.GetModel()->GetStateData(GetCurStateIndex()))->GetDopeSheetScene().SetCurrentFrame(m_iPreviewStartingFrame);
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
