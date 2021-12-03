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
	IHyEntityUi(pParent)
{
	SetSizePolicy(HYSIZEPOLICY_Expanding, HYSIZEPOLICY_Expanding);
}

/*virtual*/ HySpacer::~HySpacer()
{
}

/*virtual*/ glm::vec2 HySpacer::GetPosOffset() /*override*/
{
	return glm::vec2(0.0f, 0.0f);
}

/*virtual*/ void HySpacer::OnSetSizeHint() /*override*/
{
	m_vSizeHint = m_vMinSize;
}

/*virtual*/ glm::ivec2 HySpacer::OnResize(uint32 uiNewWidth, uint32 uiNewHeight) /*override*/
{
	return glm::ivec2(uiNewWidth, uiNewHeight);
}
