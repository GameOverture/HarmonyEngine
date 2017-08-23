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

#include "HyGuiGlobal.h"

#include <QWidget>
#include <QDir>
#include <QTreeWidget>
#include <QMenu>
#include <QThread>

#include <vector>
using std::vector;

namespace Ui {
class DataExplorerWidget;
}

class Project;
class ProjectItem;
class DataExplorerItem;
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

class DataExplorerWidget : public QWidget
{
    Q_OBJECT

    static QByteArray       sm_sInternalClipboard;

    QPoint                  m_ptDragStart;

public:
    explicit DataExplorerWidget(QWidget *parent = 0);
    ~DataExplorerWidget();
    
    Project *AddItemProject(const QString sNewProjectFilePath);

    ProjectItem *AddNewItem(Project *pProj, HyGuiItemType eNewItemType, const QString sPrefix, const QString sName, bool bOpenAfterAdd, QJsonValue initValue);
    void RemoveItem(DataExplorerItem *pItem);
    void SelectItem(DataExplorerItem *pItem);
    
    QStringList GetOpenProjectPaths();

    Project *GetCurProjSelected();
    DataExplorerItem *GetCurItemSelected();
    DataExplorerItem *GetCurSubDirSelected();

    void PasteItemSrc(QByteArray sSrc, Project *pProject);

private:
    Ui::DataExplorerWidget *ui;

    QJsonObject ReplaceIdWithProperValue(QJsonObject srcObj, QSet<AtlasFrame *> importedFrames);

    QTreeWidgetItem *GetSelectedTreeItem();

protected:
    virtual void mousePressEvent(QMouseEvent *pEvent) override;
    virtual void mouseMoveEvent(QMouseEvent *pEvent) override;
    
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
