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
#include "Gui/Widgets/HyCheckBox.h"
#include "Diagnostics/Console/IHyConsole.h"

HyCheckBox::HyCheckBox(HyEntity2d *pParent /*= nullptr*/) :
	HyButton(pParent),
	m_CheckMark(this)
{
	SetAsSideBySide();
}

HyCheckBox::HyCheckBox(const HyUiPanelInit &panelInit, HyEntity2d *pParent /*= nullptr*/) :
	HyButton(panelInit, pParent),
	m_CheckMark(this)
{
	SetAsSideBySide();
}

HyCheckBox::HyCheckBox(const HyUiPanelInit &panelInit, const HyUiTextInit &textInit, HyEntity2d *pParent /*= nullptr*/) :
	HyButton(panelInit, textInit, pParent),
	m_CheckMark(this)
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
	m_CheckMark.SetAsCircle(0, fRadius);
	m_CheckMark.SetAsCircle(1, fRadius - panel.GetFrameStrokeSize());

	if(panel.GetPanelNode())
	{
		m_CheckMark.pos.Set(panel.GetPanelNode()->pos);
		m_CheckMark.pos.Offset(panel.GetWidth() * 0.5f, panel.GetHeight() * 0.5f);
	}
	m_CheckMark.SetLayerColor(0, panel.GetFrameColor().Lighten().Lighten());
	m_CheckMark.SetLayerColor(1, panel.GetFrameColor().Lighten());

	m_CheckMark.alpha.Set(IsChecked() ? 1.0f : 0.0f);
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

	m_CheckMark.alpha.Set(bChecked ? 1.0f : 0.0f);
}
