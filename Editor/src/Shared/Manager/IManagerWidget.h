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

#include "AtlasDraw.h"

#include <QSortFilterProxyModel>
#include <QWidget>
#include <QMenu>

namespace Ui {
class IManagerWidget;
}

class IManagerModel;

class ManagerProxyModel : public QSortFilterProxyModel
{
public:
	ManagerProxyModel(QObject *pParent = nullptr);
	virtual bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;
};

class IManagerWidget : public QWidget
{
	Q_OBJECT

	IManagerModel *				m_pModel;
	AtlasDraw					m_Draw;

public:
	explicit IManagerWidget(QWidget *pParent = nullptr);
	explicit IManagerWidget(IManagerModel *pModel, QWidget *pParent = nullptr);
	~IManagerWidget();

	IManagerModel &GetModel();

	quint32 GetSelectedBankId();

	void DrawUpdate();
	void RefreshInfo();

protected:
	virtual void enterEvent(QEvent *pEvent) override;
	virtual void leaveEvent(QEvent *pEvent) override;
	virtual void resizeEvent(QResizeEvent *event) override;

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

	void on_actionImportAssets_triggered();

	void on_actionImportDirectory_triggered();

	void on_actionAddFilter_triggered();
	
private:
	Ui::IManagerWidget *ui;

	void GetSelectedItems(QList<AssetItemData *> &selectedItemsOut, QList<TreeModelItemData *> &selectedPrefixesOut);
	//void GetSelectedItemsRecursively(QList<QTreeWidgetItem *> selectedTreeItems, QList<QTreeWidgetItem *> &frameListRef, QList<QTreeWidgetItem *> &filterListRef);
};

#endif // IMANAGERWIDGET_H
