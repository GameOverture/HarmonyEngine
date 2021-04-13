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
#include "UI/Widgets/HyButton.h"
#include "Diagnostics/Console/IHyConsole.h"

HyButton::HyButton(HyEntity2d *pParent /*= nullptr*/) :
	HyLabel(pParent),
	m_fpBtnClickedCallback(nullptr),
	m_pBtnClickedParam(nullptr)
{
}

HyButton::HyButton(float fWidth, float fHeight, float fStroke, std::string sTextPrefix, std::string sTextName, HyEntity2d *pParent /*= nullptr*/) :
	HyLabel(fWidth, fHeight, fStroke, sTextPrefix, sTextName, pParent),
	m_fpBtnClickedCallback(nullptr),
	m_pBtnClickedParam(nullptr)
{
}

HyButton::HyButton(float fWidth, float fHeight, float fStroke, std::string sTextPrefix, std::string sTextName, int32 iTextDimensionsX, int32 iTextDimensionsY, int32 iTextOffsetX, int32 iTextOffsetY, HyEntity2d *pParent /*= nullptr*/) :
	HyLabel(fWidth, fHeight, fStroke, sTextPrefix, sTextName, iTextDimensionsX, iTextDimensionsY, iTextOffsetX, iTextOffsetY, pParent),
	m_fpBtnClickedCallback(nullptr),
	m_pBtnClickedParam(nullptr)
{
}

HyButton::HyButton(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName, HyEntity2d *pParent /*= nullptr*/) :
	HyLabel(sPanelPrefix, sPanelName, sTextPrefix, sTextName, pParent),
	m_fpBtnClickedCallback(nullptr),
	m_pBtnClickedParam(nullptr)
{
}

HyButton::HyButton(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName, int32 iTextDimensionsX, int32 iTextDimensionsY, int32 iTextOffsetX, int32 iTextOffsetY, HyEntity2d *pParent /*= nullptr*/) :
	HyLabel(sPanelPrefix, sPanelName, sTextPrefix, sTextName, iTextDimensionsX, iTextDimensionsY, iTextOffsetX, iTextOffsetY, pParent),
	m_fpBtnClickedCallback(nullptr),
	m_pBtnClickedParam(nullptr)
{
}

/*virtual*/ HyButton::~HyButton()
{
}

/*virtual*/ void HyButton::SetAsDisabled(bool bIsDisabled) /*override*/
{
	HyLabel::SetAsDisabled(bIsDisabled);
	IsDisabled() ? DisableMouseInput() : EnableMouseInput();

	if(m_SpritePanel.IsLoadDataValid() == false)
		return;
	
	if(IsDisabled())
	{
		if(m_SpritePanel.GetState() == HYBUTTONSTATE_Down || m_SpritePanel.GetState() == HYBUTTONSTATE_Hover)
			m_SpritePanel.SetState(HYBUTTONSTATE_Idle);
		else if(m_SpritePanel.GetState() == HYBUTTONSTATE_HighlightedDown || m_SpritePanel.GetState() == HYBUTTONSTATE_HighlightedHover)
			m_SpritePanel.SetState(HYBUTTONSTATE_Highlighted);
	}
}

/*virtual*/ void HyButton::SetAsHighlighted(bool bIsHighlighted) /*override*/
{
	if(bIsHighlighted == IsHighlighted())
		return;

	HyLabel::SetAsHighlighted(bIsHighlighted);

	if(m_SpritePanel.IsLoadDataValid() == false)
		return;

	switch(m_SpritePanel.GetState())
	{
	case HYBUTTONSTATE_Idle:
		if(IsHighlighted())
			m_SpritePanel.SetState(HYBUTTONSTATE_Highlighted);
		break;
	case HYBUTTONSTATE_Down:
		if(IsHighlighted())
			m_SpritePanel.SetState(HYBUTTONSTATE_HighlightedDown);
		break;
	case HYBUTTONSTATE_Highlighted:
		if(IsHighlighted() == false)
			m_SpritePanel.SetState(HYBUTTONSTATE_Idle);
		break;
	case HYBUTTONSTATE_HighlightedDown:
		if(IsHighlighted() == false)
			m_SpritePanel.SetState(HYBUTTONSTATE_Down);
		break;
	case HYBUTTONSTATE_Hover:
		if(IsHighlighted())
			m_SpritePanel.SetState(HYBUTTONSTATE_HighlightedHover);
		break;
	case HYBUTTONSTATE_HighlightedHover:
		if(IsHighlighted() == false)
			m_SpritePanel.SetState(HYBUTTONSTATE_Hover);
		break;
	}
}

bool HyButton::IsHideDownState() const
{
	return (m_uiInfoPanelAttribs & INFOPANELATTRIB_HideDownState) != 0;
}

void HyButton::SetHideDownState(bool bIsHideDownState)
{
	if(bIsHideDownState)
	{
		m_uiInfoPanelAttribs |= INFOPANELATTRIB_HideDownState;

		if(m_SpritePanel.GetState() == HYBUTTONSTATE_Down)
			m_SpritePanel.SetState(HYBUTTONSTATE_Idle);
		else if(m_SpritePanel.GetState() == HYBUTTONSTATE_HighlightedDown)
			m_SpritePanel.SetState(HYBUTTONSTATE_Highlighted);
	}
	else
		m_uiInfoPanelAttribs &= ~INFOPANELATTRIB_HideDownState;
}

bool HyButton::IsHideHoverState() const
{
	return (m_uiInfoPanelAttribs & INFOPANELATTRIB_HideHoverState) != 0;
}

void HyButton::SetHideHoverState(bool bIsHideHoverState)
{
	if(bIsHideHoverState)
	{
		m_uiInfoPanelAttribs |= INFOPANELATTRIB_HideHoverState;

		if(m_SpritePanel.GetState() == HYBUTTONSTATE_Hover)
			m_SpritePanel.SetState(HYBUTTONSTATE_Idle);
		else if(m_SpritePanel.GetState() == HYBUTTONSTATE_HighlightedHover)
			m_SpritePanel.SetState(HYBUTTONSTATE_Highlighted);
	}
	else
		m_uiInfoPanelAttribs &= ~INFOPANELATTRIB_HideHoverState;
}

void HyButton::SetButtonClickedCallback(HyButtonClickedCallback fpCallBack, void *pParam /*= nullptr*/)
{
	m_fpBtnClickedCallback = fpCallBack;
	m_pBtnClickedParam = pParam;
}

void HyButton::InvokeButtonClicked()
{
	OnMouseClicked();
}

/*virtual*/ void HyButton::OnMouseEnter() /*override*/
{
	if(IsHideHoverState() || m_SpritePanel.IsLoadDataValid() == false)
		return;
	
	if(IsHighlighted() == false)
	{
		if(m_SpritePanel.GetState() == HYBUTTONSTATE_Idle && m_SpritePanel.GetNumStates() > HYBUTTONSTATE_Hover)
			m_SpritePanel.SetState(HYBUTTONSTATE_Hover);
	}
	else
	{
		if(m_SpritePanel.GetState() == HYBUTTONSTATE_Highlighted && m_SpritePanel.GetNumStates() > HYBUTTONSTATE_HighlightedHover)
			m_SpritePanel.SetState(HYBUTTONSTATE_HighlightedHover);
	}
}

/*virtual*/ void HyButton::OnMouseLeave() /*override*/
{
	if(m_SpritePanel.IsLoadDataValid() == false)
		return;

	if(IsHighlighted() == false)
	{
		if(m_SpritePanel.GetState() == HYBUTTONSTATE_Down || m_SpritePanel.GetState() == HYBUTTONSTATE_Hover)
			m_SpritePanel.SetState(HYBUTTONSTATE_Idle);
	}
	else
	{
		if(m_SpritePanel.GetState() == HYBUTTONSTATE_HighlightedDown || m_SpritePanel.GetState() == HYBUTTONSTATE_HighlightedHover)
			m_SpritePanel.SetState(HYBUTTONSTATE_Highlighted);
	}
}

/*virtual*/ void HyButton::OnMouseDown() /*override*/
{
	if(IsHideDownState() || m_SpritePanel.IsLoadDataValid() == false)
		return;

	if(IsHighlighted() == false)
	{
		if(m_SpritePanel.GetNumStates() > HYBUTTONSTATE_Down)
			m_SpritePanel.SetState(HYBUTTONSTATE_Down);
	}
	else
	{
		if(m_SpritePanel.GetNumStates() > HYBUTTONSTATE_HighlightedDown)
			m_SpritePanel.SetState(HYBUTTONSTATE_HighlightedDown);
	}
}

/*virtual*/ void HyButton::OnMouseUp() /*override*/
{
	m_SpritePanel.SetState(IsHighlighted() ? HYBUTTONSTATE_Highlighted : HYBUTTONSTATE_Idle);
}

/*virtual*/ void HyButton::OnMouseClicked() /*override*/
{
	if(m_fpBtnClickedCallback)
		m_fpBtnClickedCallback(this, m_pBtnClickedParam);
}
