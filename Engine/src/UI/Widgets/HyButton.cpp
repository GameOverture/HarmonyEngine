/**************************************************************************
*	HyButton.h
*
*	Harmony Engine
*	Copyright (c) 2021 Jason Knobler
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

HyButton::HyButton(const HyPanelInit &initRef, std::string sTextPrefix, std::string sTextName, HyEntity2d *pParent /*= nullptr*/) :
	HyLabel(initRef, sTextPrefix, sTextName, pParent),
	m_fpBtnClickedCallback(nullptr),
	m_pBtnClickedParam(nullptr)
{
	OnSetup();
}

HyButton::HyButton(const HyPanelInit &initRef, std::string sTextPrefix, std::string sTextName, int32 iTextMarginLeft, int32 iTextMarginBottom, int32 iTextMarginRight, int32 iTextMarginTop, HyEntity2d *pParent /*= nullptr*/) :
	HyLabel(initRef, sTextPrefix, sTextName, iTextMarginLeft, iTextMarginBottom, iTextMarginRight, iTextMarginTop, pParent),
	m_fpBtnClickedCallback(nullptr),
	m_pBtnClickedParam(nullptr)
{
	OnSetup();
}

/*virtual*/ HyButton::~HyButton()
{
}

/*virtual*/ void HyButton::SetAsEnabled(bool bEnabled) /*override*/
{
	HyLabel::SetAsEnabled(bEnabled);

	if(m_Panel.IsSprite() == false)
		return;
	
	if(IsEnabled() == false)
	{
		if(m_Panel.GetSpriteState() == HYBUTTONSTATE_Down || m_Panel.GetSpriteState() == HYBUTTONSTATE_Hover)
			SetBtnState(HYBUTTONSTATE_Idle);
		else if(m_Panel.GetSpriteState() == HYBUTTONSTATE_HighlightedDown || m_Panel.GetSpriteState() == HYBUTTONSTATE_HighlightedHover)
			SetBtnState(HYBUTTONSTATE_Highlighted);
	}
}

bool HyButton::IsHideDownState() const
{
	return (m_uiAttribs & BTNATTRIB_HideDownState) != 0;
}

void HyButton::SetHideDownState(bool bIsHideDownState)
{
	if(bIsHideDownState)
	{
		m_uiAttribs |= BTNATTRIB_HideDownState;

		if(m_Panel.GetSpriteState() == HYBUTTONSTATE_Down)
			SetBtnState(HYBUTTONSTATE_Idle);
		else if(m_Panel.GetSpriteState() == HYBUTTONSTATE_HighlightedDown)
			SetBtnState(HYBUTTONSTATE_Highlighted);
	}
	else
		m_uiAttribs &= ~BTNATTRIB_HideDownState;
}

bool HyButton::IsHideHoverState() const
{
	return (m_uiAttribs & BTNATTRIB_HideHoverState) != 0;
}

void HyButton::SetHideHoverState(bool bIsHideHoverState)
{
	if(bIsHideHoverState)
	{
		m_uiAttribs |= BTNATTRIB_HideHoverState;

		if(m_Panel.GetSpriteState() == HYBUTTONSTATE_Hover)
			SetBtnState(HYBUTTONSTATE_Idle);
		else if(m_Panel.GetSpriteState() == HYBUTTONSTATE_HighlightedHover)
			SetBtnState(HYBUTTONSTATE_Highlighted);
	}
	else
		m_uiAttribs &= ~BTNATTRIB_HideHoverState;
}

bool HyButton::IsHighlighted() const
{
	return (m_uiAttribs & BTNATTRIB_IsHighlighted) != 0;
}

void HyButton::SetAsHighlighted(bool bIsHighlighted)
{
	if(bIsHighlighted == IsHighlighted())
		return;

	if(bIsHighlighted)
	{
		m_uiAttribs |= BTNATTRIB_IsHighlighted;
		m_Panel.SetFrameColor(0x0000FF);
	}
	else
	{
		m_uiAttribs &= ~BTNATTRIB_IsHighlighted;
		m_Panel.SetFrameColor(0x3F3F41);
	}

	if(m_Panel.IsSprite() == false)
		return;

	switch(m_Panel.GetSpriteState())
	{
	case HYBUTTONSTATE_Idle:
		if(IsHighlighted() && m_Panel.GetSprite().GetNumStates() > HYBUTTONSTATE_Highlighted)
			SetBtnState(HYBUTTONSTATE_Highlighted);
		break;
	case HYBUTTONSTATE_Down:
		if(IsHighlighted() && m_Panel.GetSprite().GetNumStates() > HYBUTTONSTATE_HighlightedDown)
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
		if(IsHighlighted() && m_Panel.GetSprite().GetNumStates() > HYBUTTONSTATE_HighlightedHover)
			SetBtnState(HYBUTTONSTATE_HighlightedHover);
		break;
	case HYBUTTONSTATE_HighlightedHover:
		if(IsHighlighted() == false)
			SetBtnState(HYBUTTONSTATE_Hover);
		break;
	}
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

/*virtual*/ void HyButton::OnSetup() /*override*/
{
	SetAsHighlighted(IsHighlighted());
	SetHoverCursor(HYMOUSECURSOR_Hand);
}

/*virtual*/ void HyButton::OnUiMouseEnter() /*override*/
{
	if(IsHideHoverState() || m_Panel.IsSprite() == false)
		return;
	
	if(IsHighlighted() == false)
	{
		if(m_Panel.GetSpriteState() == HYBUTTONSTATE_Idle && m_Panel.GetSprite().GetNumStates() > HYBUTTONSTATE_Hover)
			SetBtnState(HYBUTTONSTATE_Hover);
	}
	else
	{
		if(m_Panel.GetSpriteState() == HYBUTTONSTATE_Highlighted && m_Panel.GetSprite().GetNumStates() > HYBUTTONSTATE_HighlightedHover)
			SetBtnState(HYBUTTONSTATE_HighlightedHover);
	}
}

/*virtual*/ void HyButton::OnUiMouseLeave() /*override*/
{
	if(m_Panel.IsSprite() == false)
		return;

	if(IsHighlighted() == false)
	{
		if(m_Panel.GetSpriteState() == HYBUTTONSTATE_Down || m_Panel.GetSpriteState() == HYBUTTONSTATE_Hover)
			SetBtnState(HYBUTTONSTATE_Idle);
	}
	else
	{
		if(m_Panel.GetSpriteState() == HYBUTTONSTATE_HighlightedDown || m_Panel.GetSpriteState() == HYBUTTONSTATE_HighlightedHover)
			SetBtnState(HYBUTTONSTATE_Highlighted);
	}
}

/*virtual*/ void HyButton::OnUiMouseDown() /*override*/
{
	if(IsHideDownState() || m_Panel.IsSprite() == false)
		return;

	if(IsHighlighted() == false)
	{
		if(m_Panel.GetSprite().GetNumStates() > HYBUTTONSTATE_Down)
			SetBtnState(HYBUTTONSTATE_Down);
	}
	else
	{
		if(m_Panel.GetSprite().GetNumStates() > HYBUTTONSTATE_HighlightedDown)
			SetBtnState(HYBUTTONSTATE_HighlightedDown);
	}
}

/*virtual*/ void HyButton::OnUiMouseClicked() /*override*/
{
	if(IsHighlighted())
	{
		if(m_Panel.GetSprite().GetNumStates() > HYBUTTONSTATE_HighlightedHover)
			SetBtnState(HYBUTTONSTATE_HighlightedHover);
		else if(m_Panel.GetSprite().GetNumStates() > HYBUTTONSTATE_Highlighted)
			SetBtnState(HYBUTTONSTATE_Highlighted);
		else
			SetBtnState(HYBUTTONSTATE_Idle);
	}
	else if(m_Panel.GetSprite().GetNumStates() > HYBUTTONSTATE_Hover)
		SetBtnState(HYBUTTONSTATE_Hover);
	else
		SetBtnState(HYBUTTONSTATE_Idle);

	if(m_fpBtnClickedCallback)
		m_fpBtnClickedCallback(this, m_pBtnClickedParam);

	m_ClickedSound.PlayOneShot(true);
}

void HyButton::SetBtnState(HyButtonState eState)
{
	m_Panel.SetSpriteState(eState);
	OnBtnStateChange(eState);
}
