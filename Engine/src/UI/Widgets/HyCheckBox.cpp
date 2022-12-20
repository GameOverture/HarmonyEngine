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

HyCheckBox::HyCheckBox(const HyPanelInit &initRef, HyEntity2d *pParent /*= nullptr*/) :
	HyButton(initRef, pParent),
	m_CheckMarkStroke(this),
	m_CheckMarkFill(this)
{
	OnSetup();
}

HyCheckBox::HyCheckBox(const HyPanelInit &initRef, std::string sTextPrefix, std::string sTextName, HyEntity2d *pParent /*= nullptr*/) :
	HyButton(initRef, sTextPrefix, sTextName, pParent),
	m_CheckMarkStroke(this),
	m_CheckMarkFill(this)
{
	OnSetup();
}

HyCheckBox::HyCheckBox(const HyPanelInit &initRef, std::string sTextPrefix, std::string sTextName, int32 iTextDimensionsX, int32 iTextDimensionsY, int32 iTextOffsetX, int32 iTextOffsetY, HyEntity2d *pParent /*= nullptr*/) :
	HyButton(initRef, sTextPrefix, sTextName, iTextDimensionsX, iTextDimensionsY, iTextOffsetX, iTextOffsetY, pParent),
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
	m_CheckMarkStroke.pos.Offset(m_Panel.size.X() * 0.5f, m_Panel.size.Y() * 0.5f);
	m_CheckMarkStroke.SetTint(m_Panel.GetFrameColor().Lighten());

	m_CheckMarkFill.pos.Set(m_Panel.pos);
	m_CheckMarkFill.pos.Offset(m_Panel.size.X() * 0.5f, m_Panel.size.Y() * 0.5f);
	m_CheckMarkFill.SetTint(m_Panel.GetPanelColor().Lighten());
}

/*virtual*/ void HyCheckBox::OnUiMouseClicked() /*override*/
{
	SetChecked(!IsChecked());
}

void HyCheckBox::AssembleCheckmark()
{
	if(m_Panel.IsValid())
	{
		float fRadius = (HyMin(m_Panel.size.X(), m_Panel.size.Y()) - (m_Panel.GetFrameSize() * 4)) * 0.5f;
		m_CheckMarkStroke.SetAsCircle(fRadius);
		m_CheckMarkFill.SetAsCircle(fRadius - m_Panel.GetFrameSize());
	}

	ResetTextAndPanel();
}
