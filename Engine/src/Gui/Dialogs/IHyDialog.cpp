/**************************************************************************
*	IHyDialog.cpp
*
*	Harmony Engine
*	Copyright (c) 2025 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Gui/Dialogs/IHyDialog.h"

IHyDialog::IHyDialog(std::string sTitle, const HyUiPanelInit &panelInitRef, const HyUiPanelInit &buttonPanelInitRef, const HyUiTextInit &buttonTextInitRef, uint32_t uiButtonFlags /*= (HYDLGFLAG_Ok | HYDLGFLAG_Cancel)*/, HyEntity2d *pParent /*= nullptr*/) :
	HyGui(HYORIENT_Vertical, panelInitRef, pParent)
{
	HyButton *pNewBtn = nullptr;
	if(uiButtonFlags & HYDLGFLAG_Ok)
	{
		pNewBtn = HY_NEW HyButton(buttonPanelInitRef, buttonTextInitRef, this);
		pNewBtn->SetText("OK");
		m_ButtonList.push_back(pNewBtn);
	}
	if(uiButtonFlags & HYDLGFLAG_Yes)
	{
		pNewBtn = HY_NEW HyButton(buttonPanelInitRef, buttonTextInitRef, this);
		pNewBtn->SetText("Yes");
		m_ButtonList.push_back(pNewBtn);
	}
	if(uiButtonFlags & HYDLGFLAG_No)
	{
		pNewBtn = HY_NEW HyButton(buttonPanelInitRef, buttonTextInitRef, this);
		pNewBtn->SetText("No");
		m_ButtonList.push_back(pNewBtn);
	}
	if(uiButtonFlags & HYDLGFLAG_Apply)
	{
		pNewBtn = HY_NEW HyButton(buttonPanelInitRef, buttonTextInitRef, this);
		pNewBtn->SetText("Apply");
		m_ButtonList.push_back(pNewBtn);
	}
	if(uiButtonFlags & HYDLGFLAG_Cancel)
	{
		pNewBtn = HY_NEW HyButton(buttonPanelInitRef, buttonTextInitRef, this);
		pNewBtn->SetText("Cancel");
		m_ButtonList.push_back(pNewBtn);
	}
	if(uiButtonFlags & HYDLGFLAG_Close)
	{
		pNewBtn = HY_NEW HyButton(buttonPanelInitRef, buttonTextInitRef, this);
		pNewBtn->SetText("Close");
		m_ButtonList.push_back(pNewBtn);
	}
}

/*virtual*/ IHyDialog::~IHyDialog()
{
}

void IHyDialog::InsertButtonList()
{
	HyLayoutHandle hButtonBox = InsertLayout(HYORIENT_Horizontal);
	InsertSpacer(HYSIZEPOLICY_Expanding, 0, hButtonBox);
	for(HyButton *pBtn : m_ButtonList)
		InsertWidget(*pBtn, hButtonBox);
}
