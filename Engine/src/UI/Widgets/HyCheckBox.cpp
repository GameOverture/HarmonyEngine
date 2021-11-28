/**************************************************************************
*	HyButton.h
*
*	Harmony Engine
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "UI/Widgets/HyCheckBox.h"
#include "Diagnostics/Console/IHyConsole.h"

HyCheckBox::HyCheckBox(HyEntity2d *pParent /*= nullptr*/) :
	HyButton(pParent)
{
}

HyCheckBox::HyCheckBox(const HyPanelInit &initRef, std::string sTextPrefix, std::string sTextName, HyEntity2d *pParent /*= nullptr*/) :
	HyButton(initRef, sTextPrefix, sTextName, pParent)
{
}

HyCheckBox::HyCheckBox(const HyPanelInit &initRef, std::string sTextPrefix, std::string sTextName, int32 iTextDimensionsX, int32 iTextDimensionsY, int32 iTextOffsetX, int32 iTextOffsetY, HyEntity2d *pParent /*= nullptr*/) :
	HyButton(initRef, sTextPrefix, sTextName, iTextDimensionsX, iTextDimensionsY, iTextOffsetX, iTextOffsetY, pParent)
{
}

/*virtual*/ HyCheckBox::~HyCheckBox()
{
}
