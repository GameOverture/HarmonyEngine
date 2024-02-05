/**************************************************************************
*	HyCheckBox.h
*
*	Harmony Engine
*	Copyright (c) 2022 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "UI/Widgets/HyCheckBox.h"
#include "Diagnostics/Console/IHyConsole.h"

HyCheckBox::HyCheckBox(HyEntity2d *pParent /*= nullptr*/) :
	HyButton(pParent),
	m_CheckMarkStroke(this),
	m_CheckMarkFill(this)
{
}

HyCheckBox::HyCheckBox(const HyPanelInit &panelInit, HyEntity2d *pParent /*= nullptr*/) :
	HyButton(panelInit, pParent),
	m_CheckMarkStroke(this),
	m_CheckMarkFill(this)
{
	OnSetup();
}

HyCheckBox::HyCheckBox(const HyPanelInit &panelInit, const HyNodePath &textNodePath, HyEntity2d *pParent /*= nullptr*/) :
	HyButton(panelInit, textNodePath, pParent),
	m_CheckMarkStroke(this),
	m_CheckMarkFill(this)
{
	OnSetup();
}

HyCheckBox::HyCheckBox(const HyPanelInit &panelInit, const HyNodePath &textNodePath, int32 iTextDimensionsX, int32 iTextDimensionsY, int32 iTextOffsetX, int32 iTextOffsetY, HyEntity2d *pParent /*= nullptr*/) :
	HyButton(panelInit, textNodePath, iTextDimensionsX, iTextDimensionsY, iTextOffsetX, iTextOffsetY, pParent),
	m_CheckMarkStroke(this),
	m_CheckMarkFill(this)
{
	OnSetup();
}

/*virtual*/ HyCheckBox::~HyCheckBox()
{
}

bool HyCheckBox::IsChecked() const
{
	return m_uiAttribs & CHECKBOXATTRIB_IsChecked;
}

void HyCheckBox::SetChecked(bool bChecked)
{
	if(bChecked == IsChecked())
		return;

	if(bChecked)
		m_uiAttribs |= CHECKBOXATTRIB_IsChecked;
	else
		m_uiAttribs &= ~CHECKBOXATTRIB_IsChecked;

	ResetTextAndPanel();
	
	if(m_fpOnCheckedChanged)
		m_fpOnCheckedChanged(this, m_pCheckedChangedParam);
}

void HyCheckBox::SetCheckedChangedCallback(std::function<void(HyCheckBox *, void *)> fpCallback, void *pParam /*= nullptr*/)
{
	m_fpOnCheckedChanged = fpCallback;
	m_pCheckedChangedParam = pParam;
}

/*virtual*/ void HyCheckBox::OnSetup() /*override*/
{
	HyButton::OnSetup();
	SetAsSideBySide();

	AssembleCheckmark();
}

/*virtual*/ void HyCheckBox::ResetTextAndPanel() /*override*/
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

/*virtual*/ void HyCheckBox::OnUiMouseClicked() /*override*/
{
	HyButton::OnUiMouseClicked();
	SetChecked(!IsChecked());
}

void HyCheckBox::AssembleCheckmark()
{
	if(m_Panel.IsValid())
	{
		float fRadius = (HyMath::Min(m_Panel.GetWidth(m_Panel.scale.X()), m_Panel.GetHeight(m_Panel.scale.Y())) - (m_Panel.GetFrameStrokeSize() * 4)) * 0.5f;
		m_CheckMarkStroke.SetAsCircle(fRadius);
		m_CheckMarkFill.SetAsCircle(fRadius - m_Panel.GetFrameStrokeSize());
	}

	ResetTextAndPanel();
}
