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
	m_MultiTransform.Hide();
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

void EntityDraw::OnSelectionChange(QList<EntityTreeItemData *> selectedItemDataList, QList<EntityTreeItemData *> deselectedItemDataList)
{
	for(EntityTreeItemData *pDeselectItem : deselectedItemDataList)
	{
		bool bFound = false;
		for(EntityDrawItem *pDrawItem : m_ItemList)
		{
			if(pDrawItem->GetThisUuid() == pDeselectItem->GetThisUuid())
			{
				m_SelectedItemList.removeOne(pDrawItem);

				pDrawItem->HideTransformCtrl();
				bFound = true;
				break;
			}
		}

		if(bFound == false)
			HyGuiLog("EntityDraw::OnSelectionChange() could not find matching EntityItemDraw to deselect item: " % pDeselectItem->GetCodeName(), LOGTYPE_Error);
	}

	for(EntityTreeItemData *pTreeItemData : selectedItemDataList)
	{
		// Search for the corresponding EntityItemDraw to the current 'pTreeItemData'
		bool bFound = false;
		for(EntityDrawItem *pDrawItem : m_ItemList)
		{
			if(pDrawItem->GetThisUuid() == pTreeItemData->GetThisUuid())
			{
				m_SelectedItemList.push_back(pDrawItem);
				bFound = true;
				break;
			}
		}

		if(bFound == false)
			HyGuiLog("EntityDraw::GetSelectedItems() could not find matching EntityItemDraw for a selected item: " % pTreeItemData->GetCodeName(), LOGTYPE_Error);
	}
	
	for(EntityDrawItem *pSelectedItemDraw : m_SelectedItemList)
		pSelectedItemDraw->ShowTransformCtrl(m_SelectedItemList.size() == 1);

	RefreshTransforms();
}

/*virtual*/ void EntityDraw::OnApplyJsonMeta(QJsonObject &itemMetaObj) /*override*/
{
	SetEverythingStale();

	QJsonArray childArray = itemMetaObj["childList"].toArray();
	for(int32 i = 0; i < childArray.size(); ++i)
	{
		QJsonObject childObj = childArray[i].toObject();

		HyGuiItemType eType = HyGlobal::GetTypeFromString(childObj["itemType"].toString());
		QUuid uuid(childObj["UUID"].toString());
		EntityDrawItem *pItemWidget = FindStaleChild(eType, uuid);
		if(pItemWidget == nullptr)
		{
			QUuid itemUuid(childObj["itemUUID"].toString());

			pItemWidget = new EntityDrawItem(eType, uuid, itemUuid, this);
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
	for(EntityDrawItem *pItem : m_ItemList)
		pItem->SetStale();
}

EntityDrawItem *EntityDraw::FindStaleChild(HyGuiItemType eType, QUuid uuid)
{
	for(EntityDrawItem *pItem : m_ItemList)
	{
		if(pItem->GetGuiType() == eType && pItem->GetThisUuid() == uuid && pItem->IsStale())
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
	if(m_SelectedItemList.size() > 1)
	{
		m_MultiTransform.Show(true);
		m_MultiTransform.WrapTo(m_SelectedItemList, m_pCamera);
	}
	else
		m_MultiTransform.Hide();

	for(EntityDrawItem *pItemDraw : m_SelectedItemList)
		pItemDraw->RefreshTransform(m_pCamera);
}

//QList<EntityItemDraw *> EntityDraw::GetSelectedItems() const
//{
//	EntityWidget *pWidget = static_cast<EntityWidget *>(m_pProjItem->GetWidget());
//	QList<EntityTreeItemData *> selectedItemDataList = pWidget->GetSelectedItems(false, true);
//
//	
//
//	return matchedItemList;
//}
