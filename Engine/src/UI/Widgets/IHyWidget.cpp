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

/*virtual*/ void IHyWidget::SetAsEnabled(bool bEnabled)
{
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

bool IHyWidget::IsKeyboardFocus() const
{
	return (m_uiAttribs & UIATTRIB_IsKeyboardFocus);
}

bool IHyWidget::RequestKeyboardFocus()
{
	if(IsKeyboardFocusAllowed() && m_pParent && (m_pParent->GetInternalFlags() & NODETYPE_IsLayout) != 0)
		return static_cast<HyLayout *>(m_pParent)->RequestWidgetFocus(this);

	return false;
}

bool IHyWidget::IsHoverCursor() const
{
	return m_eHoverCursor != HYMOUSECURSOR_Default;
}

void IHyWidget::SetHoverCursor(HyMouseCursor eMouseCursor)
{
	m_eHoverCursor = eMouseCursor;
}

/*virtual*/ void IHyWidget::OnMouseEnter() /*override final*/
{
	if(IsInputAllowed())
	{
		if(IsHoverCursor())
			HyEngine::Input().SetMouseCursor(m_eHoverCursor);

		OnUiMouseEnter();
	}
}

/*virtual*/ void IHyWidget::OnMouseLeave() /*override final*/
{
	if(IsInputAllowed())
	{
		if(IsHoverCursor())
			HyEngine::Input().ResetMouseCursor();

		OnUiMouseLeave();
	}
}

/*virtual*/ void IHyWidget::OnMouseDown() /*override final*/
{
	if(IsInputAllowed())
		OnUiMouseDown();
}

/*virtual*/ void IHyWidget::OnMouseClicked() /*override final*/
{
	if(IsInputAllowed())
		OnUiMouseClicked();
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
	return m_Panel.GetPanelState();
}

void IHyWidget::SetPanelState(HyPanelState eOldState)
{
	HyPanelState eCurState = GetPanelState();
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
