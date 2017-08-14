/**************************************************************************
 *	DataExplorerWidget.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef DATAEXPLORERWIDGET_H
#define DATAEXPLORERWIDGET_H

#include <QWidget>
#include <QDir>
#include <QTreeWidget>
#include <QMenu>
#include <QThread>

#include "Project.h"

#include <vector>
using std::vector;

namespace Ui {
class DataExplorerWidget;
}

class DataExplorerLoadThread : public QThread
{
    Q_OBJECT

    QString m_sPath;

public:
    DataExplorerLoadThread(QString sPath, QObject *pParent) :   QThread(pParent),
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

class DataExplorerWidget : public QWidget
{
    Q_OBJECT

    static QByteArray      sm_sInternalClipboard;

public:
    explicit DataExplorerWidget(QWidget *parent = 0);
    ~DataExplorerWidget();
    
    Project *AddItemProject(const QString sNewProjectFilePath);

    void AddNewItem(Project *pProj, HyGuiItemType eNewItemType, const QString sPrefix, const QString sName, bool bOpenAfterAdd, QJsonValue initValue);
    void RemoveItem(DataExplorerItem *pItem);
    void SelectItem(DataExplorerItem *pItem);
    
    QStringList GetOpenProjectPaths();

    Project *GetCurProjSelected();
    DataExplorerItem *GetCurItemSelected();
    DataExplorerItem *GetCurSubDirSelected();

    void PasteItemSrc(QByteArray sSrc, Project *pProject);

private:
    Ui::DataExplorerWidget *ui;
    
    QTreeWidgetItem *GetSelectedTreeItem();

    void PutItemOnClipboard(ProjectItem *pProjItem);
    
private Q_SLOTS:
    void OnProjectLoaded(Project *pLoadedProj);
    void OnContextMenu(const QPoint &pos);
    void on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column);
    void on_treeWidget_itemSelectionChanged();

    void on_actionRename_triggered();
    
    void on_actionDeleteItem_triggered();
    
    void on_actionCutItem_triggered();

    void on_actionCopyItem_triggered();

    void on_actionPasteItem_triggered();

Q_SIGNALS:
    void LoadItemProject();
};

#endif // DATAEXPLORERWIDGET_H
