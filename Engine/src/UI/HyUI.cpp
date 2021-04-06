/**************************************************************************
*	HyUI.cpp
*
*	Harmony Engine
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "UI/HyUI.h"

HyUI::HyUI(HyEntity2d *pParent /*= nullptr*/) :
	HyEntity2d(pParent)
{
	for(int32 i = 0; i < NumOrientations; ++i)
		m_SizePolicies[i] = Preferred;
}

/*virtual*/ HyUI::~HyUI()
{
}

HyUI::SizePolicy HyUI::GetSizePolicy(Orientation eOrientation) const
{
	return m_SizePolicies[eOrientation];
}

HyUI::SizePolicy HyUI::GetHorizontalPolicy() const
{
	return m_SizePolicies[Horizontal];
}

HyUI::SizePolicy HyUI::GetVerticalPolicy() const
{
	return m_SizePolicies[Vertical];
}

void HyUI::SetSizePolicy(SizePolicy eHorizPolicy, SizePolicy eVertPolicy)
{
	m_SizePolicies[Horizontal] = eHorizPolicy;
	m_SizePolicies[Vertical] = eVertPolicy;
}

void HyUI::SetHorizontalPolicy(SizePolicy ePolicy)
{
	m_SizePolicies[Horizontal] = ePolicy;
}

void HyUI::SetVerticalPolicy(SizePolicy ePolicy)
{
	m_SizePolicies[Vertical] = ePolicy;
}

glm::ivec2 HyUI::GetSizeHint() const
{
	// If size policy is 'None' then return zero for that dimension
	return glm::ivec2(m_vSizeHint.x * (((m_SizePolicies[Horizontal] & Ignore) >> 3) ^ 1), m_vSizeHint.y * (((m_SizePolicies[Vertical] & Ignore) >> 3) ^ 1));
}
