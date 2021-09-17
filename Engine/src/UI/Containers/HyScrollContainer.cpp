/**************************************************************************
*	HyScrollContainer.cpp
*
*	Harmony Engine
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "UI/Containers/HyScrollContainer.h"

HyScrollContainer::HyScrollContainer(HyLayoutType eRootLayout, HyEntity2d *pParent /*= nullptr*/) :
	HyContainer(eRootLayout, pParent)
{
}

HyScrollContainer::HyScrollContainer(HyLayoutType eRootLayout, int32 iWidth, int32 iHeight, int32 iStroke, HyEntity2d *pParent /*= nullptr*/) :
	HyContainer(eRootLayout, iWidth, iHeight, iStroke, pParent)
{
}

/*virtual*/ HyScrollContainer::~HyScrollContainer()
{
}
