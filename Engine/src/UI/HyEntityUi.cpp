/**************************************************************************
*	HyEntityUi.cpp
*
*	Harmony Engine
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "UI/HyEntityUi.h"

HyEntityUi::HyEntityUi(UiType eType, HyEntity2d *pParent /*= nullptr*/) :
	HyEntity2d(pParent),
	m_eUI_TYPE(eType)
{
}

/*virtual*/ HyEntityUi::~HyEntityUi()
{
}

HyEntityUi::UiType HyEntityUi::GetUiType() const
{
	return m_eUI_TYPE;
}
