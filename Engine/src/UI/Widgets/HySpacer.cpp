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
	IHyWidget(pParent),
	m_vMinSize(0, 0)
{
	m_SizePolicies[HYORIEN_Horizontal] = m_SizePolicies[HYORIEN_Vertical] = HYSIZEPOLICY_Expanding;
}

/*virtual*/ HySpacer::~HySpacer()
{
}

void HySpacer::SetMinSize(int32 iWidth, int32 iHeight)
{
	m_vMinSize.x = iWidth;
	m_vMinSize.y = iHeight;
}

/*virtual*/ glm::ivec2 HySpacer::GetSizeHint() /*override*/
{
	return m_vMinSize;
}

/*virtual*/ glm::vec2 HySpacer::GetPosOffset() /*override*/
{
	return glm::vec2(0.0f, 0.0f);
}

/*virtual*/ glm::vec2 HySpacer::OnResize(int32 iNewWidth, int32 iNewHeight) /*override*/
{
	return glm::vec2(iNewWidth, iNewHeight);
}
