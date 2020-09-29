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
#include "DlgAssetProperties.h"

#include <QUndoCommand>
#include <QMessageBox>
#include <QFileDialog>
#include <QDrag>

ManagerProxyModel::ManagerProxyModel(QObject *pParent /*= nullptr*/) :
	QSortFilterProxyModel(pParent)
{ }

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

	if(pLeftItem->GetType() != pRightItem->GetType())
		return pLeftItem->GetType() < pRightItem->GetType();

	return QString::localeAwareCompare(pLeftItem->GetText(), pRightItem->GetText()) < 0;
}

ManagerTreeView::ManagerTreeView(QWidget *pParent /*= nullptr*/) :
	QTreeView(pParent)
{ }

/*virtual*/ void ManagerTreeView::startDrag(Qt::DropActions supportedActions) /*override*/
{
	QModelIndexList indexes = selectedIndexes();
	if(indexes.empty())
		return;

	QMimeData *pMimeData = model()->mimeData(indexes);
	if(pMimeData == nullptr)
		return;

	QPixmap pixmap = indexes.first().data(Qt::DecorationRole).value<QPixmap>();
	QDrag *pDrag = new QDrag(this);
	pDrag->setPixmap(pixmap);
	pDrag->setMimeData(pMimeData);
	pDrag->setHotSpot(QPoint(pixmap.width()/2, pixmap.height()/2));

	Qt::DropAction eDropAction = pDrag->exec(supportedActions);
	if(eDropAction != Qt::MoveAction)
	{
		supportedActions &= ~Qt::MoveAction;
		eDropAction = pDrag->exec(supportedActions);
	}
}

ManagerWidget::ManagerWidget(QWidget *pParent /*= nullptr*/) :
	QWidget(pParent),
	ui(new Ui::ManagerWidget),
	m_pModel(nullptr),
	m_pDraw(nullptr)
{
	ui->setupUi(this);

	// NOTE: THIS CONSTRUCTOR IS INVALID TO USE. IT EXISTS FOR QT TO ALLOW Q_OBJECT TO WORK
	HyGuiLog("ManagerWidget::ManagerWidget() invalid constructor used", LOGTYPE_Error);
}

ManagerWidget::ManagerWidget(IManagerModel *pModel, QWidget *pParent /*= nullptr*/) :
	QWidget(pParent),
	ui(new Ui::ManagerWidget),
	m_pModel(pModel),
	m_pDraw(nullptr)
{
	ui->setupUi(this);

	m_pModel->OnAllocateDraw(m_pDraw);

	ManagerProxyModel *pProxyModel = new ManagerProxyModel(this);
	pProxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
	pProxyModel->setSourceModel(m_pModel);

	// Don't display anything that starts with HyGuiInternalCharIndicator
	// TODO: replace regexp using HyGuiInternalCharIndicator
	pProxyModel->setFilterRegExp(QRegExp("^[^+]*$", Qt::CaseInsensitive));
	pProxyModel->setFilterKeyColumn(0);

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

	ui->actionDeleteAssets->setEnabled(false);
	ui->actionReplaceAssets->setEnabled(false);

	ui->btnBankSettings->setDefaultAction(ui->actionBankSettings);
	ui->btnImportAssets->setDefaultAction(ui->actionImportAssets);
	ui->btnImportDir->setDefaultAction(ui->actionImportDirectory);

	ui->btnDeleteAssets->setDefaultAction(ui->actionDeleteAssets);
	ui->btnReplaceAssets->setDefaultAction(ui->actionReplaceAssets);
	ui->btnCreateFilter->setDefaultAction(ui->actionAddFilter);

	ui->btnAddBank->setDefaultAction(ui->actionAddBank);
	ui->btnRemoveBank->setDefaultAction(ui->actionRemoveBank);

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

quint32 ManagerWidget::GetSelectedBankId()
{
	return m_pModel->GetBankIdFromBankIndex(ui->cmbBanks->currentIndex());
}

void ManagerWidget::DrawUpdate()
{
	if(m_pDraw)
	{
		QPoint pos(mapFromGlobal(QCursor::pos()).x(), mapFromGlobal(QCursor::pos()).y());
		QModelIndex index = ui->assetTree->indexAt(pos);

		//m_pDraw->SetHover(m_pModel->data(index, Qt::UserRole).value<TreeModelItemData *>());
		m_pDraw->OnDrawUpdate();
	}
}

void ManagerWidget::RefreshInfo()
{
	uint uiBankIndex = ui->cmbBanks->currentIndex();

	ui->lblBankInfo->setText(m_pModel->OnBankInfo(uiBankIndex));
	ui->actionRemoveBank->setEnabled(uiBankIndex != 0);

	// Restore expanded filters if they have been reloaded
	//QJsonArray expandedFilterArray = m_pModel->GetExpandedFiltersArray();
	//uint uiFilterIndex = 0;
	//QTreeWidgetItemIterator iter(ui->atlasList);
	//while(*iter)
	//{
	//	if((*iter)->data(0, Qt::UserRole).toString() == HYTREEWIDGETITEM_IsFilter)
	//	{
	//		if(expandedFilterArray.at(uiFilterIndex).toBool())
	//			(*iter)->setExpanded(true);
	//		else
	//			(*iter)->setExpanded(false);

	//		++uiFilterIndex;
	//	}

	//	++iter;
	//}
}

//void ManagerWidget::OnAssetTreeCurrentChanged(const QModelIndex &current, const QModelIndex &previous)
//{
//}

/*virtual*/ void ManagerWidget::enterEvent(QEvent *pEvent) /*override*/
{
	if(m_pDraw)
		m_pDraw->Show();
	QWidget::enterEvent(pEvent);
}

/*virtual*/ void ManagerWidget::leaveEvent(QEvent *pEvent) /*override*/
{
	if(m_pDraw)
		m_pDraw->Hide();
	QWidget::leaveEvent(pEvent);
}

/*virtual*/ void ManagerWidget::resizeEvent(QResizeEvent *event) /*override*/
{
	QWidget::resizeEvent(event);

	if(ui->assetTree == nullptr)
		return;

	int iTotalWidth = ui->assetTree->size().width();
	ui->assetTree->setColumnWidth(0, iTotalWidth - 60);
}

void ManagerWidget::OnContextMenu(const QPoint &pos)
{
	QPoint globalPos = ui->assetTree->viewport()->mapToGlobal(pos);
	QMenu contextMenu;
	QMenu bankMenu;

	QList<AssetItemData *> selectedAssetsList;
	QList<TreeModelItemData *> selectedFiltersList;
	GetSelectedItems(selectedAssetsList, selectedFiltersList);

	QModelIndex index = ui->assetTree->indexAt(pos);
	if(index.isValid() == false || selectedAssetsList.empty())
	{
		contextMenu.addAction(ui->actionImportAssets);
		contextMenu.addAction(ui->actionImportDirectory);
		contextMenu.addAction(ui->actionAddFilter);
	}
	else
	{
		contextMenu.addAction(ui->actionAssetSettings);

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
		}

		contextMenu.addSeparator();
		contextMenu.addAction(ui->actionImportAssets);
		contextMenu.addAction(ui->actionImportDirectory);
		contextMenu.addAction(ui->actionAddFilter);
		contextMenu.addSeparator();
		contextMenu.addAction(ui->actionDeleteAssets);
		contextMenu.addAction(ui->actionReplaceAssets);
		contextMenu.addAction(ui->actionRename);
	}

	contextMenu.exec(globalPos);

	QList<QAction *> actionAtlasGrpMoveList = bankMenu.actions();
	for(int i = 0; i < actionAtlasGrpMoveList.size(); ++i)
		delete actionAtlasGrpMoveList[i];
}

void ManagerWidget::on_actionAssetSettings_triggered()
{
	QList<AssetItemData *> selectedAssetsList;
	QList<TreeModelItemData *> selectedFiltersList;
	GetSelectedItems(selectedAssetsList, selectedFiltersList);

	if(selectedAssetsList.empty())
		return;

	DlgAssetProperties dlg(m_pModel->GetAssetType(), selectedAssetsList);
	if(dlg.exec() == QDialog::Accepted)
	{
	}
}

void ManagerWidget::on_actionDeleteAssets_triggered()
{
	QList<AssetItemData *> selectedAssetsList;
	QList<TreeModelItemData *> selectedFiltersList;
	GetSelectedItems(selectedAssetsList, selectedFiltersList);

	m_pModel->RemoveItems(selectedAssetsList, selectedFiltersList);
}

void ManagerWidget::on_actionReplaceAssets_triggered()
{
	QList<AssetItemData *> selectedAssetsList;
	QList<TreeModelItemData *> selectedFiltersList;
	GetSelectedItems(selectedAssetsList, selectedFiltersList);

	if(selectedFiltersList.empty() == false)
	{
		HyGuiLog("Please unselect any filters before replacing assets.", LOGTYPE_Warning);
		return;
	}

	// TODO: also check if all of selectedAssetsList belong to the same filter, since only replacing in this matter makes sense
	if(false)
	{
		HyGuiLog("Only select assets from under the same filter (or root).", LOGTYPE_Warning);
		return;
	}

	m_pModel->ReplaceAssets(selectedAssetsList);
}

void ManagerWidget::on_assetTree_clicked()
{
	QList<AssetItemData *> selectedAssetsList;
	QList<TreeModelItemData *> selectedFiltersList;
	GetSelectedItems(selectedAssetsList, selectedFiltersList);

	if(m_pDraw)
		m_pDraw->SetSelected(selectedAssetsList);

	int iNumSelected = selectedAssetsList.count();
	ui->actionRename->setEnabled(iNumSelected == 1);
	ui->actionDeleteAssets->setEnabled(iNumSelected != 0);
	ui->actionReplaceAssets->setEnabled(iNumSelected != 0);

	// Determine the best suited icon based on selection
	HyGuiItemType eIconType = ITEM_Unknown;
	for(int i = 0; i < selectedAssetsList.size(); ++i)
	{
		if(selectedAssetsList[i]->GetType() == ITEM_Filter)
		{
			ui->actionReplaceAssets->setEnabled(false);

			if(eIconType != ITEM_AtlasImage)
				eIconType = ITEM_Filter;
		}
		else
			eIconType = ITEM_AtlasImage;
	}

	if(eIconType != ITEM_Unknown)
		ui->actionDeleteAssets->setIcon(HyGlobal::ItemIcon(eIconType, SUBICON_Delete));
}

void ManagerWidget::on_actionRename_triggered()
{
	QList<AssetItemData *> selectedAssetsList;
	QList<TreeModelItemData *> selectedFiltersList;
	GetSelectedItems(selectedAssetsList, selectedFiltersList);

	TreeModelItemData *pItemToBeRenamed = nullptr;
	if(selectedAssetsList.isEmpty())
	{
		if(selectedFiltersList.isEmpty())
			return;

		pItemToBeRenamed = selectedFiltersList[0];
	}
	else
		pItemToBeRenamed = selectedAssetsList[0];

	DlgInputName *pDlg = new DlgInputName("Rename " % pItemToBeRenamed->GetText(), pItemToBeRenamed->GetText());
	if(pDlg->exec() == QDialog::Accepted)
		m_pModel->Rename(pItemToBeRenamed, pDlg->GetName());

	delete pDlg;
}

void ManagerWidget::on_cmbBanks_currentIndexChanged(int index)
{
	RefreshInfo();
}

void ManagerWidget::on_actionAddBank_triggered()
{
	DlgInputName *pDlg = new DlgInputName("Creating New Bank", "NewAssetBank");
	if(pDlg->exec() == QDialog::Accepted)
	{
		m_pModel->CreateNewBank(pDlg->GetName());
		ui->cmbBanks->setCurrentIndex(ui->cmbBanks->count() - 1);

		// This is a way of confirming user wants to create the bank. If canceled, the newly created empty bank is removed
		if(m_pModel->OnBankSettingsDlg(ui->cmbBanks->count() - 1) == false)
			on_actionRemoveBank_triggered();
	}

	delete pDlg;
}

void ManagerWidget::on_actionBankSettings_triggered()
{
	m_pModel->OnBankSettingsDlg(ui->cmbBanks->currentIndex());
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

	QList<AssetItemData *> selectedAssetsList;
	QList<TreeModelItemData *> selectedFiltersList;
	GetSelectedItems(selectedAssetsList, selectedFiltersList);

	m_pModel->TransferAssets(selectedAssetsList, uiNewBankId);
}

void ManagerWidget::on_actionImportAssets_triggered()
{
	QFileDialog dlg(this);
	dlg.setFileMode(QFileDialog::ExistingFiles);
	dlg.setViewMode(QFileDialog::Detail);
	dlg.setWindowModality(Qt::ApplicationModal);
	dlg.setModal(true);

	QStringList sFilterList;
	for(int i = 0; i < m_pModel->GetSupportedFileExtList().size(); ++i)
		sFilterList += "*" % m_pModel->GetSupportedFileExtList()[i];
	sFilterList += "*.*";
	dlg.setNameFilters(sFilterList);

	if(dlg.exec() == QDialog::Rejected)
		return;

	QStringList sImportImgList = dlg.selectedFiles();

	//QString sSelectedFilter(tr("PNG (*.png)"));
	//QStringList sImportImgList = QFileDialog::getOpenFileNames(this,
	//	"Import image(s) into atlases",
	//	QString(),
	//	tr("All files (*.*);;PNG (*.png)"),
	//	&sSelectedFilter);


	QList<AssetItemData *> selectedAssetsList;
	QList<TreeModelItemData *> selectedFiltersList;
	GetSelectedItems(selectedAssetsList, selectedFiltersList);

	TreeModelItemData *pParent = nullptr;
	if(selectedAssetsList.empty() == false)
		pParent = m_pModel->FindTreeItemFilter(selectedAssetsList[0]);
	else if(selectedFiltersList.empty() == false)
		pParent = selectedFiltersList[0];

	QList<TreeModelItemData *> correspondingParentList;
	QList<QUuid> correspondingUuidList;
	for(int i = 0; i < sImportImgList.size(); ++i)
	{
		correspondingParentList.append(pParent);
		correspondingUuidList.append(QUuid::createUuid());
	}

	m_pModel->ImportNewAssets(sImportImgList,
							  m_pModel->GetBankIdFromBankIndex(ui->cmbBanks->currentIndex()),
							  ITEM_Unknown, // Uses default item type of manager
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

	QList<AssetItemData *> selectedAssetsList;
	QList<TreeModelItemData *> selectedFiltersList;
	GetSelectedItems(selectedAssetsList, selectedFiltersList);

	// The 'pImportParent' will be the root point for all new AtlasTreeItem insertions (both filters and images)
	TreeModelItemData *pImportParent = nullptr;
	if(selectedAssetsList.empty() == false)
		pImportParent = m_pModel->FindTreeItemFilter(selectedAssetsList[0]);
	else if(selectedFiltersList.empty() == false)
		pImportParent = selectedFiltersList[0];

	// Store all the specified imported image paths and their corresponding parent tree items they should be inserted into
	QStringList sImportImgList;
	QList<TreeModelItemData *> correspondingParentList;
	QList<QUuid> correspondingUuidList;

	// Loop through all the specified import directories
	QStringList sDirs = dlg.selectedFiles();
	for(int iDirIndex = 0; iDirIndex < sDirs.size(); ++iDirIndex)
	{
		// Dig recursively through this directory and grab all the image files (while creating filters that resemble the folder structure they're stored in)
		QDir dirEntry(sDirs[iDirIndex]);
		TreeModelItemData *pCurFilter = m_pModel->CreateNewFilter(dirEntry.dirName(), pImportParent);

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
					dirStack.push(QPair<QFileInfoList, TreeModelItemData *>(subDir.entryInfoList(), m_pModel->CreateNewFilter(subDir.dirName(), curDir.second)));
				}
				else if(info.suffix().toLower() == "png")
				{
					sImportImgList.push_back(info.filePath());
					correspondingParentList.push_back(curDir.second);
					correspondingUuidList.append(QUuid::createUuid());
				}
			}
		}
	}

	m_pModel->ImportNewAssets(sImportImgList,
							  m_pModel->GetBankIdFromBankIndex(ui->cmbBanks->currentIndex()),
							  ITEM_Unknown, // Uses default item type of manager
							  correspondingParentList,
							  correspondingUuidList);
}

void ManagerWidget::on_actionAddFilter_triggered()
{
	DlgInputName *pDlg = new DlgInputName("Enter Atlas Group Filter Name", "New Filter");
	if(pDlg->exec() == QDialog::Accepted)
	{
		TreeModelItemData *pNewFilter = nullptr;

		QList<AssetItemData *> selectedAssetsList;
		QList<TreeModelItemData *> selectedFiltersList;
		GetSelectedItems(selectedAssetsList, selectedFiltersList);

		TreeModelItemData *pParent = nullptr;
		if(selectedAssetsList.empty() == false)
			pParent = m_pModel->FindTreeItemFilter(selectedAssetsList[0]);
		else if(selectedFiltersList.empty() == false)
			pParent = selectedFiltersList[0];

		pNewFilter = m_pModel->CreateNewFilter(pDlg->GetName(), pParent);

		//ui->atlasList->sortItems(0, Qt::AscendingOrder);
		//ui->atlasList->clearSelection();
		//ui->atlasList->expandItem(pNewFilter);
		//pNewFilter->setSelected(true);
	}

	delete pDlg;
}

void ManagerWidget::GetSelectedItems(QList<AssetItemData *> &selectedAssetsOut, QList<TreeModelItemData *> &selectedFiltersOut)
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

		itemList += m_pModel->GetItemsRecursively(selectedIndices[i]);;
	}

	// Separate out items and filters to their own respective lists, while ignoring any duplicate items while preserving the order in 'itemList'
	QSet<TreeModelItemData *> seenItemSet;
	for(int i = 0; i < itemList.size(); ++i)
	{
		if(seenItemSet.contains(itemList[i]))
			continue;
		seenItemSet.insert(itemList[i]);

		if(itemList[i]->GetType() == ITEM_Filter)
			selectedFiltersOut.append(itemList[i]);
		else
			selectedAssetsOut.append(static_cast<AssetItemData *>(itemList[i]));
	}
}
