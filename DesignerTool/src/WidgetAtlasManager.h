/**************************************************************************
 *	WidgetAtlasManager.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef WIDGETATLASMANAGER_H
#define WIDGETATLASMANAGER_H

#include <QWidget>
#include <QDir>
#include <QMouseEvent>
#include <QStringListModel>
#include <QStackedWidget>
#include <QThread>

#include "ItemAtlases.h"

namespace Ui {
class WidgetAtlasManager;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class WidgetAtlasGroupTreeWidget : public QTreeWidget
{
    WidgetAtlasManager *      m_pOwner;

public:
    WidgetAtlasGroupTreeWidget(QWidget *parent = Q_NULLPTR);
    void SetOwner(WidgetAtlasManager *pOwner);

protected:
    virtual void dropEvent(QDropEvent *e);
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class AtlasTreeItem : public QTreeWidgetItem
{
public:
    AtlasTreeItem(QTreeWidgetItem *parent, int type = Type) : QTreeWidgetItem(parent, type)
    { }

    bool operator<(const QTreeWidgetItem& other) const;
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class WidgetAtlasManager : public QWidget
{
    Q_OBJECT

    friend class WidgetAtlasGroup;

    ItemAtlases &                   m_DataRef;

    QDir                            m_MetaDir;
    QDir                            m_DataDir;
    
    QTreeWidgetItem *               m_pMouseHoverItem;

public:
    explicit WidgetAtlasManager(QWidget *parent = 0);
    explicit WidgetAtlasManager(ItemAtlases &itemDataRef, QWidget *parent = 0);
    ~WidgetAtlasManager();

    friend void AtlasManager_DrawOpen(IHyApplication &hyApp, WidgetAtlasManager &atlasMan);
    friend void AtlasManager_DrawShow(IHyApplication &hyApp, WidgetAtlasManager &atlasMan);
    friend void AtlasManager_DrawHide(IHyApplication &hyApp, WidgetAtlasManager &atlasMan);
    friend void AtlasManager_DrawUpdate(IHyApplication &hyApp, WidgetAtlasManager &atlasMan);

private Q_SLOTS:
    void on_btnAddImages_clicked();

    void on_btnAddDir_clicked();

    void on_btnSettings_clicked();

    void on_actionDeleteImages_triggered();

    void on_actionReplaceImages_triggered();

    void on_actionAddFilter_triggered();

    void on_atlasList_itemSelectionChanged();

private:
    Ui::WidgetAtlasManager *ui;

    void PreviewAtlasGroup();
    void HideAtlasGroup();

protected:
    virtual void resizeEvent(QResizeEvent *event);
};

#endif // WIDGETATLASMANAGER_H
