/**************************************************************************
*	IManagerWidget.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2020 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef IMANAGERWIDGET_H
#define IMANAGERWIDGET_H

#include <QWidget>
#include <QMenu>

namespace Ui {
class IManagerWidget;
}

class IStateData;
class ProjectItemData;

class IManagerWidget : public QWidget
{
	Q_OBJECT

public:
	explicit IManagerWidget(QWidget *pParent = nullptr);
	~IManagerWidget();

private Q_SLOTS:
	void OnContextMenu(const QPoint &pos);

	void on_actionDeleteAssets_triggered();

	void on_actionReplaceAssets_triggered();

	void on_assetTree_itemSelectionChanged();

	void on_actionRename_triggered();

	void on_cmbBanks_currentIndexChanged(int index);

	void on_actionAddBank_triggered();

	void on_actionBankSettings_triggered();

	void on_actionRemoveBank_triggered();

	void on_actionBankTransfer_triggered(QAction *pAction);

	void on_actionImportImages_triggered();

	void on_actionImportDirectory_triggered();

	void on_actionAddFilter_triggered();
	
private:
	Ui::IManagerWidget *ui;
};

#endif // IMANAGERWIDGET_H
