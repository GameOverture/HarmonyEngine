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
	if(QFile::exists(ui->txtDirectoryPath->text()) == false)
		return;

	if(m_pSyncFilterRoot->GetType() != ITEM_Filter)
	{
		HyGuiLog("DlgSyncAssets::ScanForActions() - m_pSyncRoot is not a filter", LOGTYPE_Error);
		return;
	}

	QString sSyncFilterRootPath = m_AssManRef.AssembleFilter(m_pSyncFilterRoot, true);
	QDir syncDir(ui->txtDirectoryPath->text());
	if(syncDir.exists() == false)
	{
		HyGuiLog("DlgSyncAssets::ScanForActions() - Directory does not exist", LOGTYPE_Error);
		return;
	}

	// Using `m_pSyncFilterRoot`, populate `existingItemsMap` with all assets within it
	QMap<TreeModelItem *, TreeModelItemData *> existingItemsMap;

	QStack<TreeModelItem *> stackTraverseModel;
	stackTraverseModel.push(static_cast<TreeModelItem *>(m_AssManRef.FindIndex<TreeModelItemData *>(m_pSyncFilterRoot, 0).internalPointer()));
	while(!stackTraverseModel.isEmpty())
	{
		TreeModelItem *pItem = stackTraverseModel.pop();
		TreeModelItemData *pItemData = pItem->data(0).value<TreeModelItemData *>();

		// Don't insert the root filter into the `existingItemMap` map
		if(pItemData != m_pSyncFilterRoot)
			existingItemsMap[pItem] = pItemData;

		for(int i = 0; i < pItem->GetNumChildren(); ++i)
			stackTraverseModel.push(pItem->GetChild(i));
	}

	// Processed items will be removed from this map, and any remaining items will be deleted
	QMap<TreeModelItem *, TreeModelItemData *> staleItemsMap(existingItemsMap);

	// Store actions to be performed - the order declared is the order they will be performed
	QStringList									ACTIONLIST_CreateFilters;	// Full paths of new filters to create
	QList<QPair<TreeModelItemData *, QString>>	ACTIONLIST_MoveItems;		// FIRST: Existing Item Data ptr, SECOND: Full filter path
	QList<QPair<TreeModelItemData *, QString>>	ACTIONLIST_RenameItems;		// FIRST: Existing Item Data ptr, SECOND: New name
	QList<QPair<TreeModelItemData *, QString>>	ACTIONLIST_ReplaceItems;	// FIRST: Existing Item Data ptr, SECOND: File path to new asset
	QList<TreeModelItemData *>					ACTIONLIST_DeleteItems;		// Item Data ptr of existing assets to delete
	QList<QPair<QString, QString>>				ACTIONLIST_ImportItems;		// FIRST: Full filter path (parent location), SECOND: File path to new asset

	// Dig recursively through the specified sync directory files and compare with existing assets within `existingItemMap` to populate the above action lists
	QStack<QFileInfoList> stackTraverseDir;
	stackTraverseDir.push(syncDir.entryInfoList());
	while(stackTraverseDir.isEmpty() == false)
	{
		QFileInfoList currentDir = stackTraverseDir.pop();
		for(int i = 0; i < currentDir.count(); i++)
		{
			QFileInfo currentFile = currentDir[i];
			if(currentFile.isDir() && currentFile.fileName() != ".." && currentFile.fileName() != ".")
			{
				QDir subDir(currentFile.filePath());
				stackTraverseDir.push(subDir.entryInfoList());

				// Use relative path from sync directory to check if filter exists
				bool bFilterExists = false;
				QString sSyncFilterPath = sSyncFilterRootPath + '/' + syncDir.relativeFilePath(currentFile.filePath());
				for(auto it = staleItemsMap.begin(); it != staleItemsMap.end(); ++it)
				{
					TreeModelItemData *pItemData = it.value();
					if(pItemData->GetType() == ITEM_Filter)
					{
						QString sFullFilterPath = m_AssManRef.AssembleFilter(pItemData, true);
						if(sFullFilterPath.compare(sSyncFilterPath, Qt::CaseInsensitive) == 0)
						{
							// Filter already exists
							staleItemsMap.remove(it.key());
							bFilterExists = true;
							break;
						}
					}
				}
				if(bFilterExists == false)
					ACTIONLIST_CreateFilters << sSyncFilterPath;
			}
			else // Is a file, not a directory
			{
				for(QString sExt : m_AssManRef.GetSupportedFileExtList())
				{
					if(QString('.' % currentFile.suffix()).compare(sExt, Qt::CaseInsensitive) == 0)
					{
						// Valid sync asset file found on file system. Synchronize with existing assets and populate action lists
						QString sSyncAssetConvertedFilterPath = QFileInfo(syncDir.relativeFilePath(currentFile.filePath())).path();
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
							QImage newImage(currentFile.absoluteFilePath());
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
								if(pIterAssetData->GetName().compare(currentFile.baseName(), Qt::CaseInsensitive) == 0)
									bCorrectName = true;

								break;
							}
						}

						if(pExistingAssetData == nullptr) // Not found by checksum, check by path+name - if match then append action as replace, otherwise append action as new import
						{
							bool bWasFoundByPath = false;
							for(auto it = staleItemsMap.begin(); it != staleItemsMap.end(); ++it)
							{
								if(it.value()->GetType() == ITEM_Filter)
									continue;

								IAssetItemData *pIterAssetData = static_cast<IAssetItemData *>(it.value());
								QString sIterFilterPath = m_AssManRef.AssembleFilter(pIterAssetData, false);
								if(sIterFilterPath.compare(sSyncAssetConvertedFilterPath, Qt::CaseInsensitive) == 0)
								{
									staleItemsMap.remove(it.key());

									ACTIONLIST_ReplaceItems << QPair<IAssetItemData *, QString>(pIterAssetData, currentFile.absoluteFilePath());
									bWasFoundByPath = true;
									break;
								}
							}

							if(bWasFoundByPath == false)
								ACTIONLIST_ImportItems << QPair<QString, QString>(syncDir.relativeFilePath(currentFile.filePath()), currentFile.absoluteFilePath());
						}
						else // Was found by checksum
						{
							staleItemsMap.remove(pExistingAssetNode);

							if(bCorrectLocation == false) // Move asset to correct location
								ACTIONLIST_MoveItems << QPair<IAssetItemData *, QString>(pExistingAssetData, sSyncAssetConvertedFilterPath);
							if(bCorrectName == false) // Rename asset to correct name
								ACTIONLIST_RenameItems << QPair<IAssetItemData *, QString>(pExistingAssetData, currentFile.baseName());
						}
					}
				}
			}
		}
	}

	for(TreeModelItemData *pItemData : staleItemsMap.values())
		ACTIONLIST_DeleteItems << pItemData;

	// Populate the QTreeWidget with the all the existing items within `m_pSyncFilterRoot` + any new asset imports
	ui->treeWidget->clear();
	for(TreeModelItemData *pItemData : existingItemsMap.values())
	{
		QTreeWidgetItem *pNewTreeWidgetItem = new QTreeWidgetItem();

		// Find if this item is in any of the action lists
		//ACTIONLIST_MoveItems.contains(;		// FIRST: Existing Item Data ptr, SECOND: Full filter path
		//QList<QPair<TreeModelItemData *, QString>>	ACTIONLIST_RenameItems;		// FIRST: Existing Item Data ptr, SECOND: New name
		//QList<QPair<TreeModelItemData *, QString>>	ACTIONLIST_ReplaceItems;	// FIRST: Existing Item Data ptr, SECOND: File path to new asset
		//QList<TreeModelItemData *>					ACTIONLIST_DeleteItems;		// Item Data ptr of existing assets to delete
		//QList<QPair<QString, QString>>				ACTIONLIST_ImportItems;
		
		
		//pNewTreeWidgetItem->setIcon(0, HyGlobal::ItemIcon(pItemData->GetType(), );
		//ui->treeWidget->addTopLevelItem();
	}
}

void DlgSyncAssets::ErrorCheck()
{
	bool bIsError = false;
	do
	{
		if(m_pSyncFilterRoot->GetType() != ITEM_Filter)
		{
			ui->lblError->setText("Error: Invalid SyncRoot selected");
			bIsError = true;
			break;
		}

		if(QFile::exists(ui->txtDirectoryPath->text()) == false)
		{
			ui->lblError->setText("Error: Invalid Sync Directory");
			bIsError = true;
			break;
		}

	} while(false);

	ui->lblError->setVisible(bIsError);
	ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!bIsError);
}
