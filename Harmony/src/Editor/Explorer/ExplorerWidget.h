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

#include <vector>
using std::vector;

namespace Ui {
class ExplorerWidget;
}

class Project;
class ProjectItem;
class ExplorerTreeWidget;
class ExplorerItem;
class AtlasFrame;

class ExplorerWidget : public QWidget
{
	Q_OBJECT

	QMenu *					m_pNewItemMenuRef;

public:
	explicit ExplorerWidget(QWidget *parent = 0);
	~ExplorerWidget();

	void SetItemMenuPtr(QMenu *pMenu);
	
	Project *AddProject(const QString sNewProjectFilePath);

	// If importValue doesn't equal null, then this new ProjectItem will be saved upon creation
	void AddItem(Project *pProj, HyGuiItemType eNewItemType, const QString sPrefix, const QString sName, bool bOpenAfterAdd, QJsonValue importValue);
	void RemoveItem(ExplorerItem *pItem);
	void SelectItem(ExplorerItem *pItem);
	
	QStringList GetOpenProjectPaths();

	Project *GetCurProjSelected();
	ExplorerItem *GetCurItemSelected();

	ExplorerTreeWidget *GetTreeWidget();

	void PasteItemSrc(QByteArray sSrc, Project *pProject, QString sPrefixOverride);

private:
	Ui::ExplorerWidget *ui;

	void RecursiveRemoveItem(ExplorerItem *pItem);

	QJsonObject ReplaceIdWithProperValue(QJsonObject srcObj, QSet<AtlasFrame *> importedFrames);

	QTreeWidgetItem *GetSelectedTreeItem();
	
private Q_SLOTS:
	void OnContextMenu(const QPoint &pos);
	void on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column);
	void on_treeWidget_itemSelectionChanged();

	void on_actionRename_triggered();
	
	void on_actionDeleteItem_triggered();

	void on_actionCopyItem_triggered();

	void on_actionPasteItem_triggered();

	void on_actionOpen_triggered();

Q_SIGNALS:
	void LoadItemProject();
};

#endif // DATAEXPLORERWIDGET_H
