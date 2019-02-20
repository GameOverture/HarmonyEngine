/**************************************************************************
 *	ExplorerTreeWidget.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2017 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef EXPLORERTREEWIDGET_H
#define EXPLORERTREEWIDGET_H

#include "ProjectItem.h"

#include <QTreeWidget>

class ExplorerWidget;

class ExplorerTreeWidget : public QTreeWidget
{
	ExplorerWidget *			m_pOwnerWidget;

	ExplorerItem *				m_pDraggedItem;
	QPoint						m_ptDragStart;

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
