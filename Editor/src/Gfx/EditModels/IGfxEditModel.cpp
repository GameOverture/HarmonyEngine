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

IGfxEditModel::IGfxEditModel(EditModelType eModelType, HyColor color) :
	m_eMODEL_TYPE(eModelType),
	m_GrabPointCenter(GRABPOINT_Center),
	m_eCurTransform(SHAPEMOUSEMOVE_None),
	m_mtxTransform(1.0f),
	m_iVertexIndex(-1),
	m_bTransformShiftMod(false)
{
	SetColor(color);
}

/*virtual*/ IGfxEditModel::~IGfxEditModel()
{
}

EditModelType IGfxEditModel::GetModelType() const
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

void IGfxEditModel::GetTransformPreview(glm::mat4 &mtxTransformOut, int &iVertexIndexOut) const
{
	mtxTransformOut = m_mtxTransform;
	iVertexIndexOut = m_iVertexIndex;
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
	if(m_iVertexIndex < 0 || m_iVertexIndex >= m_GrabPointList.size())
	{
		HyGuiLog("IGfxEditModel::IsHoverGrabPointSelected - Index out of range", LOGTYPE_Error);
		return false;
	}
	return m_GrabPointList[m_iVertexIndex].IsSelected();
}

ShapeMouseMoveResult IGfxEditModel::MouseMoveIdle(glm::vec2 ptWorldMousePos)
{
	ShapeMouseMoveResult eResult = DoMouseMoveIdle(ptWorldMousePos);

	for(IGfxEditView *pView : m_ViewList)
		pView->RefreshView(eResult, false);

	return eResult;
}

ShapeMouseMoveResult IGfxEditModel::MousePressEvent(bool bShiftHeld, Qt::MouseButtons uiButtonFlags, glm::vec2 ptWorldMousePos)
{
	ShapeMouseMoveResult eResult = DoMouseMoveIdle(ptWorldMousePos);
	if(eResult == SHAPEMOUSEMOVE_AppendVertex || eResult == SHAPEMOUSEMOVE_InsertVertex)
	{
		if(m_iVertexIndex == -1)
		{
			HyGuiLog("GfxShapeModel::MousePressEvent - Insert vertex index was -1 on AddVertex/Crosshair", LOGTYPE_Error);
			return SHAPEMOUSEMOVE_None;
		}
		m_GrabPointList.insert(m_GrabPointList.begin() + m_iVertexIndex, GfxGrabPointModel(GRABPOINT_VertexSelected, m_ptVertexPos));
		for(int i = 0; i < m_GrabPointList.size(); ++i)
			m_GrabPointList[i].SetSelected(false);
		m_GrabPointList[m_iVertexIndex].SetSelected(true);
	}
	else if(eResult == SHAPEMOUSEMOVE_HoverGrabPoint)
	{
		if(m_iVertexIndex == -1)
		{
			HyGuiLog("GfxShapeModel::OnMousePressEvent - Hover vertex index was -1 on HoverVertex/HoverSelectedVertex", LOGTYPE_Error);
			return SHAPEMOUSEMOVE_None;
		}

		if(bShiftHeld)
		{
			if(uiButtonFlags & Qt::LeftButton)
				m_GrabPointList[m_iVertexIndex].SetSelected(!m_GrabPointList[m_iVertexIndex].IsSelected());
			else if(uiButtonFlags & Qt::RightButton)
				m_GrabPointList[m_iVertexIndex].SetSelected(false);
		}
		else
		{
			if(uiButtonFlags & Qt::LeftButton)
				m_GrabPointList[m_iVertexIndex].SetSelected(true);
			else if(uiButtonFlags & Qt::RightButton)
				m_GrabPointList[m_iVertexIndex].SetSelected(false);
		}
	}

	for(IGfxEditView *pView : m_ViewList)
		pView->RefreshView(eResult, false);

	m_eCurTransform = eResult;
	return eResult;
}

void IGfxEditModel::MouseMarqueeReleased(Qt::MouseButtons uiButtonFlags, QPointF ptBotLeft, QPointF ptTopRight)
{
	// Select grab points within marquee
	for(GfxGrabPointModel &grabPtModel : m_GrabPointList)
	{
		QPointF ptGrabPos(grabPtModel.GetPos().x, grabPtModel.GetPos().y);
		if(ptGrabPos.x() >= ptBotLeft.x() && ptGrabPos.x() <= ptTopRight.x() &&
		   ptGrabPos.y() >= ptBotLeft.y() && ptGrabPos.y() <= ptTopRight.y())
		{
			if(uiButtonFlags & Qt::LeftButton)
				grabPtModel.SetSelected(true);
			else if(uiButtonFlags & Qt::RightButton)
				grabPtModel.SetSelected(false);
		}
	}

	for(IGfxEditView *pView : m_ViewList)
		pView->RefreshView(SHAPEMOUSEMOVE_None, false);
}

void IGfxEditModel::MouseMoveTransform(bool bShiftMod, glm::vec2 ptStartPos, glm::vec2 ptDragPos)
{
	m_bTransformShiftMod = bShiftMod;

	m_mtxTransform = glm::identity<glm::mat4>();
	if(false == (HyCompareFloat(ptStartPos.x, ptDragPos.y) && HyCompareFloat(ptStartPos.y, ptDragPos.y)))
	{
		glm::vec2 vDelta(ptDragPos.x - ptStartPos.x,
						 ptDragPos.y - ptStartPos.y);
		m_mtxTransform = glm::translate(m_mtxTransform, glm::vec3(vDelta, 0.0f));
	}

	if(m_eCurTransform == SHAPEMOUSEMOVE_Creation)
		DoTransformCreation(ptStartPos, ptDragPos);
	//else if(m_eCurTransform == SHAPEMOUSEMOVE_HoverGrabPoint)
	//	DoTransformGrabPoint(ptStartPos, ptDragPos);

	for(IGfxEditView *pView : m_ViewList)
		pView->RefreshView(m_eCurTransform, true);
}
