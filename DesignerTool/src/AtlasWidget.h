/**************************************************************************
 *	AtlasWidget.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef ATLASWIDGET_H
#define ATLASWIDGET_H

#include <QWidget>
#include <QDir>
#include <QMouseEvent>
#include <QStringListModel>
#include <QStackedWidget>

#include "AtlasModel.h"

namespace Ui {
class AtlasWidget;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class AtlasTreeWidget : public QTreeWidget
{
    AtlasWidget *      m_pOwner;

public:
    AtlasTreeWidget(QWidget *parent = Q_NULLPTR);
    void SetOwner(AtlasWidget *pOwner);

protected:
    virtual void dropEvent(QDropEvent *e) override;
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class AtlasTreeItem : public QTreeWidgetItem
{
public:
    AtlasTreeItem(AtlasTreeWidget *pView, int type = Type) : QTreeWidgetItem(pView, type)
    { }
    AtlasTreeItem(QTreeWidgetItem *parent, int type = Type) : QTreeWidgetItem(parent, type)
    { }

    bool operator<(const QTreeWidgetItem& other) const;
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class AtlasWidget : public QWidget
{
    Q_OBJECT

    friend class WidgetAtlasGroup;

    AtlasModel *                   m_pDataRef;


    QTreeWidgetItem *               m_pMouseHoverItem;

public:
    explicit AtlasWidget(QWidget *parent = 0);
    explicit AtlasWidget(AtlasModel &itemDataRef, QWidget *parent = 0);
    ~AtlasWidget();

    AtlasModel &GetData();

    QTreeWidget *GetFramesTreeWidget();

    friend void AtlasManager_DrawOpen(IHyApplication &hyApp, AtlasWidget &atlasMan);
    friend void AtlasManager_DrawShow(IHyApplication &hyApp, AtlasWidget &atlasMan);
    friend void AtlasManager_DrawHide(IHyApplication &hyApp, AtlasWidget &atlasMan);
    friend void AtlasManager_DrawUpdate(IHyApplication &hyApp, AtlasWidget &atlasMan);

private Q_SLOTS:
    void on_btnAddImages_clicked();

    void on_btnAddDir_clicked();

    void on_btnSettings_clicked();

    void on_actionDeleteImages_triggered();

    void on_actionReplaceImages_triggered();

    void on_actionAddFilter_triggered();

    void on_atlasList_itemSelectionChanged();

private:
    Ui::AtlasWidget *ui;

    void PreviewAtlasGroup();
    void HideAtlasGroup();

protected:
    virtual void resizeEvent(QResizeEvent *event) override;
};

#endif // ATLASWIDGET_H
