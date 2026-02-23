/**************************************************************************
*	IHyDialog.h
*
*	Harmony Engine
*	Copyright (c) 2025 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef IHyDialog_h__
#define IHyDialog_h__

#include "Afx/HyStdAfx.h"
#include "Gui/HyGui.h"

enum HyDlgBtnFlag
{
	HYDLGFLAG_Ok = 0x01,
	HYDLGFLAG_Cancel = 0x02,
	HYDLGFLAG_Yes = 0x04,
	HYDLGFLAG_No = 0x08,
	HYDLGFLAG_Apply = 0x10,
	HYDLGFLAG_Close = 0x20
};

class IHyDialog : public HyGui
{
	std::vector<HyButton *>		m_ButtonList;

	enum DlgBtnType
	{
		DlgBtnType_Ok,
		DlgBtnType_Cancel,
		DlgBtnType_Yes,
		DlgBtnType_No,
		DlgBtnType_Apply,
		DlgBtnType_Close,
	};

public:
	IHyDialog(std::string sTitle, const HyUiPanelInit &panelInitRef, const HyUiPanelInit &buttonPanelInitRef, const HyUiTextInit &buttonTextInitRef, uint32_t uiButtonFlags = (HYDLGFLAG_Ok | HYDLGFLAG_Cancel), HyEntity2d *pParent = nullptr);
	virtual ~IHyDialog();

protected:
	void InsertButtonList();
};

#endif /* IHyDialog_h__ */
