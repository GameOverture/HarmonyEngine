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
#include <QLabel>

namespace Ui {
class ManagerWidget;
}

class IManagerModel;
class IAssetItemData;
class TreeModelItemData;
class QGroupBox;
class QToolButton;
class QComboBox;

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
	virtual void mousePressEvent(QMouseEvent *event) override;
	virtual void startDrag(Qt::DropActions supportedActions) override;
	virtual void dropEvent(QDropEvent *pEvent) override;
};

class ManagerWidget : public QWidget
{
	Q_OBJECT

	IManagerModel *				m_pModel;
	
	// These two variables are used to keep track of the specific clicked item when the context menu is opened, instead of using GetSelected()
	TreeModelItemData *			m_pContextMenuSelection;
	bool						m_bUseContextMenuSelection;

	// Only used with Code Manager
	struct CodeWidgets
	{
		QGroupBox *				m_pGrp;
		QToolButton *			m_pSettingsBtn;
		QFrame *				m_pDividerLine;
		QToolButton *			m_pNewBtn;
		QComboBox *				m_pCmb;
		QToolButton *			m_pOpenBtn;
		QToolButton *			m_pPackageBtn;
		QToolButton *			m_pDeleteBtn;

		QLabel *				m_pLblInfo;

		CodeWidgets(ManagerWidget *pManagerWidget, Ui::ManagerWidget *pUi);
		void AssembleComboBox();
		void RefreshInfo();
		QString GetCurrentBuildUrl() const;
	};
	CodeWidgets *				m_pCodeWidgets;

public:
	explicit ManagerWidget(QWidget *pParent = nullptr);
	explicit ManagerWidget(IManagerModel *pModel, QWidget *pParent = nullptr);
	~ManagerWidget();

	IManagerModel &GetModel();
	quint32 GetSelectedBankId() const;
	int GetSelectedBankIndex() const;
	void SetSelectedBankIndex(int iBankIndex);

	int GetSelectedBuildIndex() const;
	void SetSelectedBuildIndex(int iBuildIndex);

	bool IsShowAllBanksChecked() const;
	void SetShowAllBanksChecked(bool bShowAllBanks);

	void RefreshInfo();

	QStringList GetExpandedFilters();
	void RestoreExpandedState(QStringList expandedFilterList);

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// NOTE: ExplorerWidget::GetSelected are synonymous functions - all fixes/enhancements should be copied over until refactored into a base class
	TreeModelItemData *GetSelected();
	void GetSelected(QList<IAssetItemData *> &selectedAssetsOut, QList<TreeModelItemData *> &selectedFiltersOut, bool bSortAlphabetically);

	QList<TreeModelItemData *> GetItemsRecursively(TreeModelItemData *pRootItemData) const; // Returned list includes the item at indexRef

protected:
	virtual void enterEvent(QEnterEvent *pEvent) override;
	virtual void leaveEvent(QEvent *pEvent) override;
	virtual void resizeEvent(QResizeEvent *event) override;

private Q_SLOTS:
	void OnContextMenu(const QPoint &pos);
	void OnAssetTreeSelectionChanged(const QModelIndex &current, const QModelIndex &previous);

	void on_assetTree_pressed(const QModelIndex &index);
	void on_assetTree_doubleClicked(const QModelIndex &index);

	void on_cmbBanks_currentIndexChanged(int index);

	void on_actionAssetSettings_triggered();
	void on_actionOpenAssetExplorer_triggered();
	void on_actionDeleteAssets_triggered();
	void on_actionReplaceAssets_triggered();
	void on_actionRename_triggered();
	void on_actionAddBank_triggered();
	void on_actionBankSettings_triggered();
	void on_actionOpenBankExplorer_triggered();
	void on_actionRemoveBank_triggered();
	void on_actionBankTransfer_triggered(QAction *pAction);
	void on_actionAddClassFiles_triggered();
	void on_actionSliceSpriteSheet_triggered();
	void on_actionImportAssets_triggered();
	void on_actionImportDirectory_triggered();
	void on_actionSyncFilterAssets_triggered();
	void on_actionAddFilter_triggered();
	void on_actionCreateTileSet_triggered();

	void on_chkShowAllBanks_clicked();

	void on_txtSearch_textChanged(const QString &text);

	void on_actionBuildSettings_triggered();
	void on_actionNewBuild_triggered();
	void on_actionOpenBuild_triggered();
	void on_actionPackageBuild_triggered();
	void on_actionDeleteBuild_triggered();

	void OnBuildIndex(int iIndex);
	
private:
	Ui::ManagerWidget *ui;

private Q_SLOTS:
	void OnProcessStdOut();
	void OnProcessErrorOut();
};

#endif // ManagerWidget_H
