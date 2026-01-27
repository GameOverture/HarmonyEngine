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
#include "GfxShapeModel.h"

#include <QKeyEvent>
#include <QApplication>

EntityDraw::EntityDraw(ProjectItemData *pProjItem, const FileDataPair &initFileDataRef) :
	IDrawEx(pProjItem, initFileDataRef),
	m_RootEntity(this),
	m_bPlayingPreview(false),
	m_eEditModeState(EDITMODE_Off),
	m_EditModeWindowOutline(this)
{
	m_EditModeWindowOutline.UseWindowCoordinates();
	m_EditModeWindowOutline.SetTint(HyGlobal::GetEditorColor(EDITORCOLOR_EditMode));
	m_EditModeWindowOutline.SetWireframe(true);
	m_EditModeWindowOutline.SetLineThickness(5.0f);
	m_EditModeWindowOutline.SetVisible(false);
}

/*virtual*/ EntityDraw::~EntityDraw()
{
}

/*virtual*/ void EntityDraw::OnUndoStackIndexChanged(int iIndex) /*override*/
{
	IDrawEx::OnUndoStackIndexChanged(iIndex);
	//if(GetCurAction() == HYACTION_EntityShapeDragAddPrimitive || GetCurAction() == HYACTION_EntityShapeDragAddShape)
	//	RequestClearShapeEdit();
}

/*virtual*/ void EntityDraw::OnKeyPressEvent(QKeyEvent *pEvent) /*override*/
{
	IDrawEx::OnKeyPressEvent(pEvent);

	if(pEvent->key() == Qt::Key_Backspace) // Qt::Key_Delete isn't being passed to this callback
	{
		//EntityDrawItem *pCurVertexEditItem = GetCurShapeEditItem();
		//if(pCurVertexEditItem)
		//{
		//	if(pCurVertexEditItem->GetShapeCtrl().RemoveSelectedVerts() == false)
		//		return;

		//	int iStateIndex = 0;
		//	int iFrameIndex = 0;
		//	if(m_pProjItem->GetWidget() == nullptr)
		//		HyGuiLog("EntityDraw::OnKeyPressEvent - m_pProjItem->GetWidget() is nullptr", LOGTYPE_Error);
		//	else
		//	{
		//		iStateIndex = m_pProjItem->GetWidget()->GetCurStateIndex();
		//		iFrameIndex = static_cast<EntityStateData *>(m_pProjItem->GetModel()->GetStateData(iStateIndex))->GetDopeSheetScene().GetCurrentFrame();
		//	}

		//	QUuid thisUuid = pCurVertexEditItem->GetEntityTreeItemData()->GetThisUuid();
		//	EntityTreeItemData *pTreeItemData = static_cast<EntityModel *>(m_pProjItem->GetModel())->GetTreeModel().FindTreeItemData(thisUuid);

		//	QUndoCommand *pCmd = new EntityUndoCmd_ShapeData("Removed vertices from " % pTreeItemData->GetCodeName(), *m_pProjItem, iStateIndex, iFrameIndex, pTreeItemData,  pCurVertexEditItem->GetShapeView().SerializeVemVerts(m_pCamera));
		//	m_pProjItem->GetUndoStack()->push(pCmd);
		//}
	}
}

/*virtual*/ void EntityDraw::OnKeyReleaseEvent(QKeyEvent *pEvent) /*override*/
{
	IDrawEx::OnKeyReleaseEvent(pEvent);
}

/*virtual*/ void EntityDraw::OnMouseMoveEvent(QMouseEvent *pEvent) /*override*/
{
	IDrawEx::OnMouseMoveEvent(pEvent);

	if(GetCurAction() != HYACTION_EditMode)
		return;

	EntityDrawItem *pCurEditItem = GetCurEditItem();
	if(pCurEditItem == nullptr)
	{
		HyGuiLog("EntityDraw::OnMouseMoveEvent - with null edit item!", LOGTYPE_Error);
		return;
	}
	EntityTreeItemData *pTreeItemData = pCurEditItem->GetEntityTreeItemData();

	glm::vec2 ptWorldMousePos;
	m_pCamera->ProjectToWorld(HyEngine::Input().GetMousePos(), ptWorldMousePos);

	switch(m_eEditModeState)
	{
	case EDITMODE_Off:
		HyGuiLog("EntityDraw::OnMouseMoveEvent - EDITMODE_Off reached in Edit Mode!", LOGTYPE_Error);
		break;

	case EDITMODE_Idle:
		if(pTreeItemData->GetShape2dModel())
		{
			ShapeMouseMoveResult eResult = pTreeItemData->GetShape2dModel()->MouseMoveIdle(ptWorldMousePos);
			switch(eResult)
			{
			case SHAPEMOUSEMOVE_Creation:				Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->setCursor(Qt::CrossCursor); break;
			case SHAPEMOUSEMOVE_Outside:				Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->setCursor(Qt::ArrowCursor); break;
			case SHAPEMOUSEMOVE_Inside:					Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->setCursor(Qt::SizeAllCursor); break;
			case SHAPEMOUSEMOVE_AppendVertex:			Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->setCursor(Qt::CrossCursor); break;
			case SHAPEMOUSEMOVE_InsertVertex:			Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->setCursor(Qt::CrossCursor); break;
			case SHAPEMOUSEMOVE_HoverVertex:			Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->setCursor(Qt::PointingHandCursor); break;
			case SHAPEMOUSEMOVE_HoverSelectedVertex:	Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->setCursor(Qt::SizeAllCursor); break;
			case SHAPEMOUSEMOVE_HoverCenter:			Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->setCursor(Qt::SizeAllCursor); break;
			default:
				HyGuiLog("EntityDraw::OnMouseMoveEvent - EDITMODE_Idle with unsupported shape mouse move result!", LOGTYPE_Error);
				break;
			}
		}
		else
			HyGuiLog("EntityDraw::OnMouseMoveEvent - EDITMODE_Idle with unsupported edit item type!", LOGTYPE_Error);
		break;

	case EDITMODE_MouseDownOutside:
	case EDITMODE_MouseDownTransform: {
		QPointF dragDelta = QPointF(ptWorldMousePos.x, ptWorldMousePos.y) - QPointF(m_ptDragStart.x, m_ptDragStart.y);
		if(dragDelta.manhattanLength() >= MANHATTAN_DRAG_THRESHOLD)
		{
			if(m_eEditModeState == EDITMODE_MouseDownOutside)
			{
				m_MarqueeCtrl.SetAsDrag(m_ptDragStart, ptWorldMousePos);
				m_eEditModeState = EDITMODE_MouseDragMarquee;
			}
			else // EDITMODE_MouseDownTransform
				m_eEditModeState = EDITMODE_MouseDragTransform;
		}
		break; }

	case EDITMODE_MouseDragMarquee:
		if(pTreeItemData->GetShape2dModel())
			m_MarqueeCtrl.SetAsDrag(m_ptDragStart, ptWorldMousePos);
		else
			HyGuiLog("EntityDraw::OnMouseMoveEvent - EDITMODE_MouseDragMarquee with unsupported edit item type!", LOGTYPE_Error);
		break;

	case EDITMODE_MouseDragTransform:
		if(pTreeItemData->GetShape2dModel())
		{
			bool bShiftHeld = (QApplication::keyboardModifiers() & Qt::ShiftModifier);
			pTreeItemData->GetShape2dModel()->MouseMoveTransform(bShiftHeld, m_ptDragStart, ptWorldMousePos);
		}
		else
			HyGuiLog("EntityDraw::OnMouseMoveEvent - fpDoMouseDownTransform with unsupported edit item type!", LOGTYPE_Error);
		break;

	default:
		HyGuiLog("EntityDraw::OnMouseMoveEvent - Unknown EditModeState reached!", LOGTYPE_Error);
		break;
	}
}

/*virtual*/ void EntityDraw::OnMousePressEvent(QMouseEvent *pEvent) /*override*/
{
	IDrawEx::OnMousePressEvent(pEvent);

	if(GetCurAction() != HYACTION_EditMode)
		return;

	EntityDrawItem *pCurEditItem = GetCurEditItem();
	if(pCurEditItem == nullptr)
	{
		HyGuiLog("EntityDraw::OnMousePressEvent - with null edit item!", LOGTYPE_Error);
		return;
	}
	EntityTreeItemData *pTreeItemData = pCurEditItem->GetEntityTreeItemData();

	switch(m_eEditModeState)
	{
	case EDITMODE_Off:
		HyGuiLog("EntityDraw::OnMousePressEvent - EDITMODE_Off reached in Edit Mode!", LOGTYPE_Error);
		break;

	case EDITMODE_Idle:
		m_pCamera->ProjectToWorld(HyEngine::Input().GetMousePos(), m_ptDragStart);
		
		if(pTreeItemData->GetShape2dModel())
		{
			bool bShiftHeld = (QApplication::keyboardModifiers() & Qt::ShiftModifier);
			ShapeMouseMoveResult eResult = pTreeItemData->GetShape2dModel()->MousePressEvent(bShiftHeld, pEvent->buttons(), m_ptDragStart);

			bool bStartTransform = false;
			switch(eResult)
			{
			case SHAPEMOUSEMOVE_Inside:
				bStartTransform = pTreeItemData->GetShape2dModel()->IsAllGrabPointsSelected();
				break;

			case SHAPEMOUSEMOVE_Creation:
			case SHAPEMOUSEMOVE_AppendVertex:
			case SHAPEMOUSEMOVE_InsertVertex:
			case SHAPEMOUSEMOVE_HoverVertex:
			case SHAPEMOUSEMOVE_HoverSelectedVertex:
			case SHAPEMOUSEMOVE_HoverCenter:
				bStartTransform = true;
				break;

			default:
				bStartTransform = false;
				break;
			}

			if(bStartTransform)
			{
				Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->setCursor(Qt::BlankCursor);
				m_eEditModeState = EDITMODE_MouseDownTransform;
			}
			else
			{
				Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->setCursor(Qt::ArrowCursor);
				m_eEditModeState = EDITMODE_MouseDownOutside;
			}
		}
		else
			HyGuiLog("EntityDraw::OnMousePressEvent - EDITMODE_Idle with unsupported edit item type!", LOGTYPE_Error);
		break;

	default:
		HyGuiLog("EntityDraw::OnMousePressEvent - Unhandled EditModeState reached!", LOGTYPE_Error);
		break;
	}
}

/*virtual*/ void EntityDraw::OnMouseReleaseEvent(QMouseEvent *pEvent) /*override*/
{
	IDrawEx::OnMouseReleaseEvent(pEvent);

	if(GetCurAction() != HYACTION_EditMode)
		return;

	EntityDrawItem *pCurEditItem = GetCurEditItem();
	if(pCurEditItem == nullptr)
	{
		HyGuiLog("EntityDraw::OnMousePressEvent - with null edit item!", LOGTYPE_Error);
		return;
	}
	EntityTreeItemData *pTreeItemData = pCurEditItem->GetEntityTreeItemData();

	switch(m_eEditModeState)
	{
	case EDITMODE_Off:
		HyGuiLog("EntityDraw::OnMouseReleaseEvent - EDITMODE_Off reached in Edit Mode!", LOGTYPE_Error);
		break;
	case EDITMODE_Idle:
		HyGuiLog("EntityDraw::OnMouseReleaseEvent - EDITMODE_Idle reached in Edit Mode!", LOGTYPE_Error);
		break;

	case EDITMODE_MouseDownOutside:
	case EDITMODE_MouseDragMarquee:
		if(pTreeItemData->GetShape2dModel())
		{
			b2AABB marqueeAabb;
			if(m_eEditModeState == EDITMODE_MouseDownOutside)
			{
				marqueeAabb.lowerBound = { m_ptDragStart.x, m_ptDragStart.y };
				marqueeAabb.upperBound = { m_ptDragStart.x + 1, m_ptDragStart.y + 1 };
			}
			else
				marqueeAabb = m_MarqueeCtrl.GetSelection();
			
			pTreeItemData->GetShape2dModel()->MouseMarqueeReleased(pEvent->buttons(),
																   QPointF(marqueeAabb.lowerBound.x, marqueeAabb.lowerBound.y),
																   QPointF(marqueeAabb.upperBound.x, marqueeAabb.upperBound.y));
		}
		else
			HyGuiLog("EntityDraw::OnMouseReleaseEvent - EDITMODE_MarqueeSelect with unsupported edit item type!", LOGTYPE_Error);
		
		m_MarqueeCtrl.Hide();
		break;

	case EDITMODE_MouseDownTransform:
	case EDITMODE_MouseDragTransform:
		if(pTreeItemData->GetShape2dModel())
		{
			glm::vec2 ptCurMousePos;
			m_pCamera->ProjectToWorld(HyEngine::Input().GetMousePos(), ptCurMousePos);
			QString sUndoText = pTreeItemData->GetShape2dModel()->MouseTransformReleased(QPointF(ptCurMousePos.x, ptCurMousePos.y));
			if(sUndoText.isEmpty() == false)
			{
				int iStateIndex = m_pProjItem->GetWidget()->GetCurStateIndex();
				int iFrameIndex = static_cast<EntityStateData *>(m_pProjItem->GetModel()->GetStateData(iStateIndex))->GetDopeSheetScene().GetCurrentFrame();
				EditorShape eShapeType = pTreeItemData->GetShape2dModel()->GetType();

				QUndoCommand *pCmd = new EntityUndoCmd_ShapeData(sUndoText, *m_pProjItem, iStateIndex, iFrameIndex, pTreeItemData, eShapeType, pTreeItemData->GetShape2dModel()->GetData());
				m_pProjItem->GetUndoStack()->push(pCmd);
			}
		}
		else
			HyGuiLog("EntityDraw::OnMouseReleaseEvent - EDITMODE_Transform with unsupported edit item type!", LOGTYPE_Error);
		break;

	default:
		HyGuiLog("EntityDraw::OnMousePressEvent - Unhandled EditModeState reached!", LOGTYPE_Error);
		break;
	}

	// This is to reset the cursor and edit model based on where the mouse is now that the mouse button has been released
	if(pTreeItemData->GetShape2dModel())
	{
		glm::vec2 ptCurMousePos;
		m_pCamera->ProjectToWorld(HyEngine::Input().GetMousePos(), ptCurMousePos);
		ShapeMouseMoveResult eResult = pTreeItemData->GetShape2dModel()->MouseMoveIdle(ptCurMousePos);
		switch(eResult)
		{
		case SHAPEMOUSEMOVE_Creation:				Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->setCursor(Qt::CrossCursor); break;
		case SHAPEMOUSEMOVE_Outside:				Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->setCursor(Qt::ArrowCursor); break;
		case SHAPEMOUSEMOVE_Inside:					Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->setCursor(Qt::SizeAllCursor); break;
		case SHAPEMOUSEMOVE_AppendVertex:			Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->setCursor(Qt::CrossCursor); break;
		case SHAPEMOUSEMOVE_InsertVertex:			Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->setCursor(Qt::CrossCursor); break;
		case SHAPEMOUSEMOVE_HoverVertex:			Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->setCursor(Qt::PointingHandCursor); break;
		case SHAPEMOUSEMOVE_HoverSelectedVertex:	Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->setCursor(Qt::SizeAllCursor); break;
		case SHAPEMOUSEMOVE_HoverCenter:			Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->setCursor(Qt::SizeAllCursor); break;
		default:
			HyGuiLog("EntityDraw::OnMouseMoveEvent - EDITMODE_Idle with unsupported shape mouse move result!", LOGTYPE_Error);
			break;
		}
	}
	else
		HyGuiLog("EntityDraw::OnMouseReleaseEvent - with unsupported edit item type!", LOGTYPE_Error);

	m_eEditModeState = EDITMODE_Idle;
}

bool EntityDraw::SetEditMode(bool bEnable)
{
	if(bEnable == false)
	{
		m_eEditModeState = EDITMODE_Off;
		if(GetCurAction() == HYACTION_EditMode)
		{
			EntityDrawItem *pCurEditItem = GetCurEditItem();
			if(pCurEditItem)
				pCurEditItem->ShowTransformCtrl(true);

			ClearAction();
			MainWindow::ClearStatus();
		}

		m_EditModeWindowOutline.SetVisible(false);
		return true;
	}

	if(SetAction(HYACTION_EditMode) == false)
		return false;

	EntityDrawItem *pCurEditItem = GetCurEditItem();
	if(pCurEditItem == nullptr)
	{
		HyGuiLog("EntityDraw::SetEditMode - No item selected to enter Edit Mode!", LOGTYPE_Error);
		return false;
	}

	pCurEditItem = static_cast<EntityDrawItem *>(m_SelectedItemList[0]);
	pCurEditItem->HideTransformCtrl();
	
	EntityTreeItemData *pTreeItemData = pCurEditItem->GetEntityTreeItemData();

	// Determine which EditModeState to enter based on the shape type of the current shape
	if(pTreeItemData->GetShape2dModel())
	{
		GfxShapeModel *pShapeModel = pTreeItemData->GetShape2dModel();
		MainWindow::SetStatus("Editing " % HyGlobal::ShapeName(pShapeModel->GetType()) % " Shape - " % pTreeItemData->GetCodeName(), 0);
	}
	else
		HyGuiLog("EntityDraw::SetEditMode - Unsupported edit item type!", LOGTYPE_Error);

	m_eEditModeState = EDITMODE_Idle;
	m_EditModeWindowOutline.SetVisible(true);

	DoMouseMove(QApplication::keyboardModifiers().testFlag(Qt::KeyboardModifier::ControlModifier),
				QApplication::keyboardModifiers().testFlag(Qt::KeyboardModifier::ShiftModifier));

	return true;
}

EntityDrawItem *EntityDraw::GetCurEditItem() const
{
	if(GetCurAction() == HYACTION_EditMode && m_SelectedItemList.size() == 1 && static_cast<EntityDrawItem *>(m_SelectedItemList[0])->GetEntityTreeItemData()->IsEditable())
		return static_cast<EntityDrawItem *>(m_SelectedItemList[0]);

	return nullptr;
}

//void EntityDraw::RequestClearShapeEdit()
//{
//	static_cast<EntityModel *>(m_pProjItem->GetModel())->SetShapeEditMode(false);
//}

void EntityDraw::SetExtrapolatedProperties()
{
	if(m_pProjItem->GetWidget() == nullptr)
		HyGuiLog("EntityDraw::SetExtrapolatedProperties - m_pProjItem->GetWidget() is nullptr", LOGTYPE_Error);

	EntityDopeSheetScene &entityDopeSheetSceneRef = static_cast<EntityStateData *>(m_pProjItem->GetModel()->GetStateData(m_pProjItem->GetWidget()->GetCurStateIndex()))->GetDopeSheetScene();

	const float fFRAME_DURATION = 1.0f / static_cast<EntityModel &>(entityDopeSheetSceneRef.GetStateData()->GetModel()).GetFramesPerSecond();
	const int iDESTINATION_FRAME = entityDopeSheetSceneRef.GetCurrentFrame();

	// Set the extrapolated properties for the 'm_RootEntity' item
	EntityTreeItemData *pRootTreeItemData = static_cast<EntityModel *>(m_pProjItem->GetModel())->GetTreeModel().GetRootTreeItemData();

	QMap<EntityTreeItemData *, QJsonObject> &ctorKeyFrameMapRef = static_cast<EntityModel *>(m_pProjItem->GetModel())->GetCtorKeyFramesMap();
	QMap<int, QJsonObject> ctorFrameMap = entityDopeSheetSceneRef.GetKeyFramesMap()[pRootTreeItemData];
	ctorFrameMap[-1] = ctorKeyFrameMapRef[pRootTreeItemData];

	ExtrapolateProperties(m_pProjItem->GetProject(),
							&m_RootEntity,
							nullptr,
							false,
							ITEM_None, // 'ITEM_None' indicates this is the root
							fFRAME_DURATION,
							-1,
							iDESTINATION_FRAME,
							ctorFrameMap,
							pRootTreeItemData->GetPreviewComponent());

	// Set the extrapolated properties for all the children items
	for(IDrawExItem *pDrawItem : m_ItemList)
	{
		EntityDrawItem *pEntDrawItem = static_cast<EntityDrawItem *>(pDrawItem);

		EntityTreeItemData *pEntityTreeItemData = pEntDrawItem->GetEntityTreeItemData();
		ItemType eItemType = pEntityTreeItemData->GetType();
		if(eItemType == ITEM_Prefix) // aka Shapes folder
			continue;

		if(eItemType == ITEM_Entity) // Sub-entity
		{
			QMap<EntityTreeItemData *, QJsonObject> &ctorKeyFrameMapRef = static_cast<EntityModel *>(m_pProjItem->GetModel())->GetCtorKeyFramesMap();
			QMap<int, QJsonObject> ctorFrameMap = entityDopeSheetSceneRef.GetKeyFramesMap()[pEntityTreeItemData];
			ctorFrameMap[-1] = ctorKeyFrameMapRef[pEntityTreeItemData];

			static_cast<SubEntity *>(pEntDrawItem->GetHyNode())->Extrapolate(ctorFrameMap,
																			 pEntityTreeItemData->GetPreviewComponent(),
																			 pEntityTreeItemData->IsSelected(),
																			 fFRAME_DURATION,
																			 iDESTINATION_FRAME);
		}
		else
		{
			QMap<EntityTreeItemData *, QJsonObject> &ctorKeyFrameMapRef = static_cast<EntityModel *>(m_pProjItem->GetModel())->GetCtorKeyFramesMap();
			QMap<int, QJsonObject> ctorFrameMap = entityDopeSheetSceneRef.GetKeyFramesMap()[pEntityTreeItemData];
			ctorFrameMap[-1] = ctorKeyFrameMapRef[pEntityTreeItemData];

			ExtrapolateProperties(m_pProjItem->GetProject(),
									pEntDrawItem->GetHyNode(),
									pEntDrawItem->GetEntityTreeItemData()->GetShape2dModel(),
									pEntityTreeItemData->IsSelected(),
									eItemType,
									fFRAME_DURATION,
									-1,
									iDESTINATION_FRAME,
									ctorFrameMap,
									pEntityTreeItemData->GetPreviewComponent());
		}
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

	QList<IDrawExItem *> staleItemList(m_ItemList);
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
		//bool bIsLocked = descObj["isLocked"].toBool();
		//bool bSelected = descObj["isSelected"].toBool();

		// Try to find the draw item within 'staleItemList'
		EntityDrawItem *pDrawItem = nullptr;
		for(IDrawExItem *pStaleItem : staleItemList)
		{
			EntityTreeItemData *pEntTreeItemData = static_cast<EntityDrawItem *>(pStaleItem)->GetEntityTreeItemData();
			if(pEntTreeItemData->GetType() == eType && pEntTreeItemData->GetThisUuid() == uuid)
			{
				pDrawItem = static_cast<EntityDrawItem *>(pStaleItem);
				break;
			}
		}
		
		if(pDrawItem == nullptr) // Not found within 'staleItemList', allocate new
		{
			EntityTreeItemData *pEntityTreeItemData = static_cast<EntityModel *>(m_pProjItem->GetModel())->GetTreeModel().FindTreeItemData(uuid);
			pDrawItem = new EntityDrawItem(m_pProjItem->GetProject(), pEntityTreeItemData, this, &m_RootEntity);
		}
		else // Found within 'staleItemList'
		{
			staleItemList.removeOne(pDrawItem);

			if(pDrawItem->GetEntityTreeItemData()->IsReallocateDrawItem())
			{
				EntityTreeItemData *pEntityTreeItemData = pDrawItem->GetEntityTreeItemData();
				
				// Allocate first, then delete so Harmony doesn't unload the item data
				EntityDrawItem *pOldDrawItem = pDrawItem;
				pDrawItem = new EntityDrawItem(m_pProjItem->GetProject(), pEntityTreeItemData, this, &m_RootEntity);

				if(pOldDrawItem == m_pCurHoverItem)
					ClearHover();

				delete pOldDrawItem;
				pEntityTreeItemData->SetReallocateDrawItem(false);
			}
		}

		m_ItemList.push_back(pDrawItem); // Repopulate 'm_ItemList' with the valid existing or new draw items
		
		// TODO: START HERE - When SEM is enabled, do proper selection
		m_RootEntity.ChildAppend(*pDrawItem->GetHyNode()); // Reinsert the draw item in the `descObjList` order

		// Repopulate `m_SelectedItemList` if draw item is valid to select
		if(pDrawItem->GetEntityTreeItemData()->IsSelected() && pDrawItem->GetEntityTreeItemData()->IsSelectable())
			m_SelectedItemList.push_back(pDrawItem);
	}

	// Delete all the remaining stale items (no longer used draw items)
	for(auto pStaleItem : staleItemList)
	{
		if(pStaleItem == m_pCurHoverItem)
			ClearHover();

		delete pStaleItem;
	}
	staleItemList.clear();

	// EXTRAPOLATE ALL ITEMS' PROPERTIES TO THE CURRENT FRAME IN DOPESHEET
	SetExtrapolatedProperties();

	// Based on mode and `m_SelectedItemList`, show/hide transform and shape edit ctrls
	for(IDrawExItem *pItem : m_ItemList)
		static_cast<EntityDrawItem *>(pItem)->HideTransformCtrl();

	for(IDrawExItem *pSelectedItemDraw : m_SelectedItemList)
		pSelectedItemDraw->ShowTransformCtrl(m_SelectedItemList.size() == 1);

	RefreshTransforms();
}

/*virtual*/ void EntityDraw::OnResizeRenderer() /*override*/
{
	glm::vec2 vWindowSize = HyEngine::Window().GetWindowSize();
	m_EditModeWindowOutline.SetAsBox(HyRect(vWindowSize.x, vWindowSize.y));
}

/*virtual*/ void EntityDraw::OnRequestSelection(QList<IDrawExItem *> selectionList) /*override*/
{
	selectionList = QSet<IDrawExItem *>(selectionList.begin(), selectionList.end()).values();

	bool bValidRequest = m_SelectedItemList.size() != selectionList.size(); // Ensure this request isn't redundant
	QList<QUuid> uuidList;
	for(IDrawExItem *pDrawItem : selectionList)
	{
		if(m_SelectedItemList.contains(pDrawItem) == false)
			bValidRequest = true;

		uuidList.push_back(static_cast<EntityDrawItem *>(pDrawItem)->GetEntityTreeItemData()->GetThisUuid());
	}

	if(bValidRequest == false)
		return;

	EntityWidget *pWidget = static_cast<EntityWidget *>(m_pProjItem->GetWidget());
	pWidget->RequestSelectedItems(uuidList);
}

/*virtual*/ void EntityDraw::OnPerformTransform() /*override*/
{
	QList<EntityTreeItemData *> treeItemDataList;
	QList<glm::mat4> newTransformList;
	for(IDrawExItem *pDrawItem : m_SelectedItemList)
	{
		EntityDrawItem *pEntDrawItem = static_cast<EntityDrawItem *>(pDrawItem);

		newTransformList.push_back(pEntDrawItem->GetHyNode()->GetSceneTransform(0.0f));

		EntityTreeItemData *pTreeItemData = static_cast<EntityModel *>(m_pProjItem->GetModel())->GetTreeModel().FindTreeItemData(static_cast<EntityDrawItem *>(pDrawItem)->GetEntityTreeItemData()->GetThisUuid());
		treeItemDataList.push_back(pTreeItemData);
	}

	int iStateIndex = 0;
	int iFrameIndex = 0;
	if(m_pProjItem->GetWidget() == nullptr)
		HyGuiLog("EntityDraw::DoMouseRelease_Transform - m_pProjItem->GetWidget() is nullptr", LOGTYPE_Error);
	else
	{
		iStateIndex = m_pProjItem->GetWidget()->GetCurStateIndex();

		EntityDopeSheetScene &dopeSheetSceneRef = static_cast<EntityStateData *>(m_pProjItem->GetModel()->GetStateData(iStateIndex))->GetDopeSheetScene();
		iFrameIndex = dopeSheetSceneRef.GetCurrentFrame();
	}

	// Transferring the children in 'm_ActiveTransform' back into 'm_RootEntity' will be done automatically in OnApplyJsonMeta()
	QUndoCommand *pCmd = new EntityUndoCmd_Transform(*m_pProjItem, iStateIndex, iFrameIndex, treeItemDataList, newTransformList, m_PrevTransformList);
	m_pProjItem->GetUndoStack()->push(pCmd);
}
