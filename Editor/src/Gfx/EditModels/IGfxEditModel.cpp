/**************************************************************************
*	IGfxEditModel.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2026 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "IGfxEditModel.h"
#include "IGfxEditView.h"

IGfxEditModel::IGfxEditModel(EditModeType eModelType, HyColor color) :
	m_eMODEL_TYPE(eModelType),
	m_GrabPointCenter(GRABPOINT_Center),
	m_eCurAction(EDITMODEACTION_None),
	m_mtxTransform(1.0f),
	m_iGrabPointIndex(-1),
	m_bTransformShiftMod(false)
{
	SetColor(color);
}

/*virtual*/ IGfxEditModel::~IGfxEditModel()
{
}

EditModeType IGfxEditModel::GetModelType() const
{
	return m_eMODEL_TYPE;
}

HyColor IGfxEditModel::GetColor() const
{
	return m_Color;
}

void IGfxEditModel::SetColor(HyColor color)
{
	m_Color = color;

	for(IGfxEditView *pView : m_ViewList)
		pView->RefreshColor();
}

void IGfxEditModel::Deserialize(const QList<float> &floatList)
{
	DoDeserialize(floatList);
	RefreshViews(EDITMODE_Idle, EDITMODEACTION_None);
}

void IGfxEditModel::AddView(IGfxEditView *pView)
{
	if(m_ViewList.contains(pView))
		return;
	m_ViewList.push_back(pView);
}

bool IGfxEditModel::RemoveView(IGfxEditView *pView)
{
	return m_ViewList.removeOne(pView);
}

void IGfxEditModel::RefreshViews(EditModeState eEditModeState, EditModeAction eResult) const
{
	for(IGfxEditView *pView : m_ViewList)
		pView->RefreshView(eEditModeState, eResult);
}

void IGfxEditModel::GetTransformPreview(glm::mat4 &mtxTransformOut, int &iGrabPointIndexOut) const
{
	mtxTransformOut = m_mtxTransform;
	iGrabPointIndexOut = m_iGrabPointIndex;
}

const QList<GfxGrabPointModel> &IGfxEditModel::GetGrabPointList() const
{
	return m_GrabPointList;
}

const GfxGrabPointModel &IGfxEditModel::GetGrabPoint(int iIndex) const
{
	if(iIndex < 0 || iIndex >= m_GrabPointList.size())
	{
		HyGuiLog("IGfxEditModel::GetGrabPoint - Index out of range", LOGTYPE_Error);
		return m_GrabPointCenter;
	}
	return m_GrabPointList[iIndex];
}

const GfxGrabPointModel &IGfxEditModel::GetCenterGrabPoint() const
{
	return m_GrabPointCenter;
}

int IGfxEditModel::GetNumGrabPointsSelected() const
{
	int iNumSelected = 0;
	for(const GfxGrabPointModel &grabPtModel : m_GrabPointList)
	{
		if(grabPtModel.IsSelected())
			++iNumSelected;
	}
	return iNumSelected;
}

bool IGfxEditModel::IsAllGrabPointsSelected() const
{
	for(const GfxGrabPointModel &grabPtModel : m_GrabPointList)
	{
		if(grabPtModel.IsSelected() == false)
			return false;
	}
	return true;
}

bool IGfxEditModel::IsHoverGrabPointSelected() const
{
	if(m_iGrabPointIndex < 0 || m_iGrabPointIndex >= m_GrabPointList.size())
	{
		HyGuiLog("IGfxEditModel::IsHoverGrabPointSelected - Index out of range", LOGTYPE_Error);
		return false;
	}
	return m_GrabPointList[m_iGrabPointIndex].IsSelected();
}

void IGfxEditModel::DeselectAllGrabPoints()
{
	for(GfxGrabPointModel &grabPtModel : m_GrabPointList)
		grabPtModel.SetSelected(false);
}

Qt::CursorShape IGfxEditModel::MouseMoveIdle(EditModeState eEditModeState, glm::vec2 ptWorldMousePos)
{
	EditModeAction eResult = DoMouseMoveIdle(ptWorldMousePos);

	switch(eResult)
	{
	case EDITMODEACTION_Creation:		return Qt::CrossCursor;
	case EDITMODEACTION_Outside:		return Qt::ArrowCursor;
	case EDITMODEACTION_Inside:			return IsAllGrabPointsSelected() ? Qt::SizeAllCursor : Qt::ArrowCursor;
	case EDITMODEACTION_AppendVertex:	return Qt::CrossCursor;
	case EDITMODEACTION_InsertVertex:	return Qt::CrossCursor;
	case EDITMODEACTION_HoverGrabPoint: return IsHoverGrabPointSelected() ? Qt::SizeAllCursor : m_GrabPointList[m_iGrabPointIndex].GetHoverCursor();
	case EDITMODEACTION_HoverCenter:	return Qt::SizeAllCursor;
	default:
		HyGuiLog("IGfxEditModel::MouseMoveIdle - unsupported edit mode action!", LOGTYPE_Error);
		break;
	}

	return Qt::ArrowCursor;
}

EditModeAction IGfxEditModel::MousePressEvent(EditModeState eEditModeState, bool bShiftHeld, Qt::MouseButtons uiButtonFlags, glm::vec2 ptWorldMousePos)
{
	EditModeAction eResult = DoMouseMoveIdle(ptWorldMousePos);
	if(eResult == EDITMODEACTION_AppendVertex || eResult == EDITMODEACTION_InsertVertex)
	{
		if(m_iGrabPointIndex == -1)
		{
			HyGuiLog("GfxShapeModel::MousePressEvent - Insert vertex index was -1 on AddVertex/Crosshair", LOGTYPE_Error);
			return EDITMODEACTION_None;
		}
		m_GrabPointList.insert(m_GrabPointList.begin() + m_iGrabPointIndex, GfxGrabPointModel(GRABPOINT_VertexSelected, m_ptGrabPointPos));
		for(int i = 0; i < m_GrabPointList.size(); ++i)
			m_GrabPointList[i].SetSelected(false);
		m_GrabPointList[m_iGrabPointIndex].SetSelected(true);
	}
	else if(eResult == EDITMODEACTION_HoverGrabPoint)
	{
		if(m_iGrabPointIndex == -1)
		{
			HyGuiLog("GfxShapeModel::OnMousePressEvent - Hover vertex index was -1 on HoverVertex/HoverSelectedVertex", LOGTYPE_Error);
			return EDITMODEACTION_None;
		}

		if(bShiftHeld)
		{
			if(uiButtonFlags & Qt::LeftButton)
				m_GrabPointList[m_iGrabPointIndex].SetSelected(!m_GrabPointList[m_iGrabPointIndex].IsSelected());
			else if(uiButtonFlags & Qt::RightButton)
				m_GrabPointList[m_iGrabPointIndex].SetSelected(false);
		}
		else
		{
			if(uiButtonFlags & Qt::LeftButton)
			{
				for(int i = 0; i < m_GrabPointList.size(); ++i)
					m_GrabPointList[i].SetSelected(false);
				m_GrabPointList[m_iGrabPointIndex].SetSelected(true);
			}
			else if(uiButtonFlags & Qt::RightButton)
				m_GrabPointList[m_iGrabPointIndex].SetSelected(false);
		}
	}

	m_eCurAction = eResult;
	return eResult;
}

void IGfxEditModel::MouseMarqueeReleased(EditModeState eEditModeState, bool bLeftClick, QPointF ptBotLeft, QPointF ptTopRight)
{
	// Select grab points within marquee
	for(GfxGrabPointModel &grabPtModel : m_GrabPointList)
	{
		grabPtModel.SetSelected(false);

		QPointF ptGrabPos(grabPtModel.GetPos().x, grabPtModel.GetPos().y);
		if(ptGrabPos.x() >= ptBotLeft.x() && ptGrabPos.x() <= ptTopRight.x() &&
		   ptGrabPos.y() >= ptBotLeft.y() && ptGrabPos.y() <= ptTopRight.y())
		{
			if(bLeftClick)
				grabPtModel.SetSelected(true);
		}
	}

	RefreshViews(eEditModeState, EDITMODEACTION_None);
}

void IGfxEditModel::MouseMoveTransform(EditModeState eEditModeState, bool bShiftMod, glm::vec2 ptStartPos, glm::vec2 ptDragPos)
{
	m_bTransformShiftMod = bShiftMod;

	m_mtxTransform = glm::identity<glm::mat4>();
	if(false == (HyCompareFloat(ptStartPos.x, ptDragPos.y) && HyCompareFloat(ptStartPos.y, ptDragPos.y)))
	{
		glm::vec2 vDelta(ptDragPos.x - ptStartPos.x,
						 ptDragPos.y - ptStartPos.y);
		m_mtxTransform = glm::translate(m_mtxTransform, glm::vec3(vDelta, 0.0f));
	}

	if(m_eCurAction == EDITMODEACTION_Creation)
		DoTransformCreation(ptStartPos, ptDragPos);
	//else if(m_eCurTransform == EDITMODEACTION_HoverGrabPoint)
	//	DoTransformGrabPoint(ptStartPos, ptDragPos);

	//RefreshViews(eEditModeState, m_eCurTransform);
}
