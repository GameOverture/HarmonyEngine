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
#include "UI/Dialogs/HyRadioDialog.h"

HyRadioDialog::HyRadioDialog(std::string sTitle, std::vector<std::string> sRadioList, const HyUiPanelInit &panelInitRef, const HyUiPanelInit &buttonPanelInitRef, const HyUiTextInit &buttonTextInitRef, HyEntity2d *pParent /*= nullptr*/) :
	IHyDialog(sTitle, HYORIENT_Vertical, panelInitRef, buttonPanelInitRef, buttonTextInitRef, HYDLGFLAG_Ok | HYDLGFLAG_Cancel, pParent)
{
}

/*virtual*/ HyRadioDialog::~HyRadioDialog()
{
}
