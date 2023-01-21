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
	IDraw(pProjItem, initFileDataRef),
	m_bCurHoverMultiTransform(false),
	m_pCurHoverItem(nullptr),
	m_eCurHoverGrabPoint(GRAB_None),
	m_DragState(DRAGSTATE_None)
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
		switch(Harmony::GetWidget(&m_pProjItem->GetProject())->GetCursorShape())
		{
		case Qt::PointingHandCursor:
			if(m_pCurHoverItem)
			{
				QList<EntityDrawItem *> selectList;
				selectList << m_pCurHoverItem;
				
				if(pEvent->modifiers().testFlag(Qt::KeyboardModifier::ShiftModifier))
					selectList += m_SelectedItemList;

				RequestSelection(selectList);
				break;
			}
		[[fallthrough]];
		case Qt::CrossCursor:
			RequestSelection(QList<EntityDrawItem *>());
			m_DragState = DRAGSTATE_Marquee;
			break;

		case Qt::SizeAllCursor:
			break;

		case Qt::SizeBDiagCursor:
		case Qt::SizeVerCursor:
		case Qt::SizeFDiagCursor:
		case Qt::SizeHorCursor:
			break;

		case Qt::OpenHandCursor:
			break;

		default:
			HyGuiLog("EntityDraw::OnMousePressEvent - Unknown cursor state not handled: " % QString::number(Harmony::GetWidget(&m_pProjItem->GetProject())->GetCursorShape()), LOGTYPE_Error);
		}

		
		{
			if(m_bCurHoverMultiTransform && m_MultiTransform.IsShown())
			{
				if(m_eCurHoverGrabPoint != GRAB_None)
				{
				}

			}
			else if(m_eCurHoverGrabPoint != GRAB_None)
			{

			}
			else if(m_pCurHoverItem)
			{
				//m_DragState = DRAGSTATE_Started;
			}
		}
		
		
	//	if(m_MultiTransform.IsShown() &&  == Qt::PointingHandCursor)
	}
}

/*virtual*/ void EntityDraw::OnMouseReleaseEvent(QMouseEvent *pEvent) /*override*/
{
	IDraw::OnMouseReleaseEvent(pEvent);

	m_DragState = DRAGSTATE_None;
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
		if(m_DragState != DRAGSTATE_None)
		{

		}
		else
		{
			Qt::CursorShape eNextCursorShape = Qt::CrossCursor;

			m_bCurHoverMultiTransform = false;
			m_pCurHoverItem = nullptr;
			m_eCurHoverGrabPoint = GRAB_None;

			if(m_MultiTransform.IsShown())
			{
				m_eCurHoverGrabPoint = m_MultiTransform.IsMouseOverGrabPoint();
				eNextCursorShape = GetGrabPointCursorShape(m_eCurHoverGrabPoint, m_MultiTransform.GetCachedRotation());

				if(eNextCursorShape == Qt::CrossCursor)
				{
					if(m_MultiTransform.IsMouseOverBoundingVolume())
					{
						eNextCursorShape = Qt::SizeAllCursor;
						m_bCurHoverMultiTransform = true;
					}
				}
				else
					m_bCurHoverMultiTransform = true;
			}

			if(eNextCursorShape == Qt::CrossCursor) // Not hovering over multi-transform control
			{
				if(m_SelectedItemList.size() == 1)
				{
					TransformCtrl &transformCtrlRef = m_SelectedItemList[0]->GetTransformCtrl();

					m_eCurHoverGrabPoint = transformCtrlRef.IsMouseOverGrabPoint();
					eNextCursorShape = GetGrabPointCursorShape(m_eCurHoverGrabPoint, transformCtrlRef.GetCachedRotation());
					if(eNextCursorShape == Qt::CrossCursor && transformCtrlRef.IsMouseOverBoundingVolume())
					{
						eNextCursorShape = Qt::SizeAllCursor;
						m_pCurHoverItem = m_SelectedItemList[0];
					}
				}

				if(eNextCursorShape == Qt::CrossCursor) // Not hovering over multi-transform control or the any selected item
				{
					for(int32 i = 0; i < m_ItemList.size(); ++i)
					{
						if(m_ItemList[i]->IsMouseInBounds())
						{
							eNextCursorShape = Qt::PointingHandCursor;
							m_pCurHoverItem = m_ItemList[i];

							break;
						}
					}
				}
			}

			Harmony::GetWidget(&m_pProjItem->GetProject())->SetCursorShape(eNextCursorShape);
		}
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

void EntityDraw::RequestSelection(QList<EntityDrawItem *> selectionList)
{
	QList<QUuid> uuidList;
	for(EntityDrawItem *pDrawItem : selectionList)
		uuidList.push_back(pDrawItem->GetThisUuid());

	EntityWidget *pWidget = static_cast<EntityWidget *>(m_pProjItem->GetWidget());
	pWidget->SetSelectedItems(uuidList);
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

Qt::CursorShape EntityDraw::GetGrabPointCursorShape(GrabPoint eGrabPoint, float fRotation) const
{
	fRotation = HyMath::NormalizeRange(fRotation, 0.0f, 360.0f);

	int32 iThresholds = 0;
	if(fRotation <= 22.5f || fRotation >= 337.5f)
		iThresholds = 0;
	else if(fRotation >= 22.5f && fRotation <= 67.5f)
		iThresholds = 1;
	else if(fRotation >= 67.5f && fRotation <= 112.5f)
		iThresholds = 2;
	else if(fRotation >= 112.5 && fRotation <= 157.5f)
		iThresholds = 3;
	else if(fRotation >= 157.5f && fRotation <= 202.5f)
		iThresholds = 4;
	else if(fRotation >= 202.5f && fRotation <= 247.5f)
		iThresholds = 5;
	else if(fRotation >= 247.5f && fRotation <= 292.5f)
		iThresholds = 6;
	else //if(fRotation >= 292.5f && fRotation <= 337.5f)
		iThresholds = 7;

	std::function<Qt::CursorShape(Qt::CursorShape, int32)> fpRotateCursor = [](Qt::CursorShape eStartCursor, int32 iThresholds)
	{
		const Qt::CursorShape cursorShapes[] = { Qt::SizeBDiagCursor, Qt::SizeVerCursor, Qt::SizeFDiagCursor, Qt::SizeHorCursor };
		for(int32 i = 0; i < 4; ++i)
		{
			if(eStartCursor == cursorShapes[i])
				return cursorShapes[HyMath::NormalizeRange(i + iThresholds, 0, 4)];
		}

		return eStartCursor;
	};

	switch(eGrabPoint)
	{
	default:
	case GRAB_None:
		return Qt::CrossCursor;

	case GRAB_BotLeft:
		return fpRotateCursor(Qt::SizeBDiagCursor, iThresholds);
	case GRAB_BotRight:
		return fpRotateCursor(Qt::SizeFDiagCursor, iThresholds);
	case GRAB_TopRight:
		return fpRotateCursor(Qt::SizeBDiagCursor, iThresholds);
	case GRAB_TopLeft:
		return fpRotateCursor(Qt::SizeFDiagCursor, iThresholds);
	case GRAB_BotMid:
		return fpRotateCursor(Qt::SizeVerCursor, iThresholds);
	case GRAB_MidRight:
		return fpRotateCursor(Qt::SizeHorCursor, iThresholds);
	case GRAB_TopMid:
		return fpRotateCursor(Qt::SizeVerCursor, iThresholds);
	case GRAB_MidLeft:
		return fpRotateCursor(Qt::SizeHorCursor, iThresholds);
	case GRAB_Rotate:
		return Qt::OpenHandCursor;
	}
}
