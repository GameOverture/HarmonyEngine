/**************************************************************************
*	HyButton.h
*
*	Harmony Engine
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "UI/HyButton.h"
#include "Diagnostics/Console/IHyConsole.h"

HyButton::HyButton(HyEntity2d *pParent /*= nullptr*/) :
	HyInfoPanel(pParent),
	m_fpButtonClickedCallback(nullptr),
	m_pParam(nullptr),
	m_bAllowDownState(true),
	m_bAllowHoverState(true),
	m_bIsHighlighted(false)
{
}

HyButton::HyButton(float fWidth, float fHeight, float fStroke, std::string sTextPrefix, std::string sTextName, HyEntity2d *pParent /*= nullptr*/) :
	HyInfoPanel(fWidth, fHeight, fStroke, sTextPrefix, sTextName, pParent),
	m_fpButtonClickedCallback(nullptr),
	m_pParam(nullptr),
	m_bAllowDownState(true),
	m_bAllowHoverState(true),
	m_bIsHighlighted(false)
{
	SetAsDisabled(m_bIsDisabled);
}

HyButton::HyButton(float fWidth, float fHeight, float fStroke, std::string sTextPrefix, std::string sTextName, int32 iTextDimensionsX, int32 iTextDimensionsY, int32 iTextOffsetX, int32 iTextOffsetY, HyEntity2d *pParent /*= nullptr*/) :
	HyInfoPanel(fWidth, fHeight, fStroke, sTextPrefix, sTextName, iTextDimensionsX, iTextDimensionsY, iTextOffsetX, iTextOffsetY, pParent),
	m_fpButtonClickedCallback(nullptr),
	m_pParam(nullptr),
	m_bAllowDownState(true),
	m_bAllowHoverState(true),
	m_bIsHighlighted(false)
{
	SetAsDisabled(m_bIsDisabled);
}

HyButton::HyButton(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName, HyEntity2d *pParent /*= nullptr*/) :
	HyInfoPanel(sPanelPrefix, sPanelName, sTextPrefix, sTextName, pParent),
	m_fpButtonClickedCallback(nullptr),
	m_pParam(nullptr),
	m_bAllowDownState(true),
	m_bAllowHoverState(true),
	m_bIsHighlighted(false)
{
	SetAsDisabled(m_bIsDisabled);
}

HyButton::HyButton(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName, int32 iTextDimensionsX, int32 iTextDimensionsY, int32 iTextOffsetX, int32 iTextOffsetY, HyEntity2d *pParent /*= nullptr*/) :
	HyInfoPanel(sPanelPrefix, sPanelName, sTextPrefix, sTextName, iTextDimensionsX, iTextDimensionsY, iTextOffsetX, iTextOffsetY, pParent),
	m_fpButtonClickedCallback(nullptr),
	m_pParam(nullptr),
	m_bAllowDownState(true),
	m_bAllowHoverState(true),
	m_bIsHighlighted(false)
{
	SetAsDisabled(m_bIsDisabled);
}

/*virtual*/ HyButton::~HyButton()
{
}

/*virtual*/ void HyButton::SetAsDisabled(bool bIsDisabled) /*override*/
{
	HyInfoPanel::SetAsDisabled(bIsDisabled);
	m_bIsDisabled ? DisableMouseInput() : EnableMouseInput();

	if(m_Panel.IsLoadDataValid() == false)
		return;
	
	if(m_bIsDisabled)
	{
		if(m_Panel.GetState() == HYBUTTONSTATE_Down || m_Panel.GetState() == HYBUTTONSTATE_Hover)
			m_Panel.SetState(HYBUTTONSTATE_Idle);
		else if(m_Panel.GetState() == HYBUTTONSTATE_HighlightedDown || m_Panel.GetState() == HYBUTTONSTATE_HighlightedHover)
			m_Panel.SetState(HYBUTTONSTATE_Highlighted);
	}
}

bool HyButton::IsHighlighted() const
{
	return m_bIsHighlighted;
}

void HyButton::SetAsHighlighted(bool bIsHighlighted)
{
	if(m_bIsHighlighted == bIsHighlighted)
		return;

	m_bIsHighlighted = bIsHighlighted;

	if(m_pProcPanel)
	{
		if(m_bIsHighlighted)
		{
			m_pProcPanel->m_Stroke.SetTint(0.0f, 0.0f, 1.0f);
			m_pProcPanel->m_Stroke.SetLineThickness(m_pProcPanel->m_Stroke.GetLineThickness() * 2.0f);
		}
		else
		{
			m_pProcPanel->m_Stroke.SetTint(0.3f, 0.3f, 0.3f);
			m_pProcPanel->m_Stroke.SetLineThickness(m_pProcPanel->m_Stroke.GetLineThickness() / 2.0f);
		}
	}

	if(m_Panel.IsLoadDataValid() == false)
		return;

	switch(m_Panel.GetState())
	{
	case HYBUTTONSTATE_Idle:
		if(m_bIsHighlighted)
			m_Panel.SetState(HYBUTTONSTATE_Highlighted);
		break;
	case HYBUTTONSTATE_Down:
		if(m_bIsHighlighted)
			m_Panel.SetState(HYBUTTONSTATE_HighlightedDown);
		break;
	case HYBUTTONSTATE_Highlighted:
		if(m_bIsHighlighted == false)
			m_Panel.SetState(HYBUTTONSTATE_Idle);
		break;
	case HYBUTTONSTATE_HighlightedDown:
		if(m_bIsHighlighted == false)
			m_Panel.SetState(HYBUTTONSTATE_Down);
		break;
	case HYBUTTONSTATE_Hover:
		if(m_bIsHighlighted)
			m_Panel.SetState(HYBUTTONSTATE_HighlightedHover);
		break;
	case HYBUTTONSTATE_HighlightedHover:
		if(m_bIsHighlighted == false)
			m_Panel.SetState(HYBUTTONSTATE_Hover);
		break;
	}
}

void HyButton::SetButtonClickedCallback(HyButtonClickedCallback fpCallBack, void *pParam /*= nullptr*/)
{
	m_fpButtonClickedCallback = fpCallBack;
	m_pParam = pParam;
}

void HyButton::InvokeButtonClicked()
{
	OnMouseClicked();
}

/*virtual*/ void HyButton::OnMouseEnter() /*override*/
{
	if(m_bAllowHoverState == false || m_Panel.IsLoadDataValid() == false)
		return;
	
	if(m_bIsHighlighted == false)
	{
		if(m_Panel.GetState() == HYBUTTONSTATE_Idle && m_Panel.GetNumStates() > HYBUTTONSTATE_Hover)
			m_Panel.SetState(HYBUTTONSTATE_Hover);
	}
	else
	{
		if(m_Panel.GetState() == HYBUTTONSTATE_Highlighted && m_Panel.GetNumStates() > HYBUTTONSTATE_HighlightedHover)
			m_Panel.SetState(HYBUTTONSTATE_HighlightedHover);
	}
}

/*virtual*/ void HyButton::OnMouseLeave() /*override*/
{
	if(m_Panel.IsLoadDataValid() == false)
		return;

	if(m_bIsHighlighted == false)
	{
		if(m_Panel.GetState() == HYBUTTONSTATE_Hover)
			m_Panel.SetState(HYBUTTONSTATE_Idle);
	}
	else
	{
		if(m_Panel.GetState() == HYBUTTONSTATE_HighlightedHover)
			m_Panel.SetState(HYBUTTONSTATE_Highlighted);
	}
}

/*virtual*/ void HyButton::OnMouseDown() /*override*/
{
	if(m_bAllowDownState == false || m_Panel.IsLoadDataValid() == false)
		return;

	if(m_bIsHighlighted == false)
	{
		if(m_Panel.GetNumStates() > HYBUTTONSTATE_Down)
			m_Panel.SetState(HYBUTTONSTATE_Down);
	}
	else
	{
		if(m_Panel.GetNumStates() > HYBUTTONSTATE_HighlightedDown)
			m_Panel.SetState(HYBUTTONSTATE_HighlightedDown);
	}
}

/*virtual*/ void HyButton::OnMouseUp() /*override*/
{
	m_Panel.SetState(m_bIsHighlighted ? HYBUTTONSTATE_Highlighted : HYBUTTONSTATE_Idle);
}

/*virtual*/ void HyButton::OnMouseClicked() /*override*/
{
	if(m_fpButtonClickedCallback)
		m_fpButtonClickedCallback(this, m_pParam);
}
