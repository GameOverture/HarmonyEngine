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
#include "HyEngine.h"

HyButton::HyButton(HyEntity2d *pParent /*= nullptr*/) :
	HyLabel(pParent),
	m_fpBtnClickedCallback(nullptr),
	m_pBtnClickedParam(nullptr)
{
}

HyButton::HyButton(const HyPanelInit &initRef, HyEntity2d *pParent /*= nullptr*/) :
	HyLabel(initRef, pParent),
	m_fpBtnClickedCallback(nullptr),
	m_pBtnClickedParam(nullptr)
{
	OnSetup();
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
	HyButtonState eOldState = GetBtnState();

	HyLabel::SetAsEnabled(bEnabled);
	
	SetBtnState(eOldState);
}

bool HyButton::IsHideDownState() const
{
	return (m_uiAttribs & BTNATTRIB_HideDownState) != 0;
}

void HyButton::SetHideDownState(bool bIsHideDownState)
{
	HyButtonState eOldState = GetBtnState();

	if(bIsHideDownState)
		m_uiAttribs |= BTNATTRIB_HideDownState;
	else
		m_uiAttribs &= ~BTNATTRIB_HideDownState;

	SetBtnState(eOldState);
}

bool HyButton::IsHideHoverState() const
{
	return (m_uiAttribs & BTNATTRIB_HideHoverState) != 0 && HyEngine::Input().IsUsingTouchScreen();
}

void HyButton::SetHideHoverState(bool bIsHideHoverState)
{
	HyButtonState eOldState = GetBtnState();

	if(bIsHideHoverState)
		m_uiAttribs |= BTNATTRIB_HideHoverState;
	else
		m_uiAttribs &= ~BTNATTRIB_HideHoverState;

	SetBtnState(eOldState);
}

bool HyButton::IsHighlighted() const
{
	return (m_uiAttribs & BTNATTRIB_IsHighlighted) != 0;
}

void HyButton::SetAsHighlighted(bool bIsHighlighted)
{
	HyButtonState eOldState = GetBtnState();

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

	SetBtnState(eOldState);
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

/*virtual*/ void HyButton::OnUpdate() /*override*/
{
	HyLabel::OnUpdate();

	if(m_uiAttribs & BTNATTRIB_IsDownState && HyEngine::Input().IsMouseBtnDown(HYMOUSE_BtnLeft))
		m_uiAttribs &= ~BTNATTRIB_IsDownState;
}

/*virtual*/ void HyButton::OnSetup() /*override*/
{
	SetAsHighlighted(IsHighlighted());
	SetHoverCursor(HYMOUSECURSOR_Hand);
}

/*virtual*/ void HyButton::OnUiMouseEnter() /*override*/
{
	HyButtonState eOldState = GetBtnState();

	m_uiAttribs |= BTNATTRIB_IsHoverState;

	SetBtnState(eOldState);
}

/*virtual*/ void HyButton::OnUiMouseLeave() /*override*/
{
	HyButtonState eOldState = GetBtnState();

	m_uiAttribs &= ~BTNATTRIB_IsHoverState;

	SetBtnState(eOldState);
}

/*virtual*/ void HyButton::OnUiMouseDown() /*override*/
{
	HyButtonState eOldState = GetBtnState();

	m_uiAttribs |= BTNATTRIB_IsDownState;

	SetBtnState(eOldState);
}

/*virtual*/ void HyButton::OnUiMouseClicked() /*override*/
{
	if(m_fpBtnClickedCallback)
		m_fpBtnClickedCallback(this, m_pBtnClickedParam);
	m_ClickedSound.PlayOneShot(true);

	HyButtonState eCurState = GetBtnState();
	m_Panel.SetSpriteState(eCurState);
	OnBtnStateChange(eCurState);
}

HyButtonState HyButton::GetBtnState()
{
	if(IsEnabled())
	{
		if(m_uiAttribs & BTNATTRIB_IsDownState && IsHideDownState() == false)
		{
			if(IsHighlighted())
				return HYBUTTONSTATE_HighlightedDown;
			else
				return HYBUTTONSTATE_Down;
		}
		else if(m_uiAttribs & BTNATTRIB_IsHoverState && IsHideHoverState() == false)
		{
			if(IsHighlighted())
				return HYBUTTONSTATE_HighlightedHover;
			else
				return HYBUTTONSTATE_Hover;
		}
		else
		{
			if(IsHighlighted())
				return HYBUTTONSTATE_Highlighted;
			else
				return HYBUTTONSTATE_Idle;
		}
	}
	else // not enabled
	{
		if(IsHighlighted())
			return HYBUTTONSTATE_Highlighted;
		else
			return HYBUTTONSTATE_Idle;
	}
}

void HyButton::SetBtnState(HyButtonState eOldState)
{
	HyButtonState eCurState = GetBtnState();
	if(eOldState != eCurState)
	{
		m_Panel.SetSpriteState(eCurState);
		OnBtnStateChange(eCurState);
	}
}
