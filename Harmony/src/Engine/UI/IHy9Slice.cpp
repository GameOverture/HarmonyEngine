/**************************************************************************
*	IHy9Slice.cpp
*
*	Harmony Engine
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "UI/IHy9Slice.h"
#include "HyEngine.h"

IHy9Slice::Border::Border(glm::vec2 vFillDimensions, float fBorderThickness, HyPrimitive2d &fillRef, HyEntity2d *pParent)
{
	m_Corners[LowerLeft].GetShape().SetAsCircle(fBorderThickness);
	m_Corners[LowerLeft].pos.Set(0.0f, 0.0f);
	ChildAppend(m_Corners[LowerLeft]);

	m_Corners[UpperLeft].GetShape().SetAsCircle(fBorderThickness);
	m_Corners[UpperLeft].pos.Set(0.0f, vFillDimensions.y);
	ChildAppend(m_Corners[UpperLeft]);

	m_Corners[UpperRight].GetShape().SetAsCircle(fBorderThickness);
	m_Corners[UpperRight].pos.Set(vFillDimensions.x, vFillDimensions.y);
	ChildAppend(m_Corners[UpperRight]);

	m_Corners[LowerRight].GetShape().SetAsCircle(fBorderThickness);
	m_Corners[LowerRight].pos.Set(vFillDimensions.x, 0.0f);
	ChildAppend(m_Corners[LowerRight]);

	m_Horz[Upper].GetShape().SetAsBox(vFillDimensions.x, fBorderThickness);
	m_Horz[Upper].pos.Set(0.0f, vFillDimensions.y);
	ChildAppend(m_Horz[Upper]);

	m_Horz[Lower].GetShape().SetAsBox(vFillDimensions.x, fBorderThickness);
	m_Horz[Lower].pos.Set(0.0f, -fBorderThickness);
	ChildAppend(m_Horz[Lower]);
	
	m_Vert[Left].GetShape().SetAsBox(fBorderThickness, vFillDimensions.y);
	m_Vert[Left].pos.Set(vFillDimensions.x, 0.0f);
	ChildAppend(m_Vert[Left]);

	m_Vert[Right].GetShape().SetAsBox(fBorderThickness, vFillDimensions.y);
	m_Vert[Right].pos.Set(-fBorderThickness, 0.0f);
	ChildAppend(m_Vert[Right]);

	m_StencilForCorner.AddMask(fillRef);
	m_StencilForCorner.SetAsInvertedMask();

	for(uint32 i = 0; i < NumCorners; ++i)
		m_StencilForEdges.AddMask(m_Corners[i]);
	m_StencilForEdges.SetAsInvertedMask();

	for(uint32 i = 0; i < NumCorners; ++i)
		m_Corners[i].SetStencil(&m_StencilForCorner);

	for(uint32 i = 0; i < NumHorz; ++i)
		m_Horz[i].SetStencil(&m_StencilForEdges);

	for(uint32 i = 0; i < NumVert; ++i)
		m_Vert[i].SetStencil(&m_StencilForEdges);
}

IHy9Slice::IHy9Slice(glm::vec2 vFillDimensions, float fBorderThickness, HyEntity2d *pParent) :
	HyEntity2d(pParent),
	m_vFillDimensions(vFillDimensions),
	m_fBorderThickness(fBorderThickness),
	m_fElapsedTime(0.0f),
	m_ePanelState(PANELSTATE_Hidden),
	m_Border(vFillDimensions, fBorderThickness, m_Fill, this)
{
	ChildAppend(m_Border);

	m_Fill.GetShape().SetAsBox(m_vFillDimensions.x, m_vFillDimensions.y);
	ChildAppend(m_Fill);

	m_Border.SetTint(45.0f / 255.0f, 45.0f / 255.0f, 48.0f / 255.0f);
	m_Fill.SetTint(37.0f / 255.0f, 37.0f / 255.0f, 37.0f / 255.0f);
	UseWindowCoordinates();
	SetVisible(false);
}

IHy9Slice::~IHy9Slice()
{
}

float IHy9Slice::GetWidth(bool bIncludeBorders)
{
	if(bIncludeBorders)
		return m_vFillDimensions.x + (m_fBorderThickness * 2.0f);
	else
		return m_vFillDimensions.x;
}

float IHy9Slice::GetHeight(bool bIncludeBorders)
{
	if(bIncludeBorders)
		return m_vFillDimensions.y + (m_fBorderThickness * 2.0f);
	else
		return m_vFillDimensions.y;
}

float IHy9Slice::GetBorderThickness()
{
	return m_fBorderThickness;
}

HyEntity2d &IHy9Slice::GetBorder()
{
	return m_Border;
}

HyPrimitive2d &IHy9Slice::GetFill()
{
	return m_Fill;
}

/*virtual*/ bool IHy9Slice::Show()
{
	if(IsShown() || IsTransition())
		return false;

	SetVisible(true);
	m_fElapsedTime = OnShow();
	m_ePanelState = PANELSTATE_Showing;

	return true;
}

/*virtual*/ bool IHy9Slice::Hide()
{
	if(IsShown() == false || IsTransition())
		return false;

	m_fElapsedTime = OnHide();
	m_ePanelState = PANELSTATE_Hiding;

	return true;
}

bool IHy9Slice::IsTransition()
{
	return m_ePanelState == PANELSTATE_Showing || m_ePanelState == PANELSTATE_Hiding;
}

bool IHy9Slice::IsShown()
{
	return m_ePanelState == PANELSTATE_Shown;
}

/*virtual*/ void IHy9Slice::OnUpdate() /*override*/
{
	if(m_fElapsedTime > 0.0f)
	{
		m_fElapsedTime -= Hy_UpdateStep();
		return;
	}

	switch(m_ePanelState)
	{
	case PANELSTATE_Showing:
		m_ePanelState = PANELSTATE_Shown;
		OnShown();
		break;

	case PANELSTATE_Hiding:
		m_ePanelState = PANELSTATE_Hidden;
		SetVisible(false);
		OnHidden();
		break;
	}

	m_fElapsedTime = 0.0f;
}
