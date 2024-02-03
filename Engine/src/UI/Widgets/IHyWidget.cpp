/**************************************************************************
*	IHyWidget.cpp
*
*	Harmony Engine
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "UI/Widgets/IHyWidget.h"
#include "UI/Components/HyLayout.h"
#include "HyEngine.h"

IHyWidget::IHyWidget(const HyPanelInit &initRef, HyEntity2d *pParent /*= nullptr*/) :
	IHyEntityUi(pParent),
	m_uiAttribs(0),
	m_eHoverCursor(HYMOUSECURSOR_Default),
	m_Panel(initRef, this)
{
}

/*virtual*/ IHyWidget::~IHyWidget()
{
}

bool IHyWidget::IsInputAllowed() const
{
	if(m_pParent && (m_pParent->GetInternalFlags() & NODETYPE_IsLayout) != 0)
		return IsEnabled() && static_cast<HyLayout *>(m_pParent)->IsWidgetInputAllowed();

	return IsEnabled();
}

bool IHyWidget::IsEnabled() const
{
	return (m_uiAttribs & UIATTRIB_IsDisabled) == 0;
}

bool IHyWidget::IsHideDisabled() const
{
	return (m_uiAttribs & UIATTRIB_HideDisabled) != 0;
}

void IHyWidget::SetHideDisabled(bool bIsHideDisabled)
{
	if(bIsHideDisabled)
	{
		m_uiAttribs |= UIATTRIB_HideDisabled;
		SetTint(HyColor::White);
	}
	else
	{
		m_uiAttribs &= ~UIATTRIB_HideDisabled;
		SetAsEnabled(IsEnabled());
	}
}

void IHyWidget::SetAsEnabled(bool bEnabled)
{
	HyPanelState eOldState = GetPanelState();

	if(bEnabled)
	{
		m_uiAttribs &= ~UIATTRIB_IsDisabled;
		topColor.Tween(1.0f, 1.0f, 1.0f, 0.25f);
		botColor.Tween(1.0f, 1.0f, 1.0f, 0.25f);

		EnableMouseInput();
		if(IsMouseInBounds())
			OnMouseEnter();
	}
	else
	{
		m_uiAttribs |= UIATTRIB_IsDisabled;

		if(IsHideDisabled() == false)
		{
			topColor.Tween(0.3f, 0.3f, 0.3f, 0.25f);
			botColor.Tween(0.3f, 0.3f, 0.3f, 0.25f);
		}

		DisableMouseInput();
		if(IsMouseInBounds())
			OnMouseLeave();
	}

	ApplyPanelState(eOldState);
}

bool IHyWidget::IsKeyboardFocus() const
{
	return (m_uiAttribs & UIATTRIB_IsKeyboardFocus);
}

bool IHyWidget::IsKeyboardFocusAllowed() const
{
	return (m_uiAttribs & UIATTRIB_KeyboardFocusAllowed);
}

void IHyWidget::SetKeyboardFocusAllowed(bool bEnabled)
{
	if(bEnabled)
		m_uiAttribs |= UIATTRIB_KeyboardFocusAllowed;
	else
		m_uiAttribs &= ~UIATTRIB_KeyboardFocusAllowed;
}

bool IHyWidget::RequestKeyboardFocus()
{
	if(IsKeyboardFocusAllowed() && m_pParent && (m_pParent->GetInternalFlags() & NODETYPE_IsLayout) != 0)
		return static_cast<HyLayout *>(m_pParent)->RequestWidgetFocus(this);

	return false;
}

/*virtual*/ bool IHyWidget::IsDown() const
{
	return (m_uiAttribs & UIATTRIB_IsMouseDownState) != 0;
}

bool IHyWidget::IsHideDownState() const
{
	return (m_uiAttribs & UIATTRIB_HideDownState) != 0;
}

void IHyWidget::SetHideDownState(bool bIsHideDownState)
{
	HyPanelState eOldState = GetPanelState();

	if(bIsHideDownState)
		m_uiAttribs |= UIATTRIB_HideDownState;
	else
		m_uiAttribs &= ~UIATTRIB_HideDownState;

	ApplyPanelState(eOldState);
}

bool IHyWidget::IsMouseHover() const
{
	return (m_uiAttribs & UIATTRIB_IsMouseHoverState) != 0;
}

bool IHyWidget::IsHideMouseHoverState() const
{
	return (m_uiAttribs & UIATTRIB_HideMouseHoverState) != 0 || HyEngine::Input().IsUsingTouchScreen();
}

void IHyWidget::SetHideMouseHoverState(bool bIsHideHoverState)
{
	HyPanelState eOldState = GetPanelState();

	if(bIsHideHoverState)
		m_uiAttribs |= UIATTRIB_HideMouseHoverState;
	else
		m_uiAttribs &= ~UIATTRIB_HideMouseHoverState;

	ApplyPanelState(eOldState);
}

bool IHyWidget::IsMouseHoverCursorSet() const
{
	return m_eHoverCursor != HYMOUSECURSOR_Default;
}

void IHyWidget::SetMouseHoverCursor(HyMouseCursor eMouseCursor)
{
	m_eHoverCursor = eMouseCursor;
}

bool IHyWidget::IsHighlighted() const
{
	return (m_uiAttribs & (UIATTRIB_IsHighlighted | UIATTRIB_IsKeyboardFocus)) != 0;
}

bool IHyWidget::IsHideHighlightedState() const
{
	return (m_uiAttribs & UIATTRIB_HideHighlightedState) != 0;
}

void IHyWidget::SetHideHighlightedState(bool bIsHideHighlightedState)
{
	HyPanelState eOldState = GetPanelState();

	if(bIsHideHighlightedState)
		m_uiAttribs |= UIATTRIB_HideHighlightedState;
	else
		m_uiAttribs &= ~UIATTRIB_HideHighlightedState;

	ApplyPanelState(eOldState);
}

void IHyWidget::SetAsHighlighted(bool bIsHighlighted)
{
	HyPanelState eOldState = GetPanelState();

	if(bIsHighlighted)
		m_uiAttribs |= UIATTRIB_IsHighlighted;
	else
		m_uiAttribs &= ~UIATTRIB_IsHighlighted;

	ApplyPanelState(eOldState);
}

/*virtual*/ void IHyWidget::OnUpdate() /*override final*/
{
	if(m_uiAttribs & UIATTRIB_IsMouseDownState && HyEngine::Input().IsMouseBtnDown(HYMOUSE_BtnLeft) == false)
	{
		HyPanelState eOldState = GetPanelState();
		m_uiAttribs &= ~UIATTRIB_IsMouseDownState;
		ApplyPanelState(eOldState);
	}

	OnUiUpdate();
}

/*virtual*/ void IHyWidget::OnMouseEnter() /*override final*/
{
	if(IsInputAllowed())
	{
		if(IsMouseHoverCursorSet())
			HyEngine::Input().SetMouseCursor(m_eHoverCursor);

		HyPanelState eOldState = GetPanelState();
		m_uiAttribs |= UIATTRIB_IsMouseHoverState;
		ApplyPanelState(eOldState);

		OnUiMouseEnter();
	}
}

/*virtual*/ void IHyWidget::OnMouseLeave() /*override final*/
{
	if(IsInputAllowed())
	{
		if(IsMouseHoverCursorSet())
			HyEngine::Input().ResetMouseCursor();

		HyPanelState eOldState = GetPanelState();
		m_uiAttribs &= ~UIATTRIB_IsMouseHoverState;
		ApplyPanelState(eOldState);

		OnUiMouseLeave();
	}
}

/*virtual*/ void IHyWidget::OnMouseDown() /*override final*/
{
	if(IsInputAllowed())
	{
		HyPanelState eOldState = GetPanelState();
		m_uiAttribs |= UIATTRIB_IsMouseDownState;
		ApplyPanelState(eOldState);

		OnUiMouseDown();
	}
}

/*virtual*/ void IHyWidget::OnMouseClicked() /*override final*/
{
	if(IsInputAllowed())
	{
		HyPanelState eOldState = GetPanelState();
		m_uiAttribs &= ~UIATTRIB_IsMouseDownState;
		ApplyPanelState(eOldState);

		//HyBState eCurState = GetBtnState();
		//m_Panel.SetSpriteState(eCurState);
		//OnBtnStateChange(eCurState);

		OnUiMouseClicked();
	}
}

void IHyWidget::TakeKeyboardFocus()
{
	HyAssert(IsKeyboardFocusAllowed(), "IHyWidget::TakeKeyboardFocus was invoked when keyboard focus is NOT allowed");
	
	m_uiAttribs |= UIATTRIB_IsKeyboardFocus;
	OnTakeKeyboardFocus();
}

void IHyWidget::RelinquishKeyboardFocus()
{
	m_uiAttribs &= ~UIATTRIB_IsKeyboardFocus;
	OnRelinquishKeyboardFocus();
}


HyPanelState IHyWidget::GetPanelState() const
{
	if(IsEnabled())
	{
		if(IsMouseDown() && IsHideMouseDownState() == false)
		{
			if(IsHighlighted() && IsHideHighlightedState() == false)
				return HYPANELSTATE_HighlightedDown;
			else
				return HYPANELSTATE_Down;
		}
		else if(IsMouseHover() && IsHideMouseHoverState() == false)
		{
			if(IsHighlighted() && IsHideHighlightedState() == false)
				return HYPANELSTATE_HighlightedHover;
			else
				return HYPANELSTATE_Hover;
		}
		else
		{
			if(IsHighlighted() && IsHideHighlightedState() == false)
				return HYPANELSTATE_Highlighted;
			else
				return HYPANELSTATE_Idle;
		}
	}
	else // not enabled
	{
		if(IsHighlighted() && IsHideHighlightedState() == false)
			return HYPANELSTATE_Highlighted;
		else
			return HYPANELSTATE_Idle;
	}
}

void IHyWidget::ApplyPanelState(HyPanelState eOldState)
{
	HyPanelState eCurState = GetPanelState();
	if(eOldState != eCurState)
	{
		m_Panel.ApplyPanelState(eCurState);
		OnPanelUpdated();
	}
}
