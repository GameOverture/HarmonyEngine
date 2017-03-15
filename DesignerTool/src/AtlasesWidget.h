/**************************************************************************
 *	WidgetAtlasManager.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef ATLASESWIDGET_H
#define ATLASESWIDGET_H

#include <QWidget>
#include <QDir>
#include <QMouseEvent>
#include <QStringListModel>
#include <QStackedWidget>

#include "AtlasesData.h"

namespace Ui {
class AtlasesWidget;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class AtlasesTreeWidget : public QTreeWidget
{
    AtlasesWidget *      m_pOwner;

public:
    AtlasesTreeWidget(QWidget *parent = Q_NULLPTR);
    void SetOwner(AtlasesWidget *pOwner);

protected:
    virtual void dropEvent(QDropEvent *e) override;
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class AtlasTreeItem : public QTreeWidgetItem
{
public:
    AtlasTreeItem(AtlasesTreeWidget *pView, int type = Type) : QTreeWidgetItem(pView, type)
    { }
    AtlasTreeItem(QTreeWidgetItem *parent, int type = Type) : QTreeWidgetItem(parent, type)
    { }

    bool operator<(const QTreeWidgetItem& other) const;
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class AtlasesWidget : public QWidget
{
    Q_OBJECT

    friend class WidgetAtlasGroup;

    AtlasesData *                   m_pDataRef;

    QTreeWidgetItem *               m_pMouseHoverItem;

public:
    explicit AtlasesWidget(QWidget *parent = 0);
    explicit AtlasesWidget(AtlasesData &itemDataRef, QWidget *parent = 0);
    ~AtlasesWidget();

    AtlasesData &GetData();

    QTreeWidget *GetFramesTreeWidget();

    friend void AtlasManager_DrawOpen(IHyApplication &hyApp, AtlasesWidget &atlasMan);
    friend void AtlasManager_DrawShow(IHyApplication &hyApp, AtlasesWidget &atlasMan);
    friend void AtlasManager_DrawHide(IHyApplication &hyApp, AtlasesWidget &atlasMan);
    friend void AtlasManager_DrawUpdate(IHyApplication &hyApp, AtlasesWidget &atlasMan);

private Q_SLOTS:
    void on_btnAddImages_clicked();

    void on_btnAddDir_clicked();

    void on_btnSettings_clicked();

    void on_actionDeleteImages_triggered();

    void on_actionReplaceImages_triggered();

    void on_actionAddFilter_triggered();

    void on_atlasList_itemSelectionChanged();

private:
    Ui::AtlasesWidget *ui;

    void PreviewAtlasGroup();
    void HideAtlasGroup();

protected:
    virtual void resizeEvent(QResizeEvent *event) override;
};

#endif // ATLASESWIDGET_H
