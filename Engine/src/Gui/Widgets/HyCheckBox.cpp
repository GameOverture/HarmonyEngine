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

	std::vector<glm::vec2> checkMarkVerts;
	checkMarkVerts.push_back(glm::vec2(panel.GetWidth(-0.4f), 0.0f));
	checkMarkVerts.push_back(glm::vec2(0.0f, panel.GetHeight(-0.4f)));
	checkMarkVerts.push_back(glm::vec2(panel.GetWidth(0.4f), panel.GetHeight(0.4f)));
	m_CheckMark.SetAsLineChain(0, checkMarkVerts, false, 4.0f);
	
	m_CheckMark.SetLayerColor(0, panel.GetTertiaryColor());
	m_CheckMark.pos.Set(panel.GetPanelNode()->pos);
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
