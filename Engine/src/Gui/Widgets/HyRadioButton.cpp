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
#include "Gui/Widgets/HyRadioButton.h"
#include "Diagnostics/Console/IHyConsole.h"

HyRadioButton::HyRadioButton(HyEntity2d *pParent /*= nullptr*/) :
	HyButton(pParent),
	m_CheckMarkStroke(this),
	m_CheckMarkFill(this),
	m_fpOnCheckedChanged(nullptr)
{
	m_uiEntityAttribs |= BTNATTRIB_IsAutoExclusive;
	SetAsSideBySide();
}

HyRadioButton::HyRadioButton(const HyUiPanelInit &panelInit, HyEntity2d *pParent /*= nullptr*/) :
	HyButton(panelInit, pParent),
	m_CheckMarkStroke(this),
	m_CheckMarkFill(this),
	m_fpOnCheckedChanged(nullptr)
{
	m_uiEntityAttribs |= BTNATTRIB_IsAutoExclusive;
	SetAsSideBySide();
}

HyRadioButton::HyRadioButton(const HyUiPanelInit &panelInit, const HyUiTextInit &textInit, HyEntity2d *pParent /*= nullptr*/) :
	HyButton(panelInit, textInit, pParent),
	m_CheckMarkStroke(this),
	m_CheckMarkFill(this),
	m_fpOnCheckedChanged(nullptr)
{
	m_uiEntityAttribs |= BTNATTRIB_IsAutoExclusive;
	SetAsSideBySide();
}

/*virtual*/ HyRadioButton::~HyRadioButton()
{
}

void HyRadioButton::SetCheckedChangedCallback(std::function<void(HyRadioButton *)> fpCallback)
{
	m_fpOnCheckedChanged = fpCallback;
}

/*virtual*/ void HyRadioButton::OnSetup() /*override*/
{
	SetAsSideBySide();
}

/*virtual*/ void HyRadioButton::OnAssemble() /*override*/
{
	HyButton::OnAssemble();

	float fRadius = (HyMath::Min(panel.GetWidth(), panel.GetHeight()) - (panel.GetFrameStrokeSize() * 4)) * 0.5f;
	m_CheckMarkStroke.SetAsCircle(fRadius);
	m_CheckMarkFill.SetAsCircle(fRadius - panel.GetFrameStrokeSize());

	m_CheckMarkStroke.pos.Offset(panel.GetWidth() * 0.5f, panel.GetHeight() * 0.5f);
	m_CheckMarkStroke.SetTint(panel.GetFrameColor().Lighten());

	m_CheckMarkFill.pos.Offset(panel.GetWidth() * 0.5f, panel.GetHeight() * 0.5f);
	m_CheckMarkFill.SetTint(panel.GetPanelColor().Lighten());

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
}

/*virtual*/ void HyRadioButton::OnUiMouseClicked() /*override*/
{
	HyButton::OnUiMouseClicked();
	SetChecked(!IsChecked());
}

/*virtual*/ void HyRadioButton::OnSetChecked(bool bChecked) /*override*/
{
	if(m_fpOnCheckedChanged)
		m_fpOnCheckedChanged(this);

	if(bChecked)
	{
		m_CheckMarkStroke.alpha.Set(1.0f);
		m_CheckMarkFill.alpha.Set(1.0f);
	}
	else
	{
		m_CheckMarkStroke.alpha.Set(0.0f);
		m_CheckMarkFill.alpha.Set(0.0f);
	}
}
