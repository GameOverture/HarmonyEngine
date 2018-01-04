/**************************************************************************
 *	ExplorerTreeWidget.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2017 Jason Knobler
 *
 *	Harmony Designer Tool License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef EXPLORERTREEWIDGET_H
#define EXPLORERTREEWIDGET_H

#include "ProjectItem.h"

#include <QTreeWidget>

class ExplorerWidget;

class ExplorerTreeWidget : public QTreeWidget
{
	ExplorerWidget *         m_pOwnerWidget;

	ProjectItem *           m_pDraggedProjItem;
	QPoint                  m_ptDragStart;

public:
	ExplorerTreeWidget(QWidget *pParent = nullptr);
	virtual ~ExplorerTreeWidget();

	void SetOwner(ExplorerWidget *pOwner);

protected:
	virtual void mousePressEvent(QMouseEvent *pEvent) override;
	virtual void mouseMoveEvent(QMouseEvent *pEvent) override;

	virtual void dragEnterEvent(QDragEnterEvent *pEvent) override;
	virtual void dropEvent(QDropEvent *pEvent) override;
};

#endif // EXPLORERTREEWIDGET_H
