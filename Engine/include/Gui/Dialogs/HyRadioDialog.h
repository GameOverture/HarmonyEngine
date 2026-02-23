/**************************************************************************
*	HyRadioDialog.h
*
*	Harmony Engine
*	Copyright (c) 2025 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyRadioDialog_h__
#define HyRadioDialog_h__

#include "Afx/HyStdAfx.h"
#include "Gui/Dialogs/IHyDialog.h"
#include "Gui/Widgets/HyRadioButton.h"

class HyRadioDialog : public IHyDialog
{
	std::vector<HyRadioButton *>	m_RadioList;
	HyUiPanelInit					m_ButtonPanelInit;
	HyUiTextInit					m_ButtonTextInitRef;

public:
	HyRadioDialog(std::string sTitle, std::vector<std::string> sRadioList, const HyUiPanelInit &panelInitRef, const HyUiPanelInit &buttonPanelInitRef, const HyUiTextInit &buttonTextInitRef, HyEntity2d *pParent = nullptr);
	virtual ~HyRadioDialog();

	void SetRadioList(std::vector<std::string> sRadioList);
	void SetRadioList(std::vector<std::string> sRadioList, const HyUiPanelInit &buttonPanelInitRef, const HyUiTextInit &buttonTextInitRef);
};

#endif /* HyRadioDialog_h__ */
