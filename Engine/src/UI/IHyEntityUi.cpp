/**************************************************************************
*	IHyEntityUi.cpp
*
*	Harmony Engine
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "UI/IHyEntityUi.h"
#include "UI/Containers/Components/HyLayout.h"

IHyEntityUi::IHyEntityUi(HyEntity2d *pParent /*= nullptr*/) :
	HyEntity2d(pParent),
	m_vMinSize(0, 0),
	m_bLockProportions(false),
	m_bSizeHintDirty(true),
	m_vSizeHint(0, 0)
{
	for(int32 i = 0; i < HYNUM_ORIENTATIONS; ++i)
		m_SizePolicies[i] = HYSIZEPOLICY_Fixed;
}

/*virtual*/ IHyEntityUi::~IHyEntityUi()
{
}

HySizePolicy IHyEntityUi::GetSizePolicy(HyOrientation eOrien) const
{
	return m_SizePolicies[eOrien];
}

HySizePolicy IHyEntityUi::GetHorizontalPolicy() const
{
	return m_SizePolicies[HYORIEN_Horizontal];
}

HySizePolicy IHyEntityUi::GetVerticalPolicy() const
{
	return m_SizePolicies[HYORIEN_Vertical];
}

void IHyEntityUi::SetSizePolicy(HySizePolicy eHorizPolicy, HySizePolicy eVertPolicy)
{
	m_SizePolicies[HYORIEN_Horizontal] = eHorizPolicy;
	m_SizePolicies[HYORIEN_Vertical] = eVertPolicy;

	SetMinSize(m_vMinSize.x, m_vMinSize.y); // This will 'fix' min size if policy conflicts with it

	if(m_pParent && (m_pParent->GetInternalFlags() & NODETYPE_IsLayout) != 0)
		static_cast<HyLayout *>(m_pParent)->SetLayoutDirty();
}

void IHyEntityUi::SetHorizontalPolicy(HySizePolicy ePolicy)
{
	m_SizePolicies[HYORIEN_Horizontal] = ePolicy;

	if(m_pParent && (m_pParent->GetInternalFlags() & NODETYPE_IsLayout) != 0)
		static_cast<HyLayout *>(m_pParent)->SetLayoutDirty();
}

void IHyEntityUi::SetVerticalPolicy(HySizePolicy ePolicy)
{
	m_SizePolicies[HYORIEN_Vertical] = ePolicy;

	if(m_pParent && (m_pParent->GetInternalFlags() & NODETYPE_IsLayout) != 0)
		static_cast<HyLayout *>(m_pParent)->SetLayoutDirty();
}

bool IHyEntityUi::IsLockedProportions() const
{
	return m_bLockProportions;
}

void IHyEntityUi::SetLockedProportions(bool bLockProportions)
{
	m_bLockProportions = bLockProportions;

	if(m_pParent && (m_pParent->GetInternalFlags() & NODETYPE_IsLayout) != 0)
		static_cast<HyLayout *>(m_pParent)->SetLayoutDirty();
}

glm::ivec2 IHyEntityUi::GetMinSize()
{
	glm::ivec2 vSizeHint = GetSizeHint();

	glm::ivec2 vMinSize = m_vMinSize;
	if((m_SizePolicies[HYORIEN_Horizontal] & HY_SIZEFLAG_SHRINK) == 0)
		vMinSize.x = vSizeHint.x;
	if((m_SizePolicies[HYORIEN_Vertical] & HY_SIZEFLAG_SHRINK) == 0)
		vMinSize.y = vSizeHint.y;

	return vMinSize;
}

void IHyEntityUi::SetMinSize(uint32 uiMinSizeX, uint32 uiMinSizeY)
{
	glm::ivec2 vSizeHint = GetSizeHint();
	HySetVec(m_vMinSize, HyMin(uiMinSizeX, static_cast<uint32>(vSizeHint.x)), HyMin(uiMinSizeY, static_cast<uint32>(vSizeHint.y)));

	if(m_pParent && (m_pParent->GetInternalFlags() & NODETYPE_IsLayout) != 0)
		static_cast<HyLayout *>(m_pParent)->SetLayoutDirty();
}

glm::ivec2 IHyEntityUi::GetSizeHint()
{
	if(m_bSizeHintDirty)
	{
		OnSetSizeHint();
		m_bSizeHintDirty = false;
	}
	return m_vSizeHint;
}

glm::ivec2 IHyEntityUi::Resize(uint32 uiNewWidth, uint32 uiNewHeight)
{
	glm::ivec2 vSizeHint = GetSizeHint();
	glm::ivec2 vDiff = glm::ivec2(uiNewWidth, uiNewHeight) - vSizeHint;

	// X-Axis
	if(vDiff[0] >= 0)
		uiNewWidth = vSizeHint.x + ((m_SizePolicies[0] & HY_SIZEFLAG_EXPAND) * vDiff[0]);
	else
		uiNewWidth = vSizeHint.x + (((m_SizePolicies[0] & HY_SIZEFLAG_SHRINK) >> 1) * vDiff[0]);
	uiNewWidth = HyMax(uiNewWidth, static_cast<uint32>(m_vMinSize.x));

	// Y-Axis
	if(vDiff[1] >= 0)
		uiNewHeight = vSizeHint.y + ((m_SizePolicies[1] & HY_SIZEFLAG_EXPAND) * vDiff[1]);
	else
		uiNewHeight = vSizeHint.y + (((m_SizePolicies[1] & HY_SIZEFLAG_SHRINK) >> 1) * vDiff[1]);
	uiNewHeight = HyMax(uiNewHeight, static_cast<uint32>(m_vMinSize.y));

	if(m_bLockProportions)
	{
		glm::ivec2 vProportionalSize = HyMath::LockAspectRatio(vSizeHint.x, vSizeHint.y, uiNewWidth, uiNewHeight);
		uiNewWidth = vProportionalSize.x;
		uiNewHeight = vProportionalSize.y;
	}

	return OnResize(uiNewWidth, uiNewHeight);
}
