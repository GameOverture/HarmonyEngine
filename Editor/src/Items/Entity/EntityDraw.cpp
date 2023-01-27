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
#include "EntityUndoCmds.h"

#include <QKeyEvent>

EntityDraw::EntityDraw(ProjectItemData *pProjItem, const FileDataPair &initFileDataRef) :
	IDraw(pProjItem, initFileDataRef),
	m_fMultiTransformStartRot(0.0f),
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

	if(m_bPanCameraKeyDown)
		RefreshTransforms();
	else if(pEvent->button() == Qt::LeftButton)
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

		case Qt::OpenHandCursor:	// Rotating
			Harmony::GetWidget(&m_pProjItem->GetProject())->SetCursorShape(Qt::ClosedHandCursor);
		[[fallthrough]];
		case Qt::SizeAllCursor:		// Translating
		case Qt::SizeBDiagCursor:	// Scaling
		case Qt::SizeVerCursor:		// Scaling
		case Qt::SizeFDiagCursor:	// Scaling
		case Qt::SizeHorCursor: {	// Scaling
			if(HyEngine::Input().GetWorldMousePos(m_ptDragStart) == false)
				HyGuiLog("EntityDraw::OnMousePressEvent - GetWorldMousePos failed", LOGTYPE_Error);

			TransformCtrl *pCurTransform = nullptr;
			if(m_MultiTransform.IsShown())
				pCurTransform = &m_MultiTransform;
			else
				pCurTransform = &m_SelectedItemList[0]->GetTransformCtrl();

			pCurTransform->GetCentroid(m_ptDragCenter);
			HyEngine::Window().ProjectToWorldPos2d(m_ptDragCenter, m_ptDragCenter);

			glm::vec2 ptMidRight, ptMidLeft, ptTopMid, ptBotMid;
			HyEngine::Window().ProjectToWorldPos2d(pCurTransform->GetGrabPointPos(GRAB_MidRight), ptMidRight);
			HyEngine::Window().ProjectToWorldPos2d(pCurTransform->GetGrabPointPos(GRAB_MidLeft), ptMidLeft);
			HyEngine::Window().ProjectToWorldPos2d(pCurTransform->GetGrabPointPos(GRAB_TopMid), ptTopMid);
			HyEngine::Window().ProjectToWorldPos2d(pCurTransform->GetGrabPointPos(GRAB_BotMid), ptBotMid);
			HySetVec(m_vDragStartSize, fabs(ptMidRight.x - ptMidLeft.x), fabs(ptTopMid.y - ptBotMid.y));

			switch(m_eCurHoverGrabPoint)
			{
			case GRAB_BotLeft:	m_ptDragAnchorPoint = pCurTransform->GetGrabPointPos(GRAB_TopRight); break;
			case GRAB_BotRight:	m_ptDragAnchorPoint = pCurTransform->GetGrabPointPos(GRAB_TopLeft); break;
			case GRAB_TopRight:	m_ptDragAnchorPoint = pCurTransform->GetGrabPointPos(GRAB_BotLeft); break;
			case GRAB_TopLeft:	m_ptDragAnchorPoint = pCurTransform->GetGrabPointPos(GRAB_BotRight); break;
			case GRAB_BotMid:	m_ptDragAnchorPoint = pCurTransform->GetGrabPointPos(GRAB_TopMid); break;
			case GRAB_MidRight:	m_ptDragAnchorPoint = pCurTransform->GetGrabPointPos(GRAB_MidLeft); break;
			case GRAB_TopMid:	m_ptDragAnchorPoint = pCurTransform->GetGrabPointPos(GRAB_BotMid); break;
			case GRAB_MidLeft:	m_ptDragAnchorPoint = pCurTransform->GetGrabPointPos(GRAB_MidRight); break;
			default:
				break;
			}
			HyEngine::Window().ProjectToWorldPos2d(m_ptDragAnchorPoint, m_ptDragAnchorPoint);

			m_DragState = DRAGSTATE_Starting;
			break; }

		default:
			HyGuiLog("EntityDraw::OnMousePressEvent - Unknown cursor state not handled: " % QString::number(Harmony::GetWidget(&m_pProjItem->GetProject())->GetCursorShape()), LOGTYPE_Error);
		}
	}
}

/*virtual*/ void EntityDraw::OnMouseReleaseEvent(QMouseEvent *pEvent) /*override*/
{
	IDraw::OnMouseReleaseEvent(pEvent);
	
	if(m_DragState == DRAGSTATE_Transforming)
	{
		QList<EntityTreeItemData *> treeItemDataList;
		QList<glm::mat4> newTransformList;
		for(EntityDrawItem *pDrawItem : m_SelectedItemList)
		{
			if(pDrawItem->GetGuiType() != ITEM_Shape)
				newTransformList.push_back(pDrawItem->GetNodeChild()->GetSceneTransform(0.0f));
			else
				newTransformList.push_back(m_ActiveTransform.GetSceneTransform(0.0f));

			EntityTreeItemData *pTreeItemData = static_cast<EntityModel *>(m_pProjItem->GetModel())->GetTreeModel().FindTreeItemData(pDrawItem->GetThisUuid());;
			treeItemDataList.push_back(pTreeItemData);
		}

		QUndoCommand *pCmd = new EntityUndoCmd_Transform(*m_pProjItem, treeItemDataList, newTransformList, m_PrevTransformList);
		m_pProjItem->GetUndoStack()->push(pCmd);

		m_ActiveTransform.ChildrenTransfer(*this);

		m_ActiveTransform.pos.Set(0.0f, 0.0f);
		m_ActiveTransform.rot.Set(0.0f);
		m_ActiveTransform.scale.Set(1.0f, 1.0f);
	}

	m_DragState = DRAGSTATE_None;
	RefreshTransforms();
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
			switch(m_DragState)
			{
			case DRAGSTATE_Marquee:
				break;

			case DRAGSTATE_Starting: {
				glm::vec2 ptCurMousePos;
				HyEngine::Input().GetWorldMousePos(ptCurMousePos);
				if(glm::distance(m_ptDragStart, ptCurMousePos) >= 10.0f)
				{
					m_PrevTransformList.clear();
					for(EntityDrawItem *pDrawItem : m_SelectedItemList)
					{
						if(pDrawItem->GetGuiType() != ITEM_Shape)
						{
							m_ActiveTransform.ChildAppend(*pDrawItem->GetNodeChild());
							m_PrevTransformList.push_back(pDrawItem->GetNodeChild()->GetSceneTransform(0.0f));
						}
						else
						{
							m_ActiveTransform.ShapeAppend(*pDrawItem->GetShape());
							m_PrevTransformList.push_back(glm::mat4(1.0f));
						}
					}
					
					m_fMultiTransformStartRot = m_MultiTransform.rot.Get();
					m_DragState = DRAGSTATE_Transforming;
				}
				break; }

			case DRAGSTATE_Transforming: {
				glm::vec2 ptMousePos;
				if(HyEngine::Input().GetWorldMousePos(ptMousePos) == false)
					break; // Cursor is currently dragged off render window

				switch(Harmony::GetWidget(&m_pProjItem->GetProject())->GetCursorShape())
				{
				case Qt::ClosedHandCursor: // Rotating
					m_ActiveTransform.rot_pivot.Set(m_ptDragCenter);
					m_ActiveTransform.rot.Set(HyMath::AngleFromVector(m_ptDragCenter - ptMousePos) - HyMath::AngleFromVector(m_ptDragCenter - m_ptDragStart));
					break;

				case Qt::SizeAllCursor:		// Translating
					m_ActiveTransform.pos.Set(ptMousePos - m_ptDragStart);
					break;

				case Qt::SizeBDiagCursor:	// Scaling
				case Qt::SizeVerCursor:		// Scaling
				case Qt::SizeFDiagCursor:	// Scaling
				case Qt::SizeHorCursor: {	// Scaling
					m_ActiveTransform.scale_pivot.Set(m_ptDragAnchorPoint);

					float fRequiredWidth = fabs(m_ActiveTransform.scale_pivot.X() - ptMousePos.x);//m_Init.layout.m_vLayoutExtents[eLayout].x * 2.0f;
					float fRequiredHeight = fabs(m_ActiveTransform.scale_pivot.Y() - ptMousePos.y);//m_Init.layout.m_vLayoutExtents[eLayout].y * 2.0f;

					// Determine if zooming out is required to fit required width/height
					float fZoomAmt = HyMax(fRequiredWidth / m_vDragStartSize.x, fRequiredHeight / m_vDragStartSize.y);
					m_ActiveTransform.scale.Set(fZoomAmt, fZoomAmt);

					break; }

				default:
					HyGuiLog("EntityDraw::OnMouseMoveEvent - Unknown cursor state not handled: " % QString::number(Harmony::GetWidget(&m_pProjItem->GetProject())->GetCursorShape()), LOGTYPE_Error);
				}

				RefreshTransforms();
				break; }
			}
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
					if(eNextCursorShape == Qt::CrossCursor)
					{
						if(transformCtrlRef.IsMouseOverBoundingVolume())
						{
							eNextCursorShape = Qt::SizeAllCursor;
							m_pCurHoverItem = m_SelectedItemList[0];
						}
					}
					else
						m_pCurHoverItem = m_SelectedItemList[0];
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
	pWidget->RequestSelectedItems(uuidList);
}

void EntityDraw::RefreshTransforms()
{
	if(m_SelectedItemList.size() > 1)
	{
		m_MultiTransform.Show(true);
		if(m_ActiveTransform.rot.Get() == 0.0f)
		{
			m_MultiTransform.rot_pivot.Set(0.0f, 0.0f);
			m_MultiTransform.rot.Set(0.0f);
			m_MultiTransform.WrapTo(m_SelectedItemList, m_pCamera);
		}
		else
		{
			if(m_DragState == DRAGSTATE_Transforming)
			{
				glm::vec2 ptCenterPivot;
				m_MultiTransform.GetCentroid(ptCenterPivot);
				m_MultiTransform.rot_pivot.Set(ptCenterPivot);
				m_MultiTransform.rot.Set(m_fMultiTransformStartRot + m_ActiveTransform.rot.Get());
			}
		}
	}
	else
		m_MultiTransform.Hide();

	for(EntityDrawItem *pItemDraw : m_SelectedItemList)
		pItemDraw->RefreshTransform(m_pCamera);
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
