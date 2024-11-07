/**************************************************************************
 *	ExplorerWidget.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef EXPLORERWIDGET_H
#define EXPLORERWIDGET_H

#include "Global.h"

#include <QWidget>
#include <QDir>
#include <QTreeWidget>
#include <QMenu>
#include <QThread>
#include <QSortFilterProxyModel>

#include <vector>
using std::vector;

namespace Ui {
class ExplorerWidget;
}

class ExplorerModel;
class Project;
class ExplorerItemData;
class ProjectItemData;

enum ExplorerItemFilterFlags
{
	ITEMFILTER_Sprite	= 1 << 0,
	ITEMFILTER_TileMap	= 1 << 1,
	ITEMFILTER_Text		= 1 << 2,
	ITEMFILTER_Spine	= 1 << 3,
	ITEMFILTER_Audio	= 1 << 4,
	ITEMFILTER_Entity	= 1 << 5,

	ITEMFILTER_All = (ITEMFILTER_Sprite | ITEMFILTER_TileMap | ITEMFILTER_Text | ITEMFILTER_Spine | ITEMFILTER_Audio | ITEMFILTER_Entity)
};

class ExplorerProxyModel : public QSortFilterProxyModel
{
	uint32				m_uiFilterFlags;

public:
	ExplorerProxyModel(QObject *pParent = nullptr);
	virtual bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;
	virtual bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

	void FilterByType(uint32 uiFilterFlags);

protected:
	bool IsTypeFiltered(ItemType eType) const;
};

class ExplorerTreeView : public QTreeView
{
	Q_OBJECT

public:
	ExplorerTreeView(QWidget *pParent = nullptr);

protected:
	virtual void startDrag(Qt::DropActions supportedActions) override;
};

class ExplorerWidget : public QWidget
{
	Q_OBJECT

	QActionGroup		m_FilterActionGroup;

public:
	explicit ExplorerWidget(QWidget *pParent = 0);
	~ExplorerWidget();

	void SetModel(ExplorerModel &modelRef);
	ExplorerModel *GetExplorerModel();

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// NOTE: ManagerWidget::GetSelected is a synonymous function - all fixes/enhancements should be copied over until refactored into a base class
	ExplorerItemData *GetSelected();
	void GetSelected(QList<ProjectItemData *> &selectedItemsOut, QList<ExplorerItemData *> &selectedPrefixesOut, bool bSortAlphabetically);

private:
	Ui::ExplorerWidget *ui;

	void OnFilterUpdate();
	void OnClickedItem(ExplorerItemData *pItemClicked);
	
private Q_SLOTS:
	void OnContextMenu(const QPoint &pos);

	void on_actionFilterAll_triggered();
	void on_actionFilterSprite_triggered();
	void on_actionFilterText_triggered();
	void on_actionFilterSpine_triggered();
	void on_actionFilterAudio_triggered();
	void on_actionFilterEntity_triggered();

	void on_txtSearch_textChanged(const QString &text);

	void on_treeView_doubleClicked(QModelIndex index);
	void on_treeView_clicked(QModelIndex index);

	void on_actionRename_triggered();
	
	void on_actionDeleteItem_triggered();

	void on_actionCopyItem_triggered();

	void on_actionPasteItem_triggered();

	void on_actionOpen_triggered();
};

#endif // DATAEXPLORERWIDGET_H
