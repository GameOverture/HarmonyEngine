/**************************************************************************
 *	DlgSyncAssets.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2024 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "DlgSyncAssets.h"
#include "ui_DlgSyncAssets.h"
#include "AtlasModel.h"
#include "MainWindow.h"

#include <QDir>
#include <QFileDialog>
#include <QMessageBox>

DlgSyncAssets::DlgSyncAssets(IManagerModel &assetManagerRef, TreeModelItemData *pSyncFilterRoot, quint32 uiBankId, QWidget *pParent /*= nullptr*/) :
	QDialog(pParent/*, Qt::WindowSystemMenuHint | Qt::WindowTitleHint*/),
	m_AssManRef(assetManagerRef),
	m_pSyncFilterRoot(pSyncFilterRoot),
	m_uiBankId(uiBankId),
	ui(new Ui::DlgSyncAssets)
{
	ui->setupUi(this);
	ui->lblError->setStyleSheet("QLabel { background-color : red; color : black; }");

	if(m_pSyncFilterRoot == nullptr)
	{
		HyGuiLog("DlgSyncAssets::DlgSyncAssets() - m_pSyncFilterRoot is nullptr", LOGTYPE_Error);
		return;
	}
	if(m_pSyncFilterRoot->GetType() != ITEM_Filter)
	{
		HyGuiLog("DlgSyncAssets::ScanForActions() - m_pSyncRoot is not a filter", LOGTYPE_Error);
		return;
	}

	on_btnBrowse_clicked();
}

DlgSyncAssets::~DlgSyncAssets()
{
	delete ui;
}

void DlgSyncAssets::on_buttonBox_accepted()
{
	if(QMessageBox::Yes != QMessageBox::question(MainWindow::GetInstance(), "Confirm Sync Directory Actions", "Do you want to perform the following " % HyGlobal::AssetName(m_AssManRef.GetAssetType()) % " actions?", QMessageBox::Yes, QMessageBox::No))
		return;

	// TODO: Perform the actions
}

void DlgSyncAssets::on_btnBrowse_clicked()
{
	QFileDialog dlg(this);
	dlg.setWindowTitle("Select asset directory to sync with filter: " % m_pSyncFilterRoot->GetText());
	dlg.setFileMode(QFileDialog::Directory);
	dlg.setOption(QFileDialog::ShowDirsOnly, true);
	dlg.setViewMode(QFileDialog::Detail);
	dlg.setWindowModality(Qt::ApplicationModal);
	dlg.setModal(true);

	if(dlg.exec() == QDialog::Rejected)
		return;

	QStringList sDirs = dlg.selectedFiles();
	if(sDirs.isEmpty() || sDirs.count() > 1)
	{
		HyGuiLog("DlgSyncAssets::on_btnBrowse_clicked() - Invalid directory selection", LOGTYPE_Error);
		return;
	}

	ui->txtDirectoryPath->setText(sDirs[0]);

	ScanForActions();
	ErrorCheck();
}

void DlgSyncAssets::on_txtDirectoryPath_textEdited(const QString &sText)
{
	ScanForActions();
	ErrorCheck();
}

void DlgSyncAssets::ScanForActions()
{
	QDir syncDir(ui->txtDirectoryPath->text());
	if(syncDir.exists() == false)
		return;

	ACTIONS_CreateFilterList.clear();
	ACTIONS_MoveItemMap.clear();
	ACTIONS_RenameItemMap.clear();
	ACTIONS_ReplaceItemMap.clear();
	ACTIONS_DeleteItemList.clear();
	ACTIONS_ImportItemMap.clear();

	// Using `m_pSyncFilterRoot`, populate `existingItemsMap` with all filters and assets within it
	QMap<TreeModelItem *, TreeModelItemData *> existingItemsMap;
	QStack<TreeModelItem *> stackTraverseModel;
	stackTraverseModel.push(static_cast<TreeModelItem *>(m_AssManRef.FindIndex<TreeModelItemData *>(m_pSyncFilterRoot, 0).internalPointer()));
	while(!stackTraverseModel.isEmpty())
	{
		TreeModelItem *pItem = stackTraverseModel.pop();
		TreeModelItemData *pItemData = pItem->data(0).value<TreeModelItemData *>();

		// Don't insert the root filter into the `existingItemMap` map, it is not synchronized
		if(pItemData != m_pSyncFilterRoot)
			existingItemsMap[pItem] = pItemData;

		for(int i = 0; i < pItem->GetNumChildren(); ++i)
			stackTraverseModel.push(pItem->GetChild(i));
	}

	// Copy construct the above map - Sync'ed items will be removed from this map, and any remaining items will be deleted
	QMap<TreeModelItem *, TreeModelItemData *> staleItemsMap(existingItemsMap);

	// STEP 1: Synchronize directories to existing filters
	// Dig recursively through syncDir's files and compare with existing assets within `staleItemsMap` to populate the above action lists
	QStack<QFileInfoList> stackTraverseSyncDirFileList;
	stackTraverseSyncDirFileList.push(syncDir.entryInfoList(QDir::NoDotAndDotDot, QDir::DirsFirst | QDir::Name));
	while(stackTraverseSyncDirFileList.isEmpty() == false)
	{
		QFileInfoList currentSyncDirFileList = stackTraverseSyncDirFileList.pop();
		for(int i = 0; i < currentSyncDirFileList.count(); i++)
		{
			QFileInfo currentSyncFile = currentSyncDirFileList[i];
			if(currentSyncFile.isDir() && currentSyncFile.fileName() != ".." && currentSyncFile.fileName() != ".")
			{
				QDir syncSubDir(currentSyncFile.filePath());
				stackTraverseSyncDirFileList.push(syncSubDir.entryInfoList(QDir::NoDotAndDotDot, QDir::DirsFirst | QDir::Name));

				SyncDir(QDir(currentSyncFile.filePath()), staleItemsMap);
			}
			else // Is a file, not a directory
			{
				SyncFile(currentSyncFile, staleItemsMap);
			}
		}
	}

	// STEP 2: Synchronize valid files to existing assets

	// Mark to delete the remaining items in `staleItemsMap` as they are no longer present within the syncDir
	for(TreeModelItemData *pItemData : staleItemsMap.values())
		ACTIONS_DeleteItemList << pItemData;

	// Populate the QTreeWidget with the all the existing items within `m_pSyncFilterRoot` + any new created filters and asset imports
	ui->treeWidget->clear();
	for(TreeModelItem *pItemNode : existingItemsMap.keys())
	{
		TreeModelItemData *pItemData = existingItemsMap[pItemNode];


		QTreeWidgetItem *pNewTreeWidgetItem = new QTreeWidgetItem();

		// Find if this item is in any of the action lists
		if(ACTIONS_MoveItemMap.contains(pItemData))
		{
			// Move asset to correct location
			pNewTreeWidgetItem->setText(0, "Move: " + pItemData->GetText() + " to " + ACTIONS_MoveItemMap[pItemData]);
			pNewTreeWidgetItem->setIcon(0, HyGlobal::ItemIcon(pItemData->GetType(), SUBICON_Close));
		}
		else if(ACTIONS_RenameItemMap.contains(pItemData))
		{
			// Rename asset to correct name
			pNewTreeWidgetItem->setText(0, "Rename: " + pItemData->GetText() + " to " + ACTIONS_RenameItemMap[pItemData]);
			pNewTreeWidgetItem->setIcon(0, QIcon(":/icons16x16/generic-rename.png")); // TODO: Make a subicon type for rename
		}
		else if(ACTIONS_ReplaceItemMap.contains(pItemData))
		{
			// Replace asset with new asset
			pNewTreeWidgetItem->setText(0, "Replace: " + pItemData->GetText() + " with " + ACTIONS_ReplaceItemMap[pItemData]);
			pNewTreeWidgetItem->setIcon(0, QIcon(":/icons16x16/atlas-replaceFrame.png")); // TODO: Make a subicon type for replace
		}
		else if(ACTIONS_DeleteItemList.contains(pItemData))
		{
			// Delete asset
			pNewTreeWidgetItem->setText(0, "Delete: " + pItemData->GetText());
			pNewTreeWidgetItem->setIcon(0, HyGlobal::ItemIcon(pItemData->GetType(), SUBICON_Delete));
		}
		else
		{
			// No action needed
			pNewTreeWidgetItem->setText(0, pItemData->GetText());
			pNewTreeWidgetItem->setIcon(0, HyGlobal::ItemIcon(pItemData->GetType(), SUBICON_None));
		}

		ui->treeWidget->addTopLevelItem(pNewTreeWidgetItem);
	}
}

bool DlgSyncAssets::SyncDir(QDir syncSubDir, QMap<TreeModelItem *, TreeModelItemData *> &staleItemsMapOut)
{
	QDir syncDir(ui->txtDirectoryPath->text());

	// Create relative path from sync directory to check if corresponding filter exists within 'staleItemsMapOut'
	QString sSyncFilterPath = m_AssManRef.AssembleFilter(m_pSyncFilterRoot, true) + '/' + syncDir.relativeFilePath(syncSubDir.absolutePath());
	for(auto it = staleItemsMapOut.begin(); it != staleItemsMapOut.end(); ++it)
	{
		if(it.value()->GetType() == ITEM_Filter)
		{
			QString sFullFilterPath = m_AssManRef.AssembleFilter(it.value(), true);
			if(sFullFilterPath.compare(sSyncFilterPath, Qt::CaseInsensitive) == 0)
			{
				// Filter does exist, and in the correct location
				staleItemsMapOut.remove(it.key());
				return true;
			}

			// Now just check its base name if it matches existing filters - if match, then check if its contents also match (indicating filter just moved location)
			if(it.value()->GetText().compare(syncSubDir.dirName(), Qt::CaseInsensitive) == 0)
			{
				// Base names match, now check if contents match
				bool bMatchingEntireContents = true; // Contents must find a match (either by contents or filename) to detect the filter has been moved - TODO: This will NOT pick up moved filters with deleted items
				QMap<TreeModelItemData *, QString> renamedChildrenMap; // If 'bMatchingEntireContents' is true, this list will contain children that matched contents but has different file names
				QMap<TreeModelItemData *, QString> replaceChildrenMap; // If 'bMatchingEntireContents' is true, this list will contain children that matched filenames, but has different contents

				QVector<TreeModelItem *> testChildrenContentsList = it.key()->GetChildren();
				for(TreeModelItem *pTestChildItem : testChildrenContentsList)
				{
					TreeModelItemData *pTestChildItemData = staleItemsMapOut[pTestChildItem];

					// Find a matching file within syncSubDir
					bool bFoundMatchingFile = false;
					QFileInfoList syncSubDirFileInfoList = syncSubDir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
					for(QFileInfo syncSubDirFileInfo : syncSubDirFileInfoList)
					{
						if(pTestChildItemData->GetType() == ITEM_Filter)
						{
							if(syncSubDirFileInfo.isDir() && pTestChildItemData->GetText().compare(syncSubDirFileInfo.baseName(), Qt::CaseInsensitive) == 0)
							{
								bFoundMatchingFile = true;
								break;
							}
						}
						else // Is asset (not a filter)
						{
							for(QString sExt : m_AssManRef.GetSupportedFileExtList())
							{
								if(QString('.' % syncSubDirFileInfo.suffix()).compare(sExt, Qt::CaseInsensitive) != 0)
									continue;
								quint32 uiSyncAssetChecksum = 0;
								bool bIsValidSyncAsset = false;
								switch(m_AssManRef.GetAssetType())
								{
								case ASSETMAN_Atlases: {
									QImage newImage(syncSubDirFileInfo.absoluteFilePath());
									uiSyncAssetChecksum = HyGlobal::CRCData(0, newImage.bits(), newImage.sizeInBytes());
									bIsValidSyncAsset = static_cast<AtlasModel &>(m_AssManRef).IsImageValid(newImage, m_uiBankId);
									break; }

								case ASSETMAN_Source:
								case ASSETMAN_Prefabs:
								case ASSETMAN_Audio:
								default:
									HyGuiLog("DlgSyncAssets::SyncFileContents() - Asset type not implemented", LOGTYPE_Error);
									break;
								}

								if(bIsValidSyncAsset == false)
									continue;

								if(static_cast<IAssetItemData *>(pTestChildItemData)->GetChecksum() == uiSyncAssetChecksum)
								{
									bFoundMatchingFile = true;

									if(pTestChildItemData->GetText().compare(syncSubDirFileInfo.baseName(), Qt::CaseInsensitive) != 0)
										renamedChildrenMap.insert(pTestChildItemData, syncSubDirFileInfo.baseName());

									break;
								}
								else if(pTestChildItemData->GetText().compare(syncSubDirFileInfo.baseName(), Qt::CaseInsensitive) == 0)
								{
									bFoundMatchingFile = true;
									replaceChildrenMap.insert(pTestChildItemData, syncSubDirFileInfo.absoluteFilePath());
								}
							}
						}
					}

					if(bFoundMatchingFile == false)
					{
						bMatchingEntireContents = false;
						break;
					}
				}

				if(bMatchingEntireContents)
				{
					// Filter can safely be marked as moved
					QVector<TreeModelItem *> movedChildrenContentsList = it.key()->GetChildren();
					for(TreeModelItem *pMovedChildItem : movedChildrenContentsList)
					{
						TreeModelItemData *pMovedChildItemData = staleItemsMapOut[pMovedChildItem];

						ACTIONS_MoveItemMap.insert(pMovedChildItemData, sSyncFilterPath);

						// Check for rename
						if(renamedChildrenMap.contains(pMovedChildItemData))
							ACTIONS_RenameItemMap.insert(pMovedChildItemData, renamedChildrenMap[pMovedChildItemData]);

						if(replaceChildrenMap.contains(pMovedChildItemData))
							ACTIONS_ReplaceItemMap.insert(pMovedChildItemData, replaceChildrenMap[pMovedChildItemData]);

						staleItemsMapOut.remove(pMovedChildItem);
					}

					ACTIONS_MoveItemMap.insert(it.value(), sSyncFilterPath);
					staleItemsMapOut.remove(it.key());
					
					return true;
				}
			}
		}
	}

	// Cannot find a matching existing filter
	ACTIONS_CreateFilterList << sSyncFilterPath;
}

TreeModelItem *DlgSyncAssets::FindFileContents(QFileInfo currentSyncFile, const QMap<TreeModelItem *, TreeModelItemData *> &staleItemsMap)
{
	if(currentSyncFile.isDir())
		return nullptr;

	for(QString sExt : m_AssManRef.GetSupportedFileExtList())
	{
		if(QString('.' % currentSyncFile.suffix()).compare(sExt, Qt::CaseInsensitive) != 0)
			continue;

		// Each asset manager calculates its assets' checksums differently
		quint32 uiSyncAssetChecksum = 0;
		bool bIsValidSyncAsset = false;
		switch(m_AssManRef.GetAssetType())
		{
		case ASSETMAN_Atlases: {
			QImage newImage(currentSyncFile.absoluteFilePath());
			uiSyncAssetChecksum = HyGlobal::CRCData(0, newImage.bits(), newImage.sizeInBytes());
			bIsValidSyncAsset = static_cast<AtlasModel &>(m_AssManRef).IsImageValid(newImage, m_uiBankId);
			break; }

		case ASSETMAN_Source:
		case ASSETMAN_Prefabs:
		case ASSETMAN_Audio:
		default:
			HyGuiLog("DlgSyncAssets::SyncFileContents() - Asset type not implemented", LOGTYPE_Error);
			break;
		}

		if(bIsValidSyncAsset == false)
			continue;

		for(auto it = staleItemsMap.begin(); it != staleItemsMap.end(); ++it)
		{
			if(it.value()->GetType() == ITEM_Filter)
				continue;

			IAssetItemData *pIterAssetData = static_cast<IAssetItemData *>(it.value());
			if(pIterAssetData->GetChecksum() == uiSyncAssetChecksum)
				return it.key();
		}
	}

	return nullptr;
}

bool DlgSyncAssets::SyncFile(QFileInfo currentSyncFile, QMap<TreeModelItem *, TreeModelItemData *> &staleItemsMap)
{
	QString sSyncFilterRootPath = m_AssManRef.AssembleFilter(m_pSyncFilterRoot, true);
	QDir syncDir(ui->txtDirectoryPath->text());

	for(QString sExt : m_AssManRef.GetSupportedFileExtList())
	{
		if(QString('.' % currentSyncFile.suffix()).compare(sExt, Qt::CaseInsensitive) == 0)
		{
			// Valid sync asset file found on file system. Synchronize with existing assets and populate action lists
			QString sSyncAssetConvertedFilterPath = QFileInfo(syncDir.relativeFilePath(currentSyncFile.filePath())).path();
			if(sSyncAssetConvertedFilterPath == ".")
				sSyncAssetConvertedFilterPath = sSyncFilterRootPath;
			else
				sSyncAssetConvertedFilterPath = sSyncFilterRootPath + '/' + sSyncAssetConvertedFilterPath;

			// Each asset manager calculates its assets' checksums differently
			quint32 uiSyncAssetChecksum = 0;
			bool bIsValidSyncAsset = false;
			switch(m_AssManRef.GetAssetType())
			{
			case ASSETMAN_Atlases: {
				QImage newImage(currentSyncFile.absoluteFilePath());
				uiSyncAssetChecksum = HyGlobal::CRCData(0, newImage.bits(), newImage.sizeInBytes());
				bIsValidSyncAsset = static_cast<AtlasModel &>(m_AssManRef).IsImageValid(newImage, m_uiBankId);
				break; }

			case ASSETMAN_Source:
			case ASSETMAN_Prefabs:
			case ASSETMAN_Audio:
			default:
				HyGuiLog("DlgSyncAssets::ScanForActions() - Asset type not implemented", LOGTYPE_Error);
				break;
			}

			// Check if the asset already exists
			IAssetItemData *pExistingAssetData = nullptr;
			TreeModelItem *pExistingAssetNode = nullptr;
			bool bCorrectLocation = false;
			bool bCorrectName = false;
			for(auto it = staleItemsMap.begin(); it != staleItemsMap.end(); ++it)
			{
				if(it.value()->GetType() == ITEM_Filter)
					continue;

				IAssetItemData *pIterAssetData = static_cast<IAssetItemData *>(it.value());
				QString sIterFilterPath = m_AssManRef.AssembleFilter(pIterAssetData, false);
				if(pIterAssetData->GetChecksum() == uiSyncAssetChecksum)
				{
					// Asset already exists
					pExistingAssetData = pIterAssetData;
					pExistingAssetNode = it.key();

					// Check location (TODO: Check for potential bug with duplicate assets aka two assets with the same checksum)
					if(sIterFilterPath.compare(sSyncAssetConvertedFilterPath, Qt::CaseInsensitive) == 0)
						bCorrectLocation = true;

					// Check name (TODO: Check for potential bug with duplicate assets aka two assets with the same checksum)
					if(pIterAssetData->GetName().compare(currentSyncFile.baseName(), Qt::CaseInsensitive) == 0)
						bCorrectName = true;

					break;
				}
			}

			if(pExistingAssetData == nullptr) // Not found by checksum, check by path+name - if match then append action as replace, otherwise append action as new import
			{
				for(auto it = staleItemsMap.begin(); it != staleItemsMap.end(); ++it)
				{
					if(it.value()->GetType() == ITEM_Filter)
						continue;

					IAssetItemData *pIterAssetData = static_cast<IAssetItemData *>(it.value());
					QString sIterFilterPath = m_AssManRef.AssembleFilter(pIterAssetData, false);
					if(sIterFilterPath.compare(sSyncAssetConvertedFilterPath, Qt::CaseInsensitive) == 0)
					{
						ACTIONS_ReplaceItemMap.insert(pIterAssetData, currentSyncFile.absoluteFilePath());
						staleItemsMap.remove(it.key());
						return true;
					}
				}

				ACTIONS_ImportItemMap.insert(syncDir.relativeFilePath(currentSyncFile.filePath()), currentSyncFile.absoluteFilePath());
			}
			else // Was found by checksum
			{
				if(bCorrectLocation == false) // Move asset to correct location
					ACTIONS_MoveItemMap.insert(pExistingAssetData, sSyncAssetConvertedFilterPath);
				if(bCorrectName == false) // Rename asset to correct name
					ACTIONS_RenameItemMap.insert(pExistingAssetData, currentSyncFile.baseName());
				
				staleItemsMap.remove(pExistingAssetNode);
				return true;
			}
		}
	}

	return false;
}

void DlgSyncAssets::ErrorCheck()
{
	bool bIsError = false;
	do
	{
		if(m_pSyncFilterRoot->GetType() != ITEM_Filter)
		{
			ui->lblError->setText("Error: Invalid SyncFilterRoot");
			bIsError = true;
			break;
		}

		if(QDir(ui->txtDirectoryPath->text()).exists() == false)
		{
			ui->lblError->setText("Error: Invalid Sync Directory");
			bIsError = true;
			break;
		}

	} while(false);

	ui->lblError->setVisible(bIsError);
	ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!bIsError);
}
