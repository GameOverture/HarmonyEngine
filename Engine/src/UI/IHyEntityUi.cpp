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
#include "UI/Components/HyLayout.h"

IHyEntityUi::IHyEntityUi(HyEntity2d *pParent /*= nullptr*/) :
	HyEntity2d(pParent),
	m_vMinSize(0, 0),
	m_vMaxSize(std::numeric_limits<int32>().max(), std::numeric_limits<int32>().max()),
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

HySizePolicy IHyEntityUi::GetHorizontalPolicy()
{
	return GetSizePolicy(HYORIEN_Horizontal);
}

HySizePolicy IHyEntityUi::GetVerticalPolicy()
{
	return GetSizePolicy(HYORIEN_Vertical);
}

/*virtual*/ HySizePolicy IHyEntityUi::GetSizePolicy(HyOrientation eOrien)
{
	return m_SizePolicies[eOrien];
}

void IHyEntityUi::SetSizePolicy(HySizePolicy eHorizPolicy, HySizePolicy eVertPolicy)
{
	m_SizePolicies[HYORIEN_Horizontal] = eHorizPolicy;
	m_SizePolicies[HYORIEN_Vertical] = eVertPolicy;
	SetSizeAndLayoutDirty();
}

void IHyEntityUi::SetHorizontalPolicy(HySizePolicy ePolicy)
{
	m_SizePolicies[HYORIEN_Horizontal] = ePolicy;
	SetSizeAndLayoutDirty();
}

void IHyEntityUi::SetVerticalPolicy(HySizePolicy ePolicy)
{
	m_SizePolicies[HYORIEN_Vertical] = ePolicy;
	SetSizeAndLayoutDirty();
}

bool IHyEntityUi::IsLockedProportions() const
{
	return m_bLockProportions;
}

void IHyEntityUi::SetLockedProportions(bool bLockProportions)
{
	m_bLockProportions = bLockProportions;
	SetSizeAndLayoutDirty();
}

glm::ivec2 IHyEntityUi::GetMinSize()
{
	glm::ivec2 vSizeHint = GetSizeHint();

	glm::ivec2 vMinSize = m_vMinSize;
	if((m_SizePolicies[HYORIEN_Horizontal] & HY_SIZEFLAG_SHRINK) == 0)
		vMinSize.x = vSizeHint.x;
	else
		vMinSize.x = HyMath::Min(vMinSize.x, vSizeHint.x);

	if((m_SizePolicies[HYORIEN_Vertical] & HY_SIZEFLAG_SHRINK) == 0)
		vMinSize.y = vSizeHint.y;
	else
		vMinSize.y = HyMath::Min(vMinSize.y, vSizeHint.y);

	return vMinSize;
}

void IHyEntityUi::SetMinSize(uint32 uiMinSizeX, uint32 uiMinSizeY)
{
	HySetVec(m_vMinSize, uiMinSizeX, uiMinSizeY);
	SetSizeAndLayoutDirty();
}

glm::ivec2 IHyEntityUi::GetMaxSize()
{
	glm::ivec2 vSizeHint = GetSizeHint();

	glm::ivec2 vMaxSize = m_vMaxSize;
	if((m_SizePolicies[HYORIEN_Horizontal] & HY_SIZEFLAG_EXPAND) == 0)
		vMaxSize.x = vSizeHint.x;
	else
		vMaxSize.x = HyMath::Max(vMaxSize.x, vSizeHint.x);

	if((m_SizePolicies[HYORIEN_Vertical] & HY_SIZEFLAG_EXPAND) == 0)
		vMaxSize.y = vSizeHint.y;
	else
		vMaxSize.y = HyMath::Max(vMaxSize.y, vSizeHint.y);

	return vMaxSize;
}

void IHyEntityUi::SetMaxSize(uint32 uiMaxSizeX, uint32 uiMaxSizeY)
{
	HySetVec(m_vMaxSize, uiMaxSizeX, uiMaxSizeY);
	SetSizeAndLayoutDirty();
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
	if(uiNewWidth != 0)
	{
		if(vDiff[0] >= 0)
			uiNewWidth = vSizeHint.x + ((m_SizePolicies[0] & HY_SIZEFLAG_EXPAND) * vDiff[0]);
		else
			uiNewWidth = vSizeHint.x + (((m_SizePolicies[0] & HY_SIZEFLAG_SHRINK) >> 1) * vDiff[0]);
		uiNewWidth = HyMath::Max(uiNewWidth, static_cast<uint32>(m_vMinSize.x));
		uiNewWidth = HyMath::Min(uiNewWidth, static_cast<uint32>(m_vMaxSize.x));
	}

	// Y-Axis
	if(uiNewHeight != 0)
	{
		if(vDiff[1] >= 0)
			uiNewHeight = vSizeHint.y + ((m_SizePolicies[1] & HY_SIZEFLAG_EXPAND) * vDiff[1]);
		else
			uiNewHeight = vSizeHint.y + (((m_SizePolicies[1] & HY_SIZEFLAG_SHRINK) >> 1) * vDiff[1]);
		uiNewHeight = HyMath::Max(uiNewHeight, static_cast<uint32>(m_vMinSize.y));
		uiNewHeight = HyMath::Min(uiNewHeight, static_cast<uint32>(m_vMaxSize.y));
	}

	if(m_bLockProportions && uiNewWidth != 0 && uiNewHeight != 0)
	{
		glm::ivec2 vProportionalSize = HyMath::LockAspectRatio(vSizeHint.x, vSizeHint.y, uiNewWidth, uiNewHeight);
		uiNewWidth = vProportionalSize.x;
		uiNewHeight = vProportionalSize.y;
	}

	return OnResize(uiNewWidth, uiNewHeight);
}

void IHyEntityUi::SetSizeAndLayoutDirty()
{
	m_bSizeHintDirty = true;
	if(m_pParent && (m_pParent->GetInternalFlags() & NODETYPE_IsLayout) != 0)
		static_cast<HyLayout *>(m_pParent)->SetLayoutDirty();
}
