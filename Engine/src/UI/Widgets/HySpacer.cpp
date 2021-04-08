/**************************************************************************
*	HySpacer.cpp
*
*	Harmony Engine
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "UI/Widgets/HySpacer.h"

HySpacer::HySpacer(HyEntity2d *pParent /*= nullptr*/) :
	IHyWidget(pParent)
{
}

/*virtual*/ HySpacer::~HySpacer()
{
}

/*virtual*/ void HySpacer::OnResize(int32 iNewWidth, int32 iNewHeight) /*override*/
{
}
