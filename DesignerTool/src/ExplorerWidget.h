/**************************************************************************
 *	ExplorerWidget.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef EXPLORERWIDGET_H
#define EXPLORERWIDGET_H

#include <QWidget>
#include <QDir>
#include <QTreeWidget>
#include <QMenu>
#include <QThread>

#include "Project.h"

#include <vector>
using std::vector;

namespace Ui {
class ExplorerWidget;
}

class ExplorerLoadThread : public QThread
{
    Q_OBJECT

    QString m_sPath;

public:
    ExplorerLoadThread(QString sPath, QObject *pParent) :    QThread(pParent),
                                                                m_sPath(sPath)
    { }

    void run() Q_DECL_OVERRIDE
    {
        /* ... here is the expensive or blocking operation ... */
        Project *pNewItemProject = new Project(m_sPath);
        Q_EMIT LoadFinished(pNewItemProject);
    }
Q_SIGNALS:
    void LoadFinished(Project *pLoadedItemProject);
};

class ExplorerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ExplorerWidget(QWidget *parent = 0);
    ~ExplorerWidget();
    
    Project *AddItemProject(const QString sNewProjectFilePath);
    void AddNewItem(HyGuiItemType eNewItemType, const QString sPrefix, const QString sName);
    void RemoveItem(ExplorerItem *pItem);
    
    void SelectItem(ExplorerItem *pItem);
    
    Project *GetCurProjSelected();
    ExplorerItem *GetCurItemSelected();
    ExplorerItem *GetCurSubDirSelected();
    
    QStringList GetOpenProjectPaths();

private:
    Ui::ExplorerWidget *ui;
    
    QTreeWidgetItem *GetSelectedTreeItem();
    
private Q_SLOTS:
    void OnProjectLoaded(Project *pLoadedProj);
    void OnContextMenu(const QPoint &pos);
    void on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column);
    void on_treeWidget_itemSelectionChanged();

    void on_actionRename_triggered();
    
    void on_actionDeleteItem_triggered();
    
Q_SIGNALS:
    void LoadItemProject();
};

#endif // EXPLORERWIDGET_H
