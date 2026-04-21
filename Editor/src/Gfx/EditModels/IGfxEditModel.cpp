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
#include "MainWindow.h"

IGfxEditModel::IGfxEditModel(EditModeType eModelType, HyColor color) :
	m_eMODEL_TYPE(eModelType),
	m_GrabPointCenter(GRABPOINT_Center),
	m_bSelfIntersecting(false),
	m_ptSelfIntersection(0.0f, 0.0f),
	m_bLoopClosed(false),
	m_sMalformedReason("No Data"),
	m_eCurAction(EDITMODEACTION_None),
	m_vDragDelta(0.0f, 0.0f),
	m_iGrabPointIndex(-1),
	m_ptGrabPointPos(0.0f, 0.0f)
{
	ClearAction();
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
		pView->SyncColor();
}

bool IGfxEditModel::IsValidModel() const
{
	return m_sMalformedReason.isEmpty();
}

void IGfxEditModel::Deserialize(const QJsonObject &serializedObj)
{
	m_sMalformedReason = DoDeserialize(serializedObj);
	SyncViews(EDITMODE_Idle, EDITMODEACTION_None);
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

void IGfxEditModel::SyncViews(EditModeState eEditModeState, EditModeAction eResult) const
{
	for(IGfxEditView *pView : m_ViewList)
		pView->SyncWithModel(eEditModeState, eResult);
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

const GfxGrabPointModel *IGfxEditModel::GetActiveGrabPoint() const
{
	if(m_iGrabPointIndex < 0 || m_iGrabPointIndex >= m_GrabPointList.size())
		return nullptr;

	return &m_GrabPointList[m_iGrabPointIndex];
}

const GfxGrabPointModel &IGfxEditModel::GetCenterGrabPoint() const
{
	return m_GrabPointCenter;
}

int IGfxEditModel::GetActiveGrabPointIndex() const
{
	return m_iGrabPointIndex;
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
		return false;

	return m_GrabPointList[m_iGrabPointIndex].IsSelected();
}

void IGfxEditModel::DeselectAllGrabPoints()
{
	for(GfxGrabPointModel &grabPtModel : m_GrabPointList)
		grabPtModel.SetSelected(false);
}

Qt::CursorShape IGfxEditModel::MouseMoveIdle()
{
	EditModeAction eResult = DoMouseMoveIdle();

	switch(eResult)
	{
	case EDITMODEACTION_Creation:
		MainWindow::SetStatus("Edit Mode - Creation", 0);
		return Qt::CrossCursor;

	case EDITMODEACTION_Outside:
		MainWindow::SetStatus("Edit Mode", 0);
		return Qt::ArrowCursor;

	case EDITMODEACTION_Inside:
		if(IsAllGrabPointsSelected())
		{
			MainWindow::SetStatus("Edit Mode - Translate ", 0);
			return Qt::SizeAllCursor;
		}
		else
		{
			MainWindow::SetStatus("Edit Mode - Mouse Over", 0);
			return Qt::ArrowCursor;
		}

	case EDITMODEACTION_AppendVertex:
		MainWindow::SetStatus("Edit Mode - Append Vertex", 0);
		return Qt::CrossCursor;

	case EDITMODEACTION_InsertVertex:
		MainWindow::SetStatus("Edit Mode - Insert Vertex", 0);
		return Qt::CrossCursor;

	case EDITMODEACTION_HoverGrabPoint:
		if(IsHoverGrabPointSelected())
		{
			MainWindow::SetStatus("Edit Mode - Translate Vertex", 0);
			return Qt::SizeAllCursor;
		}
		else if(m_iGrabPointIndex >= 0)
		{
			MainWindow::SetStatus("Edit Mode - Select Vertex", 0);
			return m_GrabPointList[m_iGrabPointIndex].GetHoverCursor();
		}

	case EDITMODEACTION_HoverCenter:
		MainWindow::SetStatus("Edit Mode - Translate", 0);
		return Qt::SizeAllCursor;

	default:
		HyGuiLog("IGfxEditModel::MouseMoveIdle - unsupported edit mode action!", LOGTYPE_Error);
		break;
	}

	MainWindow::SetStatus("Edit Mode", 0);
	return Qt::ArrowCursor;
}

bool IGfxEditModel::MousePressEvent(EditModeState eEditModeState, bool bShiftHeld, Qt::MouseButtons uiButtonFlags)
{
	bool bStartTransform = false;

	EditModeAction eResult = DoMouseMoveIdle();
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

		bStartTransform = true;
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

		bStartTransform = true;
	}
	else if(eResult == EDITMODEACTION_Creation)
		bStartTransform = true;
	else if(eResult == EDITMODEACTION_Inside)
		bStartTransform = IsAllGrabPointsSelected();

	m_eCurAction = eResult;

	return bStartTransform;
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

	SyncViews(eEditModeState, EDITMODEACTION_None);
}

void IGfxEditModel::MouseTransform(bool bShiftMod, glm::vec2 ptStartPos, glm::vec2 ptDragPos)
{
	HySetVec(m_vDragDelta, 0.0f, 0.0f);
	if(false == (HyCompareFloat(ptStartPos.x, ptDragPos.y) && HyCompareFloat(ptStartPos.y, ptDragPos.y)))
		HySetVec(m_vDragDelta, ptDragPos.x - ptStartPos.x, ptDragPos.y - ptStartPos.y);

	if(m_eCurAction == EDITMODEACTION_Creation)
		DoTransformCreation(bShiftMod, ptStartPos, ptDragPos);

	if(m_eCurAction == EDITMODEACTION_AppendVertex)
		m_GrabPointList[m_iGrabPointIndex].SetPos(ptDragPos);

	SyncViews(EDITMODE_MouseDragTransform, m_eCurAction);
}

glm::vec2 IGfxEditModel::GetDragDelta() const
{
	return m_vDragDelta;
}

void IGfxEditModel::ClearAction()
{
	m_eCurAction = EDITMODEACTION_None;
	m_vDragDelta = glm::vec2(0.0f, 0.0f);
	m_iGrabPointIndex = -1;
	m_ptGrabPointPos = glm::vec2(0.0f, 0.0f);
}
