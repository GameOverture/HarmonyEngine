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

class ExplorerProxyModel : public QSortFilterProxyModel
{
public:
	ExplorerProxyModel(QObject *pParent = nullptr);
	virtual bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;
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

public:
	explicit ExplorerWidget(QWidget *pParent = 0);
	~ExplorerWidget();

	void SetModel(ExplorerModel &modelRef);
	ExplorerModel *GetExplorerModel();

	ExplorerItemData *GetFirstSelectedItem();
	void GetSelectedItems(QList<ExplorerItemData *> &selectedItemsOut, QList<ExplorerItemData *> &selectedPrefixesOut);

private:
	Ui::ExplorerWidget *ui;
	
private Q_SLOTS:
	void OnContextMenu(const QPoint &pos);
	void on_treeView_doubleClicked(QModelIndex index);
	void on_treeView_clicked(QModelIndex index);

	void on_actionRename_triggered();
	
	void on_actionDeleteItem_triggered();

	void on_actionCopyItem_triggered();

	void on_actionPasteItem_triggered();

	void on_actionOpen_triggered();
};

#endif // DATAEXPLORERWIDGET_H
