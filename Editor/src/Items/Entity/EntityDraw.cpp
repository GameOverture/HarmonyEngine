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
	m_bIsShapeAddPrimitive(false),
	m_ShapeEditModeWindowOutline(this)
{
	m_ShapeEditModeWindowOutline.UseWindowCoordinates();
	m_ShapeEditModeWindowOutline.SetTint(HyGlobal::GetEditorColor(EDITORCOLOR_ShapeGrabPointSelectedFill));
	m_ShapeEditModeWindowOutline.SetWireframe(true);
	m_ShapeEditModeWindowOutline.SetLineThickness(5.0f);
	m_ShapeEditModeWindowOutline.SetVisible(false);
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
		EntityDrawItem *pCurVertexEditItem = GetCurShapeEditItem();
		if(IsActionSemIdle() && pCurVertexEditItem)
		{
			if(pCurVertexEditItem->GetShapeCtrl().RemoveSelectedVerts() == false)
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

			QUuid thisUuid = pCurVertexEditItem->GetEntityTreeItemData()->GetThisUuid();
			EntityTreeItemData *pTreeItemData = static_cast<EntityModel *>(m_pProjItem->GetModel())->GetTreeModel().FindTreeItemData(thisUuid);

			QUndoCommand *pCmd = new EntityUndoCmd_ShapeData("Removed vertices from " % pTreeItemData->GetCodeName(), *m_pProjItem, iStateIndex, iFrameIndex, pTreeItemData, pCurVertexEditItem->GetShapeCtrl().SerializeVemVerts(m_pCamera));
			m_pProjItem->GetUndoStack()->push(pCmd);
		}
	}
}

/*virtual*/ void EntityDraw::OnKeyReleaseEvent(QKeyEvent *pEvent) /*override*/
{
	IDrawEx::OnKeyReleaseEvent(pEvent);
}

/*virtual*/ void EntityDraw::OnMouseMoveEvent(QMouseEvent *pEvent) /*override*/
{
	IDrawEx::OnMouseMoveEvent(pEvent);

	glm::vec2 ptCurMousePos;
	m_pCamera->ProjectToWorld(HyEngine::Input().GetMousePos(), ptCurMousePos);

	EntityDrawItem *pCurVertexEditItem = GetCurShapeEditItem();

	if(GetCurAction() == HYACTION_EntityAddShapePending)
	{
		if(glm::distance(m_ptDragStart, ptCurMousePos) >= 2.0f)
			SetAction(HYACTION_EntityAddShapeDrag);
	}
	else if(GetCurAction() == HYACTION_EntityAddShapeDrag)
	{
		m_DragShape.SetAsDrag(pEvent->modifiers().testFlag(Qt::KeyboardModifier::ShiftModifier), m_ptDragStart, ptCurMousePos, m_pCamera);
	}
	else if(GetCurAction() == HYACTION_EntitySemMarquee)
	{
		glm::vec2 ptWorldMousePos;
		m_pCamera->ProjectToWorld(HyEngine::Input().GetMousePos(), ptWorldMousePos);

		m_DragShape.Setup(SHAPE_Box, HyGlobal::GetEditorColor(EDITORCOLOR_Marquee), 0.25f, 1.0f);
		m_DragShape.SetAsDrag(/*bShiftMod*/false, m_ptDragStart, ptWorldMousePos, m_pCamera); // Don't do centering when holding shift and marquee selecting
	}
	else if((IsActionSemIdle() || GetCurAction() == HYACTION_EntitySemInvalid) && pCurVertexEditItem)
	{
		m_pCurHoverItem = nullptr;
		for(int32 i = m_ItemList.size() - 1; i >= 0; --i) // iterate backwards to prioritize selecting items with higher display order
		{
			if(m_ItemList[i]->IsMouseInBounds() && m_ItemList[i]->IsSelectable())
			{
				m_pCurHoverItem = m_ItemList[i];

				if(m_SelectedItemList.contains(m_pCurHoverItem) == false)
					SetAction(HYACTION_EntitySemHoverItem);

				break;
			}
		}
		if(GetCurAction() == HYACTION_EntitySemHoverItem && m_pCurHoverItem == nullptr)
			ClearAction();


		DrawAction eTrySemHoverAction = pCurVertexEditItem->GetShapeCtrl().GetMouseSemHoverAction(pEvent->modifiers().testFlag(Qt::KeyboardModifier::ControlModifier), pEvent->modifiers().testFlag(Qt::KeyboardModifier::ShiftModifier), false);
		if(eTrySemHoverAction == HYACTION_EntitySem)
			ClearAction();

		SetAction(eTrySemHoverAction);
	}
	else if(IsActionSemTransforming() && pCurVertexEditItem)
	{
		pCurVertexEditItem->GetShapeCtrl().TransformSemVerts(GetCurAction(), m_ptDragStart, ptCurMousePos, m_pCamera);
	}
}

/*virtual*/ void EntityDraw::OnMousePressEvent(QMouseEvent *pEvent) /*override*/
{
	IDrawEx::OnMousePressEvent(pEvent);

	if(pEvent->button() == Qt::LeftButton)
	{
		EntityDrawItem *pCurVertexEditItem = GetCurShapeEditItem();

		if(GetCurAction() == HYACTION_EntityAddShape)
		{
			//m_PressTimer.InitStart(0.5f); // No timer, must drag a minimal amount
			SetAction(HYACTION_EntityAddShapePending);
		}
		else if(IsActionSemIdle() && pCurVertexEditItem)
		{
			DrawAction eSemAction = pCurVertexEditItem->GetShapeCtrl().GetMouseSemHoverAction(pEvent->modifiers().testFlag(Qt::KeyboardModifier::ControlModifier), pEvent->modifiers().testFlag(Qt::KeyboardModifier::ShiftModifier), true);
			if(eSemAction == HYACTION_EntitySem)
			{
				pCurVertexEditItem->GetShapeCtrl().UnselectAllVemVerts();
				SetAction(HYACTION_EntitySemMarquee);
			}
			else
			{
				// Convert the hover action to the transform action
				switch(eSemAction)
				{
				case HYACTION_EntitySemHoverTranslate:			eSemAction = HYACTION_EntitySemTranslating; break;
				case HYACTION_EntitySemHoverAddVertex:			eSemAction = HYACTION_EntitySemAddingVertex; break;
				case HYACTION_EntitySemHoverGrabVertex:			eSemAction = HYACTION_EntitySemTranslateVertex; break;
				case HYACTION_EntitySemHoverRadiusHorizontal:	eSemAction = HYACTION_EntitySemRadiusHorizontal; break;
				case HYACTION_EntitySemHoverRadiusVertical:		eSemAction = HYACTION_EntitySemRadiusVertical; break;
				
				case HYACTION_EntitySemInvalid:
					break;

				default:
					HyGuiLog("EntityDraw::OnMousePressEvent - Invalid eSemHoverAction", LOGTYPE_Error);
				}

				if(SetAction(eSemAction))
					pCurVertexEditItem->GetShapeCtrl().TransformSemVerts(GetCurAction(), m_ptDragStart, m_ptDragStart, m_pCamera);

				//m_PressTimer.InitStart(0.5f);
				//m_eDragState = DRAGSTATE_Pending;
			}
		}
	}
	else if(pEvent->button() == Qt::RightButton)
	{
		if(IsActionSemIdle() || IsActionSemTransforming())
			RequestClearShapeEdit();
	}
}

/*virtual*/ void EntityDraw::OnMouseReleaseEvent(QMouseEvent *pEvent) /*override*/
{
	IDrawEx::OnMouseReleaseEvent(pEvent);

	EntityDrawItem *pCurVertexEditItem = GetCurShapeEditItem();

	if(GetCurAction() == HYACTION_EntityAddShapePending || GetCurAction() == HYACTION_EntityAddShapeDrag)
	{
		if(GetCurAction() == HYACTION_EntityAddShapeDrag)
		{
			QUndoCommand *pCmd = new EntityUndoCmd_AddNewShape(*m_pProjItem,
																m_DragShape.GetShapeType(),
																m_DragShape.Serialize(),
																m_bIsShapeAddPrimitive,
																-1);
			m_pProjItem->GetUndoStack()->push(pCmd);

			if(m_pProjItem->GetWidget())
				static_cast<EntityWidget *>(m_pProjItem->GetWidget())->UncheckAll();
				
			ClearAction();
		}
	}
	else if(GetCurAction() == HYACTION_EntitySemMarquee)
	{
		b2AABB marqueeAabb;
		HyShape2d tmpShape;
		m_DragShape.GetPrimitive().CalcLocalBoundingShape(tmpShape);
		tmpShape.ComputeAABB(marqueeAabb, glm::mat4(1.0f));

		if(pCurVertexEditItem)
		{
			if(pEvent->modifiers().testFlag(Qt::KeyboardModifier::ShiftModifier) == false)
				pCurVertexEditItem->GetShapeCtrl().UnselectAllVemVerts();
			pCurVertexEditItem->GetShapeCtrl().SelectVemVerts(marqueeAabb, m_pCamera);
		}
		else
		{
			// TODO: Select all shapes within the marquee
		}

		m_DragShape.Setup(SHAPE_None, HyColor::White, 1.0f, 1.0f);
		ClearAction();
	}
	else if(IsActionSemTransforming() && pCurVertexEditItem)
	{
		EntityTreeItemData *pTreeItemData = static_cast<EntityModel *>(m_pProjItem->GetModel())->GetTreeModel().FindTreeItemData(pCurVertexEditItem->GetEntityTreeItemData()->GetThisUuid());

		int iStateIndex = 0;
		int iFrameIndex = 0;
		if(m_pProjItem->GetWidget() == nullptr)
			HyGuiLog("EntityDraw::OnMouseReleaseEvent - m_pProjItem->GetWidget() is nullptr", LOGTYPE_Error);
		else
		{
			iStateIndex = m_pProjItem->GetWidget()->GetCurStateIndex();
			iFrameIndex = static_cast<EntityStateData *>(m_pProjItem->GetModel()->GetStateData(iStateIndex))->GetDopeSheetScene().GetCurrentFrame();
		}

		QString sUndoText;
		switch(GetCurAction())
		{
		case HYACTION_EntitySemTranslating:
		case HYACTION_EntitySemTranslateVertex:
			sUndoText = "Translate vert(s) on " % pTreeItemData->GetCodeName();
			break;
		
		case HYACTION_EntitySemRadiusHorizontal:
		case HYACTION_EntitySemRadiusVertical:
			sUndoText = "Adjust circle radius on " % pTreeItemData->GetCodeName();
			break;

		case HYACTION_EntitySemAddingVertex:
			sUndoText = "Add vertex to " % pTreeItemData->GetCodeName();
			break;

		default:
			HyGuiLog("EntityDraw::OnMouseReleaseEvent - Invalid GetCurAction()", LOGTYPE_Error);
			break;
		}

		QUndoCommand *pCmd = new EntityUndoCmd_ShapeData(sUndoText, *m_pProjItem, iStateIndex, iFrameIndex, pTreeItemData, pCurVertexEditItem->GetShapeCtrl().SerializeVemVerts(m_pCamera));
		m_pProjItem->GetUndoStack()->push(pCmd);

		ClearAction();
	}
}

bool EntityDraw::IsSemEnabled() const
{
	return GetBackgroundAction() == HYACTION_EntitySem;
}

bool EntityDraw::IsActionSemIdle() const
{
	return m_eDrawAction == HYACTION_EntitySem ||
		   m_eDrawAction == HYACTION_EntitySemHoverTranslate ||
		   m_eDrawAction == HYACTION_EntitySemHoverAddVertex ||
		   m_eDrawAction == HYACTION_EntitySemHoverGrabVertex ||
		   m_eDrawAction == HYACTION_EntitySemHoverRadiusHorizontal ||
		   m_eDrawAction == HYACTION_EntitySemHoverRadiusVertical;
}

bool EntityDraw::IsActionSemTransforming() const
{
	return m_eDrawAction == HYACTION_EntitySemTranslating ||
		   m_eDrawAction == HYACTION_EntitySemAddingVertex ||
		   m_eDrawAction == HYACTION_EntitySemTranslateVertex ||
		   m_eDrawAction == HYACTION_EntitySemRadiusHorizontal ||
		   m_eDrawAction == HYACTION_EntitySemRadiusVertical;
}

EditorShape EntityDraw::GetShapeAddType() const
{
	return m_DragShape.GetShapeType();
}

bool EntityDraw::SetAsShapeAdd(EditorShape eShape, bool bAsPrimitive)
{
	if(SetAction(HYACTION_EntityAddShape))
	{
		m_bIsShapeAddPrimitive = bAsPrimitive;
		OnRequestSelection(QList<IDrawExItem *>()); // Clear any selected item

		m_DragShape.Setup(eShape, bAsPrimitive ? HyColor::White : HyGlobal::GetEditorColor(EDITORCOLOR_Shape), 1.0f, 1.0f);

		return true;
	}

	return false;
}

void EntityDraw::SetAsShapeEditMode(bool bEnable)
{
	if(bEnable)
	{
		if(SetBackgroundAction(HYACTION_EntitySem))
		{
			EntityDrawItem *pCurVertexEditItem = GetCurShapeEditItem();

			if(pCurVertexEditItem)
			{
				pCurVertexEditItem = static_cast<EntityDrawItem *>(m_SelectedItemList[0]);
				pCurVertexEditItem->HideTransformCtrl();
				pCurVertexEditItem->GetShapeCtrl().EnableVertexEditMode();
			}

			m_ShapeEditModeWindowOutline.SetVisible(true);
		}
	}
	else
	{
		m_DragShape.Setup(SHAPE_None, HyColor::White, 1.0f, 1.0f);

		EntityDrawItem *pCurVertexEditItem = GetCurShapeEditItem();
		if(pCurVertexEditItem)
		{
			pCurVertexEditItem->ShowTransformCtrl(true);
			pCurVertexEditItem->GetShapeCtrl().ClearVertexEditMode();
		}

		ClearAction();
		if(m_eBackgroundDrawAction == HYACTION_EntitySem)
			ClearBackgroundAction();

		m_ShapeEditModeWindowOutline.SetVisible(false);
	}
}

void EntityDraw::ActivateVemOnNextJsonMeta()
{
	m_bActivateVemOnNextJsonMeta = true;
}

EntityDrawItem *EntityDraw::GetCurShapeEditItem() const
{
	if(IsSemEnabled() && m_SelectedItemList.size() == 1)
		return static_cast<EntityDrawItem *>(m_SelectedItemList[0]);

	return nullptr;
}

void EntityDraw::RequestClearShapeEdit()
{
	static_cast<EntityModel *>(m_pProjItem->GetModel())->SetShapeEditMode(false);
}

void EntityDraw::SetExtrapolatedProperties()
{
	if(m_pProjItem->GetWidget() == nullptr)
		HyGuiLog("EntityDraw::SetExtrapolatedProperties - m_pProjItem->GetWidget() is nullptr", LOGTYPE_Error);

	EntityDopeSheetScene &entityDopeSheetSceneRef = static_cast<EntityStateData *>(m_pProjItem->GetModel()->GetStateData(m_pProjItem->GetWidget()->GetCurStateIndex()))->GetDopeSheetScene();

	const float fFRAME_DURATION = 1.0f / static_cast<EntityModel &>(entityDopeSheetSceneRef.GetStateData()->GetModel()).GetFramesPerSecond();
	const int iDESTINATION_FRAME = entityDopeSheetSceneRef.GetCurrentFrame();

	// Set the extrapolated properties for the 'm_RootEntity' item
	EntityTreeItemData *pRootTreeItemData = static_cast<EntityModel *>(m_pProjItem->GetModel())->GetTreeModel().GetRootTreeItemData();

	ExtrapolateProperties(&m_RootEntity,
						  nullptr,
						  false,
						  ITEM_Unknown, // 'ITEM_Unknown' indicates this is the root
						  fFRAME_DURATION,
						  0,
						  iDESTINATION_FRAME,
						  entityDopeSheetSceneRef.GetKeyFramesMap()[pRootTreeItemData],
						  m_pCamera);

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
			const QMap<int, QJsonObject> &mergedMapRef = entityDopeSheetSceneRef.GetKeyFramesMap()[pEntityTreeItemData];
			static_cast<SubEntity *>(pEntDrawItem->GetHyNode())->Extrapolate(mergedMapRef,
																			pEntityTreeItemData->IsSelected(),
																			fFRAME_DURATION,
																			iDESTINATION_FRAME,
																			m_pCamera);
		}
		else
		{
			ExtrapolateProperties(pEntDrawItem->GetHyNode(),
								  &pEntDrawItem->GetShapeCtrl(),
								  pEntityTreeItemData->IsSelected(),
								  eItemType,
								  fFRAME_DURATION,
								  0,
								  iDESTINATION_FRAME,
								  entityDopeSheetSceneRef.GetKeyFramesMap()[pEntityTreeItemData],
								  m_pCamera);
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

		if(pDrawItem->GetEntityTreeItemData()->IsSelected() && pDrawItem->GetEntityTreeItemData()->IsSelectable())
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
		for(IDrawExItem *pSelectedItemDraw : m_SelectedItemList)
			pSelectedItemDraw->HideTransformCtrl();

		static_cast<EntityModel *>(m_pProjItem->GetModel())->SetShapeEditMode(true);
		m_bActivateVemOnNextJsonMeta = false;
	}
	else
	{
		bool bShowGrabPoints = m_SelectedItemList.size() == 1;
		for(IDrawExItem *pSelectedItemDraw : m_SelectedItemList)
			pSelectedItemDraw->ShowTransformCtrl(bShowGrabPoints);
	}

	RefreshTransforms();
}

/*virtual*/ void EntityDraw::OnResizeRenderer() /*override*/
{
	glm::vec2 vWindowSize = HyEngine::Window().GetWindowSize();
	m_ShapeEditModeWindowOutline.SetAsBox(HyRect(vWindowSize.x, vWindowSize.y));
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

		// This updates the preview of a shape (its 'outline') when being transformed
		if(pEntDrawItem->GetEntityTreeItemData()->GetType() == ITEM_BoundingVolume)
			pEntDrawItem->GetShapeCtrl().Setup(pEntDrawItem->GetShapeCtrl().GetShapeType(), HyGlobal::GetEditorColor(EDITORCOLOR_Shape), 0.7f, 0.0f);

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
		iFrameIndex = static_cast<EntityStateData *>(m_pProjItem->GetModel()->GetStateData(iStateIndex))->GetDopeSheetScene().GetCurrentFrame();
	}

	// Transferring the children in 'm_ActiveTransform' back into 'm_RootEntity' will be done automatically in OnApplyJsonMeta()
	QUndoCommand *pCmd = new EntityUndoCmd_Transform(*m_pProjItem, iStateIndex, iFrameIndex, treeItemDataList, newTransformList, m_PrevTransformList);
	m_pProjItem->GetUndoStack()->push(pCmd);
}
