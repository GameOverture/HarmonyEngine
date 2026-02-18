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
	SetAsSideBySide();
}

HyCheckBox::HyCheckBox(const HyUiPanelInit &panelInit, HyEntity2d *pParent /*= nullptr*/) :
	HyButton(panelInit, pParent),
	m_CheckMarkStroke(this),
	m_CheckMarkFill(this)
{
	SetAsSideBySide();
}

HyCheckBox::HyCheckBox(const HyUiPanelInit &panelInit, const HyUiTextInit &textInit, HyEntity2d *pParent /*= nullptr*/) :
	HyButton(panelInit, textInit, pParent),
	m_CheckMarkStroke(this),
	m_CheckMarkFill(this)
{
	SetAsSideBySide();
}

/*virtual*/ HyCheckBox::~HyCheckBox()
{
}

void HyCheckBox::SetCheckedChangedCallback(std::function<void(HyCheckBox *)> fpCallback)
{
	m_fpOnCheckedChanged = fpCallback;
}

/*virtual*/ void HyCheckBox::OnSetup() /*override*/
{
	SetAsSideBySide();
}

/*virtual*/ void HyCheckBox::OnAssemble() /*override*/
{
	HyButton::OnAssemble();

	float fRadius = (HyMath::Min(panel.GetWidth(), panel.GetHeight()) - (panel.GetFrameStrokeSize() * 4)) * 0.5f;
	m_CheckMarkStroke.SetAsCircle(fRadius);
	m_CheckMarkFill.SetAsCircle(fRadius - panel.GetFrameStrokeSize());

	//m_CheckMarkStroke.pos.Set(panel.pos);
	m_CheckMarkStroke.pos.Offset(panel.GetWidth() * 0.5f, panel.GetHeight() * 0.5f);
	m_CheckMarkStroke.SetTint(panel.GetFrameColor().Lighten());

	//m_CheckMarkFill.pos.Set(panel.pos);
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

/*virtual*/ void HyCheckBox::OnUiMouseClicked() /*override*/
{
	HyButton::OnUiMouseClicked();
	SetChecked(!IsChecked());
}

/*virtual*/ void HyCheckBox::OnSetChecked(bool bChecked) /*override*/
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
