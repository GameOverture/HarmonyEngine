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
class ExplorerItem;

class ExplorerProxyModel : public QSortFilterProxyModel
{
public:
	ExplorerProxyModel(QObject *pParent = nullptr);
	virtual bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;
};

class ExplorerWidget : public QWidget
{
	Q_OBJECT

	QMenu *					m_pNewItemMenuRef;

public:
	explicit ExplorerWidget(QWidget *pParent = 0);
	~ExplorerWidget();

	void SetModel(ExplorerModel &modelRef);
	ExplorerModel *GetModel();

	void SetItemMenuPtr(QMenu *pMenu);

	ExplorerItem *GetFirstSelectedItem();
	void GetSelectedItems(QList<ExplorerItem *> &selectedItemsOut, QList<ExplorerItem *> &selectedPrefixesOut);

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

Q_SIGNALS:
	void LoadItemProject();
};

#endif // DATAEXPLORERWIDGET_H
