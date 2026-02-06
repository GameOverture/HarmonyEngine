/**************************************************************************
*	GfxGrabPointModel.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2026 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef GfxGrabPointModel_H
#define GfxGrabPointModel_H

#include "Global.h"

enum GrabPointType
{
	GRABPOINT_Invalid = -1,
	
	GRABPOINT_Transform = 0,
	GRABPOINT_TransformRotate,

	GRABPOINT_Vertex,
	GRABPOINT_VertexSelected,
	GRABPOINT_Endpoint,
	GRABPOINT_EndpointSelected,

	GRABPOINT_ShapeCtrlAll,
	GRABPOINT_ShapeCtrlHorz,
	GRABPOINT_ShapeCtrlVert,

	GRABPOINT_Center
};

class GfxGrabPointModel
{
	GrabPointType				m_eType;
	glm::vec2					m_ptPosition;

public:
	GfxGrabPointModel();
	GfxGrabPointModel(GrabPointType eType);
	GfxGrabPointModel(GrabPointType eType, glm::vec2 ptPosition);
	~GfxGrabPointModel();

	void Setup(GrabPointType eType);
	void Setup(glm::vec2 ptPosition);
	void Setup(GrabPointType eType, glm::vec2 ptPosition);

	glm::vec2 GetPos() const;
	float GetRadius() const;
	bool TestPoint(glm::vec2 ptWorld) const;

	bool IsSelected() const;
	void SetSelected(bool bIsSelected);

	HyColor GetOutlineColor() const;
	HyColor GetFillColor() const;

	Qt::CursorShape GetHoverCursor() const;
};

#endif // GfxGrabPointModel_H
