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

IHyWidget::IHyWidget(HyEntity2d *pParent /*= nullptr*/) :
	IHyEntityUi(pParent),
	m_eHoverCursor(HYMOUSECURSOR_Default),
	m_ePanelState(HYPANELSTATE_NotUsed),
	m_Panel(this)
{
}

/*virtual*/ IHyWidget::~IHyWidget()
{
}

bool IHyWidget::IsButton() const
{
	return (m_uiAttribs & WIDGETATTRIB_IsTypeButton) != 0;
}

/*virtual*/ bool IHyWidget::SetState(uint32 uiStateIndex) /*override*/
{
	if(IHyLoadable::SetState(uiStateIndex) == false || m_Panel.SetState(uiStateIndex) == false)
		return false;
	
	m_ePanelState = HYPANELSTATE_NotUsed;
	OnPanelUpdated();

	return true;
}

/*virtual*/ uint32 IHyWidget::GetNumStates() /*override*/
{
	return m_Panel.GetNumStates();
}

HyPanelState IHyWidget::GetPanelState() const
{
	return m_ePanelState;
}

bool IHyWidget::IsUsingPanelStates() const
{
	return m_ePanelState != HYPANELSTATE_NotUsed;
}

void IHyWidget::UsePanelStates()
{
	m_ePanelState = HYPANELSTATE_Idle;

	if(m_Panel.SetState(m_ePanelState))
	{
		OnPanelUpdated();
		SetAssembleNeeded();
	}
}

bool IHyWidget::IsPanelVisible() const
{
	return m_Panel.IsVisible();
}

void IHyWidget::SetPanelVisible(bool bVisible)
{
	m_Panel.SetVisible(bVisible);
}

bool IHyWidget::IsPanelBoundingVolume() const
{
	return m_Panel.IsBoundingVolume();
}

bool IHyWidget::IsPanelNode() const
{
	return m_Panel.IsNode();
}

IHyBody2d *IHyWidget::GetPanelNode()
{
	return m_Panel.GetNode();
}

bool IHyWidget::IsInputAllowed() const
{
	if(m_pParent && (m_pParent->GetInternalFlags() & NODETYPE_IsLayout) != 0)
		return IsEnabled() && static_cast<HyLayout *>(m_pParent)->IsWidgetInputAllowed();

	return IsEnabled();
}

bool IHyWidget::IsEnabled() const
{
	return (m_uiAttribs & WIDGETATTRIB_IsDisabled) == 0;
}

bool IHyWidget::IsHideDisabled() const
{
	return (m_uiAttribs & WIDGETATTRIB_HideDisabled) != 0;
}

void IHyWidget::SetHideDisabled(bool bIsHideDisabled)
{
	if(bIsHideDisabled)
	{
		m_uiAttribs |= WIDGETATTRIB_HideDisabled;
		SetTint(HyColor::White);
	}
	else
	{
		m_uiAttribs &= ~WIDGETATTRIB_HideDisabled;
		SetAsEnabled(IsEnabled());
	}
}

void IHyWidget::SetAsEnabled(bool bEnabled)
{
	if(bEnabled)
	{
		m_uiAttribs &= ~WIDGETATTRIB_IsDisabled;
		topColor.Tween(1.0f, 1.0f, 1.0f, 0.25f);
		botColor.Tween(1.0f, 1.0f, 1.0f, 0.25f);

		EnableMouseInput();
	}
	else
	{
		m_uiAttribs |= WIDGETATTRIB_IsDisabled;

		if(IsHideDisabled() == false)
		{
			topColor.Tween(0.3f, 0.3f, 0.3f, 0.25f);
			botColor.Tween(0.3f, 0.3f, 0.3f, 0.25f);
		}

		DisableMouseInput();
	}

	ApplyPanelState();
}

bool IHyWidget::IsKeyboardFocus() const
{
	return (m_uiAttribs & WIDGETATTRIB_IsKeyboardFocus);
}

bool IHyWidget::IsKeyboardFocusAllowed() const
{
	return (m_uiAttribs & WIDGETATTRIB_KeyboardFocusAllowed);
}

void IHyWidget::SetKeyboardFocusAllowed(bool bEnabled)
{
	if(bEnabled)
		m_uiAttribs |= WIDGETATTRIB_KeyboardFocusAllowed;
	else
		m_uiAttribs &= ~WIDGETATTRIB_KeyboardFocusAllowed;
}

bool IHyWidget::RequestKeyboardFocus()
{
	if(IsKeyboardFocusAllowed() && m_pParent && (m_pParent->GetInternalFlags() & NODETYPE_IsLayout) != 0)
		return static_cast<HyLayout *>(m_pParent)->RequestWidgetFocus(this);

	return false;
}

bool IHyWidget::IsHideDownState() const
{
	return (m_uiAttribs & WIDGETATTRIB_HideDownState) != 0;
}

void IHyWidget::SetHideDownState(bool bIsHideDownState)
{
	if(bIsHideDownState)
		m_uiAttribs |= WIDGETATTRIB_HideDownState;
	else
		m_uiAttribs &= ~WIDGETATTRIB_HideDownState;

	ApplyPanelState();
}

bool IHyWidget::IsHideMouseHoverState() const
{
	return (m_uiAttribs & WIDGETATTRIB_HideMouseHoverState) != 0 || HyEngine::Input().IsUsingTouchScreen();
}

void IHyWidget::SetHideMouseHoverState(bool bIsHideHoverState)
{
	if(bIsHideHoverState)
		m_uiAttribs |= WIDGETATTRIB_HideMouseHoverState;
	else
		m_uiAttribs &= ~WIDGETATTRIB_HideMouseHoverState;

	ApplyPanelState();
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
	return (m_uiAttribs & (WIDGETATTRIB_IsHighlighted | WIDGETATTRIB_IsKeyboardFocus)) != 0;
}

bool IHyWidget::IsHideHighlightedState() const
{
	return (m_uiAttribs & WIDGETATTRIB_HideHighlightedState) != 0;
}

void IHyWidget::SetHideHighlightedState(bool bIsHideHighlightedState)
{
	if(bIsHideHighlightedState)
		m_uiAttribs |= WIDGETATTRIB_HideHighlightedState;
	else
		m_uiAttribs &= ~WIDGETATTRIB_HideHighlightedState;

	ApplyPanelState();
}

void IHyWidget::SetAsHighlighted(bool bIsHighlighted)
{
	if(bIsHighlighted)
		m_uiAttribs |= WIDGETATTRIB_IsHighlighted;
	else
		m_uiAttribs &= ~WIDGETATTRIB_IsHighlighted;

	ApplyPanelState();
}

/*virtual*/ bool IHyWidget::IsDepressed() const
{
	return IsMouseDown();
}

/*virtual*/ void IHyWidget::OnMouseEnter() /*override final*/
{
	if(IsInputAllowed())
	{
		if(IsMouseHoverCursorSet())
			HyEngine::Input().SetMouseCursor(m_eHoverCursor);

		ApplyPanelState();
		OnUiMouseEnter();
	}
}

/*virtual*/ void IHyWidget::OnMouseLeave() /*override final*/
{
	if(IsInputAllowed())
	{
		if(IsMouseHoverCursorSet())
			HyEngine::Input().ResetMouseCursor();

		ApplyPanelState();
		OnUiMouseLeave();
	}
}

/*virtual*/ void IHyWidget::OnMouseDown() /*override final*/
{
	if(IsInputAllowed())
	{
		ApplyPanelState();
		OnUiMouseDown();
	}
}

/*virtual*/ void IHyWidget::OnMouseUp() /*override final*/
{
	ApplyPanelState();
}

/*virtual*/ void IHyWidget::OnMouseClicked() /*override final*/
{
	if(IsInputAllowed())
	{
		ApplyPanelState();
		RequestKeyboardFocus();
		OnUiMouseClicked();
	}
}

void IHyWidget::TakeKeyboardFocus()
{
	HyAssert(IsKeyboardFocusAllowed(), "IHyWidget::TakeKeyboardFocus was invoked when keyboard focus is NOT allowed");
	
	m_uiAttribs |= WIDGETATTRIB_IsKeyboardFocus;
	OnTakeKeyboardFocus();

	ApplyPanelState();
}

void IHyWidget::RelinquishKeyboardFocus()
{
	m_uiAttribs &= ~WIDGETATTRIB_IsKeyboardFocus;
	OnRelinquishKeyboardFocus();

	ApplyPanelState();
}

HyPanelState IHyWidget::CalcPanelState()
{
	if(IsEnabled())
	{
		if(IsDepressed() && IsHideDownState() == false)
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

void IHyWidget::ApplyPanelState()
{
	if(IsUsingPanelStates())
	{
		HyPanelState eCurState = CalcPanelState();
		if(m_ePanelState != eCurState)
		{
			m_ePanelState = eCurState;
			m_Panel.SetState(m_ePanelState);
			OnPanelUpdated();
		}
	}
}
