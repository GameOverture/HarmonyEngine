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

HyButton::HyButton(int32 iWidth, int32 iHeight, int32 iStroke, std::string sTextPrefix, std::string sTextName, HyEntity2d *pParent /*= nullptr*/) :
	HyLabel(iWidth, iHeight, iStroke, sTextPrefix, sTextName, pParent),
	m_fpBtnClickedCallback(nullptr),
	m_pBtnClickedParam(nullptr)
{
}

HyButton::HyButton(int32 iWidth, int32 iHeight, int32 iStroke, std::string sTextPrefix, std::string sTextName, int32 iTextMarginLeft, int32 iTextMarginBottom, int32 iTextMarginRight, int32 iTextMarginTop, HyEntity2d *pParent /*= nullptr*/) :
	HyLabel(iWidth, iHeight, iStroke, sTextPrefix, sTextName, iTextMarginLeft, iTextMarginBottom, iTextMarginRight, iTextMarginTop, pParent),
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

HyButton::HyButton(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName, int32 iTextMarginLeft, int32 iTextMarginBottom, int32 iTextMarginRight, int32 iTextMarginTop, HyEntity2d *pParent /*= nullptr*/) :
	HyLabel(sPanelPrefix, sPanelName, sTextPrefix, sTextName, iTextMarginLeft, iTextMarginBottom, iTextMarginRight, iTextMarginTop, pParent),
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
			SetBtnState(HYBUTTONSTATE_Idle);
		else if(m_SpritePanel.GetState() == HYBUTTONSTATE_HighlightedDown || m_SpritePanel.GetState() == HYBUTTONSTATE_HighlightedHover)
			SetBtnState(HYBUTTONSTATE_Highlighted);
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
		if(IsHighlighted() && m_SpritePanel.GetNumStates() > HYBUTTONSTATE_Highlighted)
			SetBtnState(HYBUTTONSTATE_Highlighted);
		break;
	case HYBUTTONSTATE_Down:
		if(IsHighlighted() && m_SpritePanel.GetNumStates() > HYBUTTONSTATE_HighlightedDown)
			SetBtnState(HYBUTTONSTATE_HighlightedDown);
		break;
	case HYBUTTONSTATE_Highlighted:
		if(IsHighlighted() == false)
			SetBtnState(HYBUTTONSTATE_Idle);
		break;
	case HYBUTTONSTATE_HighlightedDown:
		if(IsHighlighted() == false)
			SetBtnState(HYBUTTONSTATE_Down);
		break;
	case HYBUTTONSTATE_Hover:
		if(IsHighlighted() && m_SpritePanel.GetNumStates() > HYBUTTONSTATE_HighlightedHover)
			SetBtnState(HYBUTTONSTATE_HighlightedHover);
		break;
	case HYBUTTONSTATE_HighlightedHover:
		if(IsHighlighted() == false)
			SetBtnState(HYBUTTONSTATE_Hover);
		break;
	}
}

bool HyButton::IsHideDownState() const
{
	return (m_uiPanelAttribs & PANELATTRIB_HideDownState) != 0;
}

void HyButton::SetHideDownState(bool bIsHideDownState)
{
	if(bIsHideDownState)
	{
		m_uiPanelAttribs |= PANELATTRIB_HideDownState;

		if(m_SpritePanel.GetState() == HYBUTTONSTATE_Down)
			SetBtnState(HYBUTTONSTATE_Idle);
		else if(m_SpritePanel.GetState() == HYBUTTONSTATE_HighlightedDown)
			SetBtnState(HYBUTTONSTATE_Highlighted);
	}
	else
		m_uiPanelAttribs &= ~PANELATTRIB_HideDownState;
}

bool HyButton::IsHideHoverState() const
{
	return (m_uiPanelAttribs & PANELATTRIB_HideHoverState) != 0;
}

void HyButton::SetHideHoverState(bool bIsHideHoverState)
{
	if(bIsHideHoverState)
	{
		m_uiPanelAttribs |= PANELATTRIB_HideHoverState;

		if(m_SpritePanel.GetState() == HYBUTTONSTATE_Hover)
			SetBtnState(HYBUTTONSTATE_Idle);
		else if(m_SpritePanel.GetState() == HYBUTTONSTATE_HighlightedHover)
			SetBtnState(HYBUTTONSTATE_Highlighted);
	}
	else
		m_uiPanelAttribs &= ~PANELATTRIB_HideHoverState;
}

void HyButton::SetButtonClickedCallback(HyButtonClickedCallback fpCallBack, void *pParam /*= nullptr*/, std::string sAudioPrefix /*= ""*/, std::string sAudioName /*= ""*/)
{
	m_fpBtnClickedCallback = fpCallBack;
	m_pBtnClickedParam = pParam;
	m_ClickedSound.Init(sAudioPrefix, sAudioName, this);
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
			SetBtnState(HYBUTTONSTATE_Hover);
	}
	else
	{
		if(m_SpritePanel.GetState() == HYBUTTONSTATE_Highlighted && m_SpritePanel.GetNumStates() > HYBUTTONSTATE_HighlightedHover)
			SetBtnState(HYBUTTONSTATE_HighlightedHover);
	}
}

/*virtual*/ void HyButton::OnMouseLeave() /*override*/
{
	if(m_SpritePanel.IsLoadDataValid() == false)
		return;

	if(IsHighlighted() == false)
	{
		if(m_SpritePanel.GetState() == HYBUTTONSTATE_Down || m_SpritePanel.GetState() == HYBUTTONSTATE_Hover)
			SetBtnState(HYBUTTONSTATE_Idle);
	}
	else
	{
		if(m_SpritePanel.GetState() == HYBUTTONSTATE_HighlightedDown || m_SpritePanel.GetState() == HYBUTTONSTATE_HighlightedHover)
			SetBtnState(HYBUTTONSTATE_Highlighted);
	}
}

/*virtual*/ void HyButton::OnMouseDown() /*override*/
{
	if(IsHideDownState() || m_SpritePanel.IsLoadDataValid() == false)
		return;

	if(IsHighlighted() == false)
	{
		if(m_SpritePanel.GetNumStates() > HYBUTTONSTATE_Down)
			SetBtnState(HYBUTTONSTATE_Down);
	}
	else
	{
		if(m_SpritePanel.GetNumStates() > HYBUTTONSTATE_HighlightedDown)
			SetBtnState(HYBUTTONSTATE_HighlightedDown);
	}
}

/*virtual*/ void HyButton::OnMouseClicked() /*override*/
{
	if(IsHighlighted())
	{
		if(m_SpritePanel.GetNumStates() > HYBUTTONSTATE_HighlightedHover)
			SetBtnState(HYBUTTONSTATE_HighlightedHover);
		else if(m_SpritePanel.GetNumStates() > HYBUTTONSTATE_Highlighted)
			SetBtnState(HYBUTTONSTATE_Highlighted);
		else
			SetBtnState(HYBUTTONSTATE_Idle);
	}
	else if(m_SpritePanel.GetNumStates() > HYBUTTONSTATE_Hover)
		SetBtnState(HYBUTTONSTATE_Hover);
	else
		SetBtnState(HYBUTTONSTATE_Idle);

	if(m_fpBtnClickedCallback)
		m_fpBtnClickedCallback(this, m_pBtnClickedParam);

	m_ClickedSound.PlayOneShot(true);
}

void HyButton::SetBtnState(HyButtonState eState)
{
	m_SpritePanel.SetState(eState);
	OnBtnStateChange(eState);
}
