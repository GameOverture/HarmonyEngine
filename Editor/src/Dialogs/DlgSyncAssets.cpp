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
#include "IManagerModel.h"
#include "MainWindow.h"

#include <QDir>
#include <QFileDialog>
#include <QMessageBox>

DlgSyncAssets::DlgSyncAssets(IManagerModel &assetManagerRef, TreeModelItemData *pSyncFilterRoot, QWidget *pParent /*= nullptr*/) :
	QDialog(pParent/*, Qt::WindowSystemMenuHint | Qt::WindowTitleHint*/),
	m_AssManRef(assetManagerRef),
	m_pSyncFilterRoot(pSyncFilterRoot),
	ui(new Ui::DlgSyncAssets)
{
	ui->setupUi(this);

	ui->lblError->setStyleSheet("QLabel { background-color : red; color : black; }");

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

	QDir syncDir(ui->txtDirectoryPath->text());
	if(syncDir.exists() == false)
	{
		HyGuiLog("DlgSyncAssets::ScanForActions() - Directory does not exist", LOGTYPE_Error);
		return;
	}

	// Using `m_pSyncFilterRoot`, populate `existingItemMap` with all assets within it
	QMap<TreeModelItem *, TreeModelItemData *> existingItemMap;

	QStack<TreeModelItem *> stackTraverseModel;
	stackTraverseModel.push(static_cast<TreeModelItem *>(m_AssManRef.FindIndex<TreeModelItemData *>(m_pSyncFilterRoot, 0).internalPointer()));
	while(!stackTraverseModel.isEmpty())
	{
		TreeModelItem *pItem = stackTraverseModel.pop();
		existingItemMap[pItem] = pItem->data(0).value<TreeModelItemData *>();

		for(int i = 0; i < pItem->GetNumChildren(); ++i)
			stackTraverseModel.push(pItem->GetChild(i));
	}

	// Store actions to be perform - the order declared is the order they will be performed
	QStringList sCreateFilterList;						// Relative paths of new filters to create
	QList<QPair<QString, QString>> sMoveItemList;		// FIRST: Old path, SECOND: New path
	QList<QPair<QString, QString>> sReplaceItemList;	// FIRST: Existing path, SECOND: File path to new asset
	QStringList sDeleteItemList;						// Relative paths of existing assets to delete
	QList<QPair<QString, QString>> sImportItemList;		// FIRST: Filter Location, SECOND: File path to new asset

	// Dig recursively through the specified sync directory files and compare with existing assets within `existingItemMap` to populate the above action lists
	QStack<QFileInfoList> stackTraverseDir;
	stackTraverseDir.push(syncDir.entryInfoList());
	while(stackTraverseDir.isEmpty() == false)
	{
		QFileInfoList curDir = stackTraverseDir.pop();
		for(int i = 0; i < curDir.count(); i++)
		{
			QFileInfo info = curDir[i];
			if(info.isDir() && info.fileName() != ".." && info.fileName() != ".")
			{
				QDir subDir(info.filePath());
				stackTraverseDir.push(subDir.entryInfoList());

				// Use relative path from sync directory to check if filter exists
				QString sRelDir = syncDir.relativeFilePath(info.filePath());
				for(auto it = existingItemMap.begin(); it != existingItemMap.end(); ++it)
				{
					TreeModelItemData *pItemData = it.value();
					if(pItemData->GetType() == ITEM_Filter)
					{
						QString sFullFilterPath = m_AssManRef.AssembleFilter(pItemData, true);
						QFileInfo filterFile(sFullFilterPath);
						if(filterFile.isRelative() == false)
						{
							HyGuiLog("DlgSyncAssets::ScanForActions() - Filter path is not relative: " % filterFile.filePath(), LOGTYPE_Error);
							continue;
						}

						if(filterFile.filePath().compare(sRelDir, Qt::CaseInsensitive) == 0)
						{
							// Filter already exists
							existingItemMap.remove(it.key());
							break;
						}
					}
				}
			}
			else
			{
				for(QString sExt : m_AssManRef.GetSupportedFileExtList())
				{
					if(QString('.' % info.suffix()).compare(sExt, Qt::CaseInsensitive) == 0)
					{
						// Valid asset file found. Synchronize with existing assets and populate action lists
						QDir relDir = syncDir.relativeFilePath(info.filePath());
						QString sRelPath = relDir.filePath(info.fileName());

						// Check if the asset already exists
						bool bExists = false;
						for(auto it = existingItemMap.begin(); it != existingItemMap.end(); ++it)
						{
							QString sIterFilter = m_AssManRef.AssembleFilter(it.value(), false);
							if(sIterFilter.compare(sRelPath, Qt::CaseInsensitive) == 0)
							{
								bExists = true;
								break;
							}
						}
						
						break;
					}
				}
			}
		}
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
