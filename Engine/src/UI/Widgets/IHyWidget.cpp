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
	m_eHoverCursor(HYMOUSECURSOR_Default)
{
}

/*virtual*/ IHyWidget::~IHyWidget()
{
}

bool IHyWidget::IsButton() const
{
	return (m_uiEntityAttribs & WIDGETATTRIB_IsTypeButton) != 0;
}

bool IHyWidget::IsInputAllowed() const
{
	if(m_pParent && (m_pParent->GetInternalFlags() & NODETYPE_IsLayout) != 0)
		return IsEnabled() && static_cast<HyLayout *>(m_pParent)->IsWidgetInputAllowed();

	return IsEnabled();
}

bool IHyWidget::IsEnabled() const
{
	return (m_uiEntityAttribs & WIDGETATTRIB_IsDisabled) == 0;
}

bool IHyWidget::IsHideDisabled() const
{
	return (m_uiEntityAttribs & WIDGETATTRIB_HideDisabled) != 0;
}

void IHyWidget::SetHideDisabled(bool bIsHideDisabled)
{
	if(bIsHideDisabled)
	{
		m_uiEntityAttribs |= WIDGETATTRIB_HideDisabled;
		SetTint(HyColor::White);
	}
	else
	{
		m_uiEntityAttribs &= ~WIDGETATTRIB_HideDisabled;
		SetEnabled(IsEnabled());
	}
}

void IHyWidget::SetEnabled(bool bEnabled)
{
	if(bEnabled)
	{
		m_uiEntityAttribs &= ~WIDGETATTRIB_IsDisabled;
		topColor.Tween(1.0f, 1.0f, 1.0f, 0.25f);
		botColor.Tween(1.0f, 1.0f, 1.0f, 0.25f);

		EnableMouseInput();
	}
	else
	{
		m_uiEntityAttribs |= WIDGETATTRIB_IsDisabled;

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
	return (m_uiEntityAttribs & WIDGETATTRIB_IsKeyboardFocus);
}

bool IHyWidget::IsKeyboardFocusAllowed() const
{
	return (m_uiEntityAttribs & WIDGETATTRIB_KeyboardFocusAllowed);
}

void IHyWidget::SetKeyboardFocusAllowed(bool bEnabled)
{
	if(bEnabled)
		m_uiEntityAttribs |= WIDGETATTRIB_KeyboardFocusAllowed;
	else
		m_uiEntityAttribs &= ~WIDGETATTRIB_KeyboardFocusAllowed;
}

bool IHyWidget::RequestKeyboardFocus()
{
	if(IsKeyboardFocusAllowed() && m_pParent && (m_pParent->GetInternalFlags() & NODETYPE_IsLayout) != 0)
		return static_cast<HyLayout *>(m_pParent)->RequestWidgetFocus(this);

	return false;
}

bool IHyWidget::IsHideDownState() const
{
	return (m_uiEntityAttribs & WIDGETATTRIB_HideDownState) != 0;
}

void IHyWidget::SetHideDownState(bool bIsHideDownState)
{
	if(bIsHideDownState)
		m_uiEntityAttribs |= WIDGETATTRIB_HideDownState;
	else
		m_uiEntityAttribs &= ~WIDGETATTRIB_HideDownState;

	ApplyPanelState();
}

bool IHyWidget::IsHideHoverState() const
{
	return (m_uiEntityAttribs & WIDGETATTRIB_HideMouseHoverState) != 0 || HyEngine::Input().IsUsingTouchScreen();
}

void IHyWidget::SetHideHoverState(bool bIsHideHoverState)
{
	if(bIsHideHoverState)
		m_uiEntityAttribs |= WIDGETATTRIB_HideMouseHoverState;
	else
		m_uiEntityAttribs &= ~WIDGETATTRIB_HideMouseHoverState;

	ApplyPanelState();
}

bool IHyWidget::IsHoverCursorSet() const
{
	return m_eHoverCursor != HYMOUSECURSOR_Default;
}

void IHyWidget::SetHoverCursor(HyMouseCursor eMouseCursor)
{
	m_eHoverCursor = eMouseCursor;
}

bool IHyWidget::IsHighlighted() const
{
	return (m_uiEntityAttribs & (WIDGETATTRIB_IsHighlighted | WIDGETATTRIB_IsKeyboardFocus)) != 0;
}

void IHyWidget::SetHighlighted(bool bIsHighlighted)
{
	if(bIsHighlighted)
		m_uiEntityAttribs |= WIDGETATTRIB_IsHighlighted;
	else
		m_uiEntityAttribs &= ~WIDGETATTRIB_IsHighlighted;

	ApplyPanelState();
}

bool IHyWidget::IsHideHighlighted() const
{
	return (m_uiEntityAttribs & WIDGETATTRIB_HideHighlightedState) != 0;
}

void IHyWidget::SetHideHighlighted(bool bIsHideHighlighted)
{
	if(bIsHideHighlighted)
		m_uiEntityAttribs |= WIDGETATTRIB_HideHighlightedState;
	else
		m_uiEntityAttribs &= ~WIDGETATTRIB_HideHighlightedState;

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
		if(IsHoverCursorSet())
			HyEngine::Input().SetMouseCursor(m_eHoverCursor);

		ApplyPanelState();
		OnUiMouseEnter();
	}
}

/*virtual*/ void IHyWidget::OnMouseLeave() /*override final*/
{
	if(IsInputAllowed())
	{
		if(IsHoverCursorSet())
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
	
	m_uiEntityAttribs |= WIDGETATTRIB_IsKeyboardFocus;
	OnTakeKeyboardFocus();

	ApplyPanelState();
}

void IHyWidget::RelinquishKeyboardFocus()
{
	m_uiEntityAttribs &= ~WIDGETATTRIB_IsKeyboardFocus;
	OnRelinquishKeyboardFocus();

	ApplyPanelState();
}

HyPanelState IHyWidget::CalcPanelState()
{
	if(IsEnabled())
	{
		if(IsDepressed() && IsHideDownState() == false)
		{
			if(IsHighlighted() && IsHideHighlighted() == false)
				return HYPANELSTATE_HighlightedDown;
			else
				return HYPANELSTATE_Down;
		}
		else if(IsMouseHover() && IsHideHoverState() == false)
		{
			if(IsHighlighted() && IsHideHighlighted() == false)
				return HYPANELSTATE_HighlightedHover;
			else
				return HYPANELSTATE_Hover;
		}
		else
		{
			if(IsHighlighted() && IsHideHighlighted() == false)
				return HYPANELSTATE_Highlighted;
			else
				return HYPANELSTATE_Idle;
		}
	}
	else // not enabled
	{
		if(IsHighlighted() && IsHideHighlighted() == false)
			return HYPANELSTATE_Highlighted;
		else
			return HYPANELSTATE_Idle;
	}
}

void IHyWidget::ApplyPanelState()
{
	
	//if(IsUsingPanelStates())
	//{
	//	HyPanelState eCurState = ;
	//	if(m_ePanelState != eCurState)
	//	{
	//		m_ePanelState = eCurState;
	//		SetState(m_ePanelState);
	//	}
	//}

	OnApplyWidgetState(CalcPanelState());
}
