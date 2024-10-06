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
class TreeModelItemData;

class DlgSyncAssets : public QDialog
{
	Q_OBJECT

	IManagerModel &			m_AssManRef;
	TreeModelItemData *		m_pSyncFilterRoot;



public:
	explicit DlgSyncAssets(IManagerModel &assetManagerRef, TreeModelItemData *pSyncFilterRoot, QWidget *pParent = nullptr);
	~DlgSyncAssets();

private Q_SLOTS:
	void on_buttonBox_accepted();
	void on_btnBrowse_clicked();

	void on_txtDirectoryPath_textEdited(const QString &sText);

private:
	Ui::DlgSyncAssets *ui;

	void ScanForActions();

	void ErrorCheck();
};


#endif // DlgSyncAssets_H
