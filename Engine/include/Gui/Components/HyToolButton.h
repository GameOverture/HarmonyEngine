/**************************************************************************
*	HyToolButton.h
*
*	Harmony Engine
*	Copyright (c) 2023 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyToolButton_h__
#define HyToolButton_h__

#include "Afx/HyStdAfx.h"
#include "Gui/Widgets/HyButton.h"

enum HyToolButtonType
{
	HYTOOLBTN_Close = 0,
	HYTOOLBTN_LeftArrow,
	HYTOOLBTN_RightArrow,
	HYTOOLBTN_UpArrow,
	HYTOOLBTN_DownArrow
};

class HyToolButton : public HyButton
{
	HyPrimitive2d			m_X1;
	HyPrimitive2d			m_X2;

public:
	HyToolButton(HyToolButtonType eToolBtnType, const HyUiPanelInit &panelInit, HyEntity2d *pParent = nullptr);
	virtual ~HyToolButton();
};

#endif /* HyToolButton_h__ */
