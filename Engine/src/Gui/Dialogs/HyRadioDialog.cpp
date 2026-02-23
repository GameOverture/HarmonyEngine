/**************************************************************************
*	HyRadioDialog.cpp
*
*	Harmony Engine
*	Copyright (c) 2025 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Gui/Dialogs/HyRadioDialog.h"

HyRadioDialog::HyRadioDialog(std::string sTitle, std::vector<std::string> sRadioList, const HyUiPanelInit &panelInitRef, const HyUiPanelInit &buttonPanelInitRef, const HyUiTextInit &buttonTextInitRef, HyEntity2d *pParent /*= nullptr*/) :
	IHyDialog(sTitle, panelInitRef, buttonPanelInitRef, buttonTextInitRef, HYDLGFLAG_Ok | HYDLGFLAG_Cancel, pParent),
	m_ButtonPanelInit(buttonPanelInitRef),
	m_ButtonTextInitRef(buttonTextInitRef)
{
	SetRadioList(sRadioList);
}

/*virtual*/ HyRadioDialog::~HyRadioDialog()
{
}

void HyRadioDialog::SetRadioList(std::vector<std::string> sRadioList)
{
	// Clear out any existing radio buttons
	for(HyRadioButton *pRadioBtn : m_RadioList)
		delete pRadioBtn;
	m_RadioList.clear();

	ClearItems();

	// Create new radio buttons
	for(const std::string &sRadio : sRadioList)
	{
		HyRadioButton *pRadioButton = HY_NEW HyRadioButton(m_ButtonPanelInit, m_ButtonTextInitRef, this);
		pRadioButton->SetText(sRadio);
		m_RadioList.push_back(pRadioButton);
		InsertWidget(*pRadioButton);
	}

	InsertButtonList();
}

void HyRadioDialog::SetRadioList(std::vector<std::string> sRadioList, const HyUiPanelInit &buttonPanelInitRef, const HyUiTextInit &buttonTextInitRef)
{
	m_ButtonPanelInit = buttonPanelInitRef;
	m_ButtonTextInitRef = buttonTextInitRef;
	SetRadioList(sRadioList);
}
