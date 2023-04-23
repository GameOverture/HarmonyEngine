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
class IAssetItemData;
class TreeModelItemData;

class ManagerProxyModel : public QSortFilterProxyModel
{
	int32						m_iFilterBankIndex;

public:
	ManagerProxyModel(QObject *pParent = nullptr);

	void FilterByBankIndex(int iBankIndex);

protected:
	virtual bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;
	virtual bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
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

public:
	explicit ManagerWidget(QWidget *pParent = nullptr);
	explicit ManagerWidget(IManagerModel *pModel, QWidget *pParent = nullptr);
	~ManagerWidget();

	IManagerModel &GetModel();
	quint32 GetSelectedBankId();

	void RefreshInfo();

	QStringList GetExpandedFilters();
	void RestoreExpandedState(QStringList expandedFilterList);

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// NOTE: ExplorerWidget::GetSelected is a synonymous function - all fixes/enhancements should be copied over until refactored into a base class
	TreeModelItemData *GetSelected(QList<IAssetItemData *> &selectedItemsOut, QList<TreeModelItemData *> &selectedPrefixesOut);

protected:
	virtual void enterEvent(QEvent *pEvent) override;
	virtual void leaveEvent(QEvent *pEvent) override;
	virtual void resizeEvent(QResizeEvent *event) override;

private Q_SLOTS:
	void OnContextMenu(const QPoint &pos);

	void on_actionAssetSettings_triggered();

	void on_actionDeleteAssets_triggered();

	void on_actionReplaceAssets_triggered();

	void on_assetTree_clicked();

	void on_actionRename_triggered();

	void on_cmbBanks_currentIndexChanged(int index);

	void on_actionAddBank_triggered();

	void on_actionBankSettings_triggered();

	void on_actionRemoveBank_triggered();

	void on_actionBankTransfer_triggered(QAction *pAction);

	void on_actionGenerateAsset_triggered();

	void on_actionImportAssets_triggered();

	void on_actionImportDirectory_triggered();

	void on_actionAddFilter_triggered();

	void on_chkShowAllBanks_clicked();

	void on_txtSearch_textChanged(const QString &text);

	void on_actionImportTileSheet_triggered();
	
private:
	Ui::ManagerWidget *ui;
};

#endif // ManagerWidget_H
