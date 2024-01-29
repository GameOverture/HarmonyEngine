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
	return (m_uiAttribs & BTNATTRIB_HideHoverState) != 0 || HyEngine::Input().IsUsingTouchScreen();
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
		m_uiAttribs |= BTNATTRIB_IsHighlighted;
	else
		m_uiAttribs &= ~BTNATTRIB_IsHighlighted;

	SetBtnState(eOldState);
}

bool HyButton::IsMouseHover() const
{
	return (m_uiAttribs & BTNATTRIB_IsHoverState) != 0;
}

bool HyButton::IsDown() const
{
	return (m_uiAttribs & (BTNATTRIB_IsMouseDownState | BTNATTRIB_IsKbDownState)) != 0;
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

	if(m_uiAttribs & BTNATTRIB_IsMouseDownState && HyEngine::Input().IsMouseBtnDown(HYMOUSE_BtnLeft) == false)
	{
		HyButtonState eOldState = GetBtnState();
		m_uiAttribs &= ~BTNATTRIB_IsMouseDownState;
		SetBtnState(eOldState);
	}
}

/*virtual*/ void HyButton::OnTakeKeyboardFocus() /*override*/
{
	SetAsHighlighted(true);
}

/*virtual*/ void HyButton::OnRelinquishKeyboardFocus() /*override*/
{
	SetAsHighlighted(false);
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
	m_uiAttribs |= BTNATTRIB_IsMouseDownState;
	SetBtnState(eOldState);
}

/*virtual*/ void HyButton::OnUiMouseClicked() /*override*/
{
	if(m_fpBtnClickedCallback)
		m_fpBtnClickedCallback(this, m_pBtnClickedParam);

	if(m_ClickedSound.IsLoadDataValid() && m_ClickedSound.IsLoaded())
		m_ClickedSound.PlayOneShot(true);

	HyButtonState eCurState = GetBtnState();
	m_Panel.SetSpriteState(eCurState);
	OnBtnStateChange(eCurState);
}

/*virtual*/ void HyButton::OnUiKeyboardInput(HyKeyboardBtn eBtn, HyBtnPressState eBtnState, HyKeyboardModifer iMods) /*override*/
{
	if(eBtn == HYKEY_Space || eBtn == HYKEY_Enter)
	{
		if(eBtnState == HYBTN_Press && (m_uiAttribs & BTNATTRIB_IsKbDownState) == 0)
		{
			HyButtonState eOldState = GetBtnState();
			m_uiAttribs |= BTNATTRIB_IsKbDownState;
			SetBtnState(eOldState);
		}
		else if(eBtnState == HYBTN_Release && (m_uiAttribs & BTNATTRIB_IsKbDownState) != 0)
		{
			HyButtonState eOldState = GetBtnState();
			m_uiAttribs &= ~BTNATTRIB_IsKbDownState;
			SetBtnState(eOldState);

			if(IsDown() == false)
				InvokeButtonClicked();
		}
	}
}

/*virtual*/ void HyButton::OnSetup() /*override*/
{
	SetKeyboardFocusAllowed(true);
	SetAsHighlighted(IsHighlighted());
	SetHoverCursor(HYMOUSECURSOR_Hand);

	m_PanelColor = m_Panel.GetPanelColor();
	m_FrameColor = m_Panel.GetFrameColor();
}

HyButtonState HyButton::GetBtnState()
{
	if(IsEnabled())
	{
		if(IsDown() && IsHideDownState() == false)
		{
			if(IsHighlighted())
				return HYBUTTONSTATE_HighlightedDown;
			else
				return HYBUTTONSTATE_Down;
		}
		else if(IsMouseHover() && IsHideHoverState() == false)
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
		if(m_Panel.IsSprite())
		{
			uint32 uiNumStates = m_Panel.GetSprite().GetNumStates();
			if(static_cast<uint32>(eCurState) < uiNumStates)
				m_Panel.SetSpriteState(eCurState);
			else if(IsHighlighted() && uiNumStates > HYBUTTONSTATE_Highlighted)
				m_Panel.SetSpriteState(HYBUTTONSTATE_Highlighted);
			else
				m_Panel.SetSpriteState(HYBUTTONSTATE_Idle);
		}
		else
		{
			HyLog(eCurState);
			if(IsDown())
				m_Panel.SetPanelColor(m_PanelColor.Darken());
			else if(IsMouseHover())
				m_Panel.SetPanelColor(m_PanelColor.Lighten());
			else
				m_Panel.SetPanelColor(m_PanelColor);

			if(IsHighlighted())
				m_Panel.SetFrameColor(m_FrameColor.Lighten());
			else
				m_Panel.SetFrameColor(m_FrameColor);
		}

		OnBtnStateChange(eCurState);
	}
}
