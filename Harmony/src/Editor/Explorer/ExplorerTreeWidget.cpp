/**************************************************************************
 *	ExplorerTreeWidget.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2017 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "ExplorerTreeWidget.h"
#include "ExplorerWidget.h"
#include "ExplorerItem.h"
#include "ProjectItemMimeData.h"
#include "Project.h"

#include <QDrag>
#include <QMouseEvent>
#include <QApplication>

ExplorerTreeWidget::ExplorerTreeWidget(QWidget *pParent) :  QTreeWidget(pParent),
															m_pOwnerWidget(nullptr),
															m_pDraggedProjItem(nullptr)
{
	setContextMenuPolicy(Qt::CustomContextMenu);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setDragEnabled(true);
	setDropIndicatorShown(true);
	viewport()->setAcceptDrops(true);

	//setDragDropMode(QAbstractItemView::DragDrop);
	//setDragDropMode(QAbstractItemView::InternalMove);
}

/*virtual*/ ExplorerTreeWidget::~ExplorerTreeWidget()
{
}

void ExplorerTreeWidget::SetOwner(ExplorerWidget *pOwner)
{
	m_pOwnerWidget = pOwner;
}

/*virtual*/ void ExplorerTreeWidget::mousePressEvent(QMouseEvent *pEvent) /*override*/
{
	if(pEvent->button() == Qt::LeftButton)
	{
		QTreeWidgetItem *pClickedTreeItem = itemAt(mapFromGlobal(QCursor::pos()));
		if(pClickedTreeItem)
		{
			ExplorerItem *pExplorerItem = pClickedTreeItem->data(0, Qt::UserRole).value<ExplorerItem *>();

			if(pExplorerItem->IsProjectItem())
			{
				m_pDraggedProjItem = static_cast<ProjectItem *>(pExplorerItem);
				m_ptDragStart = pEvent->pos();
			}
		}
	}

	QTreeWidget::mousePressEvent(pEvent);
}

/*virtual*/ void ExplorerTreeWidget::mouseMoveEvent(QMouseEvent *pEvent) /*override*/
{
	if((pEvent->buttons() & Qt::LeftButton) == 0)
		m_pDraggedProjItem = nullptr;

	if(m_pDraggedProjItem != nullptr &&
	   (pEvent->pos() - m_ptDragStart).manhattanLength() >= QApplication::startDragDistance())
	{
		QDrag *pDrag = new QDrag(m_pDraggedProjItem);
		pDrag->setMimeData(new ProjectItemMimeData(m_pDraggedProjItem));
		pDrag->setPixmap(m_pDraggedProjItem->GetIcon(SUBICON_None).pixmap(16, 16));

		Qt::DropAction dropAction = pDrag->exec(Qt::CopyAction | Qt::MoveAction | Qt::LinkAction);

		int temp = 0;
		switch(dropAction)
		{
		case Qt::CopyAction:
			temp++;
			break;

		case Qt::MoveAction:
			temp++;
			break;

		case Qt::LinkAction:
			temp++;
			break;
		}
	}

	QTreeWidget::mouseMoveEvent(pEvent);
}

/*virtual*/ void ExplorerTreeWidget::dragEnterEvent(QDragEnterEvent *pEvent) /*override*/
{
	if(pEvent->mimeData()->hasFormat(HYGUI_MIMETYPE))
	{
		QByteArray dragDataSrc = pEvent->mimeData()->data(HYGUI_MIMETYPE);
		QJsonDocument userDoc = QJsonDocument::fromJson(dragDataSrc);

		Project &projRef = static_cast<ProjectItem *>(pEvent->source())->GetProject();

		QJsonObject dragObj = userDoc.object();
		if(dragObj["project"].toString().toLower() != projRef.GetAbsPath().toLower())
			pEvent->acceptProposedAction();
		else
			pEvent->ignore();
	}
	//else
	//	pEvent->ignore();

	QTreeWidget::dragEnterEvent(pEvent);
}

/*virtual*/ void ExplorerTreeWidget::dropEvent(QDropEvent *pEvent) /*override*/
{
	//if(pEvent->mimeData()->hasFormat(HYGUI_MIMETYPE))
	//{
	//	QByteArray dragDataSrc = pEvent->mimeData()->data(HYGUI_MIMETYPE);
	//	QJsonDocument userDoc = QJsonDocument::fromJson(dragDataSrc);

	//	Project &projRef = static_cast<ProjectItem *>(pEvent->source())->GetProject();

	//	QJsonObject dragObj = userDoc.object();
	//	if(dragObj["project"].toString().toLower() != projRef.GetAbsPath().toLower())
	//	{
	//		pEvent->acceptProposedAction();
	//		m_pOwnerWidget->PasteItemSrc(dragDataSrc, &projRef, static_cast<ProjectItem *>(pEvent->source())->GetPrefix());
	//	}
	//	else
	//		pEvent->ignore();
	//}
	////else
	////	pEvent->ignore();


	//sortItems(0, Qt::AscendingOrder);
	////m_pOwner->GetData().WriteMetaSettings();

	//QTreeWidget::dropEvent(pEvent);
}
