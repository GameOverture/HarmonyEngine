/**************************************************************************
 *	DlgSyncAssets.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2024 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef DlgSyncAssets_H
#define DlgSyncAssets_H

#include <QDialog>

namespace Ui {
class DlgSyncAssets;
}

class IManagerModel;
class TreeModelItem;
class TreeModelItemData;

class DlgSyncAssets : public QDialog
{
	Q_OBJECT

	IManagerModel &			m_AssManRef;
	TreeModelItemData *		m_pSyncFilterRoot;
	quint32					m_uiBankId;

	// Store actions to be performed - the order declared is the order they will be performed
	QStringList									ACTIONS_CreateFilterList;	// Full paths of new filters to create
	QMap<TreeModelItemData *, QString>			ACTIONS_MoveItemMap;		// KEY: Existing Item Data ptr, VALUE: Full filter path
	QMap<TreeModelItemData *, QString>			ACTIONS_RenameItemMap;		// KEY: Existing Item Data ptr, VALUE: New name
	QMap<TreeModelItemData *, QString>			ACTIONS_ReplaceItemMap;		// KEY: Existing Item Data ptr, VALUE: File path to new asset
	QList<TreeModelItemData *>					ACTIONS_DeleteItemList;		// Item Data ptr of existing assets to delete
	QMap<QString, QString>						ACTIONS_ImportItemMap;		// KEY: Full filter path (parent location), VALUE: File path to new asset

public:
	explicit DlgSyncAssets(IManagerModel &assetManagerRef, TreeModelItemData *pSyncFilterRoot, quint32 uiBankId, QWidget *pParent = nullptr);
	~DlgSyncAssets();

private Q_SLOTS:
	void on_buttonBox_accepted();
	void on_btnBrowse_clicked();

	void on_txtDirectoryPath_textEdited(const QString &sText);

private:
	Ui::DlgSyncAssets *ui;

	void ScanForActions();
	bool SyncDir(QDir currentSyncDir, QMap<TreeModelItem *, TreeModelItemData *> &staleItemsMapOut);
	TreeModelItem *FindFileContents(QFileInfo currentSyncFile, const QMap<TreeModelItem *, TreeModelItemData *> &staleItemsMap);
	bool SyncFile(QFileInfo currentSyncFile, QMap<TreeModelItem *, TreeModelItemData *> &staleItemsMapOut);

	void ErrorCheck();
};


#endif // DlgSyncAssets_H
