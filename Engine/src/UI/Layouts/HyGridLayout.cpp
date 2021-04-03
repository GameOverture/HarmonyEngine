/**************************************************************************
*	HyBoxLayout.cpp
*
*	Harmony Engine
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "UI/Layouts/HyBoxLayout.h"

HyBoxLayout::HyBoxLayout(HyEntity2d *pParent /*= nullptr*/) :
	IHyLayout(pParent)
{
}

/*virtual*/ HyBoxLayout::~HyBoxLayout()
{
}


/*virtual*/ void HyBoxLayout::OnDoLayout() /*override*/
{
}
