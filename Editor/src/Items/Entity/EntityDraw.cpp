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
#include "EntityUndoCmds.h"

#include <QKeyEvent>

EntityDraw::EntityDraw(ProjectItemData *pProjItem, const FileDataPair &initFileDataRef) :
	IDrawEx(pProjItem, initFileDataRef),
	m_RootEntity(this),
	m_bActivateVemOnNextJsonMeta(false),
	m_bPlayingPreview(false),
	m_eShapeEditState(SHAPESTATE_None),
	m_pCurVertexEditItem(nullptr),
	m_eCurVemAction(ShapeCtrl::VEMACTION_None)
{

}

/*virtual*/ EntityDraw::~EntityDraw()
{
}

/*virtual*/ void EntityDraw::OnUndoStackIndexChanged(int iIndex) /*override*/
{
	if(m_eShapeEditState == SHAPESTATE_DragAddPrimitive || m_eShapeEditState == SHAPESTATE_DragAddShape)
		RequestClearShapeEdit();
}

/*virtual*/ void EntityDraw::OnKeyPressEvent(QKeyEvent *pEvent) /*override*/
{
	if(m_bPlayingPreview)
	{
		IDraw::OnKeyPressEvent(pEvent);
		return;
	}

	IDrawEx::OnKeyPressEvent(pEvent);

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
}

/*virtual*/ void EntityDraw::OnKeyReleaseEvent(QKeyEvent *pEvent) /*override*/
{
	if(m_bPlayingPreview)
	{
		IDraw::OnKeyReleaseEvent(pEvent);
		return;
	}

	IDrawEx::OnKeyReleaseEvent(pEvent);
}

/*virtual*/ void EntityDraw::OnMouseMoveEvent(QMouseEvent *pEvent) /*override*/
{
	if(m_bPlayingPreview)
	{
		IDraw::OnMouseMoveEvent(pEvent);
		return;
	}

	IDrawEx::OnMouseMoveEvent(pEvent);
}

/*virtual*/ void EntityDraw::OnMousePressEvent(QMouseEvent *pEvent) /*override*/
{
	if(m_bPlayingPreview)
	{
		IDraw::OnMousePressEvent(pEvent);
		return;
	}

	IDrawEx::OnMousePressEvent(pEvent);
}

/*virtual*/ void EntityDraw::OnMouseReleaseEvent(QMouseEvent *pEvent) /*override*/
{
	if(m_bPlayingPreview)
	{
		IDraw::OnMouseReleaseEvent(pEvent);
		return;
	}

	IDrawEx::OnMouseReleaseEvent(pEvent);
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

	Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->RestoreCursorShape();
}

void EntityDraw::SetExtrapolatedProperties(bool bPreviewPlaying)
{
	if(m_pProjItem->GetWidget() == nullptr)
		HyGuiLog("EntityDraw::SetExtrapolatedProperties - m_pProjItem->GetWidget() is nullptr", LOGTYPE_Error);

	EntityDopeSheetScene &entityDopeSheetSceneRef = static_cast<EntityStateData *>(m_pProjItem->GetModel()->GetStateData(m_pProjItem->GetWidget()->GetCurStateIndex()))->GetDopeSheetScene();

	const float fFRAME_DURATION = 1.0f / static_cast<EntityModel &>(entityDopeSheetSceneRef.GetStateData()->GetModel()).GetFramesPerSecond();
	const int iCURRENT_FRAME = entityDopeSheetSceneRef.GetCurrentFrame();

	// Set the extrapolated properties for the 'm_RootEntity' item
	EntityTreeItemData *pRootTreeItemData = static_cast<EntityModel *>(m_pProjItem->GetModel())->GetTreeModel().GetRootTreeItemData();
	QMap<int, QList<TimelineEvent>> timelineEventList = entityDopeSheetSceneRef.AssembleTimelineEvents(pRootTreeItemData);

	ExtrapolateProperties(&m_RootEntity,
						  nullptr,
						  false,
						  ITEM_Unknown, // 'ITEM_Unknown' indicates this is the root
						  fFRAME_DURATION,
						  iCURRENT_FRAME,
						  entityDopeSheetSceneRef.GetKeyFramesMap()[pRootTreeItemData],
						  timelineEventList,
						  bPreviewPlaying == false,
						  m_pCamera);

	// Set the extrapolated properties for all the children items
	for(EntityDrawItem *pDrawItem : m_ItemList)
	{
		QMap<int, QList<TimelineEvent>> mergedTimelineEventList = timelineEventList;

		EntityTreeItemData *pEntityTreeItemData = pDrawItem->GetEntityTreeItemData();
		ItemType eItemType = pEntityTreeItemData->GetType();
		if(eItemType == ITEM_Prefix) // aka Shapes folder
			continue;
		else if(eItemType == ITEM_Entity) // Sub-Entity
		{
			QMap<int, QList<TimelineEvent>> subEntTimelineEventList = entityDopeSheetSceneRef.AssembleTimelineEvents(pEntityTreeItemData);

			// Merge 'subEntTimelineEventList' into 'mergedTimelineEventList'
			for(auto cachedEventIter = subEntTimelineEventList.begin(); cachedEventIter != subEntTimelineEventList.end(); ++cachedEventIter)
				mergedTimelineEventList[cachedEventIter.key()].append(cachedEventIter.value());
		}

		ExtrapolateProperties(pDrawItem->GetHyNode(),
							  &pDrawItem->GetShapeCtrl(),
							  pEntityTreeItemData->IsSelected(),
							  eItemType,
							  fFRAME_DURATION,
							  iCURRENT_FRAME,
							  entityDopeSheetSceneRef.GetKeyFramesMap()[pEntityTreeItemData],
							  mergedTimelineEventList,
							  bPreviewPlaying == false,
							  m_pCamera);
	}

	RefreshTransforms();
}

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

	SetExtrapolatedProperties(false);

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
