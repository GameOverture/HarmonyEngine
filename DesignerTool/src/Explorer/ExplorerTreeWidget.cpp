/**************************************************************************
 *	ExplorerTreeWidget.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2017 Jason Knobler
 *
 *	Harmony Designer Tool License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
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
		ProjectItemMimeData *pNewMimeData = new ProjectItemMimeData(m_pDraggedProjItem);

		QDrag *pDrag = new QDrag(m_pDraggedProjItem);
		pDrag->setMimeData(pNewMimeData);

		Qt::DropAction dropAction = pDrag->exec(Qt::LinkAction);
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
	else
		pEvent->ignore();

	QTreeWidget::dragEnterEvent(pEvent);
}

/*virtual*/ void ExplorerTreeWidget::dropEvent(QDropEvent *pEvent) /*override*/
{
	if(pEvent->mimeData()->hasFormat(HYGUI_MIMETYPE))
	{
		QByteArray dragDataSrc = pEvent->mimeData()->data(HYGUI_MIMETYPE);
		QJsonDocument userDoc = QJsonDocument::fromJson(dragDataSrc);

		Project &projRef = static_cast<ProjectItem *>(pEvent->source())->GetProject();

		QJsonObject dragObj = userDoc.object();
		if(dragObj["project"].toString().toLower() != projRef.GetAbsPath().toLower())
		{
			pEvent->acceptProposedAction();
			m_pOwnerWidget->PasteItemSrc(dragDataSrc, &projRef);
		}
		else
			pEvent->ignore();
	}
	else
		pEvent->ignore();


	sortItems(0, Qt::AscendingOrder);
	//m_pOwner->GetData().WriteMetaSettings();

	QTreeWidget::dropEvent(pEvent);
}
