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
	return (m_uiAttribs & UIATTRIB_CanKeyboardFocus);
}

void IHyWidget::SetKeyboardFocusAllowed(bool bEnabled)
{
	if(bEnabled)
		m_uiAttribs |= UIATTRIB_CanKeyboardFocus;
	else
		m_uiAttribs &= ~UIATTRIB_CanKeyboardFocus;
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
	if(IsHoverCursor() && IsEnabled())
		HyEngine::Input().SetMouseCursor(m_eHoverCursor);
}

/*virtual*/ void IHyWidget::OnMouseLeave() /*override*/
{
	if(IsHoverCursor())
		HyEngine::Input().ResetMouseCursor();
}

/*virtual*/ void IHyWidget::OnMouseClicked() /*override*/
{

}
