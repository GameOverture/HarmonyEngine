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

IHy9Slice::IHy9Slice(glm::vec2 vFillDimensions, float fBorderThickness, HyEntity2d *pParent) :
	HyEntity2d(pParent),
	m_vFillDimensions(vFillDimensions),
	m_fBorderThickness(fBorderThickness),
	m_fElapsedTime(0.0f),
	m_ePanelState(PANELSTATE_Hidden)
{
	m_Fill.GetShape().SetAsBox(m_vFillDimensions.x, m_vFillDimensions.x);
	ChildAppend(m_Fill);

	m_Corners[LowerLeft].GetShape().SetAsCircle(m_fBorderThickness);
	m_Corners[LowerLeft].pos.Set(0.0f, 0.0f);
	ChildAppend(m_Corners[LowerLeft]);

	m_Corners[UpperLeft].GetShape().SetAsCircle(m_fBorderThickness);
	m_Corners[UpperLeft].pos.Set(0.0f, m_vFillDimensions.y);
	ChildAppend(m_Corners[LowerLeft]);

	m_Corners[UpperRight].GetShape().SetAsCircle(m_fBorderThickness);
	m_Corners[UpperRight].pos.Set(m_vFillDimensions.x, m_vFillDimensions.y);
	ChildAppend(m_Corners[LowerLeft]);

	m_Corners[LowerRight].GetShape().SetAsCircle(m_fBorderThickness);
	m_Corners[LowerRight].pos.Set(m_vFillDimensions.x, 0.0f);
	ChildAppend(m_Corners[LowerLeft]);

	m_Horz[Upper].GetShape().SetAsBox(m_vFillDimensions.x, m_fBorderThickness);
	m_Horz[Upper].pos.Set(0.0f, m_vFillDimensions.y);
	ChildAppend(m_Horz[Upper]);

	m_Horz[Lower].GetShape().SetAsBox(m_vFillDimensions.x, m_fBorderThickness);
	m_Horz[Lower].pos.Set(0.0f, -m_fBorderThickness);
	ChildAppend(m_Horz[Lower]);
	
	m_Vert[Left].GetShape().SetAsBox(m_fBorderThickness, m_vFillDimensions.y);
	m_Vert[Left].pos.Set(m_vFillDimensions.x, 0.0f);
	ChildAppend(m_Vert[Left]);

	m_Vert[Right].GetShape().SetAsBox(m_fBorderThickness, m_vFillDimensions.y);
	m_Vert[Right].pos.Set(-m_fBorderThickness, 0.0f);
	ChildAppend(m_Vert[Right]);

	UseWindowCoordinates();
	SetEnabled(false);
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

/*virtual*/ void IHy9Slice::Show()
{
	if(IsShown() || IsTransition())
		return;

	SetEnabled(true);
	m_fElapsedTime = OnShow();
	m_ePanelState = PANELSTATE_Showing;
}

/*virtual*/ void IHy9Slice::Hide()
{
	if(IsShown() == false || IsTransition())
		return;

	m_fElapsedTime = OnHide();
	m_ePanelState = PANELSTATE_Hiding;
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
		SetEnabled(false);
		OnHidden();
		break;
	}

	m_fElapsedTime = 0.0f;
}
