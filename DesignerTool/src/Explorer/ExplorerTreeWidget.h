/**************************************************************************
 *	ExplorerTreeWidget.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2017 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef EXPLORERTREEWIDGET_H
#define EXPLORERTREEWIDGET_H

#include "ProjectItem.h"

#include <QTreeWidget>

class ProjectWidget;

class ExplorerTreeWidget : public QTreeWidget
{
    ProjectWidget *         m_pOwnerWidget;

    ProjectItem *           m_pDraggedProjItem;
    QPoint                  m_ptDragStart;

public:
    ExplorerTreeWidget(QWidget *pParent = nullptr);
    virtual ~ExplorerTreeWidget();

    void SetOwner(ProjectWidget *pOwner);

protected:
    virtual void mousePressEvent(QMouseEvent *pEvent) override;
    virtual void mouseMoveEvent(QMouseEvent *pEvent) override;

    virtual void dragEnterEvent(QDragEnterEvent *pEvent) override;
    virtual void dropEvent(QDropEvent *pEvent) override;
};

#endif // EXPLORERTREEWIDGET_H
