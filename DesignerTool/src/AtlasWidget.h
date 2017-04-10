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

#include "AtlasModel.h"
#include "AtlasDraw.h"

#include <QWidget>
#include <QDir>
#include <QMouseEvent>
#include <QStringListModel>
#include <QStackedWidget>


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

    AtlasModel *                    m_pModel;
    AtlasDraw                       m_Draw;

    QTreeWidgetItem *               m_pMouseHoverItem;

public:
    explicit AtlasWidget(QWidget *parent = 0);
    explicit AtlasWidget(AtlasModel *pModel, IHyApplication *pHyApp, QWidget *parent = 0);
    ~AtlasWidget();

    AtlasModel &GetData();
    QTreeWidget *GetFramesTreeWidget();

    void DrawUpdate(IHyApplication &hyApp);

    void StashTreeWidgets();
    
protected:
    virtual void enterEvent(QEvent *pEvent) override;
    virtual void leaveEvent(QEvent *pEvent) override;
    virtual void resizeEvent(QResizeEvent *event) override;

private Q_SLOTS:
    void on_btnAddImages_clicked();

    void on_btnAddDir_clicked();

    void on_btnSettings_clicked();

    void on_actionDeleteImages_triggered();

    void on_actionReplaceImages_triggered();

    void on_actionAddFilter_triggered();

    void on_atlasList_itemSelectionChanged();

    void OnContextMenu(const QPoint &pos);

    void on_actionRename_triggered();

private:
    Ui::AtlasWidget *ui;

    void GetSelectedItemsRecursively(QList<QTreeWidgetItem *> selectedTreeItems, QList<QTreeWidgetItem *> &frameListRef, QList<QTreeWidgetItem *> &filterListRef);
};

#endif // ATLASWIDGET_H
