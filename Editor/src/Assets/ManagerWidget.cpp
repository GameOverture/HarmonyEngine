/**************************************************************************
*	ManagerWidget.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2020 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Global.h"
#include "ManagerWidget.h"
#include "ui_ManagerWidget.h"

#include "ProjectItemData.h"
#include "IManagerModel.h"
#include "GlobalUndoCmds.h"
#include "DlgInputName.h"
#include "SpriteModels.h"
#include "TextModel.h"
#include "EntityModel.h"
#include "PrefabModel.h"
#include "MainWindow.h"
#include "AuxAssetInspector.h"
#include "AuxTileSet.h"
#include "DlgAssetProperties.h"
#include "DlgSyncAssets.h"
#include "SourceModel.h"
#include "SourceFile.h"
#include "AssetMimeData.h"
#include "DlgNewBuild.h"
#include "DlgPackageBuild.h"

#include <QUndoCommand>
#include <QMessageBox>
#include <QFileDialog>
#include <QDrag>
#include <QMouseEvent>
#include <QDesktopServices>
#include <QProcess>

ManagerProxyModel::ManagerProxyModel(QObject *pParent /*= nullptr*/) :
	QSortFilterProxyModel(pParent),
	m_iFilterBankIndex(-1)
{
	setFilterCaseSensitivity(Qt::CaseInsensitive);
}

void ManagerProxyModel::FilterByBankIndex(int iBankIndex)
{
	m_iFilterBankIndex = iBankIndex;
	invalidateFilter();
}

/*virtual*/ bool ManagerProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const /*override*/
{
	TreeModelItemData *pLeftItem = sourceModel()->data(left, Qt::UserRole).value<TreeModelItemData *>();
	TreeModelItemData *pRightItem = sourceModel()->data(right, Qt::UserRole).value<TreeModelItemData *>();

	if((pLeftItem == nullptr && pRightItem == nullptr) || (pLeftItem == nullptr && pRightItem != nullptr))
		return false;
	if(pLeftItem != nullptr && pRightItem == nullptr)
		return true;

	if(pLeftItem->GetType() == ITEM_Filter && pRightItem->GetType() != ITEM_Filter)
		return true;
	if(pLeftItem->GetType() != ITEM_Filter && pRightItem->GetType() == ITEM_Filter)
		return false;

	if(static_cast<IManagerModel *>(sourceModel())->GetAssetType() != ASSETMAN_Source)
	{
		if(pLeftItem->GetType() != pRightItem->GetType())
			return pLeftItem->GetType() < pRightItem->GetType();
	}

	return QString::localeAwareCompare(pLeftItem->GetText(), pRightItem->GetText()) < 0;
}

/*virtual*/ bool ManagerProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const /*override*/
{
	QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);
	TreeModelItemData *pItemData = sourceModel()->data(index, Qt::UserRole).value<TreeModelItemData *>();
	
	if(pItemData)
	{
		//// Don't display anything that starts with HyGuiInternalCharIndicator
		//if(pItemData->GetText().isEmpty() == false && pItemData->GetText()[0] == HyGuiInternalCharIndicator)
		//	return false;

		QRegularExpression searchFilter = filterRegularExpression();

		if(pItemData->GetType() == ITEM_Filter)
		{
			QList<TreeModelItemData *> itemsInFilterList = static_cast<IManagerModel *>(sourceModel())->GetItemsRecursively(index);
			if(itemsInFilterList.size() == 1)
				return true; // Always show empty filters

			for(auto pItem : itemsInFilterList)
			{
				if(pItem->GetType() != ITEM_Filter)
				{
					if(m_iFilterBankIndex < 0 || m_iFilterBankIndex == static_cast<IManagerModel *>(sourceModel())->GetBankIndexFromBankId(static_cast<IAssetItemData *>(pItem)->GetBankId()))
					{
						if(searchFilter.isValid() == false || pItem->GetText().contains(searchFilter))
							return true;
					}
				}
			}
			return false;
		}
		else if(searchFilter.isValid() && pItemData->GetText().contains(searchFilter) == false)
			return false;
		else
		{
			IAssetItemData *pAssetItemData = static_cast<IAssetItemData *>(pItemData);
			if(m_iFilterBankIndex < 0 || m_iFilterBankIndex == static_cast<IManagerModel *>(sourceModel())->GetBankIndexFromBankId(pAssetItemData->GetBankId()))
				return true;
		}
	}

	return false;
}

ManagerWidget::CodeWidgets::CodeWidgets(ManagerWidget *pManagerWidget, Ui::ManagerWidget *pUi) :
	m_pGrp(nullptr),
	m_pSettingsBtn(nullptr),
	m_pNewBtn(nullptr),
	m_pCmb(nullptr),
	m_pOpenBtn(nullptr),
	m_pDeleteBtn(nullptr)
{
	// Setup Group Box build member widgets
	m_pGrp = new QGroupBox("Build", pManagerWidget);
	m_pGrp->setLayout(new QVBoxLayout());
	// Set top and bottom margins to 2 pixels
	m_pGrp->layout()->setContentsMargins(2, 2, 2, 4);
	m_pGrp->layout()->setSpacing(4);

	QHBoxLayout *pTopRow = new QHBoxLayout();
	pTopRow->setSpacing(2);
	m_pGrp->layout()->addItem(pTopRow);
	//
	m_pSettingsBtn = new QToolButton(pManagerWidget);
	m_pSettingsBtn->setDefaultAction(pUi->actionBuildSettings);
	pTopRow->addWidget(m_pSettingsBtn);
	//
	m_pDividerLine = new QFrame(pManagerWidget);
	m_pDividerLine->setFrameShape(QFrame::VLine);
	m_pDividerLine->setFrameShadow(QFrame::Sunken);
	pTopRow->addWidget(m_pDividerLine);
	//
	m_pNewBtn = new QToolButton(pManagerWidget);
	m_pNewBtn->setDefaultAction(pUi->actionNewBuild);
	pTopRow->addWidget(m_pNewBtn);
	//
	m_pCmb = new QComboBox(pManagerWidget);
	pTopRow->addWidget(m_pCmb);
	//
	m_pOpenBtn = new QToolButton(pManagerWidget);
	m_pOpenBtn->setDefaultAction(pUi->actionOpenBuild);
	pTopRow->addWidget(m_pOpenBtn);
	//
	m_pPackageBtn = new QToolButton(pManagerWidget);
	m_pPackageBtn->setDefaultAction(pUi->actionPackageBuild);
	pTopRow->addWidget(m_pPackageBtn);
	//
	m_pDeleteBtn = new QToolButton(pManagerWidget);
	m_pDeleteBtn->setDefaultAction(pUi->actionDeleteBuild);
	pTopRow->addWidget(m_pDeleteBtn);

	QHBoxLayout *pBotRow = new QHBoxLayout();
	m_pGrp->layout()->addItem(pBotRow);

	m_pLblInfo = new QLabel(pManagerWidget);
	m_pLblInfo->setText("No Builds Found");
	pBotRow->addWidget(m_pLblInfo);
	pBotRow->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));

	pUi->verticalLayout->insertWidget(0, m_pGrp);

	AssembleComboBox();
}

void ManagerWidget::CodeWidgets::AssembleComboBox()
{
	m_pCmb->clear();
	if(Harmony::GetProject() == nullptr)
		return;

	// Populate the build combo box
	QDir buildDir(Harmony::GetProject()->GetBuildAbsPath());
	QStringList buildDirList = buildDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

	for(auto sDirName : buildDirList)
	{
		QDir dir(buildDir.absolutePath() % "/" % sDirName);

		// Open sDirName.build to read build json settings
		QFile buildFile(dir.absolutePath() % "/" % sDirName % HYGUIPATH_BuildExt);
		if(buildFile.open(QIODevice::ReadOnly | QIODevice::Text))
		{
			QJsonDocument jsonDoc = QJsonDocument::fromJson(buildFile.readAll());
			if(jsonDoc.isObject())
			{
				// Read and process the build settings from jsonObj
				QJsonObject jsonObj = jsonDoc.object();
				m_pCmb->addItem(QIcon(":/icons16x16/Build-Desktop.png"), sDirName, QVariant(jsonObj));

				//switch(static_cast<BuildType>(jsonObj["type"].toInt()))
				//{
				//case BUILD_Desktop:
				//	m_pCmb->addItem(QIcon(":/icons16x16/Build-Desktop.png"), sDirName, QVariant(jsonObj));
				//	break;

				//case BUILD_Browser:
				//	m_pCmb->addItem(QIcon(":/icons16x16/Build-Browser.png"), sDirName, QVariant(jsonObj));
				//	break;

				//default:
				//	HyGuiLog("Unknown build type completed", LOGTYPE_Error);
				//	break;
				//}
			}
		}
		else
			HyGuiLog("Failed to open build file: " % buildFile.fileName(), LOGTYPE_Error);
	}

	m_pOpenBtn->defaultAction()->setEnabled(m_pCmb->count() > 0);
	m_pDeleteBtn->defaultAction()->setEnabled(m_pCmb->count() > 0);
}

void ManagerWidget::CodeWidgets::RefreshInfo()
{
	if(m_pCmb->currentIndex() < 0)
	{
		m_pLblInfo->setText("No Builds Found");
		return;
	}
	QJsonObject jsonObj = m_pCmb->currentData().toJsonObject();
	QFileInfo fileInfo(jsonObj["path"].toString());

	if(Harmony::GetProject())
	{
		QDir buildDir(Harmony::GetProject()->GetDirPath());// GetBuildAbsPath());
		m_pLblInfo->setText(buildDir.relativeFilePath(fileInfo.absoluteFilePath()));
	}
}

QString ManagerWidget::CodeWidgets::GetCurrentBuildUrl() const
{
	if(m_pCmb->currentIndex() < 0)
		return QString();
	QJsonObject jsonObj = m_pCmb->currentData().toJsonObject();
	return jsonObj["path"].toString();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ManagerTreeView::ManagerTreeView(QWidget *pParent /*= nullptr*/) :
	QTreeView(pParent)
{ }

/*virtual*/ void ManagerTreeView::mousePressEvent(QMouseEvent *event) /*override*/
{
	QModelIndex index = indexAt(event->pos());
	// Handle the mouse press event here
	// Note: the index may be invalid if the mouse was pressed in an empty area
	if(index.isValid() == false)
	{
		selectionModel()->clear();
		selectionModel()->clearSelection();
		selectionModel()->clearCurrentIndex();
	}
	else
		QTreeView::mousePressEvent(event);
}

/*virtual*/ void ManagerTreeView::startDrag(Qt::DropActions supportedActions) /*override*/
{
	QModelIndexList indexes = selectedIndexes();
	if(indexes.empty())
		return;

	QMimeData *pMimeData = model()->mimeData(indexes);
	if(pMimeData == nullptr)
		return;

	// Special case for Source Manager, close any source files that are open in any editor. Abort if any of them are dirty
	if(static_cast<ManagerWidget *>(parent())->GetModel().GetAssetType() == ASSETMAN_Source)
	{
		ManagerWidget *pManagerWidget = static_cast<ManagerWidget *>(parent());
		Project &projRef = pManagerWidget->GetModel().GetProjOwner();

		// Acquire a list of the SourceFiles being moved using the Project and mime data's assetUUIDs
		QList<SourceFile *> srcFileList;
		QJsonArray assetsArray = static_cast<const AssetMimeData *>(pMimeData)->GetAssetsArray(ASSETMAN_Source);
		for(int i = 0; i < assetsArray.size(); ++i)
		{
			QJsonObject assetObj = assetsArray[i].toObject();

			TreeModelItemData *pFoundAsset = projRef.FindItemData(QUuid(assetObj["assetUUID"].toString()));
			if(pFoundAsset)
				srcFileList.push_back(static_cast<SourceFile *>(pFoundAsset));
		}

		// Close any open editors that are editing the SourceFiles being moved
		for(SourceFile *pSrcFile : srcFileList)
		{
			if(pSrcFile->TryCloseAllCodeEditors() == false)
				return;
		}
	}


	QPixmap pixmap = indexes.first().data(Qt::DecorationRole).value<QPixmap>();
	QDrag *pDrag = new QDrag(this);
	pDrag->setPixmap(pixmap);
	pDrag->setMimeData(pMimeData);
	pDrag->setHotSpot(QPoint(pixmap.width()/2, pixmap.height()/2));

	Qt::DropAction eDropAction = pDrag->exec(supportedActions);
}

/*virtual*/ void ManagerTreeView::dropEvent(QDropEvent *pEvent) /*override*/
{
	if(static_cast<ManagerWidget *>(parent())->GetModel().GetAssetType() != ASSETMAN_Source)
		QTreeView::dropEvent(pEvent);
	else // Do a special case for Source Manager - move files to correct directory location
	{
		// Acquire the Project using this ManagerTreeView's parent (which is a ManagerWidget)
		ManagerWidget *pManagerWidget = static_cast<ManagerWidget *>(parent());
		Project &projRef = pManagerWidget->GetModel().GetProjOwner();

		// Acquire a list of the SourceFiles being moved using the Project and mime data's assetUUIDs
		QList<SourceFile *> srcFileList;
		const AssetMimeData *pMimeData = static_cast<const AssetMimeData *>(pEvent->mimeData());
		QJsonArray assetsArray = pMimeData->GetAssetsArray(ASSETMAN_Source);
		for(int i = 0; i < assetsArray.size(); ++i)
		{
			QJsonObject assetObj = assetsArray[i].toObject();

			TreeModelItemData *pFoundAsset = projRef.FindItemData(QUuid(assetObj["assetUUID"].toString()));
			if(pFoundAsset)
				srcFileList.push_back(static_cast<SourceFile *>(pFoundAsset));
		}

		// Store a list of the old paths (before the move)
		QStringList sOldPathList;
		for(SourceFile *pSrcFile : srcFileList)
			sOldPathList.push_back(pSrcFile->GetAbsMetaFilePath());

		// Move items to new filter location
		QTreeView::dropEvent(pEvent);

		// Store a list of the new paths (after the move)
		QStringList sNewPathList;
		for(SourceFile *pSrcFile : srcFileList)
			sNewPathList.push_back(pSrcFile->GetAbsMetaFilePath());
		
		// Finally, move the source files on disk
		for(int i = 0; i < sOldPathList.size(); ++i)
		{
			if(QFile::rename(sOldPathList[i], sNewPathList[i]) == false)
				HyGuiLog("ManagerTreeView::dropEvent() failed to rename file: " % sOldPathList[i] % " to " % sNewPathList[i], LOGTYPE_Error);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ManagerWidget::ManagerWidget(QWidget *pParent /*= nullptr*/) :
	QWidget(pParent),
	ui(new Ui::ManagerWidget),
	m_pModel(nullptr),
	m_pContextMenuSelection(nullptr),
	m_bUseContextMenuSelection(false),
	m_pCodeWidgets(nullptr)
{
	ui->setupUi(this);

	// NOTE: THIS CONSTRUCTOR IS INVALID TO USE. IT EXISTS FOR QT TO ALLOW Q_OBJECT TO WORK
	HyGuiLog("ManagerWidget::ManagerWidget() invalid constructor used", LOGTYPE_Error);
}

ManagerWidget::ManagerWidget(IManagerModel *pModel, QWidget *pParent /*= nullptr*/) :
	QWidget(pParent),
	ui(new Ui::ManagerWidget),
	m_pModel(pModel),
	m_pContextMenuSelection(nullptr),
	m_bUseContextMenuSelection(false),
	m_pCodeWidgets(nullptr)
{
	ui->setupUi(this);

	switch(m_pModel->GetAssetType())
	{
	case ASSETMAN_Source: {
		QToolButton *pBtnAddClassFiles = new QToolButton(this);
		pBtnAddClassFiles->setDefaultAction(ui->actionAddClassFiles);
		ui->lytBottomToolBar->insertWidget(0, pBtnAddClassFiles);
		break; }

	case ASSETMAN_Atlases: {
		QToolButton *pBtnCreateTileSet = new QToolButton(this);
		pBtnCreateTileSet->setDefaultAction(ui->actionCreateTileSet);
		ui->lytBottomToolBar->insertWidget(0, pBtnCreateTileSet);

		QToolButton *pBtnSliceSpriteSheet = new QToolButton(this);
		pBtnSliceSpriteSheet->setDefaultAction(ui->actionSliceSpriteSheet);
		ui->lytBottomToolBar->insertWidget(0, pBtnSliceSpriteSheet);
		break; }

	case ASSETMAN_Audio:
		break;

	default:
		HyGuiLog("ManagerWidget::ManagerWidget() unknown asset manager type", LOGTYPE_Error);
		break;
	}


	if(m_pModel->IsSingleBank())
	{
		ui->grpBank->hide();
		ui->chkShowAllBanks->hide();
	}

	ManagerProxyModel *pProxyModel = new ManagerProxyModel(this);
	pProxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
	pProxyModel->setSourceModel(m_pModel);

	ui->assetTree->setModel(pProxyModel);
	ui->assetTree->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	ui->assetTree->setDragEnabled(true);
	ui->assetTree->setDropIndicatorShown(true);
	ui->assetTree->setDragDropMode(QAbstractItemView::InternalMove);
	//ui->assetTree->SetAtlasOwner(this);
	//ui->assetTree->viewport()->setAcceptDrops(true);
	ui->assetTree->sortByColumn(0, Qt::AscendingOrder);
	ui->assetTree->setSortingEnabled(true);
	ui->assetTree->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(ui->assetTree, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(OnContextMenu(const QPoint&)));
	connect(ui->assetTree->selectionModel(), SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)), this, SLOT(OnAssetTreeSelectionChanged(const QModelIndex &, const QModelIndex &)));

	// Setup Actions
	ui->actionImportAssets->setIcon(HyGlobal::AssetIcon(m_pModel->GetAssetType(), SUBICON_None));
	ui->actionImportDirectory->setIcon(HyGlobal::AssetIcon(m_pModel->GetAssetType(), SUBICON_Open));
	//ui->actionDeleteAssets->setIcon(HyGlobal::AssetIcon(m_pModel->GetAssetType(), SUBICON_Delete)); // Uses standard delete icon
	ui->actionAssetSettings->setIcon(HyGlobal::AssetIcon(m_pModel->GetAssetType(), SUBICON_Settings));

	ui->actionDeleteAssets->setEnabled(false);
	ui->actionReplaceAssets->setEnabled(false);

	ui->btnAddBank->setDefaultAction(ui->actionAddBank);
	ui->btnRemoveBank->setDefaultAction(ui->actionRemoveBank);
	ui->btnOpenExplorer->setDefaultAction(ui->actionOpenBankExplorer);
	ui->btnBankSettings->setDefaultAction(ui->actionBankSettings);

	ui->btnCreateFilter->setDefaultAction(ui->actionAddFilter);
	ui->btnImportAssets->setDefaultAction(ui->actionImportAssets);
	ui->btnImportDir->setDefaultAction(ui->actionImportDirectory);
	//ui->btnSyncFilterAssets->setDefaultAction(ui->actionSyncFilterAssets);

	ui->btnDeleteAssets->setDefaultAction(ui->actionDeleteAssets);
	ui->btnReplaceAssets->setDefaultAction(ui->actionReplaceAssets);

	if(m_pModel->GetAssetType() == ASSETMAN_Source) // Code Assets
	{
		m_pCodeWidgets = new CodeWidgets(this, ui);
		QObject::connect(m_pCodeWidgets->m_pCmb, SIGNAL(currentIndexChanged(int)), this, SLOT(OnBuildIndex(int)));
		
		// Change text and tool tips of Source Manager to make more sense
		ui->actionImportAssets->setText("Add Existing File(s)");
		ui->actionImportAssets->setToolTip("Add existing file(s) to this project");
		ui->actionImportDirectory->setText("Add Existing Directory");
		ui->actionImportDirectory->setToolTip("Add existing directory of source and header files to this project");
		ui->actionAddFilter->setToolTip("Insert a new filter to organize files");
	}

	ui->cmbBanks->clear();
	ui->cmbBanks->setModel(m_pModel->GetBanksModel());

	//QList<AtlasTreeItem *> atlasTreeItemList = m_pModel->GetTopLevelTreeItemList();
	//for(int i = 0; i < atlasTreeItemList.size(); ++i)
	//	ui->atlasList->addTopLevelItem(atlasTreeItemList[i]);



	//QJsonArray expandedArray = settingsObj["expanded"].toArray();
	//if(expandedArray.isEmpty() == false)
	//{
	//	for(int i = 0; i < atlasFiltersTreeItemList.size(); ++i)
	//		atlasFiltersTreeItemList[i]->setExpanded(expandedArray[i].toBool());
	//}

	RefreshInfo();


}

ManagerWidget::~ManagerWidget()
{
	delete ui;
}

IManagerModel &ManagerWidget::GetModel()
{
	return *m_pModel;
}

quint32 ManagerWidget::GetSelectedBankId() const
{
	return m_pModel->GetBankIdFromBankIndex(ui->cmbBanks->currentIndex());
}

int ManagerWidget::GetSelectedBankIndex() const
{
	return ui->cmbBanks->currentIndex();
}

void ManagerWidget::SetSelectedBankIndex(int iBankIndex)
{
	ui->cmbBanks->setCurrentIndex(iBankIndex);
	RefreshInfo();
}

int ManagerWidget::GetSelectedBuildIndex() const
{
	if(m_pCodeWidgets)
		return m_pCodeWidgets->m_pCmb->currentIndex();

	return -1;
}

void ManagerWidget::SetSelectedBuildIndex(int iBuildIndex)
{
	if(m_pCodeWidgets)
	{
		m_pCodeWidgets->m_pCmb->setCurrentIndex(iBuildIndex);
		RefreshInfo();
	}
}

bool ManagerWidget::IsShowAllBanksChecked() const
{
	return ui->chkShowAllBanks->isChecked();
}

void ManagerWidget::SetShowAllBanksChecked(bool bShowAllBanks)
{
	ui->chkShowAllBanks->setChecked(bShowAllBanks);
	RefreshInfo();
}

void ManagerWidget::RefreshInfo()
{
	bool bShowAllBanks = ui->chkShowAllBanks->isChecked();
	if(bShowAllBanks)
	{
		ui->cmbBanks->setEnabled(false);
		ui->lblBankInfo->setText("Showing all banks");
		ui->actionRemoveBank->setEnabled(false);
		ui->actionBankSettings->setEnabled(false);

		static_cast<ManagerProxyModel *>(ui->assetTree->model())->FilterByBankIndex(-1);
	}
	else
	{
		ui->cmbBanks->setEnabled(true);
		int iBankIndex = ui->cmbBanks->currentIndex();
		if(iBankIndex < 0)
		{
			HyGuiLog("ManagerWidget::RefreshInfo() - No bank selected", LOGTYPE_Error);
			iBankIndex = 0;
		}

		ui->lblBankInfo->setText(m_pModel->OnBankInfo(iBankIndex));
		ui->actionRemoveBank->setEnabled(iBankIndex != 0);
		ui->actionBankSettings->setEnabled(true);

		static_cast<ManagerProxyModel *>(ui->assetTree->model())->FilterByBankIndex(iBankIndex);
	}

	if(m_pCodeWidgets)
		m_pCodeWidgets->RefreshInfo();
}

QStringList ManagerWidget::GetExpandedFilters()
{
	QStringList expandedList;

	QModelIndexList indexList = GetModel().GetAllIndices();
	for(QModelIndex srcIndex : indexList)
	{
		QModelIndex proxyIndex = static_cast<ManagerProxyModel *>(ui->assetTree->model())->mapFromSource(srcIndex);
		if(ui->assetTree->isExpanded(proxyIndex))
		{
			TreeModelItemData *pItemData = GetModel().data(srcIndex, Qt::UserRole).value<TreeModelItemData *>();
			if(pItemData->GetType() == ITEM_Filter)
				expandedList << GetModel().AssembleFilter(pItemData, true);
		}
	}

	return expandedList;
}

void ManagerWidget::RestoreExpandedState(QStringList expandedFilterList)
{
	QModelIndexList indexList = GetModel().GetAllIndices();
	for(QModelIndex srcIndex : indexList)
	{
		TreeModelItemData *pItemData = GetModel().data(srcIndex, Qt::UserRole).value<TreeModelItemData *>();
		if(pItemData->GetType() != ITEM_Filter)
			continue;

		bool bIsExpanded = false;
		QString sItemDataFilter = GetModel().AssembleFilter(pItemData, true);
		for(auto sFilter : expandedFilterList)
		{
			if(sItemDataFilter.compare(sFilter, Qt::CaseInsensitive) == 0)
			{
				bIsExpanded = true;
				break;
			}
		}

		QModelIndex proxyIndex = static_cast<ManagerProxyModel *>(ui->assetTree->model())->mapFromSource(srcIndex);
		ui->assetTree->setExpanded(proxyIndex, bIsExpanded);
	}
}

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// NOTE: ExplorerWidget::GetSelected are synonymous functions - all fixes/enhancements should be copied over until refactored into a base class
TreeModelItemData *ManagerWidget::GetSelected()
{
	TreeModelItemData *pFirstItemSelected = nullptr;
	QModelIndex curIndex = ui->assetTree->selectionModel()->currentIndex();

	QModelIndex mappedIndex = static_cast<ManagerProxyModel *>(ui->assetTree->model())->mapToSource(curIndex);
	if(mappedIndex.isValid())
		pFirstItemSelected = static_cast<ManagerProxyModel *>(ui->assetTree->model())->sourceModel()->data(mappedIndex, Qt::UserRole).value<TreeModelItemData *>();
	return pFirstItemSelected;
}
void ManagerWidget::GetSelected(QList<IAssetItemData *> &selectedAssetsOut, QList<TreeModelItemData *> &selectedFiltersOut, bool bSortAlphabetically)
{
	selectedAssetsOut.clear();
	selectedFiltersOut.clear();
	QItemSelection selectedItems = static_cast<ManagerProxyModel *>(ui->assetTree->model())->mapSelectionToSource(ui->assetTree->selectionModel()->selection());
	QModelIndexList selectedIndices = selectedItems.indexes();

	QList<TreeModelItemData *> itemList;
	for(int i = 0; i < selectedIndices.size(); ++i)
	{
		if(selectedIndices[i].column() != 0)
			continue;

		itemList += m_pModel->GetItemsRecursively(selectedIndices[i]);
	}

	// Separate out items and filters to their own respective lists, while ignoring any duplicate items while preserving the order in 'itemList'
	// Also only select assets that are in the currently selected bank
	uint uiBankId = static_cast<IManagerModel *>(static_cast<ManagerProxyModel *>(ui->assetTree->model())->sourceModel())->GetBankIdFromBankIndex(ui->cmbBanks->currentIndex());

	QSet<TreeModelItemData *> seenItemSet;
	for(int i = 0; i < itemList.size(); ++i)
	{
		if(seenItemSet.contains(itemList[i]))
			continue;

		seenItemSet.insert(itemList[i]);

		if(itemList[i]->GetType() == ITEM_Filter)
			selectedFiltersOut.append(itemList[i]);
		else
		{
			if(ui->chkShowAllBanks->isChecked() == false && uiBankId != static_cast<IAssetItemData *>(itemList[i])->GetBankId())
				continue;

			selectedAssetsOut.append(static_cast<IAssetItemData *>(itemList[i]));
		}
	}

	// The items within 'selectedAssetsOut' and 'selectedFiltersOut' are not sorted. Sort them either by row depth or alphabetically
	if(bSortAlphabetically)
	{
		std::sort(selectedAssetsOut.begin(), selectedAssetsOut.end(), [](IAssetItemData *pA, IAssetItemData *pB) {
			return QString::localeAwareCompare(pA->GetName(), pB->GetName()) < 0;
			});
		std::sort(selectedFiltersOut.begin(), selectedFiltersOut.end(), [](TreeModelItemData *pA, TreeModelItemData *pB) {
			return QString::localeAwareCompare(pA->GetText(), pB->GetText()) < 0;
			});
	}
	else // Sort by row depth
	{
		// Sort 'selectedItemsOut' by row depth so that we can process the deepest items first
		std::sort(selectedAssetsOut.begin(), selectedAssetsOut.end(), [&](TreeModelItemData *pA, TreeModelItemData *pB) {
			return m_pModel->CalculateDepth(pA) > m_pModel->CalculateDepth(pB);
			});
		std::sort(selectedFiltersOut.begin(), selectedFiltersOut.end(), [&](TreeModelItemData *pA, TreeModelItemData *pB) {
			return m_pModel->CalculateDepth(pA) > m_pModel->CalculateDepth(pB);
			});
	}
}

QList<TreeModelItemData *> ManagerWidget::GetItemsRecursively(TreeModelItemData *pRootItemData) const
{
	QModelIndex index = m_pModel->FindIndex<TreeModelItemData *>(pRootItemData, 0);
	return m_pModel->GetItemsRecursively(index);
}

/*virtual*/ void ManagerWidget::enterEvent(QEnterEvent*pEvent) /*override*/
{
	QWidget::enterEvent(pEvent);
}

/*virtual*/ void ManagerWidget::leaveEvent(QEvent *pEvent) /*override*/
{
	QWidget::leaveEvent(pEvent);
}

/*virtual*/ void ManagerWidget::resizeEvent(QResizeEvent *event) /*override*/
{
	QWidget::resizeEvent(event);

	if(ui->assetTree == nullptr)
		return;

	// TODO: Use formula to account for device pixels and scaling using QWindow::devicePixelRatio()
	const int iInfoColumnWidth = 130;

	int iTotalWidth = ui->assetTree->size().width();
	ui->assetTree->setColumnWidth(0, iTotalWidth - iInfoColumnWidth);
}

void ManagerWidget::OnContextMenu(const QPoint &pos)
{
	QPoint globalPos = ui->assetTree->viewport()->mapToGlobal(pos);
	QMenu contextMenu;
	QMenu bankMenu;

	m_pContextMenuSelection = GetSelected();
	m_bUseContextMenuSelection = true;

	QList<IAssetItemData *> selectedAssetsList; QList<TreeModelItemData *> selectedFiltersList;
	GetSelected(selectedAssetsList, selectedFiltersList, true);

	QModelIndex index = ui->assetTree->indexAt(pos);
	if(index.isValid() == false || selectedAssetsList.empty())
	{
		switch(m_pModel->GetAssetType())
		{
		case ASSETMAN_Source:
			contextMenu.addAction(ui->actionAddClassFiles);
			break;

		case ASSETMAN_Atlases:
			contextMenu.addAction(ui->actionSliceSpriteSheet);
			contextMenu.addAction(ui->actionCreateTileSet);
			break;

		case ASSETMAN_Audio:
			break;

		default:
			HyGuiLog("ManagerWidget::OnContextMenu() unknown asset manager type", LOGTYPE_Error);
			break;
		}

		contextMenu.addAction(ui->actionAddFilter);
		contextMenu.addAction(ui->actionImportAssets);
		contextMenu.addAction(ui->actionImportDirectory);
		if(selectedFiltersList.empty() == false)
		{
			contextMenu.addSeparator();
			contextMenu.addAction(ui->actionRename);
			contextMenu.addSeparator();
			contextMenu.addAction(ui->actionDeleteAssets);
		}

		if(index.isValid() == false)
		{
			ui->assetTree->selectionModel()->clear();
			ui->assetTree->selectionModel()->clearSelection();
			ui->assetTree->selectionModel()->clearCurrentIndex();
		}
	}
	else if(m_pModel->GetAssetType() == ASSETMAN_Source)
	{
		// If any selected item is 'HySrcEntityFilter' or a child of 'HySrcEntityFilter', then cancel context menu
		for(auto selectedItem : selectedAssetsList)
		{
			if(selectedItem->GetFilter() == HySrcEntityFilter)
				return;
		}
		for(auto selectedFilter : selectedFiltersList)
		{
			if(selectedFilter->GetText() == HySrcEntityFilter)
				return;
		}

		contextMenu.addAction(ui->actionOpenAssetExplorer);
		contextMenu.addSeparator();
		contextMenu.addAction(ui->actionAddClassFiles);
		contextMenu.addAction(ui->actionImportAssets);
		contextMenu.addAction(ui->actionImportDirectory);
		contextMenu.addAction(ui->actionAddFilter);
		contextMenu.addSeparator();
		contextMenu.addAction(ui->actionDeleteAssets);
		contextMenu.addAction(ui->actionRename);
	}
	else
	{
		contextMenu.addAction(ui->actionOpenAssetExplorer);

		if(m_pModel->GetNumBanks() > 1)
		{
			bankMenu.setIcon(QIcon(":/icons16x16/atlas-assemble.png"));
			bankMenu.setTitle("Asset Banks...");
			bankMenu.setToolTip("By organizing assets into banks, you can logically group assets to control what gets loaded.");

			for(int i = 0; i < m_pModel->GetNumBanks(); ++i)
			{
				QAction *pActionBankMove = new QAction();
				pActionBankMove->setText(m_pModel->GetBankName(i));
				pActionBankMove->setIcon(QIcon(":/icons16x16/atlas-assemble.png"));
				pActionBankMove->setData(m_pModel->GetBankIdFromBankIndex(i));

				bankMenu.addAction(pActionBankMove);
			}

			// TODO: confirm using connect on a temp QMenu is ok and doesn't leak anything
			connect(&bankMenu, SIGNAL(triggered(QAction*)), this, SLOT(on_actionBankTransfer_triggered(QAction*)));

			contextMenu.addMenu(&bankMenu);
			contextMenu.addSeparator();
		}

		switch(m_pModel->GetAssetType())
		{
		case ASSETMAN_Atlases:
			contextMenu.addAction(ui->actionSliceSpriteSheet);
			contextMenu.addAction(ui->actionCreateTileSet);
			break;

		case ASSETMAN_Audio:
			break;

		default:
			HyGuiLog("ManagerWidget::OnContextMenu() unknown asset manager type", LOGTYPE_Error);
			break;
		}
		contextMenu.addAction(ui->actionImportAssets);
		contextMenu.addAction(ui->actionImportDirectory);
		contextMenu.addAction(ui->actionAddFilter);

		// Check if any selected assets are 'generated' from their project item. If so, prevent delete/replace
		bool bHasGeneratedAssets = false;
		for(int32 i = 0; i < selectedAssetsList.size(); ++i)
		{
			if(selectedAssetsList[i]->GetType() == ITEM_Text ||
			   selectedAssetsList[i]->GetType() == ITEM_Spine ||
			   selectedAssetsList[i]->GetType() == ITEM_Prefab)
			{
				bHasGeneratedAssets = true;
				break;
			}
		}
		contextMenu.addSeparator();
		if(bHasGeneratedAssets == false)
		{
			contextMenu.addAction(ui->actionDeleteAssets);
			contextMenu.addAction(ui->actionReplaceAssets);
		}
		contextMenu.addAction(ui->actionRename);

		contextMenu.addSeparator();
		contextMenu.addAction(ui->actionAssetSettings);
	}

	contextMenu.exec(globalPos);

	QList<QAction *> actionAtlasGrpMoveList = bankMenu.actions();
	for(int i = 0; i < actionAtlasGrpMoveList.size(); ++i)
		delete actionAtlasGrpMoveList[i];
}

void ManagerWidget::OnAssetTreeSelectionChanged(const QModelIndex &current, const QModelIndex &previous)
{
	// Call `IWidget::OnUpdateActions()` on the currently open project item
	ProjectItemData *pCurrentOpenItem = m_pModel->GetProjOwner().GetCurrentOpenItem();
	if(pCurrentOpenItem && pCurrentOpenItem->GetWidget())
		pCurrentOpenItem->GetWidget()->OnUpdateActions();
}

void ManagerWidget::on_assetTree_pressed(const QModelIndex &index)
{
	QList<IAssetItemData *> selectedAssetsList; QList<TreeModelItemData *> selectedFiltersList;
	GetSelected(selectedAssetsList, selectedFiltersList, true);

	int iNumSelected = selectedAssetsList.count();
	ui->actionRename->setEnabled(iNumSelected == 1 || selectedFiltersList.empty() == false);
	ui->actionDeleteAssets->setEnabled(iNumSelected != 0 || selectedFiltersList.empty() == false);
	ui->actionReplaceAssets->setEnabled(iNumSelected != 0);

	static_cast<AuxAssetInspector *>(MainWindow::GetAuxWidget(AUXTAB_AssetInspector))->SetFocusedAssets(m_pModel->GetAssetType(), selectedAssetsList);

	if(iNumSelected == 1 && selectedFiltersList.empty() && selectedAssetsList[0]->GetType() == ITEM_AtlasTileSet)
	{
		static_cast<AuxTileSet *>(MainWindow::GetAuxWidget(AUXTAB_TileSet))->Init(static_cast<AtlasTileSet *>(selectedAssetsList[0]));
		MainWindow::FocusAuxWidget(AUXTAB_TileSet);
	}
	else
		MainWindow::HideAuxWidget(AUXTAB_TileSet);
}

void ManagerWidget::on_assetTree_doubleClicked(const QModelIndex &index)
{
	MainWindow::FocusAuxWidget(AUXTAB_AssetInspector);
}

void ManagerWidget::on_cmbBanks_currentIndexChanged(int index)
{
	RefreshInfo();
}

void ManagerWidget::on_actionAssetSettings_triggered()
{
	QList<IAssetItemData *> selectedAssetsList; QList<TreeModelItemData *> selectedFiltersList;
	GetSelected(selectedAssetsList, selectedFiltersList, true);

	if(selectedAssetsList.empty())
		return;

	DlgAssetProperties dlg(m_pModel, selectedAssetsList);
	if(dlg.exec() == QDialog::Accepted)
	{
		QList<ProjectItemData *> tmpUnusedList;
		if(m_pModel->GetAffectedItems(dlg.GetChangedAssets(), tmpUnusedList))
		{
			dlg.ApplyChanges();
			m_pModel->ReplaceAssets(dlg.GetChangedAssets(), false);
		}
	}
}

void ManagerWidget::on_actionOpenAssetExplorer_triggered()
{
	QList<IAssetItemData *> selectedAssetsList; QList<TreeModelItemData *> selectedFiltersList;
	GetSelected(selectedAssetsList, selectedFiltersList, true);

	if(selectedAssetsList.empty())
		return;

	IAssetItemData *pAsset = selectedAssetsList[0];
	if(pAsset)
		HyGlobal::OpenFileInExplorer(pAsset->GetAbsMetaFilePath());
	else
		HyGuiLog("Asset does not exist", LOGTYPE_Error);
}

void ManagerWidget::on_actionDeleteAssets_triggered()
{
	QList<IAssetItemData *> selectedAssetsList; QList<TreeModelItemData *> selectedFiltersList;
	GetSelected(selectedAssetsList, selectedFiltersList, false); // False sorts by depth, so we delete from bottom up

	m_pModel->RemoveItems(selectedAssetsList, selectedFiltersList, true);
}

void ManagerWidget::on_actionReplaceAssets_triggered()
{
	QList<IAssetItemData *> selectedAssetsList; QList<TreeModelItemData *> selectedFiltersList;
	GetSelected(selectedAssetsList, selectedFiltersList, true);

	if(selectedFiltersList.empty() == false)
	{
		HyGuiLog("Please unselect any filters before replacing assets.", LOGTYPE_Warning);
		return;
	}

	m_pModel->ReplaceAssets(selectedAssetsList, true);
	static_cast<AuxAssetInspector *>(MainWindow::GetAuxWidget(AUXTAB_AssetInspector))->SetFocusedAssets(m_pModel->GetAssetType(), selectedAssetsList);
}

void ManagerWidget::on_actionRename_triggered()
{
	TreeModelItemData *pItemToBeRenamed = GetSelected();

	DlgInputName *pDlg = nullptr;
	if(m_pModel->GetAssetType() == ASSETMAN_Source)
	{
		if(pItemToBeRenamed->IsAssetItem() && static_cast<SourceFile *>(pItemToBeRenamed)->TryCloseAllCodeEditors() == false)
		{
			HyGuiLog("Rename canceled (" % pItemToBeRenamed->GetText() % ")", LOGTYPE_Normal);
			return;
		}

		std::function<QString(QString)> fpErrorCheck = [this](QString sNewName) -> QString
		{
			TreeModelItemData *pItemToBeRenamed = GetSelected();
			if(pItemToBeRenamed->GetType() == ITEM_Filter)
			{
				QString sFilterPath = m_pModel->AssembleFilter(pItemToBeRenamed, true);
				QDir srcDir(m_pModel->GetMetaDir().absoluteFilePath(sFilterPath));
				srcDir.cdUp();
				if(QDir(srcDir.filePath(sNewName)).exists())
					return QString("A filter with this name already exists");
			}
			else
			{
				// TODO: THIS IS WRONG!!!!!!!!!!!!
				QFileInfo srcFileInfo(static_cast<SourceFile *>(pItemToBeRenamed)->GetAbsMetaFilePath());
				if(QFile::exists(srcFileInfo.absoluteDir().filePath(sNewName)))
					return QString("A file with this name already exists");
			}

			return QString();
		};
		pDlg = new DlgInputName("Rename " % pItemToBeRenamed->GetText(), pItemToBeRenamed->GetText(), HyGlobal::FileNameValidator(), fpErrorCheck, nullptr);
	}
	else
		pDlg = new DlgInputName("Rename " % pItemToBeRenamed->GetText(), pItemToBeRenamed->GetText(), HyGlobal::FreeFormValidator(), nullptr, nullptr);

	if(pDlg->exec() == QDialog::Accepted)
	{
		if(m_pModel->GetAssetType() != ASSETMAN_Source)
			m_pModel->Rename(pItemToBeRenamed, pDlg->GetName());
		else
		{
			QString sOldName = QFileInfo(pDlg->GetOldName()).baseName();
			QString sNewName = QFileInfo(pDlg->GetName()).baseName();

			if(pItemToBeRenamed->IsAssetItem())
			{
				m_pModel->Rename(pItemToBeRenamed, pDlg->GetName());

				// Ask to rename old corresponding TreeModelItemData in same filter/directory (.h <-> .cpp) to new name
				SourceFile *pRenamedSrcFile = static_cast<SourceFile *>(pItemToBeRenamed);

				TreeModelItemData *pFilter = m_pModel->FindTreeItemFilter(pItemToBeRenamed, true);
				QList<TreeModelItemData *> itemList = m_pModel->GetItemsRecursively(m_pModel->FindIndex<TreeModelItemData *>(pFilter, 0));

				if(pRenamedSrcFile->GetMetaFileExt() == ".h")
				{
					for(TreeModelItemData *pItem : itemList)
					{
						if(pItem->IsAssetItem() && pItem->GetText() == sOldName % ".cpp")
						{
							if(QMessageBox::Yes == QMessageBox::question(this, "Rename Corresponding File", "Also rename " % static_cast<SourceFile *>(pItem)->GetFilter() % "/" % sOldName % ".cpp to " % sNewName % ".cpp?", QMessageBox::Yes | QMessageBox::No))
								m_pModel->Rename(pItem, sNewName % ".cpp");
						}
					}
				}
				else if(pRenamedSrcFile->GetMetaFileExt() == ".cpp")
				{
					for(TreeModelItemData *pItem : itemList)
					{
						if(pItem->IsAssetItem() && pItem->GetText() == sOldName % ".h")
						{
							if(QMessageBox::Yes == QMessageBox::question(this, "Rename Corresponding File", "Also rename " % static_cast<SourceFile *>(pItem)->GetFilter() % "/" % sOldName % ".h to " % sNewName % ".h?", QMessageBox::Yes | QMessageBox::No))
								m_pModel->Rename(pItem, sNewName % ".h");
						}
					}
				}
			}
			else // Renamed filter
			{
				QString sParentPath = m_pModel->AssembleFilter(pItemToBeRenamed, false);
				QString sOldFilterPath = sParentPath % "/" % sOldName;
				QString sNewFilterPath = sParentPath % "/" % sNewName;
				sOldFilterPath = m_pModel->GetMetaDir().absoluteFilePath(sOldFilterPath);
				sNewFilterPath = m_pModel->GetMetaDir().absoluteFilePath(sNewFilterPath);

				QDir parentDir(sOldFilterPath);
				parentDir.cdUp();
				parentDir.rename(sOldFilterPath, sNewFilterPath);

				m_pModel->Rename(pItemToBeRenamed, pDlg->GetName());
			}
		}

		// HACK: I can't seem to make this ProxyModel resort/refresh other than by calling this?
		m_pModel->GetProjOwner().SaveUserData(); // Save expanded state so below hack works nicer
		static_cast<ManagerProxyModel *>(ui->assetTree->model())->setFilterWildcard("x");
		static_cast<ManagerProxyModel *>(ui->assetTree->model())->setFilterWildcard(ui->txtSearch->text());
	}

	delete pDlg;
}

void ManagerWidget::on_actionAddBank_triggered()
{
	DlgInputName *pDlg = new DlgInputName("Creating New Bank", "NewAssetBank", HyGlobal::FreeFormValidator(), nullptr, nullptr);
	if(pDlg->exec() == QDialog::Accepted)
	{
		m_pModel->CreateNewBank(pDlg->GetName());
		ui->cmbBanks->setCurrentIndex(ui->cmbBanks->count() - 1);

		// This is a way of confirming user wants to create the bank. If canceled, the newly created empty bank is removed
		if(m_pModel->OnBankSettingsDlg(ui->cmbBanks->count() - 1) == false)
			on_actionRemoveBank_triggered();

		RefreshInfo();
	}

	delete pDlg;
}

void ManagerWidget::on_actionBankSettings_triggered()
{
	m_pModel->OnBankSettingsDlg(ui->cmbBanks->currentIndex());
}

void ManagerWidget::on_actionOpenBankExplorer_triggered()
{
	BankData *pBankData = static_cast<BanksModel *>(m_pModel->GetBanksModel())->GetBank(ui->cmbBanks->currentIndex());
	if(pBankData)
	{
		QDir atlasDir(pBankData->m_sAbsPath);
		QFileInfoList fileInfoList = atlasDir.entryInfoList(QDir::NoDotAndDotDot | QDir::Files, QDir::Name);
		if(fileInfoList.empty())
			HyGlobal::OpenFileInExplorer(pBankData->m_sAbsPath);
		else
			HyGlobal::OpenFileInExplorer(fileInfoList[0].absoluteFilePath());
	}
	else
		HyGuiLog("Bank does not exist", LOGTYPE_Error);
}

void ManagerWidget::on_actionRemoveBank_triggered()
{
	uint uiCurIndex = ui->cmbBanks->currentIndex();
	ui->cmbBanks->setCurrentIndex(0);

	m_pModel->RemoveBank(m_pModel->GetBankIdFromBankIndex(uiCurIndex));
}

void ManagerWidget::on_actionBankTransfer_triggered(QAction *pAction)
{
	quint32 uiNewBankId = static_cast<quint32>(pAction->data().toInt());    // Which bank ID we're transferring to

	QList<IAssetItemData *> selectedAssetsList; QList<TreeModelItemData *> selectedFiltersList;
	GetSelected(selectedAssetsList, selectedFiltersList, true);

	m_pModel->TransferAssets(selectedAssetsList, uiNewBankId);
}

void ManagerWidget::on_actionAddClassFiles_triggered()
{
	if(m_pModel->GetAssetType() != ASSETMAN_Source)
	{
		HyGuiLog("ManagerWidget::on_actionAddClassFiles_triggered() - Not a Source Asset Manager", LOGTYPE_Error);
		return;
	}
	
	TreeModelItemData *pFirstSelected = nullptr;
	if(m_bUseContextMenuSelection)
	{
		pFirstSelected = m_pContextMenuSelection;
		m_bUseContextMenuSelection = false;
	}
	else
		pFirstSelected = GetSelected();

	static_cast<SourceModel *>(m_pModel)->OnAddClass(pFirstSelected);
}

void ManagerWidget::on_actionSliceSpriteSheet_triggered()
{
	if(m_pModel->GetAssetType() != ASSETMAN_Atlases)
	{
		HyGuiLog("ManagerWidget::on_actionSliceSpriteSheet_triggered() - Not an Atlas Asset Manager", LOGTYPE_Error);
		return;
	}

	TreeModelItemData *pFirstSelected = nullptr;
	if(m_bUseContextMenuSelection)
	{
		pFirstSelected = m_pContextMenuSelection;
		m_bUseContextMenuSelection = false;
	}
	else
		pFirstSelected = GetSelected();

	static_cast<AtlasModel *>(m_pModel)->OnSliceSprite(m_pModel->GetBankIdFromBankIndex(ui->cmbBanks->currentIndex()), pFirstSelected);
}

void ManagerWidget::on_actionImportAssets_triggered()
{
	QFileDialog dlg(this);
	dlg.setFileMode(QFileDialog::ExistingFiles);
	dlg.setViewMode(QFileDialog::Detail);
	dlg.setWindowModality(Qt::ApplicationModal);
	dlg.setModal(true);

	QStringList sFilterList;
	if(m_pModel->GetAssetType() == ASSETMAN_Source)
		sFilterList += "Code(*.cpp *.c *.h *.hpp *.cxx)";
	for(int i = 0; i < m_pModel->GetSupportedFileExtList().size(); ++i)
		sFilterList += "*" % m_pModel->GetSupportedFileExtList()[i];
	sFilterList += "*.*";
	dlg.setNameFilters(sFilterList);

	if(dlg.exec() == QDialog::Rejected)
		return;

	QStringList sImportList = dlg.selectedFiles();

	TreeModelItemData *pFirstSelected = nullptr;
	if(m_bUseContextMenuSelection)
	{
		pFirstSelected = m_pContextMenuSelection;
		m_bUseContextMenuSelection = false;
	}
	else
		pFirstSelected = GetSelected();

	TreeModelItemData *pParent = m_pModel->FindTreeItemFilter(pFirstSelected, true);

	QVector<TreeModelItemData *> correspondingParentList;
	QVector<QUuid> correspondingUuidList;
	for(int i = 0; i < sImportList.size(); ++i)
	{
		correspondingParentList.append(pParent);
		correspondingUuidList.append(QUuid::createUuid());
	}

	m_pModel->ImportNewAssets(sImportList,
							  m_pModel->GetBankIdFromBankIndex(ui->cmbBanks->currentIndex()),
							  correspondingParentList,
							  correspondingUuidList);
}

void ManagerWidget::on_actionImportDirectory_triggered()
{
	QFileDialog dlg(this);
	dlg.setFileMode(QFileDialog::Directory);
	dlg.setOption(QFileDialog::ShowDirsOnly, true);
	dlg.setViewMode(QFileDialog::Detail);
	dlg.setWindowModality(Qt::ApplicationModal);
	dlg.setModal(true);

	if(dlg.exec() == QDialog::Rejected)
		return;

	TreeModelItemData *pFirstSelected = nullptr;
	if(m_bUseContextMenuSelection)
	{
		pFirstSelected = m_pContextMenuSelection;
		m_bUseContextMenuSelection = false;
	}
	else
		pFirstSelected = GetSelected();

	// The 'pImportParent' will be the root point for all new AtlasTreeItem insertions (both filters and images)
	TreeModelItemData *pImportParent = m_pModel->FindTreeItemFilter(pFirstSelected, true);

	// Store all the specified imported image paths and their corresponding parent tree items they should be inserted into
	QStringList sImportList;
	QVector<TreeModelItemData *> correspondingParentList;
	QVector<QUuid> correspondingUuidList;

	// Loop through all the specified import directories
	QStringList sDirs = dlg.selectedFiles();
	for(int iDirIndex = 0; iDirIndex < sDirs.size(); ++iDirIndex)
	{
		// Dig recursively through this directory and grab all the image files (while creating filters that resemble the folder structure they're stored in)
		QDir dirEntry(sDirs[iDirIndex]);
		TreeModelItemData *pCurFilter = m_pModel->CreateNewFilter(dirEntry.dirName(), pImportParent, false);

		QStack<QPair<QFileInfoList, TreeModelItemData *>> dirStack;
		dirStack.push(QPair<QFileInfoList, TreeModelItemData *>(dirEntry.entryInfoList(), pCurFilter));

		while(dirStack.isEmpty() == false)
		{
			QPair<QFileInfoList, TreeModelItemData *> curDir = dirStack.pop();
			QFileInfoList list = curDir.first;

			for(int i = 0; i < list.count(); i++)
			{
				QFileInfo info = list[i];
				if(info.isDir() && info.fileName() != ".." && info.fileName() != ".")
				{
					QDir subDir(info.filePath());
					dirStack.push(QPair<QFileInfoList, TreeModelItemData *>(subDir.entryInfoList(), m_pModel->CreateNewFilter(subDir.dirName(), curDir.second, false)));
				}
				else
				{
					for(auto sExt : m_pModel->GetSupportedFileExtList())
					{
						if(QString('.' % info.suffix()).compare(sExt, Qt::CaseInsensitive) == 0)
						{
							sImportList.push_back(info.filePath());
							correspondingParentList.push_back(curDir.second);
							correspondingUuidList.append(QUuid::createUuid());
							break;
						}
					}
				}
			}
		}
	}

	m_pModel->ImportNewAssets(sImportList,
							  m_pModel->GetBankIdFromBankIndex(ui->cmbBanks->currentIndex()),
							  correspondingParentList,
							  correspondingUuidList);
}

void ManagerWidget::on_actionSyncFilterAssets_triggered()
{
	TreeModelItemData *pSyncFilterRoot = nullptr;
	if(m_bUseContextMenuSelection)
	{
		pSyncFilterRoot = m_pContextMenuSelection;
		m_bUseContextMenuSelection = false;
	}
	else
		pSyncFilterRoot = GetSelected();

	DlgSyncAssets *pDlg = new DlgSyncAssets(*m_pModel, pSyncFilterRoot, m_pModel->GetBankIdFromBankIndex(ui->cmbBanks->currentIndex()));
	if(pDlg->exec() == QDialog::Accepted)
	{
		
	}
}

void ManagerWidget::on_actionAddFilter_triggered()
{
	DlgInputName *pDlg = new DlgInputName("Enter Atlas Group Filter Name", "New Filter", HyGlobal::FreeFormValidator(), nullptr, nullptr);
	if(pDlg->exec() == QDialog::Accepted)
	{
		TreeModelItemData *pNewFilter = nullptr;

		TreeModelItemData *pFirstSelected = nullptr;
		if(m_bUseContextMenuSelection)
		{
			pFirstSelected = m_pContextMenuSelection;
			m_bUseContextMenuSelection = false;
		}
		else
			pFirstSelected = GetSelected();

		TreeModelItemData *pParent = m_pModel->FindTreeItemFilter(pFirstSelected, true);
		pNewFilter = m_pModel->CreateNewFilter(pDlg->GetName(), pParent, true);
	}

	delete pDlg;
}

void ManagerWidget::on_actionCreateTileSet_triggered()
{
	if(m_pModel->GetAssetType() != ASSETMAN_Atlases)
	{
		HyGuiLog("ManagerWidget::on_actionCreateTileSet_triggered() - Asset type is not Atlases", LOGTYPE_Error);
		return;
	}

	uint uiBankId = static_cast<IManagerModel *>(static_cast<ManagerProxyModel *>(ui->assetTree->model())->sourceModel())->GetBankIdFromBankIndex(ui->cmbBanks->currentIndex());

	TreeModelItemData *pParentTreeItem = nullptr;
	if(m_bUseContextMenuSelection)
	{
		pParentTreeItem = m_pContextMenuSelection;
		m_bUseContextMenuSelection = false;
	}
	else
		pParentTreeItem = GetSelected();

	pParentTreeItem = m_pModel->FindTreeItemFilter(pParentTreeItem, true);

	DlgInputName dlgInputName("Enter TileSet Name", "New TileSet", HyGlobal::FileNameValidator(), nullptr, nullptr);
	if(dlgInputName.exec() == QDialog::Accepted)
	{
		QString sName = dlgInputName.GetName();
		AtlasModel *pAtlasModel = static_cast<AtlasModel *>(m_pModel);
		AtlasTileSet *pNewTileSet = pAtlasModel->GenerateTileSet(sName, pParentTreeItem, uiBankId);
		QModelIndex tileSetIndex = pAtlasModel->FindIndex<AtlasTileSet *>(pNewTileSet, 0);

		ui->assetTree->expand(tileSetIndex);
		//ui->assetTree->selectionModel()->select(tileSetIndex, QItemSelectionModel::ClearAndSelect);
	}
}

void ManagerWidget::on_chkShowAllBanks_clicked()
{
	RefreshInfo();
}

void ManagerWidget::on_txtSearch_textChanged(const QString &text)
{
	static_cast<ManagerProxyModel *>(ui->assetTree->model())->setFilterWildcard(text);

	//// This expands all items that are found in text search filter, but is too slow and also doesn't collapse afterwards
	//if(text.isEmpty() == false)
	//{
	//	QModelIndexList indexList = GetModel().GetAllIndices();
	//	for(QModelIndex srcIndex : indexList)
	//	{
	//		if(srcIndex.isValid() && static_cast<ManagerProxyModel *>(ui->assetTree->model())->IsPassFilter(srcIndex))
	//			ui->assetTree->expand(srcIndex);
	//	}
	//}
}

void ManagerWidget::on_actionBuildSettings_triggered()
{
	if(Harmony::GetProject() == nullptr)
	{
		HyGuiLog("on_actionBuildSettings_triggered invoked with no loaded project", LOGTYPE_Error);
		return;
	}

	Harmony::GetProject()->GetSourceModel().OnBankSettingsDlg(0);
}

void ManagerWidget::on_actionNewBuild_triggered()
{
	if(Harmony::GetProject() == nullptr)
	{
		HyGuiLog("on_actionNewBuild_triggered invoked with no loaded project", LOGTYPE_Error);
		return;
	}

	DlgNewBuild *pDlg = new DlgNewBuild(*Harmony::GetProject(), this);
	if(pDlg->exec() == QDialog::Accepted)
	{
		QString sBuildPath = Harmony::GetProject()->GetBuildAbsPath();
		QDir rootBuildDir(sBuildPath);
		QDir buildDir(pDlg->GetAbsBuildDir());
		if(rootBuildDir.exists())
		{
			if(buildDir.exists())
				HyGuiLog("Build directory already exists", LOGTYPE_Error);
		}
		else if(false == rootBuildDir.mkpath("."))
		{
			HyGuiLog("Could not create root build directory", LOGTYPE_Error);
			return;
		}

		QProcess *pBuildProcess = new QProcess(this);
		QObject::connect(pBuildProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(OnProcessStdOut()));
		QObject::connect(pBuildProcess, SIGNAL(readyReadStandardError()), this, SLOT(OnProcessErrorOut()));
		QObject::connect(pBuildProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
			[this, buildDir](int exitCode, QProcess::ExitStatus exitStatus)
			{
				QFileInfoList buildFileInfoList = buildDir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);

				QString sDirName = buildDir.dirName();
				QString sBestAbsFilePath = buildDir.absolutePath();
				
				for(auto fileInfo : buildFileInfoList)
				{
					if(fileInfo.suffix().compare("sln", Qt::CaseInsensitive) == 0) // Look for .sln (Visual Studio)
					{
						sBestAbsFilePath = fileInfo.absoluteFilePath();
						break;
					}
					// TODO: scan for other popular IDE's
				}

				// Write the build JSON file
				QJsonObject jsonObj;
				jsonObj["type"] = 0; // NOTE: For potential future use
				jsonObj["name"] = sDirName;
				jsonObj["path"] = sBestAbsFilePath;

				QFile jsonFile(buildDir.filePath(sDirName % HYGUIPATH_BuildExt));
				if(jsonFile.open(QIODevice::WriteOnly))
				{
					if(jsonFile.write(QJsonDocument(jsonObj).toJson()) == -1)
						HyGuiLog("Could not write build JSON file", LOGTYPE_Error);
					jsonFile.close();
				}
				else
					HyGuiLog("Could not create build JSON file", LOGTYPE_Error);

				m_pCodeWidgets->AssembleComboBox();
				m_pCodeWidgets->m_pCmb->setCurrentIndex(m_pCodeWidgets->m_pCmb->findText(sDirName));
				QMessageBox::information(nullptr, "Build Complete", "Build '" % buildDir.dirName() % "' has completed.");
			});

		QStringList sArgList = pDlg->GetProcOptions();
		pBuildProcess->start("cmake", sArgList);
	}
	delete pDlg;
}

void ManagerWidget::on_actionOpenBuild_triggered()
{
	if(Harmony::GetProject() == nullptr)
	{
		HyGuiLog("on_actionOpenBuild_triggered invoked with no loaded project", LOGTYPE_Error);
		return;
	}
	if(m_pCodeWidgets == nullptr)
	{
		HyGuiLog("Build widgets not initialized", LOGTYPE_Error);
		return;
	}

	QDesktopServices::openUrl(QUrl(m_pCodeWidgets->GetCurrentBuildUrl()));

	//if(bIdeFileFound)
	//	connect(pBuildsMenu, &QMenu::triggered, this, [this, sBestAbsFilePath]() { QDesktopServices::openUrl(QUrl(sBestAbsFilePath)); });
	//else // Couldn't determine the exact IDE file, so just open the build directory in explorer
	//	connect(pBuildsMenu, &QMenu::triggered, this, [this, sBestAbsFilePath]() { HyGlobal::OpenFileInExplorer(sBestAbsFilePath); });
	//QDir buildDir(Harmony::GetProject()->GetBuildAbsPath());
	//if(buildDir.exists() == false)
	//{
	//	HyGuiLog("Build directory does not exist", LOGTYPE_Error);
	//	return;
	//}
	//HyGlobal::OpenFileInExplorer(buildDir.absolutePath());
}

void ManagerWidget::on_actionPackageBuild_triggered()
{
	if(Harmony::GetProject() == nullptr)
	{
		HyGuiLog("on_actionPackageBuild_triggered invoked with no loaded project", LOGTYPE_Error);
		return;
	}

	DlgPackageBuild *pDlg = new DlgPackageBuild(*Harmony::GetProject(), this);
	if(pDlg->exec() == QDialog::Accepted)
	{
		QDir packageDir(pDlg->GetAbsPackageDir());
		if(false == packageDir.mkpath("."))
		{
			HyGuiLog("Could not create package directory", LOGTYPE_Error);
			return;
		}

		QProcess *pBuildProcess = new QProcess(this);
		QObject::connect(pBuildProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(OnProcessStdOut()));
		QObject::connect(pBuildProcess, SIGNAL(readyReadStandardError()), this, SLOT(OnProcessErrorOut()));
		QObject::connect(pBuildProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
			[this, packageDir](int exitCode, QProcess::ExitStatus exitStatus)
			{
				QMessageBox::information(nullptr, "Package Complete", "Package '" % packageDir.dirName() % "' has completed.");
				HyGlobal::OpenFileInExplorer(packageDir.absolutePath());
			});

		QString sProc = pDlg->GetProc();
		QStringList sArgList = pDlg->GetProcOptions();
		pBuildProcess->start(sProc, sArgList);
	}
	delete pDlg;
}

void ManagerWidget::on_actionDeleteBuild_triggered()
{
	QString sBuildPath = Harmony::GetProject()->GetBuildAbsPath() % m_pCodeWidgets->m_pCmb->currentText();
	QDir buildDir(sBuildPath);
	if(buildDir.exists())
	{
		QFileInfoList tempFileInfoList = buildDir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);
		if(tempFileInfoList.isEmpty() == false &&
			QMessageBox::Yes == QMessageBox::question(MainWindow::GetInstance(), "Delete existing build", "Do you want to delete build: " % buildDir.dirName() % "?", QMessageBox::Yes, QMessageBox::No))
		{
			buildDir.removeRecursively();

			if(m_pCodeWidgets)
				m_pCodeWidgets->AssembleComboBox();
		}
	}
}

void ManagerWidget::OnBuildIndex(int iIndex)
{
	RefreshInfo();
}

void ManagerWidget::OnProcessStdOut()
{
	QProcess *p = (QProcess *)sender();
	HyGuiLog(p->readAllStandardOutput(), LOGTYPE_Normal);
}

void ManagerWidget::OnProcessErrorOut()
{
	QProcess *p = (QProcess *)sender();
	HyGuiLog(p->readAllStandardError(), LOGTYPE_Info);
}
