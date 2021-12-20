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
#include "UI/Containers/Components/HyLayout.h"
#include "HyEngine.h"

IHyWidget::IHyWidget(HyEntity2d *pParent /*= nullptr*/) :
	IHyEntityUi(pParent),
	m_uiAttribs(0),
	m_eHoverCursor(HYMOUSECURSOR_Default)
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
		SetTint(1.0f, 1.0f, 1.0f);
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

/*virtual*/ void IHyWidget::OnMouseEnter() /*override*/
{
	if(IsInputAllowed() && IsHoverCursor())
		HyEngine::Input().SetMouseCursor(m_eHoverCursor);
}

/*virtual*/ void IHyWidget::OnMouseLeave() /*override*/
{
	if(IsInputAllowed() && IsHoverCursor())
		HyEngine::Input().ResetMouseCursor();
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
