/**************************************************************************
*	ManagerWidget.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2020 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef ManagerWidget_H
#define ManagerWidget_H

#include "IManagerDraw.h"

#include <QSortFilterProxyModel>
#include <QWidget>
#include <QMenu>

namespace Ui {
class ManagerWidget;
}

class IManagerModel;
class AssetItemData;
class TreeModelItemData;

class ManagerProxyModel : public QSortFilterProxyModel
{
public:
	ManagerProxyModel(QObject *pParent = nullptr);
	virtual bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;
};

class ManagerTreeView : public QTreeView
{
	Q_OBJECT

public:
	ManagerTreeView(QWidget *pParent = nullptr);

protected:
	virtual void startDrag(Qt::DropActions supportedActions) override;
};

class ManagerWidget : public QWidget
{
	Q_OBJECT

	IManagerModel *				m_pModel;
	IManagerDraw *				m_pDraw;

public:
	explicit ManagerWidget(QWidget *pParent = nullptr);
	explicit ManagerWidget(IManagerModel *pModel, QWidget *pParent = nullptr);
	~ManagerWidget();

	IManagerModel &GetModel();

	quint32 GetSelectedBankId();

	void DrawUpdate();
	void RefreshInfo();

	//void OnAssetTreeCurrentChanged(const QModelIndex &current, const QModelIndex &previous);

protected:
	virtual void enterEvent(QEvent *pEvent) override;
	virtual void leaveEvent(QEvent *pEvent) override;
	virtual void resizeEvent(QResizeEvent *event) override;

private Q_SLOTS:
	void OnContextMenu(const QPoint &pos);

	void on_actionDeleteAssets_triggered();

	void on_actionReplaceAssets_triggered();

	void on_assetTree_clicked();

	void on_actionRename_triggered();

	void on_cmbBanks_currentIndexChanged(int index);

	void on_actionAddBank_triggered();

	void on_actionBankSettings_triggered();

	void on_actionRemoveBank_triggered();

	void on_actionBankTransfer_triggered(QAction *pAction);

	void on_actionImportAssets_triggered();

	void on_actionImportDirectory_triggered();

	void on_actionAddFilter_triggered();
	
private:
	Ui::ManagerWidget *ui;

public:
	void GetSelectedItems(QList<AssetItemData *> &selectedItemsOut, QList<TreeModelItemData *> &selectedPrefixesOut);
	//void GetSelectedItemsRecursively(QList<QTreeWidgetItem *> selectedTreeItems, QList<QTreeWidgetItem *> &frameListRef, QList<QTreeWidgetItem *> &filterListRef);
};

#endif // ManagerWidget_H
