/**************************************************************************
*	EntityDraw.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Global.h"
#include "EntityDraw.h"
#include "EntityWidget.h"
#include "MainWindow.h"
#include "HarmonyWidget.h"

#include <QKeyEvent>

EntityDraw::EntityDraw(ProjectItemData *pProjItem, const FileDataPair &initFileDataRef) :
	IDraw(pProjItem, initFileDataRef)
{
	m_Transform.SetDisplayOrder(9999999);
	m_Transform.SetVisible(false);
}

/*virtual*/ EntityDraw::~EntityDraw()
{
	SetEverythingStale();
	DeleteStaleChildren();
}

/*virtual*/ void EntityDraw::OnKeyPressEvent(QKeyEvent *pEvent) /*override*/
{
	IDraw::OnKeyPressEvent(pEvent);
}

/*virtual*/ void EntityDraw::OnKeyReleaseEvent(QKeyEvent *pEvent) /*override*/
{
	IDraw::OnKeyReleaseEvent(pEvent);
}

/*virtual*/ void EntityDraw::OnMousePressEvent(QMouseEvent *pEvent) /*override*/
{
	IDraw::OnMousePressEvent(pEvent);

	if(pEvent->button() == Qt::LeftButton)
	{
		if(m_bPanCameraKeyDown && m_bIsCameraPanning == false)
		{
			m_bIsCameraPanning = true;
			m_ptOldMousePos = pEvent->localPos();
			Harmony::GetWidget(&m_pProjItem->GetProject())->SetCursor(Qt::ClosedHandCursor);
		}
	}
}

/*virtual*/ void EntityDraw::OnMouseReleaseEvent(QMouseEvent *pEvent) /*override*/
{
	IDraw::OnMouseReleaseEvent(pEvent);
}

/*virtual*/ void EntityDraw::OnMouseWheelEvent(QWheelEvent *pEvent) /*override*/
{
	IDraw::OnMouseWheelEvent(pEvent);
}

/*virtual*/ void EntityDraw::OnMouseMoveEvent(QMouseEvent *pEvent) /*override*/
{
	IDraw::OnMouseMoveEvent(pEvent);

	if(m_bPanCameraKeyDown)
		RefreshTransforms();
	else
	{
		QPointF ptCurMousePos = pEvent->localPos();
		
		// Iterate backwards since those generally have higher display ordering
		//for(int32 i = m_ItemList.size() - 1; i >= 0; --i)
		//{
		//	if(m_ItemList[i]->IsMouseInBounds())
		//	{

		//		Harmony::GetWidget(&m_pProjItem->GetProject())->SetCursor(Qt::PointingHandCursor);

		//	}
		//	else
		//		Harmony::GetWidget(&m_pProjItem->GetProject())->RestoreCursor();
		//}
	}
}

void EntityDraw::RefreshSelectedItems()
{
	QList<EntityItemDraw *> selectedDrawList = GetSelectedItems();

	//selectedDrawList
	
}

/*virtual*/ void EntityDraw::OnApplyJsonMeta(QJsonObject &itemMetaObj) /*override*/
{
	SetEverythingStale();

	QJsonArray childArray = itemMetaObj["childList"].toArray();
	for(int32 i = 0; i < childArray.size(); ++i)
	{
		QJsonObject childObj = childArray[i].toObject();

		HyGuiItemType eType = HyGlobal::GetTypeFromString(childObj["itemType"].toString());
		QUuid uuid(childObj["itemUUID"].toString());
		EntityItemDraw *pItemWidget = FindStaleChild(eType, uuid);
		if(pItemWidget == nullptr)
		{
			pItemWidget = new EntityItemDraw(eType, uuid, this);
			m_ItemList.push_back(pItemWidget);
		}

		pItemWidget->RefreshJson(m_pCamera, childObj);
	}
	DeleteStaleChildren();

	QJsonArray shapeArray = itemMetaObj["shapeList"].toArray();
	for(int32 i = 0; i < shapeArray.size(); ++i)
	{
		QJsonObject shapeObj = shapeArray[i].toObject();
	}
}

/*virtual*/ void EntityDraw::OnShow() /*override*/
{
	SetVisible(true);
}

/*virtual*/ void EntityDraw::OnHide() /*override*/
{
	SetVisible(false);
}

/*virtual*/ void EntityDraw::OnResizeRenderer() /*override*/
{
	RefreshTransforms();
}

/*virtual*/ void EntityDraw::OnZoom(HyZoomLevel eZoomLevel) /*override*/
{
	RefreshTransforms();
}

void EntityDraw::SetEverythingStale()
{
	for(EntityItemDraw *pItem : m_ItemList)
		pItem->SetStale();
}

EntityItemDraw *EntityDraw::FindStaleChild(HyGuiItemType eType, QUuid uuid)
{
	for(EntityItemDraw *pItem : m_ItemList)
	{
		if(pItem->GetGuiType() == eType && pItem->GetUuid() == uuid && pItem->IsStale())
			return pItem;
	}

	return nullptr;
}

void EntityDraw::DeleteStaleChildren()
{
	for(auto iter = m_ItemList.begin(); iter != m_ItemList.end(); )
	{
		if((*iter)->IsStale())
		{
			delete (*iter);
			iter = m_ItemList.erase(iter);
		}
		else
			++iter;
	}
}

void EntityDraw::RefreshTransforms()
{
	QList<EntityItemDraw *> selectedItemDrawList = GetSelectedItems();
	if(selectedItemDrawList.empty())
		return;

	if(selectedItemDrawList.size() == 1)
		m_Transform.WrapTo(selectedItemDrawList[0], m_pCamera);

	m_Transform.WrapTo(selectedItemDrawList, m_pCamera);
}

QList<EntityItemDraw *> EntityDraw::GetSelectedItems() const
{
	EntityWidget *pWidget = static_cast<EntityWidget *>(m_pProjItem->GetWidget());
	QList<EntityTreeItemData *> selectedItemDataList = pWidget->GetSelectedItems(false, true);

	QList<EntityItemDraw *> matchedItemList;
	for(EntityTreeItemData *pTreeItemData : selectedItemDataList)
	{
		// Search for the corresponding EntityItemDraw to the current 'pTreeItemData'
		bool bFound = false;
		for(EntityItemDraw *pItemDraw : m_ItemList)
		{
			if(pItemDraw->GetUuid() == pTreeItemData->GetUuid())
			{
				matchedItemList.push_back(pItemDraw);
				bFound = true;
				break;
			}
		}

		if(bFound == false)
			HyGuiLog("EntityDraw::GetSelectedItems() could not find matching EntityItemDraw for a selected item", LOGTYPE_Error);
	}

	return matchedItemList;
}
