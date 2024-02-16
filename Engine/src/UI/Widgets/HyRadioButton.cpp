/**************************************************************************
*	HyRadioButton.h
*
*	Harmony Engine
*	Copyright (c) 2024 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "UI/Widgets/HyRadioButton.h"
#include "Diagnostics/Console/IHyConsole.h"

HyRadioButton::HyRadioButton(HyEntity2d *pParent /*= nullptr*/) :
	HyButton(pParent),
	m_CheckMarkStroke(this),
	m_CheckMarkFill(this)
{
	m_uiAttribs &= ~WIDGETATTRIB_TypeMask;									// Clear the widget type bits in case derived class set the type
	m_uiAttribs |= (HYWIDGETTYPE_RadioButton << WIDGETATTRIB_TypeOffset);	// Set the widget type bits in the proper location
}

HyRadioButton::HyRadioButton(const HyPanelInit &panelInit, HyEntity2d *pParent /*= nullptr*/) :
	HyButton(panelInit, pParent),
	m_CheckMarkStroke(this),
	m_CheckMarkFill(this)
{
	m_uiAttribs &= ~WIDGETATTRIB_TypeMask;									// Clear the widget type bits in case derived class set the type
	m_uiAttribs |= (HYWIDGETTYPE_RadioButton << WIDGETATTRIB_TypeOffset);	// Set the widget type bits in the proper location

	OnSetup();
}

HyRadioButton::HyRadioButton(const HyPanelInit &panelInit, const HyNodePath &textNodePath, HyEntity2d *pParent /*= nullptr*/) :
	HyButton(panelInit, textNodePath, pParent),
	m_CheckMarkStroke(this),
	m_CheckMarkFill(this)
{
	m_uiAttribs &= ~WIDGETATTRIB_TypeMask;									// Clear the widget type bits in case derived class set the type
	m_uiAttribs |= (HYWIDGETTYPE_RadioButton << WIDGETATTRIB_TypeOffset);	// Set the widget type bits in the proper location

	OnSetup();
}

HyRadioButton::HyRadioButton(const HyPanelInit &panelInit, const HyNodePath &textNodePath, int32 iTextDimensionsX, int32 iTextDimensionsY, int32 iTextOffsetX, int32 iTextOffsetY, HyEntity2d *pParent /*= nullptr*/) :
	HyButton(panelInit, textNodePath, iTextDimensionsX, iTextDimensionsY, iTextOffsetX, iTextOffsetY, pParent),
	m_CheckMarkStroke(this),
	m_CheckMarkFill(this)
{
	m_uiAttribs &= ~WIDGETATTRIB_TypeMask;									// Clear the widget type bits in case derived class set the type
	m_uiAttribs |= (HYWIDGETTYPE_RadioButton << WIDGETATTRIB_TypeOffset);	// Set the widget type bits in the proper location

	OnSetup();
}

/*virtual*/ HyRadioButton::~HyRadioButton()
{
}

bool HyRadioButton::IsChecked() const
{
	return m_uiAttribs & RADIOBTNATTRIB_IsChecked;
}

void HyRadioButton::SetChecked(bool bChecked)
{
	if(bChecked == IsChecked())
		return;

	if(bChecked)
		m_uiAttribs |= RADIOBTNATTRIB_IsChecked;
	else
		m_uiAttribs &= ~RADIOBTNATTRIB_IsChecked;

	ResetTextAndPanel();
	
	if(m_fpOnCheckedChanged)
		m_fpOnCheckedChanged(this, m_pCheckedChangedParam);
}

void HyRadioButton::SetCheckedChangedCallback(std::function<void(HyRadioButton *, void *)> fpCallback, void *pParam /*= nullptr*/)
{
	m_fpOnCheckedChanged = fpCallback;
	m_pCheckedChangedParam = pParam;
}

/*virtual*/ void HyRadioButton::OnSetup() /*override*/
{
	HyButton::OnSetup();
	SetAsSideBySide();

	AssembleCheckmark();
}

/*virtual*/ void HyRadioButton::ResetTextAndPanel() /*override*/
{
	HyButton::ResetTextAndPanel();

	if(IsChecked())
	{
		m_CheckMarkStroke.alpha.Set(1.0f);
		m_CheckMarkFill.alpha.Set(1.0f);
	}
	else
	{
		m_CheckMarkStroke.alpha.Set(0.0f);
		m_CheckMarkFill.alpha.Set(0.0f);
	}

	m_CheckMarkStroke.pos.Set(m_Panel.pos);
	m_CheckMarkStroke.pos.Offset(m_Panel.GetWidth(m_Panel.scale.X()) * 0.5f, m_Panel.GetHeight(m_Panel.scale.Y()) * 0.5f);
	m_CheckMarkStroke.SetTint(m_Panel.GetFrameColor().Lighten());

	m_CheckMarkFill.pos.Set(m_Panel.pos);
	m_CheckMarkFill.pos.Offset(m_Panel.GetWidth(m_Panel.scale.X()) * 0.5f, m_Panel.GetHeight(m_Panel.scale.Y()) * 0.5f);
	m_CheckMarkFill.SetTint(m_Panel.GetPanelColor().Lighten());
}

/*virtual*/ void HyRadioButton::OnUiMouseClicked() /*override*/
{
	HyButton::OnUiMouseClicked();
	SetChecked(!IsChecked());
}

void HyRadioButton::AssembleCheckmark()
{
	if(m_Panel.IsValid())
	{
		float fRadius = (HyMath::Min(m_Panel.GetWidth(m_Panel.scale.X()), m_Panel.GetHeight(m_Panel.scale.Y())) - (m_Panel.GetFrameStrokeSize() * 4)) * 0.5f;
		m_CheckMarkStroke.SetAsCircle(fRadius);
		m_CheckMarkFill.SetAsCircle(fRadius - m_Panel.GetFrameStrokeSize());
	}

	ResetTextAndPanel();
}
