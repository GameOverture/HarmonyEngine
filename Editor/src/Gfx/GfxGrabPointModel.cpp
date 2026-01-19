/**************************************************************************
*	GfxGrabPointModel.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2026 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Global.h"
#include "GfxGrabPointModel.h"
#include "GfxGrabPointView.h"

#define GRABPOINT_SELECT_RADIUS		6.0f	// In world units

GfxGrabPointModel::GfxGrabPointModel() :
	m_eType(GRABPOINT_Invalid)
{
}

GfxGrabPointModel::GfxGrabPointModel(GrabPointType eType)
{
	Setup(eType, m_ptPosition);
}

GfxGrabPointModel::GfxGrabPointModel(GrabPointType eType, glm::vec2 ptPosition)
{
	Setup(eType, m_ptPosition);
}

GfxGrabPointModel::~GfxGrabPointModel()
{
}

void GfxGrabPointModel::Setup(GrabPointType eType)
{
	Setup(eType, m_ptPosition);
}

void GfxGrabPointModel::Setup(glm::vec2 ptPosition)
{
	Setup(m_eType, ptPosition);
}

void GfxGrabPointModel::Setup(GrabPointType eType, glm::vec2 ptPosition)
{
	m_eType = eType;
	m_ptPosition = ptPosition;
}

glm::vec2 GfxGrabPointModel::GetPos() const
{
	return m_ptPosition;
}

float GfxGrabPointModel::GetRadius() const
{
	return GRABPOINT_SELECT_RADIUS;
}

bool GfxGrabPointModel::TestPoint(glm::vec2 ptWorld) const
{
	return glm::distance(m_ptPosition, ptWorld) <= GRABPOINT_SELECT_RADIUS;
}

bool GfxGrabPointModel::IsSelected() const
{
	return m_eType == GRABPOINT_VertexSelected || m_eType == GRABPOINT_EndpointSelected;
}

void GfxGrabPointModel::SetSelected(bool bIsSelected)
{
	if(m_eType == GRABPOINT_Vertex || m_eType == GRABPOINT_VertexSelected)
		m_eType = bIsSelected ? GRABPOINT_VertexSelected : GRABPOINT_Vertex;
	else if(m_eType == GRABPOINT_Endpoint || m_eType == GRABPOINT_EndpointSelected)
		m_eType = bIsSelected ? GRABPOINT_EndpointSelected : GRABPOINT_Endpoint;
	else
		HyGuiLog("GfxGrabPointModel::SetSelected called on unsupported GrabPointType: " % QString::number(m_eType), LOGTYPE_Error);
}

HyColor GfxGrabPointModel::GetOutlineColor() const
{
	switch(m_eType)
	{ 
	case GRABPOINT_Transform:			return HyGlobal::GetEditorColor(EDITORCOLOR_TransformGrabPointOutline);
	case GRABPOINT_TransformRotate:		return HyGlobal::GetEditorColor(EDITORCOLOR_TransformRotateGrabPointOutline);
	case GRABPOINT_Vertex:				return HyGlobal::GetEditorColor(EDITORCOLOR_ShapeGrabPointOutline);
	case GRABPOINT_VertexSelected:		return HyGlobal::GetEditorColor(EDITORCOLOR_ShapeGrabPointSelectedOutline);
	case GRABPOINT_Endpoint:			return HyGlobal::GetEditorColor(EDITORCOLOR_ShapeGrabPointOutline);
	case GRABPOINT_EndpointSelected:	return HyGlobal::GetEditorColor(EDITORCOLOR_ShapeGrabPointSelectedOutline);
	case GRABPOINT_Center:				return HyGlobal::GetEditorColor(EDITORCOLOR_CenterGrabPointOutline);
	
	case GRABPOINT_Invalid:
	default:
		HyGuiLog("GfxGrabPointModel::GetOutlineColor called on unsupported GrabPointType: " % QString::number(m_eType), LOGTYPE_Error);
		break;
	}

	return HyColor();
}

HyColor GfxGrabPointModel::GetFillColor() const
{
	switch(m_eType)
	{ 
	case GRABPOINT_Transform:			return HyGlobal::GetEditorColor(EDITORCOLOR_TransformGrabPointFill);
	case GRABPOINT_TransformRotate:		return HyGlobal::GetEditorColor(EDITORCOLOR_TransformRotateGrabPointFill);
	case GRABPOINT_Vertex:				return HyGlobal::GetEditorColor(EDITORCOLOR_ShapeGrabPointFill);
	case GRABPOINT_VertexSelected:		return HyGlobal::GetEditorColor(EDITORCOLOR_ShapeGrabPointSelectedFill);
	case GRABPOINT_Endpoint:			return HyGlobal::GetEditorColor(EDITORCOLOR_ShapeGrabPointFill);
	case GRABPOINT_EndpointSelected:	return HyGlobal::GetEditorColor(EDITORCOLOR_ShapeGrabPointSelectedFill);
	case GRABPOINT_Center:				return HyGlobal::GetEditorColor(EDITORCOLOR_CenterGrabPointFill);
	
	case GRABPOINT_Invalid:
	default:
		HyGuiLog("GfxGrabPointModel::GetFillColor called on unsupported GrabPointType: " % QString::number(m_eType), LOGTYPE_Error);
		break;
	}
	return HyColor();
}
