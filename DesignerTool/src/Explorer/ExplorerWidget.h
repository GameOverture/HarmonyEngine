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
class ExplorerTreeItem;
class AtlasFrame;

class DataExplorerLoadThread : public QThread
{
    Q_OBJECT

    QString m_sPath;

public:
    DataExplorerLoadThread(QString sPath, QObject *pParent) :   QThread(pParent),
                                                                m_sPath(sPath)
    { }

    virtual void run() override;

Q_SIGNALS:
    void LoadFinished(Project *pLoadedItemProject);
};

class ExplorerWidget : public QWidget
{
    Q_OBJECT

    static QByteArray       sm_sInternalClipboard;

public:
    explicit ExplorerWidget(QWidget *parent = 0);
    ~ExplorerWidget();
    
    Project *AddItemProject(const QString sNewProjectFilePath);

    ProjectItem *AddNewItem(Project *pProj, HyGuiItemType eNewItemType, const QString sPrefix, const QString sName, bool bOpenAfterAdd, QJsonValue initValue);
    void RemoveItem(ExplorerTreeItem *pItem);
    void SelectItem(ExplorerTreeItem *pItem);
    
    QStringList GetOpenProjectPaths();

    Project *GetCurProjSelected();
    ExplorerTreeItem *GetCurItemSelected();
    ExplorerTreeItem *GetCurSubDirSelected();

    void PasteItemSrc(QByteArray sSrc, Project *pProject);

private:
    Ui::ExplorerWidget *ui;

    QJsonObject ReplaceIdWithProperValue(QJsonObject srcObj, QSet<AtlasFrame *> importedFrames);

    QTreeWidgetItem *GetSelectedTreeItem();
    
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
