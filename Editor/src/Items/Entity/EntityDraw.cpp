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
	m_RootEntity(this),
	m_DragShape(this),
	m_MultiTransform(this),
	m_fMultiTransformStartRot(0.0f),
	m_pCurHoverItem(nullptr),
	m_eCurHoverGrabPoint(TransformCtrl::GRAB_None),
	m_bSelectionHandled(false),
	m_eDragState(DRAGSTATE_None),
	m_eShapeEditState(SHAPESTATE_None),
	m_pCurVertexEditItem(nullptr),
	m_eCurVemAction(ShapeCtrl::VEMACTION_None),
	m_SnapGuideHorz(this),
	m_SnapGuideVert(this),
	m_bActivateVemOnNextJsonMeta(false),
	m_bPlayingPreview(false)
{
	m_MultiTransform.Hide();
	m_PressTimer.SetExpiredCallback(OnMousePressTimer, this);

	m_SnapGuideHorz.SetVisible(false);
	m_SnapGuideHorz.SetTint(HyColor::DarkGreen);
	m_SnapGuideHorz.SetLineThickness(4.0f);
	//m_SnapGuideHorz.UseWindowCoordinates();
	m_SnapGuideHorz.SetDisplayOrder(DISPLAYORDER_SnapGuide);

	m_SnapGuideVert.SetVisible(false);
	m_SnapGuideVert.SetTint(HyColor::DarkGreen);
	m_SnapGuideVert.SetLineThickness(4.0f);
	//m_SnapGuideVert.UseWindowCoordinates();
	m_SnapGuideVert.SetDisplayOrder(DISPLAYORDER_SnapGuide);
}

/*virtual*/ EntityDraw::~EntityDraw()
{
	for(auto pItem : m_ItemList)
		delete pItem;
}

/*virtual*/ void EntityDraw::OnUndoStackIndexChanged(int iIndex) /*override*/
{
	if(m_eShapeEditState == SHAPESTATE_DragAddPrimitive || m_eShapeEditState == SHAPESTATE_DragAddShape)
		RequestClearShapeEdit();
}

/*virtual*/ void EntityDraw::OnKeyPressEvent(QKeyEvent *pEvent) /*override*/
{
	IDraw::OnKeyPressEvent(pEvent);
	
	if(m_bPanCameraKeyDown || m_bPlayingPreview)
		RefreshTransforms();
	else
	{
		if(pEvent->key() == Qt::Key_Backspace) // Qt::Key_Delete isn't being passed to this callback
		{
			if(m_eShapeEditState == SHAPESTATE_VertexEditMode)
			{
				if(m_pCurVertexEditItem->GetShapeCtrl().TransformVemVerts(ShapeCtrl::VEMACTION_RemoveSelected, glm::vec2(), glm::vec2(), m_pCamera) == false)
					return;

				int iStateIndex = 0;
				int iFrameIndex = 0;
				if(m_pProjItem->GetWidget() == nullptr)
					HyGuiLog("EntityDraw::OnKeyPressEvent - m_pProjItem->GetWidget() is nullptr", LOGTYPE_Error);
				else
				{
					iStateIndex = m_pProjItem->GetWidget()->GetCurStateIndex();
					iFrameIndex = static_cast<EntityStateData *>(m_pProjItem->GetModel()->GetStateData(iStateIndex))->GetDopeSheetScene().GetCurrentFrame();
				}

				QUuid thisUuid = m_pCurVertexEditItem->GetEntityTreeItemData()->GetThisUuid();
				EntityTreeItemData *pTreeItemData = static_cast<EntityModel *>(m_pProjItem->GetModel())->GetTreeModel().FindTreeItemData(thisUuid);
				QUndoCommand *pCmd = new EntityUndoCmd_ShapeData(*m_pProjItem, iStateIndex, iFrameIndex, pTreeItemData, ShapeCtrl::VEMACTION_RemoveSelected, m_pCurVertexEditItem->GetShapeCtrl().SerializeVemVerts(m_pCamera));
				m_pProjItem->GetUndoStack()->push(pCmd);
			}
		}
		if(pEvent->key() == Qt::Key_Control || pEvent->key() == Qt::Key_Shift)
			DoMouseMove(pEvent->modifiers().testFlag(Qt::KeyboardModifier::ControlModifier), pEvent->modifiers().testFlag(Qt::KeyboardModifier::ShiftModifier));
	}

	if(pEvent->key() == Qt::Key_Left ||
		pEvent->key() == Qt::Key_Right ||
		pEvent->key() == Qt::Key_Up ||
		pEvent->key() == Qt::Key_Down)
	{
		BeginTransform(false);

		if(pEvent->key() == Qt::Key_Left)
			m_vNudgeTranslate.setX(m_vNudgeTranslate.x() - 1);
		if(pEvent->key() == Qt::Key_Right)
			m_vNudgeTranslate.setX(m_vNudgeTranslate.x() + 1);
		if(pEvent->key() == Qt::Key_Up)
			m_vNudgeTranslate.setY(m_vNudgeTranslate.y() + 1);
		if(pEvent->key() == Qt::Key_Down)
			m_vNudgeTranslate.setY(m_vNudgeTranslate.y() - 1);

		m_ActiveTransform.pos.Set(m_vNudgeTranslate.x(), m_vNudgeTranslate.y());
		RefreshTransforms();
	}
}

/*virtual*/ void EntityDraw::OnKeyReleaseEvent(QKeyEvent *pEvent) /*override*/
{
	IDraw::OnKeyReleaseEvent(pEvent);
	
	if(m_bPanCameraKeyDown || m_bPlayingPreview)
		RefreshTransforms();
	else
	{
		if(pEvent->key() == Qt::Key_Control || pEvent->key() == Qt::Key_Shift)
			DoMouseMove(pEvent->modifiers().testFlag(Qt::KeyboardModifier::ControlModifier), pEvent->modifiers().testFlag(Qt::KeyboardModifier::ShiftModifier));
	}

	if(m_bPlayingPreview == false &&
		pEvent->isAutoRepeat() == false &&
		(pEvent->key() == Qt::Key_Left ||
		pEvent->key() == Qt::Key_Right ||
		pEvent->key() == Qt::Key_Up ||
		pEvent->key() == Qt::Key_Down))
	{
		// We can reuse 'DoMouseRelease_Transform' to submit the nudging transform (via undo/redo cmd), while also resetting/cleaning up the state
		DoMouseRelease_Transform();
		m_eDragState = DRAGSTATE_None;
	}
}

/*virtual*/ void EntityDraw::OnMouseWheelEvent(QWheelEvent *pEvent) /*override*/
{
	IDraw::OnMouseWheelEvent(pEvent);
}

/*virtual*/ void EntityDraw::OnMouseMoveEvent(QMouseEvent *pEvent) /*override*/
{
	IDraw::OnMouseMoveEvent(pEvent);

	if(m_bPanCameraKeyDown || m_bPlayingPreview)
		RefreshTransforms();
	else if(Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->GetCursorShape() != Qt::WaitCursor)
		DoMouseMove(pEvent->modifiers().testFlag(Qt::KeyboardModifier::ControlModifier), pEvent->modifiers().testFlag(Qt::KeyboardModifier::ShiftModifier));
}

/*virtual*/ void EntityDraw::OnMousePressEvent(QMouseEvent *pEvent) /*override*/
{
	IDraw::OnMousePressEvent(pEvent);

	if(m_bPanCameraKeyDown || m_bPlayingPreview)
		RefreshTransforms();
	else if(pEvent->button() == Qt::LeftButton && Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->GetCursorShape() != Qt::WaitCursor)
	{
		if(m_eShapeEditState != SHAPESTATE_None)
			DoMousePress_ShapeEdit(pEvent->modifiers().testFlag(Qt::KeyboardModifier::ControlModifier), pEvent->modifiers().testFlag(Qt::KeyboardModifier::ShiftModifier));
		else if(m_eDragState == DRAGSTATE_None)
			DoMousePress_Select(pEvent->modifiers().testFlag(Qt::KeyboardModifier::ControlModifier), pEvent->modifiers().testFlag(Qt::KeyboardModifier::ShiftModifier));
	}
	else if(pEvent->button() == Qt::RightButton)
	{
		if(m_eShapeEditState == SHAPESTATE_DragAddPrimitive || m_eShapeEditState == SHAPESTATE_DragAddShape)
			RequestClearShapeEdit();
	}
}

/*virtual*/ void EntityDraw::OnMouseReleaseEvent(QMouseEvent *pEvent) /*override*/
{
	if(m_bPanCameraKeyDown || m_bPlayingPreview)
		IDraw::OnMouseReleaseEvent(pEvent);
	else if(Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->GetCursorShape() != Qt::WaitCursor)
	{
		IDraw::OnMouseReleaseEvent(pEvent);

		if(m_eShapeEditState != SHAPESTATE_None)
			DoMouseRelease_ShapeEdit(pEvent->modifiers().testFlag(Qt::KeyboardModifier::ControlModifier), pEvent->modifiers().testFlag(Qt::KeyboardModifier::ShiftModifier));
		else if(pEvent->button() == Qt::LeftButton)
		{
			if(m_eDragState == DRAGSTATE_None ||
				m_eDragState == DRAGSTATE_Marquee ||
				m_eDragState == DRAGSTATE_Pending)
			{
				DoMouseRelease_Select(pEvent->modifiers().testFlag(Qt::KeyboardModifier::ControlModifier), pEvent->modifiers().testFlag(Qt::KeyboardModifier::ShiftModifier));
			}
			else // DRAGSTATE_Transforming or DRAGSTATE_Nudging
				DoMouseRelease_Transform();
		}
	}

	m_eDragState = DRAGSTATE_None;
	RefreshTransforms();
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
			if(m_eDragState == DRAGSTATE_Transforming || m_eDragState == DRAGSTATE_Nudging)
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

	for(EntityDrawItem *pItemDraw : m_ItemList /*m_SelectedItemList*/)
		pItemDraw->RefreshTransform(m_pCamera);
}

void EntityDraw::SetShapeEditDrag(EditorShape eShape, bool bAsPrimitive)
{
	RequestSelection(QList<EntityDrawItem *>()); // Clear any selected item

	m_eDragState = DRAGSTATE_None;
	m_eShapeEditState = bAsPrimitive ? SHAPESTATE_DragAddPrimitive : SHAPESTATE_DragAddShape;
	m_DragShape.Setup(eShape, bAsPrimitive ? ENTCOLOR_Primitive : ENTCOLOR_Shape, 1.0f, 1.0f);
	
	Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->SetCursorShape(Qt::CrossCursor);
}

void EntityDraw::ActivateVemOnNextJsonMeta()
{
	m_bActivateVemOnNextJsonMeta = true;
}

void EntityDraw::SetShapeEditVertex()
{
	ItemType eType = m_SelectedItemList[0]->GetEntityTreeItemData()->GetType();

	if(m_SelectedItemList.count() != 1 ||
	  (eType != ITEM_Primitive && eType != ITEM_BoundingVolume && eType != ITEM_Text))
	{
		HyGuiLog("EntityDraw::SetShapeEditVertex() invoked when selection is invalid", LOGTYPE_Error);
		return;
	}

	m_eDragState = DRAGSTATE_None;
	m_eShapeEditState = SHAPESTATE_VertexEditMode;

	m_pCurVertexEditItem = m_SelectedItemList[0];
	m_pCurVertexEditItem->HideTransformCtrl();
	m_pCurVertexEditItem->GetShapeCtrl().EnableVertexEditMode();

	Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->SetCursorShape(Qt::ArrowCursor);
}

void EntityDraw::RequestClearShapeEdit()
{
	static_cast<EntityModel *>(m_pProjItem->GetModel())->ClearShapeEdit();
}

void EntityDraw::ClearShapeEdit()
{
	m_eDragState = DRAGSTATE_None;
	m_eShapeEditState = SHAPESTATE_None;
	m_DragShape.Setup(SHAPE_None, ENTCOLOR_Clear, 1.0f, 1.0f);

	if(m_pCurVertexEditItem)
	{
		if(m_SelectedItemList.count() == 1 && m_SelectedItemList[0] == m_pCurVertexEditItem)
			m_pCurVertexEditItem->ShowTransformCtrl(true);
		m_pCurVertexEditItem->GetShapeCtrl().ClearVertexEditMode();
		m_pCurVertexEditItem = nullptr;
	}
	m_eCurVemAction = ShapeCtrl::VEMACTION_None;

	Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->SetCursorShape(Qt::ArrowCursor);
}

void EntityDraw::SetExtrapolatedProperties()
{
	if(m_pProjItem->GetWidget() == nullptr)
		HyGuiLog("EntityDraw::SetExtrapolatedProperties - m_pProjItem->GetWidget() is nullptr", LOGTYPE_Error);

	EntityDopeSheetScene &entityDopeSheetSceneRef = static_cast<EntityStateData *>(m_pProjItem->GetModel()->GetStateData(m_pProjItem->GetWidget()->GetCurStateIndex()))->GetDopeSheetScene();

	const float fFRAME_DURATION = 1.0f / static_cast<EntityModel &>(entityDopeSheetSceneRef.GetStateData()->GetModel()).GetFramesPerSecond();
	const int iCURRENT_FRAME = entityDopeSheetSceneRef.GetCurrentFrame();

	// Set the extrapolated properties for the 'm_RootEntity' item
	EntityTreeItemData *pRootTreeItemData = static_cast<EntityModel *>(m_pProjItem->GetModel())->GetTreeModel().GetRootTreeItemData();
	ExtrapolateProperties(&m_RootEntity,
						  nullptr,
						  false,
						  ITEM_Unknown, // 'ITEM_Unknown' indicates this is the root
						  fFRAME_DURATION,
						  iCURRENT_FRAME,
						  entityDopeSheetSceneRef.GetKeyFramesMap()[pRootTreeItemData],
						  entityDopeSheetSceneRef.GetEventMap(),
						  m_pCamera);

	// Set the extrapolated properties for all the children items
	for(EntityDrawItem *pDrawItem : m_ItemList)
	{
		EntityTreeItemData *pEntityTreeItemData = pDrawItem->GetEntityTreeItemData();
		ItemType eItemType = pEntityTreeItemData->GetType();
		if(eItemType == ITEM_Prefix) // aka Shapes folder
			continue;

		ExtrapolateProperties(pDrawItem->GetHyNode(),
							  &pDrawItem->GetShapeCtrl(),
							  pEntityTreeItemData->IsSelected(),
							  eItemType,
							  fFRAME_DURATION,
							  iCURRENT_FRAME,
							  entityDopeSheetSceneRef.GetKeyFramesMap()[pEntityTreeItemData],
							  entityDopeSheetSceneRef.GetEventMap(),
							  m_pCamera);
	}

	RefreshTransforms();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*virtual*/ void EntityDraw::OnApplyJsonMeta(QJsonObject &itemMetaObj) /*override*/
{
	for(auto pItem : m_ItemList)
	{
		pItem->GetHyNode()->ParentDetach();
		if(pItem->GetHyNode()->GetInternalFlags() & NODETYPE_IsBody)
			static_cast<IHyBody2d *>(pItem->GetHyNode())->ResetDisplayOrder();
	}

	QList<EntityDrawItem *> staleItemList(m_ItemList);
	m_ItemList.clear();
	m_SelectedItemList.clear();

	QJsonArray descChildArray = itemMetaObj["descChildList"].toArray();
	QJsonArray descShapeArray = itemMetaObj["descShapeList"].toArray();

	// Pull out all the valid json objects that represent items in the entity
	QList<QJsonObject> descObjList;

	// Add 'shape' items first because they usually encompass large portions of the entire entity, making child item selection more difficult
	for(int32 i = 0; i < descShapeArray.size(); ++i)
	{
		if(descShapeArray[i].isObject())
			descObjList.push_back(descShapeArray[i].toObject());
		else if(descShapeArray[i].isArray())
		{
			QJsonArray arrayFolder = descShapeArray[i].toArray();
			for(int32 j = 0; j < arrayFolder.size(); ++j)
				descObjList.push_back(arrayFolder[j].toObject());
		}
	}

	// Add children items next
	for(int32 i = 0; i < descChildArray.size(); ++i)
	{
		if(descChildArray[i].isObject())
			descObjList.push_back(descChildArray[i].toObject());
		else if(descChildArray[i].isArray())
		{
			QJsonArray arrayFolder = descChildArray[i].toArray();
			for(int32 j = 0; j < arrayFolder.size(); ++j)
				descObjList.push_back(arrayFolder[j].toObject());
		}
	}

	// Process all the items in 'descObjList'
	for(int32 i = 0; i < descObjList.size(); ++i)
	{
		QJsonObject descObj = descObjList[i];
		ItemType eType = HyGlobal::GetTypeFromString(descObj["itemType"].toString());
		QUuid uuid(descObj["UUID"].toString());
		bool bSelected = descObj["isSelected"].toBool();

		EntityDrawItem *pDrawItem = nullptr;
		for(EntityDrawItem *pStaleItem : staleItemList)
		{
			if(pStaleItem->GetEntityTreeItemData()->GetType() == eType && pStaleItem->GetEntityTreeItemData()->GetThisUuid() == uuid)
			{
				pDrawItem = pStaleItem;
				break;
			}
		}
		if(pDrawItem == nullptr)
		{
			EntityTreeItemData *pEntityTreeItemData = static_cast<EntityModel *>(m_pProjItem->GetModel())->GetTreeModel().FindTreeItemData(uuid);
			pDrawItem = new EntityDrawItem(m_pProjItem->GetProject(), pEntityTreeItemData, this, &m_RootEntity);
		}
		else
		{
			staleItemList.removeOne(pDrawItem);

			if(pDrawItem->GetEntityTreeItemData()->IsReallocateDrawItem())
			{
				EntityTreeItemData *pEntityTreeItemData = pDrawItem->GetEntityTreeItemData();
				
				// Allocate first, then delete so Harmony doesn't unload the item data
				EntityDrawItem *pOldDrawItem = pDrawItem;
				pDrawItem = new EntityDrawItem(m_pProjItem->GetProject(), pEntityTreeItemData, this, &m_RootEntity);

				delete pOldDrawItem;
				pEntityTreeItemData->SetReallocateDrawItem(false);
			}
		}

		m_ItemList.push_back(pDrawItem);
		m_RootEntity.ChildAppend(*pDrawItem->GetHyNode());

		if(bSelected)
			m_SelectedItemList.push_back(pDrawItem);
		else
			pDrawItem->HideTransformCtrl();
	}
	// Delete all the remaining stale items
	for(auto pStaleItem : staleItemList)
		delete pStaleItem;
	staleItemList.clear();

	SetExtrapolatedProperties();

	if(m_bActivateVemOnNextJsonMeta)
	{
		for(EntityDrawItem *pSelectedItemDraw : m_SelectedItemList)
			pSelectedItemDraw->HideTransformCtrl();

		static_cast<EntityModel *>(m_pProjItem->GetModel())->SetShapeEditVemMode(true);
		m_bActivateVemOnNextJsonMeta = false;
	}
	else
	{
		if(m_eShapeEditState == SHAPESTATE_None)
		{
			bool bShowGrabPoints = m_SelectedItemList.size() == 1;
			for(EntityDrawItem *pSelectedItemDraw : m_SelectedItemList)
				pSelectedItemDraw->ShowTransformCtrl(bShowGrabPoints);
		}
		else
		{
			for(EntityDrawItem *pSelectedItemDraw : m_SelectedItemList)
				pSelectedItemDraw->HideTransformCtrl();
		}
	}

	RefreshTransforms();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

void EntityDraw::RequestSelection(QList<EntityDrawItem *> selectionList)
{
	selectionList = QSet<EntityDrawItem *>(selectionList.begin(), selectionList.end()).values();

	bool bValidRequest = m_SelectedItemList.size() != selectionList.size(); // Ensure this request isn't redundant
	QList<QUuid> uuidList;
	for(EntityDrawItem *pDrawItem : selectionList)
	{
		if(m_SelectedItemList.contains(pDrawItem) == false)
			bValidRequest = true;

		uuidList.push_back(pDrawItem->GetEntityTreeItemData()->GetThisUuid());
	}

	if(bValidRequest == false)
		return;

	EntityWidget *pWidget = static_cast<EntityWidget *>(m_pProjItem->GetWidget());
	pWidget->RequestSelectedItems(uuidList);
}

Qt::CursorShape EntityDraw::GetGrabPointCursorShape(TransformCtrl::GrabPointType eGrabPoint, float fRotation) const
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
	case TransformCtrl::GRAB_None:
		return Qt::ArrowCursor;

	case TransformCtrl::GRAB_BotLeft:
		return fpRotateCursor(Qt::SizeBDiagCursor, iThresholds);
	case TransformCtrl::GRAB_BotRight:
		return fpRotateCursor(Qt::SizeFDiagCursor, iThresholds);
	case TransformCtrl::GRAB_TopRight:
		return fpRotateCursor(Qt::SizeBDiagCursor, iThresholds);
	case TransformCtrl::GRAB_TopLeft:
		return fpRotateCursor(Qt::SizeFDiagCursor, iThresholds);
	case TransformCtrl::GRAB_BotMid:
		return fpRotateCursor(Qt::SizeVerCursor, iThresholds);
	case TransformCtrl::GRAB_MidRight:
		return fpRotateCursor(Qt::SizeHorCursor, iThresholds);
	case TransformCtrl::GRAB_TopMid:
		return fpRotateCursor(Qt::SizeVerCursor, iThresholds);
	case TransformCtrl::GRAB_MidLeft:
		return fpRotateCursor(Qt::SizeHorCursor, iThresholds);
	case TransformCtrl::GRAB_Rotate:
		return Qt::OpenHandCursor;
	}
}

void EntityDraw::DoMouseMove(bool bCtrlMod, bool bShiftMod)
{
	if(m_eShapeEditState == SHAPESTATE_None)
	{
		if(m_eDragState == DRAGSTATE_None ||
			m_eDragState == DRAGSTATE_Marquee ||
			m_eDragState == DRAGSTATE_Pending)
		{
			DoMouseMove_Select(bCtrlMod, bShiftMod);
		}
		else if(m_eDragState == DRAGSTATE_Transforming)
			DoMouseMove_Transform(bCtrlMod, bShiftMod);
	}
	else
		DoMouseMove_ShapeEdit(bCtrlMod, bShiftMod);
}

void EntityDraw::DoMouseMove_Select(bool bCtrlMod, bool bShiftMod)
{
	if(m_eDragState == DRAGSTATE_Marquee)
	{
		Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->SetCursorShape(Qt::CrossCursor);

		glm::vec2 ptCurMousePos;
		HyEngine::Input().GetWorldMousePos(ptCurMousePos);
		
		m_DragShape.Setup(SHAPE_Box, ENTCOLOR_Marquee, 0.25f, 1.0f);
		m_DragShape.SetAsDrag(/*bShiftMod*/false, m_ptDragStart, ptCurMousePos, m_pCamera); // Don't do centering when holding shift and marquee selecting
	}
	else if(m_eDragState == DRAGSTATE_Pending)
	{
		glm::vec2 ptCurMousePos;
		HyEngine::Input().GetWorldMousePos(ptCurMousePos);
		if(glm::distance(m_ptDragStart, ptCurMousePos) >= 2.0f)
			BeginTransform(true);
	}
	else // 'm_eDragState' is DRAGSTATE_None
	{
		if(Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->GetCursorShape() != Qt::WaitCursor)
			Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->SetCursorShape(Qt::ArrowCursor);
		
		m_pCurHoverItem = nullptr;
		for(int32 i = m_ItemList.size() - 1; i >= 0; --i) // iterate backwards to prioritize selecting items with higher display order
		{
			if(m_ItemList[i]->IsMouseInBounds())
			{
				m_pCurHoverItem = m_ItemList[i];
				break;
			}
		}
		m_eCurHoverGrabPoint = TransformCtrl::GRAB_None;

		if(m_MultiTransform.IsShown())
		{
			m_eCurHoverGrabPoint = m_MultiTransform.IsMouseOverGrabPoint();
			Qt::CursorShape eNextCursorShape = GetGrabPointCursorShape(m_eCurHoverGrabPoint, m_MultiTransform.GetCachedRotation());
			Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->SetCursorShape(eNextCursorShape);
		}

		if(m_SelectedItemList.size() == 1)
		{
			TransformCtrl &transformCtrlRef = m_SelectedItemList[0]->GetTransformCtrl();

			m_eCurHoverGrabPoint = transformCtrlRef.IsMouseOverGrabPoint();
			Qt::CursorShape eNextCursorShape = GetGrabPointCursorShape(m_eCurHoverGrabPoint, transformCtrlRef.GetCachedRotation());
			Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->SetCursorShape(eNextCursorShape);
			if(eNextCursorShape != Qt::ArrowCursor)
				m_pCurHoverItem = m_SelectedItemList[0]; // Override whatever might be above this item, because we're hovering over a grab point
		}
	}
}

void EntityDraw::DoMousePress_Select(bool bCtrlMod, bool bShiftMod)
{
	if(Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->GetCursorShape() == Qt::OpenHandCursor)
		Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->SetCursorShape(Qt::ClosedHandCursor);

	if(HyEngine::Input().GetWorldMousePos(m_ptDragStart) == false)
		HyGuiLog("EntityDraw::DoMousePress - GetWorldMousePos failed", LOGTYPE_Error);

	if(m_eCurHoverGrabPoint != TransformCtrl::GRAB_None)
		BeginTransform(true);
	else
	{
		// Check if the click position (m_ptDragStart) is over an item
		if((m_MultiTransform.IsShown() && m_MultiTransform.IsMouseOverBoundingVolume()) == false &&
			m_pCurHoverItem == nullptr)
		{
			m_eDragState = DRAGSTATE_Marquee;
		}
		else
		{
			m_PressTimer.InitStart(0.5f);

			// Select the hover item if it's not apart of the selection
			if(m_SelectedItemList.contains(m_pCurHoverItem) == false)
			{
				// Special Case: Allow user to optionally translate selected item(s) if possible - meaning the mouse is ALSO hovering the selected item(s)
				//               Therefore, don't decide to make selection until the is mouse button release
				bool bSpecialCase = m_MultiTransform.IsShown() && m_MultiTransform.IsMouseOverBoundingVolume();
				for(EntityDrawItem *pSelectedItem : m_SelectedItemList)
				{
					if(pSelectedItem->IsMouseInBounds())
					{
						bSpecialCase = true;
						break;
					}
				}

				if(bSpecialCase == false)
				{
					QList<EntityDrawItem *> selectList;
					selectList << m_pCurHoverItem;

					if(bShiftMod)
						selectList += m_SelectedItemList;

					RequestSelection(selectList);
					m_bSelectionHandled = true;
				}
			}

			m_eDragState = DRAGSTATE_Pending;
		}
	}
}

void EntityDraw::DoMouseRelease_Select(bool bCtrlMod, bool bShiftMod)
{
	m_PressTimer.Reset();

	QList<EntityDrawItem *> affectedItemList;	// Items that are getting selected or deselected
	if(m_eDragState == DRAGSTATE_Marquee)
	{
		b2AABB marqueeAabb;
		HyShape2d tmpShape;
		m_DragShape.GetPrimitive().CalcLocalBoundingShape(tmpShape);
		tmpShape.ComputeAABB(marqueeAabb, glm::mat4(1.0f));
		
		for(EntityDrawItem *pItem : m_ItemList)
		{
			if(pItem->GetTransformCtrl().IsContained(marqueeAabb, m_pCamera))
				affectedItemList << pItem;
		}

		m_DragShape.Setup(SHAPE_None, ENTCOLOR_Clear, 1.0f, 1.0f);
	}
	else if(m_pCurHoverItem) // This covers the resolution of "Special Case" in EntityDraw::DoMousePress_Select
		affectedItemList << m_pCurHoverItem;

	if(m_bSelectionHandled == false)
	{
		if(bShiftMod == false)
			RequestSelection(affectedItemList);
		else
		{
			QList<EntityDrawItem *> selectList = m_SelectedItemList;

			for(EntityDrawItem *pAffectedItem : affectedItemList)
			{
				if(m_SelectedItemList.contains(pAffectedItem))
					selectList.removeOne(pAffectedItem);
				else
					selectList.append(pAffectedItem);
			}

			RequestSelection(selectList);
		}
	}

	// Reset
	m_bSelectionHandled = false;
	Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->SetCursorShape(Qt::ArrowCursor);
}

void EntityDraw::BeginTransform(bool bWithMouse)
{
	if(m_eDragState == DRAGSTATE_Transforming || m_eDragState == DRAGSTATE_Nudging)
		return;

	TransformCtrl *pCurTransform = nullptr;
	if(m_MultiTransform.IsShown())
		pCurTransform = &m_MultiTransform;
	else// if(m_pCurHoverItem)
		pCurTransform = &m_SelectedItemList[0]->GetTransformCtrl(); // &m_pCurHoverItem->GetTransformCtrl();

	if(pCurTransform)
	{
		// Set 'm_ptDragCenter'
		pCurTransform->GetCentroid(m_ptDragCenter);
		m_pCamera->ProjectToWorld(m_ptDragCenter, m_ptDragCenter);

		// Set 'm_vDragStartSize'
		glm::vec2 ptMidRight = pCurTransform->GetGrabPointWorldPos(TransformCtrl::GRAB_MidRight, m_pCamera);
		glm::vec2 ptMidLeft = pCurTransform->GetGrabPointWorldPos(TransformCtrl::GRAB_MidLeft, m_pCamera);
		glm::vec2 ptTopMid = pCurTransform->GetGrabPointWorldPos(TransformCtrl::GRAB_TopMid, m_pCamera);
		glm::vec2 ptBotMid = pCurTransform->GetGrabPointWorldPos(TransformCtrl::GRAB_BotMid, m_pCamera);
		HySetVec(m_vDragStartSize, glm::distance(ptMidLeft, ptMidRight), glm::distance(ptTopMid, ptBotMid));
	}

	m_PrevTransformList.clear();
	for(EntityDrawItem *pDrawItem : m_SelectedItemList)
	{
		if(pDrawItem->GetHyNode()->GetInternalFlags() & NODETYPE_IsBody)
		{
			IHyBody2d *pDrawBody = static_cast<IHyBody2d *>(pDrawItem->GetHyNode());
			pDrawBody->SetDisplayOrder(pDrawBody->GetDisplayOrder()); // This enables the 'EXPLICIT_DisplayOrder' flag to be used during m_ActiveTransform's parental guidance
		}

		m_ActiveTransform.ChildAppend(*pDrawItem->GetHyNode());
		m_PrevTransformList.push_back(pDrawItem->GetHyNode()->GetSceneTransform(0.0f));
	}

	m_fMultiTransformStartRot = m_MultiTransform.rot.Get();

	if(bWithMouse)
	{
		// The mouse cursor must be set when transforming - it is used to determine the type of transform
		// If it isn't set, then it must be translating (it isn't from GetGrabPointCursorShape())
		if(Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->GetCursorShape() == Qt::ArrowCursor)
			Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->SetCursorShape(Qt::SizeAllCursor);

		m_eDragState = DRAGSTATE_Transforming;
	}
	else
		m_eDragState = DRAGSTATE_Nudging;
}

void EntityDraw::DoMouseMove_Transform(bool bCtrlMod, bool bShiftMod)
{
	glm::vec2 ptMousePos;
	if(HyEngine::Input().GetWorldMousePos(ptMousePos) == false)
		return; // Cursor is currently dragged off render window

	TransformCtrl *pCurTransform = nullptr;
	if(m_MultiTransform.IsShown())
		pCurTransform = &m_MultiTransform;
	else
		pCurTransform = &m_SelectedItemList[0]->GetTransformCtrl();

	// Gather snapping candidates
	std::vector<glm::vec2> snapCandidateList;
	uint32 uiSnappingSettings = m_pProjItem->GetProject().GetSnappingSettings();
	float fSnapTolerance = static_cast<float>(uiSnappingSettings & SNAPSETTING_ToleranceMask);
	if(uiSnappingSettings & SNAPSETTING_Enabled)
	{
		if(uiSnappingSettings & SNAPSETTING_Grid)
		{
			//glm::vec2 ptGridSize = glm::vec2(DEFAULT_GRID_SIZE, DEFAULT_GRID_SIZE);// m_pProjItem->GetProject().GetGridSize();
			//ptGridSize = HyMath::RoundVec(m_ActiveTransform.pos.Get() / ptGridSize) * ptGridSize;				
			//m_ActiveTransform.pos.Set(HyMath::RoundToNearest(m_ActiveTransform.pos.GetX(), DEFAULT_GRID_SIZE), HyMath::RoundToNearest(m_ActiveTransform.pos.GetY(), DEFAULT_GRID_SIZE));
		}
		if(uiSnappingSettings & SNAPSETTING_Guides)
		{
			// TODO: implement guides
			glm::vec2 ptGuidePos;
		}
		if(uiSnappingSettings & SNAPSETTING_Origin)
		{
			snapCandidateList.push_back(glm::vec2(0, 0));
		}
		if(uiSnappingSettings & SNAPSETTING_Items)
		{
			for(EntityDrawItem *pItem : m_ItemList)
			{
				// Find snap candidates, and test against them
				if(pItem->GetEntityTreeItemData()->IsSelected() == false)
				{
					snapCandidateList.push_back(pItem->GetTransformCtrl().GetGrabPointWorldPos(TransformCtrl::GRAB_BotLeft, m_pCamera));
					snapCandidateList.push_back(pItem->GetTransformCtrl().GetGrabPointWorldPos(TransformCtrl::GRAB_BotRight, m_pCamera));
					snapCandidateList.push_back(pItem->GetTransformCtrl().GetGrabPointWorldPos(TransformCtrl::GRAB_TopRight, m_pCamera));
					snapCandidateList.push_back(pItem->GetTransformCtrl().GetGrabPointWorldPos(TransformCtrl::GRAB_TopLeft, m_pCamera));

					if(uiSnappingSettings & SNAPSETTING_ItemMidPoints)
					{
						glm::vec2 ptMid;
						pItem->GetTransformCtrl().GetCentroid(ptMid);
						m_pCamera->ProjectToWorld(ptMid, ptMid);
						snapCandidateList.push_back(ptMid);
					}
				}
			}
		}
	}

	// The mouse cursor must be set when transforming - it is used to determine the type of transform
	switch(Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->GetCursorShape())
	{
	case Qt::ClosedHandCursor: // Rotating
		m_ActiveTransform.rot_pivot.Set(m_ptDragCenter);
		if(bShiftMod)
		{
			float fRot = HyMath::AngleFromVector(m_ptDragCenter - ptMousePos) - HyMath::AngleFromVector(m_ptDragCenter - m_ptDragStart);

			if(m_ActiveTransform.ChildCount() == 1)
				m_ActiveTransform.ChildGet(0)->rot.Set(HyMath::RoundToNearest(m_ActiveTransform.ChildGet(0)->rot.Get(), 15.0f));

			m_ActiveTransform.rot.Set(HyMath::RoundToNearest(fRot, 15.0f));
		}
		else
			m_ActiveTransform.rot.Set(HyMath::Round(HyMath::AngleFromVector(m_ptDragCenter - ptMousePos) - HyMath::AngleFromVector(m_ptDragCenter - m_ptDragStart)));
		break;

	case Qt::SizeAllCursor:		// Translating
		if(bShiftMod)
		{
			glm::vec2 ptTarget = ptMousePos;
			const glm::vec2 UNIT_VECTOR_LIST[] = {
				glm::vec2(1, 1),
				glm::vec2(1, 0),
				glm::vec2(1, -1),
				glm::vec2(0, -1),
				glm::vec2(-1, -1),
				glm::vec2(-1, 0),
				glm::vec2(-1, 1),
				glm::vec2(0, 1)
			};

			glm::vec2 ptClosest = m_ptDragCenter;
			float fMinDist = glm::length(ptTarget - m_ptDragCenter);
			for(int i = 0; i < 8; ++i)
			{
				glm::vec2 ptCandidate = HyMath::ClosestPointOnRay(m_ptDragCenter, glm::normalize(UNIT_VECTOR_LIST[i]), ptTarget);
				float fDist = glm::length(ptTarget - ptCandidate);
				if(fDist < fMinDist)
				{
					ptClosest = ptCandidate;
					fMinDist = fDist;
				}
			}

			m_ActiveTransform.pos.Set(HyMath::RoundVec(ptClosest - m_ptDragCenter));
		}
		else
			m_ActiveTransform.pos.Set(HyMath::RoundVec(ptMousePos - m_ptDragStart));

		if(snapCandidateList.empty() == false)
		{
			RefreshTransforms(); // Need to refreshTransforms here because of the above m_ActiveTransform.pos.Set() call

			bool bSnapX = false;
			bool bSnapY = false;

			for(glm::vec2 ptSnapCandidate : snapCandidateList)
			{
				for(int i = 0; i <= TransformCtrl::GRAB_TopLeft + 1; ++i)
				{
					glm::vec2 ptTestPoint;
					if(i == TransformCtrl::GRAB_TopLeft + 1)
					{
						pCurTransform->GetCentroid(ptTestPoint);
						m_pCamera->ProjectToWorld(ptTestPoint, ptTestPoint);
					}
					else
						ptTestPoint = pCurTransform->GetGrabPointWorldPos(static_cast<TransformCtrl::GrabPointType>(i), m_pCamera);

					if(bSnapX == false && abs(ptSnapCandidate.x - ptTestPoint.x) <= fSnapTolerance)
					{
						// Perform the snap
						m_ActiveTransform.pos.Offset(ptSnapCandidate.x - ptTestPoint.x, 0.0f);
						
						// Determine the horizontal snap guide line (in scene coordinates)
						glm::vec2 ptStartPos, ptEndPos;
						if(ptTestPoint.y < ptSnapCandidate.y)
							ptTestPoint.y = pCurTransform->GetSceneAABB().lowerBound.y;
						else
							ptTestPoint.y = pCurTransform->GetSceneAABB().upperBound.y;

						ptTestPoint.x = ptSnapCandidate.x;

						// Convert to camera coordinates
						//m_pCamera->ProjectToCamera(ptTestPoint, ptTestPoint);
						//m_pCamera->ProjectToCamera(ptSnapCandidate, ptSnapCandidate);

						// Initialize the horizontal snap guide line
						m_SnapGuideHorz.SetAsLineSegment(ptTestPoint, ptSnapCandidate);
						if(m_SnapGuideHorz.IsVisible() == false)
						{
							m_SnapGuideHorz.alpha.Set(0.0f);
							m_SnapGuideHorz.alpha.Tween(1.0f, 0.33f);
						}
						m_SnapGuideHorz.SetVisible(true);
						bSnapX = true;
					}
					if(bSnapY == false && abs(ptSnapCandidate.y - ptTestPoint.y) <= fSnapTolerance)
					{
						// Perform the snap
						m_ActiveTransform.pos.Offset(0.0f, ptSnapCandidate.y - ptTestPoint.y);

						// Determine the vertical snap guide line (in scene coordinates)
						glm::vec2 ptStartPos, ptEndPos;
						if(ptTestPoint.x < ptSnapCandidate.x)
							ptTestPoint.x = pCurTransform->GetSceneAABB().lowerBound.x;
						else
							ptTestPoint.x = pCurTransform->GetSceneAABB().upperBound.x;

						ptTestPoint.y = ptSnapCandidate.y;

						// Convert to camera coordinates
						//m_pCamera->ProjectToCamera(ptTestPoint, ptTestPoint);
						//m_pCamera->ProjectToCamera(ptSnapCandidate, ptSnapCandidate);

						// Initialize the vertical snap guide line
						m_SnapGuideVert.SetAsLineSegment(ptTestPoint, ptSnapCandidate);
						if(m_SnapGuideVert.IsVisible() == false)
						{
							m_SnapGuideVert.alpha.Set(0.0f);
							m_SnapGuideVert.alpha.Tween(1.0f, 0.33f);
						}
						m_SnapGuideVert.SetVisible(true);
						bSnapY = true;
					}

					if(bSnapX && bSnapY)
						break;
				}

				if(bSnapX && bSnapY)
					break;
			}

			if(bSnapX == false)
				m_SnapGuideHorz.SetVisible(false);
			if(bSnapY == false)
				m_SnapGuideVert.SetVisible(false);
		}
		break;

	case Qt::SizeBDiagCursor:	// Scaling
	case Qt::SizeVerCursor:		// Scaling
	case Qt::SizeFDiagCursor:	// Scaling
	case Qt::SizeHorCursor: {	// Scaling
		bool bUniformScale = true;
		TransformCtrl::GrabPointType eAnchorPoint = TransformCtrl::GRAB_None;
		TransformCtrl::GrabPointType eAnchorWidth = TransformCtrl::GRAB_None;
		TransformCtrl::GrabPointType eAnchorHeight = TransformCtrl::GRAB_None;
		glm::bvec2 bScaleDimensions;
		switch(m_eCurHoverGrabPoint)
		{
		case TransformCtrl::GRAB_BotLeft:
			bUniformScale = true;
			eAnchorPoint = TransformCtrl::GRAB_TopRight;
			eAnchorWidth = TransformCtrl::GRAB_TopLeft;
			eAnchorHeight = TransformCtrl::GRAB_BotRight;
			bScaleDimensions.x = bScaleDimensions.y = true;
			break;
		case TransformCtrl::GRAB_BotRight:
			bUniformScale = true;
			eAnchorPoint = TransformCtrl::GRAB_TopLeft;
			eAnchorWidth = TransformCtrl::GRAB_TopRight;
			eAnchorHeight = TransformCtrl::GRAB_BotLeft;
			bScaleDimensions.x = bScaleDimensions.y = true;
			break;
		case TransformCtrl::GRAB_TopRight:
			bUniformScale = true;
			eAnchorPoint = TransformCtrl::GRAB_BotLeft;
			eAnchorWidth = TransformCtrl::GRAB_BotRight;
			eAnchorHeight = TransformCtrl::GRAB_TopLeft;
			bScaleDimensions.x = bScaleDimensions.y = true;
			break;
		case TransformCtrl::GRAB_TopLeft:
			bUniformScale = true;
			eAnchorPoint = TransformCtrl::GRAB_BotRight;
			eAnchorWidth = TransformCtrl::GRAB_BotLeft;
			eAnchorHeight = TransformCtrl::GRAB_TopRight;
			bScaleDimensions.x = bScaleDimensions.y = true;
			break;
		case TransformCtrl::GRAB_BotMid:
			bUniformScale = false;
			eAnchorPoint = TransformCtrl::GRAB_TopMid;
			eAnchorHeight = TransformCtrl::GRAB_BotMid;
			bScaleDimensions.x = false;
			bScaleDimensions.y = true;
			break;
		case TransformCtrl::GRAB_MidRight:
			bUniformScale = false;
			eAnchorPoint = TransformCtrl::GRAB_MidLeft;
			eAnchorWidth = TransformCtrl::GRAB_MidRight;
			bScaleDimensions.x = true;
			bScaleDimensions.y = false;
			break;
		case TransformCtrl::GRAB_TopMid:
			bUniformScale = false;
			eAnchorPoint = TransformCtrl::GRAB_BotMid;
			eAnchorHeight = TransformCtrl::GRAB_TopMid;
			bScaleDimensions.x = false;
			bScaleDimensions.y = true;
			break;
		case TransformCtrl::GRAB_MidLeft:
			bUniformScale = false;
			eAnchorPoint = TransformCtrl::GRAB_MidRight;
			eAnchorWidth = TransformCtrl::GRAB_MidLeft;
			bScaleDimensions.x = true;
			bScaleDimensions.y = false;
			break;
		default:
			break;
		}

		glm::vec2 ptDragAnchorPoint = pCurTransform->GetGrabPointWorldPos(eAnchorPoint, m_pCamera);
		m_ActiveTransform.scale_pivot.Set(ptDragAnchorPoint);
		m_ActiveTransform.rot_pivot.Set(m_ptDragCenter);

		glm::vec2 vDesiredSize(m_vDragStartSize);
		if(bScaleDimensions.x)
		{
			glm::vec2 ptAnchorWidth = pCurTransform->GetGrabPointWorldPos(eAnchorWidth, m_pCamera);
			if(ptAnchorWidth == ptDragAnchorPoint)
				vDesiredSize.x = 0.01f;
			else
			{
				ptAnchorWidth = HyMath::ClosestPointOnRay(ptDragAnchorPoint, glm::normalize(ptAnchorWidth - ptDragAnchorPoint), ptMousePos);
				vDesiredSize.x = glm::distance(ptDragAnchorPoint, ptAnchorWidth);
			}
		}
		if(bScaleDimensions.y)
		{
			glm::vec2 ptAnchorHeight = pCurTransform->GetGrabPointWorldPos(eAnchorHeight, m_pCamera);
			if(ptAnchorHeight == ptDragAnchorPoint)
				vDesiredSize.y = 0.01f;
			else
			{
				ptAnchorHeight = HyMath::ClosestPointOnRay(ptDragAnchorPoint, glm::normalize(ptAnchorHeight - ptDragAnchorPoint), ptMousePos);
				vDesiredSize.y = glm::distance(ptDragAnchorPoint, ptAnchorHeight);
			}
		}

		if(bShiftMod)
			bUniformScale = !bUniformScale;

		glm::vec2 vScaleAmt(1.0f);
		if(bUniformScale)
		{
			float fScaleAmt = HyMath::Max(vDesiredSize.x / m_vDragStartSize.x, vDesiredSize.y / m_vDragStartSize.y);
			HySetVec(vScaleAmt, fScaleAmt, fScaleAmt);
		}
		else
			HySetVec(vScaleAmt, vDesiredSize.x / m_vDragStartSize.x, vDesiredSize.y / m_vDragStartSize.y);

		// TODO: When transforming a single item that has a non-zero rotation, it skews the scaling during preview
		if(std::isnan(vScaleAmt.x) || std::isnan(vScaleAmt.y))
		{
			HyGuiLog("Scaling resulted in NaN", LOGTYPE_Error);
			int asdf = 0;
			asdf++;
		}

		m_ActiveTransform.scale.Set(vScaleAmt);

		break; }

	case Qt::ArrowCursor:
	case Qt::WaitCursor:
		m_eDragState = DRAGSTATE_None;
		break;

	default:
		HyGuiLog("EntityDraw::OnMouseMoveEvent - Unknown cursor state not handled: " % QString::number(Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->GetCursorShape()), LOGTYPE_Error);
	}

	// This updates the preview of a shape (its 'outline') when being transformed
	for(EntityDrawItem *pSelectedItem : m_SelectedItemList)
	{
		if(pSelectedItem->GetEntityTreeItemData()->GetType() == ITEM_BoundingVolume)
			pSelectedItem->GetShapeCtrl().Setup(pSelectedItem->GetShapeCtrl().GetShapeType(), ENTCOLOR_Shape, 0.7f, 0.0f);
	}

	RefreshTransforms();
}

void EntityDraw::DoMouseRelease_Transform()
{
	QList<EntityTreeItemData *> treeItemDataList;
	QList<glm::mat4> newTransformList;
	for(EntityDrawItem *pDrawItem : m_SelectedItemList)
	{
		newTransformList.push_back(pDrawItem->GetHyNode()->GetSceneTransform(0.0f));

		EntityTreeItemData *pTreeItemData = static_cast<EntityModel *>(m_pProjItem->GetModel())->GetTreeModel().FindTreeItemData(pDrawItem->GetEntityTreeItemData()->GetThisUuid());
		treeItemDataList.push_back(pTreeItemData);
	}

	int iStateIndex = 0;
	int iFrameIndex = 0;
	if(m_pProjItem->GetWidget() == nullptr)
		HyGuiLog("EntityDraw::DoMouseRelease_Transform - m_pProjItem->GetWidget() is nullptr", LOGTYPE_Error);
	else
	{
		iStateIndex = m_pProjItem->GetWidget()->GetCurStateIndex();
		iFrameIndex = static_cast<EntityStateData *>(m_pProjItem->GetModel()->GetStateData(iStateIndex))->GetDopeSheetScene().GetCurrentFrame();
	}

	// Transferring the children in 'm_ActiveTransform' back into 'm_RootEntity' will be done automatically in OnApplyJsonMeta()
	QUndoCommand *pCmd = new EntityUndoCmd_Transform(*m_pProjItem, iStateIndex, iFrameIndex, treeItemDataList, newTransformList, m_PrevTransformList);
	m_pProjItem->GetUndoStack()->push(pCmd);

	m_vNudgeTranslate.setX(0);
	m_vNudgeTranslate.setY(0);

	// Reset 'm_ActiveTransform' to prep for the next transform
	m_ActiveTransform.pos.Set(0.0f, 0.0f);
	m_ActiveTransform.rot.Set(0.0f);
	m_ActiveTransform.scale.Set(1.0f, 1.0f);

	m_SnapGuideHorz.SetVisible(false);
	m_SnapGuideVert.SetVisible(false);

	Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->SetCursorShape(Qt::ArrowCursor);
}

void EntityDraw::DoMouseMove_ShapeEdit(bool bCtrlMod, bool bShiftMod)
{
	glm::vec2 ptCurMousePos;
	HyEngine::Input().GetWorldMousePos(ptCurMousePos);

	switch(m_eDragState)
	{
	case DRAGSTATE_None:
		if(m_eShapeEditState == SHAPESTATE_VertexEditMode)
		{
			m_eCurVemAction = m_pCurVertexEditItem->GetShapeCtrl().GetMouseVemAction(bCtrlMod, bShiftMod, false);

			Qt::CursorShape eCursorShape = Qt::ArrowCursor;
			switch(m_eCurVemAction)
			{
			case ShapeCtrl::VEMACTION_Invalid:				eCursorShape = Qt::ForbiddenCursor; break;
			case ShapeCtrl::VEMACTION_Translate:			eCursorShape = Qt::SizeAllCursor; break;
			case ShapeCtrl::VEMACTION_GrabPoint:			eCursorShape = Qt::PointingHandCursor; break;
			case ShapeCtrl::VEMACTION_RadiusHorizontal:		eCursorShape = Qt::SizeHorCursor; break;
			case ShapeCtrl::VEMACTION_RadiusVertical:		eCursorShape = Qt::SizeVerCursor; break;
			case ShapeCtrl::VEMACTION_Add:					eCursorShape = Qt::CrossCursor; break;
			case ShapeCtrl::VEMACTION_None:					eCursorShape = Qt::ArrowCursor; break;
			default:
				HyGuiLog("EntityDraw::DoMouseMove_ShapeEdit() - Unhandled ShapeCtrl::VemAction for DRAGSTATE_None", LOGTYPE_Error);
				break;
			}
			Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->SetCursorShape(eCursorShape);
		}
		break;

	case DRAGSTATE_Marquee:
		Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->SetCursorShape(Qt::CrossCursor);

		m_DragShape.Setup(SHAPE_Box, ENTCOLOR_Marquee, 0.25f, 1.0f);
		m_DragShape.SetAsDrag(/*bShiftMod*/false, m_ptDragStart, ptCurMousePos, m_pCamera); // Don't do centering when holding shift and marquee selecting
		break;

	case DRAGSTATE_Pending:
		if(glm::distance(m_ptDragStart, ptCurMousePos) >= 2.0f)
			m_eDragState = DRAGSTATE_Transforming;
		break;

	case DRAGSTATE_Transforming:
		if(m_eShapeEditState == SHAPESTATE_DragAddPrimitive || m_eShapeEditState == SHAPESTATE_DragAddShape)
			m_DragShape.SetAsDrag(bShiftMod, m_ptDragStart, ptCurMousePos, m_pCamera);
		else // SHAPESTATE_VertexEditMode
		{
			if(m_pCurVertexEditItem->GetShapeCtrl().TransformVemVerts(m_eCurVemAction, m_ptDragStart, ptCurMousePos, m_pCamera))
			{
				if(m_eCurVemAction != ShapeCtrl::VEMACTION_Add)
					Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->SetCursorShape(Qt::BlankCursor);
			}
			else
				Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->SetCursorShape(Qt::ForbiddenCursor);
		}
		break;
	}
}

void EntityDraw::DoMousePress_ShapeEdit(bool bCtrlMod, bool bShiftMod)
{
	if(HyEngine::Input().GetWorldMousePos(m_ptDragStart) == false)
		HyGuiLog("EntityDraw::DoMousePress - GetWorldMousePos failed", LOGTYPE_Error);

	if(m_eShapeEditState == SHAPESTATE_DragAddPrimitive || m_eShapeEditState == SHAPESTATE_DragAddShape)
	{
		m_PressTimer.InitStart(0.5f);
		m_eDragState = DRAGSTATE_Pending;
	}
	else // SHAPESTATE_VertexEditMode
	{
		if(m_eCurVemAction != ShapeCtrl::VEMACTION_Translate && m_eCurVemAction != ShapeCtrl::VEMACTION_Add && bShiftMod == false)
			m_pCurVertexEditItem->GetShapeCtrl().UnselectAllVemVerts();

		m_eCurVemAction = m_pCurVertexEditItem->GetShapeCtrl().GetMouseVemAction(bCtrlMod, bShiftMod, true);

		if(m_eCurVemAction == ShapeCtrl::VEMACTION_None)
			m_eDragState = DRAGSTATE_Marquee;
		else if(m_eCurVemAction != ShapeCtrl::VEMACTION_Invalid) // A GrabPoint is selected
		{
			if(m_eCurVemAction == ShapeCtrl::VEMACTION_Add)
			{
				if(m_pCurVertexEditItem->GetShapeCtrl().TransformVemVerts(m_eCurVemAction, m_ptDragStart, m_ptDragStart, m_pCamera) == false)
					Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->SetCursorShape(Qt::ForbiddenCursor);
			}
			else
				Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->SetCursorShape(Qt::BlankCursor);

			m_PressTimer.InitStart(0.5f);
			m_eDragState = DRAGSTATE_Pending;
		}
	}
}

void EntityDraw::DoMouseRelease_ShapeEdit(bool bCtrlMod, bool bShiftMod)
{
	if(m_eShapeEditState == SHAPESTATE_DragAddPrimitive || m_eShapeEditState == SHAPESTATE_DragAddShape)
	{
		if(m_eDragState == DRAGSTATE_Transforming)
		{
			QUndoCommand *pCmd = new EntityUndoCmd_AddNewShape(*m_pProjItem,
				m_DragShape.GetShapeType(),
				m_DragShape.Serialize(),
				m_eShapeEditState == SHAPESTATE_DragAddPrimitive,
				-1);
			m_pProjItem->GetUndoStack()->push(pCmd);

			m_DragShape.Setup(SHAPE_None, ENTCOLOR_Clear, 1.0f, 1.0f);
		}
	}
	else // SHAPESTATE_VertexEditMode
	{
		if(m_eDragState == DRAGSTATE_Marquee)
		{
			b2AABB marqueeAabb;
			HyShape2d tmpShape;
			m_DragShape.GetPrimitive().CalcLocalBoundingShape(tmpShape);
			tmpShape.ComputeAABB(marqueeAabb, glm::mat4(1.0f));

			if(bShiftMod == false)
				m_pCurVertexEditItem->GetShapeCtrl().UnselectAllVemVerts();
			m_pCurVertexEditItem->GetShapeCtrl().SelectVemVerts(marqueeAabb, m_pCamera);

			m_DragShape.Setup(SHAPE_None, ENTCOLOR_Clear, 1.0f, 1.0f);
		}
		else if(m_eDragState == DRAGSTATE_Transforming || (m_eDragState == DRAGSTATE_Pending && m_eCurVemAction == ShapeCtrl::VEMACTION_Add))
		{
			EntityTreeItemData *pTreeItemData = static_cast<EntityModel *>(m_pProjItem->GetModel())->GetTreeModel().FindTreeItemData(m_pCurVertexEditItem->GetEntityTreeItemData()->GetThisUuid());

			int iStateIndex = 0;
			int iFrameIndex = 0;
			if(m_pProjItem->GetWidget() == nullptr)
				HyGuiLog("EntityDraw::DoMouseRelease_ShapeEdit - m_pProjItem->GetWidget() is nullptr", LOGTYPE_Error);
			else
			{
				iStateIndex = m_pProjItem->GetWidget()->GetCurStateIndex();
				iFrameIndex = static_cast<EntityStateData *>(m_pProjItem->GetModel()->GetStateData(iStateIndex))->GetDopeSheetScene().GetCurrentFrame();
			}

			QUndoCommand *pCmd = new EntityUndoCmd_ShapeData(*m_pProjItem, iStateIndex, iFrameIndex, pTreeItemData, m_eCurVemAction, m_pCurVertexEditItem->GetShapeCtrl().SerializeVemVerts(m_pCamera));
			m_pProjItem->GetUndoStack()->push(pCmd);
		}

		Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->SetCursorShape(Qt::ArrowCursor);
	}
}

/*static*/ void EntityDraw::OnMousePressTimer(void *pData)
{
	EntityDraw *pThis = static_cast<EntityDraw *>(pData);

	if(pThis->m_eDragState == DRAGSTATE_Marquee)
		return;

	if(pThis->m_eShapeEditState == SHAPESTATE_None && pThis->m_eDragState == DRAGSTATE_Pending)
		pThis->BeginTransform(true);
}
