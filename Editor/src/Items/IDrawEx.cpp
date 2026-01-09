/**************************************************************************
 *	IDrawEx.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2024 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "IDrawEx.h"
#include "ProjectItemData.h"
#include "Harmony.h"

#include <QKeyEvent>
#include <QApplication>

IDrawEx::IDrawEx(ProjectItemData *pProjItem, const FileDataPair &initFileDataRef) :
	IDraw(pProjItem, initFileDataRef),
	m_DragShape(this),
	m_MultiTransform(this),
	m_fMultiTransformStartRot(0.0f),
	m_pCurHoverItem(nullptr),
	m_eCurHoverGrabPoint(TransformCtrl::GRAB_None),
	m_bSelectionHandled(false),
	m_SnapGuideHorz(this),
	m_SnapGuideVert(this)
{
	m_MultiTransform.Hide();
	m_PressTimer.SetExpiredCallback(
		[this]()
		{
			if(GetCurAction() == HYACTION_Pending)
				BeginTransform();
			//else if(GetCurAction() == HYACTION_EntityAddShapePending)
			//	SetAction(HYACTION_EntityAddShapeDrag);
		});

	m_SnapGuideHorz.SetVisible(false);
	m_SnapGuideHorz.SetTint(HyColor::Red);
	m_SnapGuideHorz.UseWindowCoordinates();
	m_SnapGuideHorz.SetDisplayOrder(DISPLAYORDER_SnapGuide);

	m_SnapGuideVert.SetVisible(false);
	m_SnapGuideVert.SetTint(HyColor::Red);
	m_SnapGuideVert.UseWindowCoordinates();
	m_SnapGuideVert.SetDisplayOrder(DISPLAYORDER_SnapGuide);
}

/*virtual*/ IDrawEx::~IDrawEx()
{
	for(auto pItem : m_ItemList)
		delete pItem;
}

QList<IDrawExItem *> IDrawEx::GetDrawItemList()
{
	return m_ItemList;
}

/*virtual*/ void IDrawEx::ResizeRenderer() /*override*/
{
	IDraw::ResizeRenderer();
	
	RefreshTransforms();
	
	glm::vec2 vWindowSize = HyEngine::Window().GetWindowSize();
	m_SnapGuideHorz.SetAsLineSegment(glm::vec2(0.0f, 0.0f), glm::vec2(vWindowSize.x, 0.0f));
	m_SnapGuideVert.SetAsLineSegment(glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, vWindowSize.y));
}

/*virtual*/ void IDrawEx::OnUndoStackIndexChanged(int iIndex) /*override*/
{
	RefreshTransforms();
}

/*virtual*/ void IDrawEx::OnKeyPressEvent(QKeyEvent *pEvent) /*override*/
{
	IDraw::OnKeyPressEvent(pEvent);

	if(IsCameraPanning())
		RefreshTransforms();

	if(pEvent->key() == Qt::Key_Left ||
		pEvent->key() == Qt::Key_Right ||
		pEvent->key() == Qt::Key_Up ||
		pEvent->key() == Qt::Key_Down)
	{
		BeginTransform();
		SetAction(HYACTION_TransformingNudging);

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

	DoMouseMove(pEvent->modifiers().testFlag(Qt::KeyboardModifier::ControlModifier), pEvent->modifiers().testFlag(Qt::KeyboardModifier::ShiftModifier));
}

/*virtual*/ void IDrawEx::OnKeyReleaseEvent(QKeyEvent *pEvent) /*override*/
{
	IDraw::OnKeyReleaseEvent(pEvent);

	if(IsCameraPanning())
		RefreshTransforms();

	if(pEvent->key() == Qt::Key_Control || pEvent->key() == Qt::Key_Shift)
		DoMouseMove(pEvent->modifiers().testFlag(Qt::KeyboardModifier::ControlModifier), pEvent->modifiers().testFlag(Qt::KeyboardModifier::ShiftModifier));

	if(pEvent->isAutoRepeat() == false &&
		(pEvent->key() == Qt::Key_Left ||
		 pEvent->key() == Qt::Key_Right ||
		 pEvent->key() == Qt::Key_Up ||
		 pEvent->key() == Qt::Key_Down))
	{
		// We can reuse 'DoMouseRelease_Transform' to submit the nudging transform (via undo/redo cmd), while also resetting/cleaning up the state
		DoMouseRelease_Transform();
	}
}

/*virtual*/ void IDrawEx::OnMouseWheelEvent(QWheelEvent *pEvent) /*override*/
{
	IDraw::OnMouseWheelEvent(pEvent);
}

/*virtual*/ void IDrawEx::OnMouseMoveEvent(QMouseEvent *pEvent) /*override*/
{
	IDraw::OnMouseMoveEvent(pEvent);

	if(IsCameraPanning())
		RefreshTransforms();

	DoMouseMove(pEvent->modifiers().testFlag(Qt::KeyboardModifier::ControlModifier), pEvent->modifiers().testFlag(Qt::KeyboardModifier::ShiftModifier));
}

/*virtual*/ void IDrawEx::OnMousePressEvent(QMouseEvent *pEvent) /*override*/
{
	IDraw::OnMousePressEvent(pEvent);

	if(IsCameraPanning())
		RefreshTransforms();

	if(pEvent->button() == Qt::LeftButton)
	{
		m_pCamera->ProjectToWorld(HyEngine::Input().GetMousePos(), m_ptDragStart);

		if(m_eCurHoverGrabPoint != TransformCtrl::GRAB_None)
			BeginTransform();
		else
		{
			// Check if the click position (m_ptDragStart) is over an item
			if((m_MultiTransform.IsShown() && m_MultiTransform.IsMouseOverBoundingVolume()) == false &&
				m_pCurHoverItem == nullptr)
			{
				SetAction(HYACTION_MarqueeStart);
			}
			else if(SetAction(HYACTION_Pending))
			{
				m_PressTimer.InitStart(0.5f);

				// Select the hover item if it's not apart of the selection
				if(m_SelectedItemList.contains(m_pCurHoverItem) == false)
				{
					// Special Case: Allow user to optionally translate selected item(s) if possible - meaning the mouse is ALSO hovering the selected item(s)
					//               Therefore, don't decide to make selection until the is mouse button release
					bool bSpecialCase = m_MultiTransform.IsShown() && m_MultiTransform.IsMouseOverBoundingVolume();
					for(IDrawExItem *pSelectedItem : m_SelectedItemList)
					{
						if(pSelectedItem->IsMouseInBounds())
						{
							bSpecialCase = true;
							break;
						}
					}

					if(bSpecialCase == false)
					{
						QList<IDrawExItem *> selectList;
						selectList << m_pCurHoverItem;

						if(pEvent->modifiers().testFlag(Qt::KeyboardModifier::ShiftModifier))
							selectList += m_SelectedItemList;

						OnRequestSelection(selectList);
						m_bSelectionHandled = true;
					}
				}
			}
		}
	}
}

/*virtual*/ void IDrawEx::OnMouseReleaseEvent(QMouseEvent *pEvent) /*override*/
{
	IDraw::OnMouseReleaseEvent(pEvent);

	if(pEvent->button() == Qt::LeftButton)
	{
		// Wrap up selection actions
		if(GetCurAction() == HYACTION_MarqueeStart || GetCurAction() == HYACTION_MarqueeDrag || GetCurAction() == HYACTION_Pending)
		{
			m_PressTimer.Reset();

			QList<IDrawExItem *> affectedItemList;	// Items that are getting selected or deselected
			if(GetCurAction() == HYACTION_MarqueeStart || GetCurAction() == HYACTION_MarqueeDrag)
			{
				b2AABB marqueeAabb;
				HyShape2d tmpShape;
				m_DragShape.GetPrimitive(true).CalcLocalBoundingShape(tmpShape);
				tmpShape.ComputeAABB(marqueeAabb, glm::mat4(1.0f));

				for(IDrawExItem *pItem : m_ItemList)
				{
					if(pItem->GetTransformCtrl().IsContained(marqueeAabb, m_pCamera) && pItem->IsSelectable())
						affectedItemList << pItem;
				}

				m_DragShape.Setup(SHAPE_None, HyColor::White, 1.0f, 1.0f);
			}
			else if(m_pCurHoverItem && m_pCurHoverItem->IsSelectable()) // This covers the resolution of "Special Case" in DoMousePress
				affectedItemList << m_pCurHoverItem;

			if(m_bSelectionHandled == false)
			{
				if(pEvent->modifiers().testFlag(Qt::KeyboardModifier::ShiftModifier) == false)
					OnRequestSelection(affectedItemList);
				else
				{
					QList<IDrawExItem *> selectList = m_SelectedItemList;

					for(IDrawExItem *pAffectedItem : affectedItemList)
					{
						if(m_SelectedItemList.contains(pAffectedItem))
							selectList.removeOne(pAffectedItem);
						else
							selectList.append(pAffectedItem);
					}

					OnRequestSelection(selectList);
				}
			}

			// Reset
			m_bSelectionHandled = false;
			ClearAction();
		}
		else if(IsActionTransforming())
			DoMouseRelease_Transform();
	}
	
	RefreshTransforms();
}

void IDrawEx::RefreshTransforms()
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
		else if(IsActionTransforming())
		{
			glm::vec2 ptCenterPivot;
			m_MultiTransform.GetCentroid(ptCenterPivot);
			m_MultiTransform.rot_pivot.Set(ptCenterPivot);
			m_MultiTransform.rot.Set(m_fMultiTransformStartRot + m_ActiveTransform.rot.Get());
		}
	}
	else
		m_MultiTransform.Hide();

	for(IDrawExItem *pItemDraw : m_ItemList /*m_SelectedItemList*/)
		pItemDraw->RefreshTransform(m_pCamera);
}

void IDrawEx::ClearHover()
{
	m_pCurHoverItem = nullptr;
	m_eCurHoverGrabPoint = TransformCtrl::GRAB_None;
}

/*virtual*/ void IDrawEx::OnCameraUpdated() /*override*/
{
	RefreshTransforms();
	DoMouseMove(QApplication::keyboardModifiers().testFlag(Qt::KeyboardModifier::ControlModifier), QApplication::keyboardModifiers().testFlag(Qt::KeyboardModifier::ShiftModifier));
}

void IDrawEx::DoMouseMove(bool bCtrlMod, bool bShiftMod)
{
	glm::vec2 ptWorldMousePos;
	m_pCamera->ProjectToWorld(HyEngine::Input().GetMousePos(), ptWorldMousePos);

	if(GetCurAction() == HYACTION_MarqueeStart || GetCurAction() == HYACTION_MarqueeDrag)
	{
		SetAction(HYACTION_MarqueeDrag);

		m_DragShape.Setup(SHAPE_Box, HyGlobal::GetEditorColor(EDITORCOLOR_Marquee), 0.25f, 1.0f);
		m_DragShape.SetAsDrag(/*bShiftMod*/false, m_ptDragStart, ptWorldMousePos, m_pCamera); // Don't do centering when holding shift and marquee selecting
	}
	else if(GetCurAction() == HYACTION_Pending)
	{
		if(glm::distance(m_ptDragStart, ptWorldMousePos) >= 2.0f)
			BeginTransform();
	}
	else if(IsActionTransforming())
	{
		if(GetCurAction() != HYACTION_TransformingNudging)
			DoMouseMove_Transform(bCtrlMod, bShiftMod);
	}
	else
	{
		m_pCurHoverItem = nullptr;
		for(int32 i = m_ItemList.size() - 1; i >= 0; --i) // iterate backwards to prioritize selecting items with higher display order
		{
			if(m_ItemList[i]->IsMouseInBounds() && m_ItemList[i]->IsSelectable())
			{
				m_pCurHoverItem = m_ItemList[i];

				if(m_SelectedItemList.contains(m_pCurHoverItem) == false)
					SetAction(HYACTION_HoverItem);

				break;
			}
		}
		if(GetCurAction() == HYACTION_HoverItem && m_pCurHoverItem == nullptr)
			ClearAction();

		m_eCurHoverGrabPoint = TransformCtrl::GRAB_None;

		if(m_MultiTransform.IsShown())
		{
			m_eCurHoverGrabPoint = m_MultiTransform.IsMouseOverGrabPoint();
			SetTransformHoverActionViaGrabPoint(m_eCurHoverGrabPoint, m_MultiTransform.GetCachedRotation());
		}

		if(m_SelectedItemList.size() == 1)
		{
			TransformCtrl &transformCtrlRef = m_SelectedItemList[0]->GetTransformCtrl();
			m_eCurHoverGrabPoint = transformCtrlRef.IsMouseOverGrabPoint();
			if(SetTransformHoverActionViaGrabPoint(m_eCurHoverGrabPoint, transformCtrlRef.GetCachedRotation()))
				m_pCurHoverItem = m_SelectedItemList[0]; // Override whatever might be above this item, because we're hovering over a grab point
			else if(m_eDrawAction == HYACTION_HoverScale || m_eDrawAction == HYACTION_HoverRotate)
				ClearAction();
		}
	}
}

bool IDrawEx::SetTransformHoverActionViaGrabPoint(TransformCtrl::GrabPointType eGrabPoint, float fRotation)
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
		return false;

	case TransformCtrl::GRAB_BotLeft:
		if(m_eDrawAction <= HYACTION_HoverScale)
		{
			m_eDrawAction = HYACTION_HoverScale;
			Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->setCursor(fpRotateCursor(Qt::SizeBDiagCursor, iThresholds));
			return true;
		}
		break;
	case TransformCtrl::GRAB_BotRight:
		if(m_eDrawAction <= HYACTION_HoverScale)
		{
			m_eDrawAction = HYACTION_HoverScale;
			Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->setCursor(fpRotateCursor(Qt::SizeFDiagCursor, iThresholds));
			return true;
		}
		break;
	case TransformCtrl::GRAB_TopRight:
		if(m_eDrawAction <= HYACTION_HoverScale)
		{
			m_eDrawAction = HYACTION_HoverScale;
			Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->setCursor(fpRotateCursor(Qt::SizeBDiagCursor, iThresholds));
			return true;
		}
		break;
	case TransformCtrl::GRAB_TopLeft:
		if(m_eDrawAction <= HYACTION_HoverScale)
		{
			m_eDrawAction = HYACTION_HoverScale;
			Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->setCursor(fpRotateCursor(Qt::SizeFDiagCursor, iThresholds));
			return true;
		}
		break;
	case TransformCtrl::GRAB_BotMid:
		if(m_eDrawAction <= HYACTION_HoverScale)
		{
			m_eDrawAction = HYACTION_HoverScale;
			Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->setCursor(fpRotateCursor(Qt::SizeVerCursor, iThresholds));
			return true;
		}
		break;
	case TransformCtrl::GRAB_MidRight:
		if(m_eDrawAction <= HYACTION_HoverScale)
		{
			m_eDrawAction = HYACTION_HoverScale;
			Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->setCursor(fpRotateCursor(Qt::SizeHorCursor, iThresholds));
			return true;
		}
		break;
	case TransformCtrl::GRAB_TopMid:
		if(m_eDrawAction <= HYACTION_HoverScale)
		{
			m_eDrawAction = HYACTION_HoverScale;
			Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->setCursor(fpRotateCursor(Qt::SizeVerCursor, iThresholds));
			return true;
		}
		break;
	case TransformCtrl::GRAB_MidLeft:
		if(m_eDrawAction <= HYACTION_HoverScale)
		{
			m_eDrawAction = HYACTION_HoverScale;
			Harmony::GetHarmonyWidget(&m_pProjItem->GetProject())->setCursor(fpRotateCursor(Qt::SizeHorCursor, iThresholds));
			return true;
		}
		break;
	case TransformCtrl::GRAB_Rotate:
		return SetAction(HYACTION_HoverRotate);
	}

	return false;
}

void IDrawEx::BeginTransform()
{
	if(IsActionTransforming() || m_SelectedItemList.isEmpty())
		return;

	TransformCtrl *pCurTransform = nullptr;
	if(m_MultiTransform.IsShown())
		pCurTransform = &m_MultiTransform;
	else
		pCurTransform = &m_SelectedItemList[0]->GetTransformCtrl();

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
	for(IDrawExItem *pDrawItem : m_SelectedItemList)
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

	if(GetCurAction() == HYACTION_HoverScale)
		SetAction(HYACTION_TransformingScale);
	else if(GetCurAction() == HYACTION_HoverRotate)
		SetAction(HYACTION_TransformingRotation);
	else
		SetAction(HYACTION_TransformingTranslate);
}

void IDrawEx::DoMouseMove_Transform(bool bCtrlMod, bool bShiftMod)
{
	glm::vec2 ptWorldMousePos;
	m_pCamera->ProjectToWorld(HyEngine::Input().GetMousePos(), ptWorldMousePos);

	TransformCtrl *pCurTransform = nullptr;
	if(m_MultiTransform.IsShown())
		pCurTransform = &m_MultiTransform;
	else
		pCurTransform = &m_SelectedItemList[0]->GetTransformCtrl();

	// Process the transform
	switch(GetCurAction())
	{
	case HYACTION_TransformingRotation:
		m_ActiveTransform.rot_pivot.Set(m_ptDragCenter);
		if(bShiftMod)
		{
			float fRot = HyMath::AngleFromVector(m_ptDragCenter - ptWorldMousePos) - HyMath::AngleFromVector(m_ptDragCenter - m_ptDragStart);

			if(m_ActiveTransform.ChildCount() == 1)
				m_ActiveTransform.ChildGet(0)->rot.Set(HyMath::RoundToNearest(m_ActiveTransform.ChildGet(0)->rot.Get(), 15.0f));

			m_ActiveTransform.rot.Set(HyMath::RoundToNearest(fRot, 15.0f));
		}
		else
			m_ActiveTransform.rot.Set(HyMath::Round(HyMath::AngleFromVector(m_ptDragCenter - ptWorldMousePos) - HyMath::AngleFromVector(m_ptDragCenter - m_ptDragStart)));
		break;

	case HYACTION_TransformingTranslate:
		if(bShiftMod)
		{
			glm::vec2 ptTarget = ptWorldMousePos;
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
			m_ActiveTransform.pos.Set(HyMath::RoundVec(ptWorldMousePos - m_ptDragStart));
		break;

	case HYACTION_TransformingScale: {
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
				ptAnchorWidth = HyMath::ClosestPointOnRay(ptDragAnchorPoint, glm::normalize(ptAnchorWidth - ptDragAnchorPoint), ptWorldMousePos);
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
				ptAnchorHeight = HyMath::ClosestPointOnRay(ptDragAnchorPoint, glm::normalize(ptAnchorHeight - ptDragAnchorPoint), ptWorldMousePos);
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

	default:
		HyGuiLog("EntityDraw::OnMouseMoveEvent - Unknown transform state not handled: " % QString::number(GetCurAction()), LOGTYPE_Error);
		break;
	}

	RefreshTransforms(); // Need to refreshTransforms first here because m_ActiveTransform was updated, and pCurTransform might be used below

	SnapCandidates snapCandidates;
	GetSnapCandidateList(snapCandidates);
	if(snapCandidates.IsEmpty() == false && GetCurAction() != HYACTION_TransformingRotation) 
	{
		glm::vec2 vSnapOffset = SnapTransform(snapCandidates, pCurTransform);
		m_ActiveTransform.pos.Offset(vSnapOffset);

		RefreshTransforms();
	}
}

void IDrawEx::DoMouseRelease_Transform()
{
	OnPerformTransform();

	m_vNudgeTranslate.setX(0);
	m_vNudgeTranslate.setY(0);

	// Reset 'm_ActiveTransform' to prep for the next transform
	m_ActiveTransform.pos.Set(0.0f, 0.0f);
	m_ActiveTransform.rot.Set(0.0f);
	m_ActiveTransform.scale.Set(1.0f, 1.0f);

	m_SnapGuideHorz.SetVisible(false);
	m_SnapGuideVert.SetVisible(false);

	ClearAction();
}

void IDrawEx::GetSnapCandidateList(SnapCandidates &snapCandidatesOut)
{
	snapCandidatesOut.Clear();

	uint32 uiSnappingSettings = m_pProjItem->GetProject().GetSnappingSettings();
	snapCandidatesOut.m_fSnapTolerance = static_cast<float>(uiSnappingSettings & SNAPSETTING_ToleranceMask);

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
			QList<QPair<HyOrientation, int>> guideList = m_GuideMap.keys();
			for(auto guidePair : guideList)
			{
				if(guidePair.first == HYORIENT_Horizontal)
					snapCandidatesOut.m_HorzSet.insert(static_cast<float>(guidePair.second));
				else
					snapCandidatesOut.m_VertSet.insert(static_cast<float>(guidePair.second));
			}
		}
		if(uiSnappingSettings & SNAPSETTING_Origin)
		{
			snapCandidatesOut.m_HorzSet.insert(0.0f);
			snapCandidatesOut.m_VertSet.insert(0.0f);
		}
		if(uiSnappingSettings & SNAPSETTING_Items)
		{
			for(IDrawExItem *pItem : m_ItemList)
			{
				if(pItem->IsSelected() == false && pItem->GetTransformCtrl().IsValid())
				{
					glm::vec2 ptGrabPt = pItem->GetTransformCtrl().GetGrabPointWorldPos(TransformCtrl::GRAB_BotLeft, m_pCamera);
					snapCandidatesOut.m_HorzSet.insert(ptGrabPt.y);
					snapCandidatesOut.m_VertSet.insert(ptGrabPt.x);

					ptGrabPt = pItem->GetTransformCtrl().GetGrabPointWorldPos(TransformCtrl::GRAB_BotRight, m_pCamera);
					snapCandidatesOut.m_HorzSet.insert(ptGrabPt.y);
					snapCandidatesOut.m_VertSet.insert(ptGrabPt.x);

					ptGrabPt = pItem->GetTransformCtrl().GetGrabPointWorldPos(TransformCtrl::GRAB_TopRight, m_pCamera);
					snapCandidatesOut.m_HorzSet.insert(ptGrabPt.y);
					snapCandidatesOut.m_VertSet.insert(ptGrabPt.x);

					ptGrabPt = pItem->GetTransformCtrl().GetGrabPointWorldPos(TransformCtrl::GRAB_TopLeft, m_pCamera);
					snapCandidatesOut.m_HorzSet.insert(ptGrabPt.y);
					snapCandidatesOut.m_VertSet.insert(ptGrabPt.x);

					if(uiSnappingSettings & SNAPSETTING_ItemMidPoints)
					{
						glm::vec2 ptMid;
						pItem->GetTransformCtrl().GetCentroid(ptMid);
						m_pCamera->ProjectToWorld(ptMid, ptMid);

						snapCandidatesOut.m_HorzSet.insert(ptMid.y);
						snapCandidatesOut.m_VertSet.insert(ptMid.x);
					}
				}
			}
		}
	}
}

glm::vec2 IDrawEx::SnapTransform(const SnapCandidates &snapCandidatesRef, TransformCtrl *pCurTransform)
{
	glm::vec2 vSnapOffset(0.0f, 0.0f);

	m_SnapGuideHorz.SetVisible(false);
	m_SnapGuideVert.SetVisible(false);
	bool bSnappedHorz = false;
	bool bSnappedVert = false;
	for(int i = TransformCtrl::GRAB_BotLeft; i <= TransformCtrl::GRAB_TopLeft + 1; ++i)
	{
		glm::vec2 ptTestPoint;
		if(i == TransformCtrl::GRAB_TopLeft + 1) // When +1 past GRAB_TopLeft, it indicates we're checking the center
		{
			pCurTransform->GetCentroid(ptTestPoint);
			m_pCamera->ProjectToWorld(ptTestPoint, ptTestPoint);
		}
		else
			ptTestPoint = pCurTransform->GetGrabPointWorldPos(static_cast<TransformCtrl::GrabPointType>(i), m_pCamera);

		if(bSnappedHorz == false)
		{
			for(float fSnapCandidate : snapCandidatesRef.m_HorzSet)
			{
				if(abs(fSnapCandidate - ptTestPoint.y) <= snapCandidatesRef.m_fSnapTolerance)
				{
					vSnapOffset.y = fSnapCandidate - ptTestPoint.y;

					glm::vec2 ptSnapPos;
					m_pCamera->ProjectToCamera(glm::vec2(0.0f, fSnapCandidate), ptSnapPos);
					m_SnapGuideHorz.pos.SetY(ptSnapPos.y);
					m_SnapGuideHorz.SetVisible(true);

					bSnappedHorz = true;
					break;
				}
			}
		}
		if(bSnappedVert == false)
		{
			for(float fSnapCandidate : snapCandidatesRef.m_VertSet)
			{
				if(abs(fSnapCandidate - ptTestPoint.x) <= snapCandidatesRef.m_fSnapTolerance)
				{
					vSnapOffset.x = fSnapCandidate - ptTestPoint.x;

					glm::vec2 ptSnapPos;
					m_pCamera->ProjectToCamera(glm::vec2(fSnapCandidate, 0.0f), ptSnapPos);
					m_SnapGuideVert.pos.SetX(ptSnapPos.x);
					m_SnapGuideVert.SetVisible(true);

					bSnappedVert = true;
					break;
				}
			}
		}

		if(bSnappedHorz && bSnappedVert)
			break;
	}

	return vSnapOffset;
}
