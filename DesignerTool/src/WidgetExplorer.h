/**************************************************************************
 *	WidgetExplorer.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef WIDGETEXPLORER_H
#define WIDGETEXPLORER_H

#include <QWidget>
#include <QDir>
#include <QTreeWidget>
#include <QMenu>
#include <QThread>

#include "Project.h"

#include <vector>
using std::vector;

namespace Ui {
class WidgetExplorer;
}

class WidgetExplorerLoadThread : public QThread
{
    Q_OBJECT

    QString m_sPath;

public:
    WidgetExplorerLoadThread(QString sPath, QObject *pParent) :    QThread(pParent),
                                                                m_sPath(sPath)
    { }

    void run() Q_DECL_OVERRIDE
    {
        /* ... here is the expensive or blocking operation ... */
        ItemProject *pNewItemProject = new ItemProject(m_sPath);
        Q_EMIT LoadFinished(pNewItemProject);
    }
Q_SIGNALS:
    void LoadFinished(ItemProject *pLoadedItemProject);
};

class WidgetExplorer : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetExplorer(QWidget *parent = 0);
    ~WidgetExplorer();
    
    void AddItemProject(const QString sNewProjectFilePath);
    void AddItem(eItemType eNewItemType, const QString sPrefix, const QString sName, bool bOpenAfterAdd);
    void RemoveItem(Item *pItem);
    
    void SelectItem(Item *pItem);
    
    ItemProject *GetCurProjSelected();
    Item *GetCurItemSelected();
    Item *GetCurDirSelected(bool bIncludePrefixDirs);
    //Item *GetItemByPath(QString sItemPathAbsolute);
    
    QStringList GetOpenProjectPaths();

private:
    Ui::WidgetExplorer *ui;
    
    void ProcessItem(Item *pItem);
    
    QTreeWidgetItem *GetSelectedTreeItem();
    
private Q_SLOTS:
    void OnProjectLoaded(ItemProject *pLoadedProj);
    void OnContextMenu(const QPoint &pos);
    void on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column);
    void on_treeWidget_itemSelectionChanged();

Q_SIGNALS:
    void LoadItemProject();
};

#endif // WIDGETEXPLORER_H
