/**************************************************************************
*	IHySizer2d.cpp
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
	m_vActualSize(0, 0),
	m_vMinSize(0, 0),
	m_vMaxSize(std::numeric_limits<int32>().max(), std::numeric_limits<int32>().max()),
	m_vSizeHint(0, 0),
	m_bSizeDirty(true),
	m_bLockProportions(false)
{
	for(int32 i = 0; i < HYNUM_ORIENTATIONS; ++i)
		m_SizePolicies[i] = HYSIZEPOLICY_Fixed;
}

/*virtual*/ IHyEntityUi::~IHyEntityUi()
{
}

/*virtual*/ float IHyEntityUi::GetWidth(float fPercent /*= 1.0f*/) /*override*/
{
	if(m_bSizeDirty)
		Resize(m_vActualSize.x, m_vActualSize.y);

	return m_vActualSize.x * fPercent;
}

/*virtual*/ float IHyEntityUi::GetHeight(float fPercent /*= 1.0f*/) /*override*/
{
	if(m_bSizeDirty)
		Resize(m_vActualSize.x, m_vActualSize.y);

	return m_vActualSize.y * fPercent;
}

/*virtual*/ void IHyEntityUi::CalcLocalBoundingShape(HyShape2d &shapeOut) /*override*/
{
	shapeOut.SetAsBox(GetWidth(), GetHeight());
}

float IHyEntityUi::GetSizeDimension(int32 iDimensionIndex, float fPercent /*= 1.0f*/)
{
	if(iDimensionIndex == HYORIENT_Horizontal)
		return GetWidth(fPercent);

	return GetHeight(fPercent);
}

glm::ivec2 IHyEntityUi::SetSizeDimension(int32 iDimensionIndex, uint32 uiSizeHint)
{
	if(iDimensionIndex == HYORIENT_Horizontal)
		return Resize(uiSizeHint, m_vActualSize.y);
	return Resize(m_vActualSize.x, uiSizeHint);
}

HySizePolicy IHyEntityUi::GetHorizontalPolicy() const
{
	return GetSizePolicy(HYORIENT_Horizontal);
}

HySizePolicy IHyEntityUi::GetVerticalPolicy() const
{
	return GetSizePolicy(HYORIENT_Vertical);
}

/*virtual*/ HySizePolicy IHyEntityUi::GetSizePolicy(HyOrientation eOrien) const
{
	return m_SizePolicies[eOrien];
}

void IHyEntityUi::SetSizePolicy(HySizePolicy eHorizPolicy, HySizePolicy eVertPolicy)
{
	if(m_SizePolicies[HYORIENT_Horizontal] == eHorizPolicy && m_SizePolicies[HYORIENT_Vertical] == eVertPolicy)
		return;

	m_SizePolicies[HYORIENT_Horizontal] = eHorizPolicy;
	m_SizePolicies[HYORIENT_Vertical] = eVertPolicy;
	SetSizeDirty();
}

void IHyEntityUi::SetHorizontalPolicy(HySizePolicy ePolicy)
{
	if(m_SizePolicies[HYORIENT_Horizontal] == ePolicy)
		return;

	m_SizePolicies[HYORIENT_Horizontal] = ePolicy;
	SetSizeDirty();
}

void IHyEntityUi::SetVerticalPolicy(HySizePolicy ePolicy)
{
	if(m_SizePolicies[HYORIENT_Vertical] == ePolicy)
		return;

	m_SizePolicies[HYORIENT_Vertical] = ePolicy;
	SetSizeDirty();
}

bool IHyEntityUi::IsLockedProportions() const
{
	return m_bLockProportions;
}

void IHyEntityUi::SetLockedProportions(bool bLockProportions)
{
	if(m_bLockProportions == bLockProportions)
		return;

	m_bLockProportions = bLockProportions;
	SetSizeDirty();
}

//bool IHyEntityUi::IsAutoSize() const
//{
//	return m_vSizeHint.x <= 0 || m_vSizeHint.y <= 0;
//}

glm::ivec2 IHyEntityUi::GetMinSize()
{
	glm::ivec2 vSizeHint = GetPreferredSize();

	glm::ivec2 vMinSize = m_vMinSize;
	if((m_SizePolicies[HYORIENT_Horizontal] & HY_SIZEFLAG_SHRINK) == 0)
		vMinSize.x = vSizeHint.x;
	else
		vMinSize.x = HyMath::Min(vMinSize.x, vSizeHint.x);

	if((m_SizePolicies[HYORIENT_Vertical] & HY_SIZEFLAG_SHRINK) == 0)
		vMinSize.y = vSizeHint.y;
	else
		vMinSize.y = HyMath::Min(vMinSize.y, vSizeHint.y);

	return vMinSize;
}

void IHyEntityUi::SetMinSize(uint32 uiMinSizeX, uint32 uiMinSizeY)
{
	if(m_vMinSize.x == uiMinSizeX && m_vMinSize.y == uiMinSizeY)
		return;

	HySetVec(m_vMinSize, uiMinSizeX, uiMinSizeY);
	SetSizeDirty();
}

glm::ivec2 IHyEntityUi::GetMaxSize()
{
	glm::ivec2 vSizeHint = GetPreferredSize();

	glm::ivec2 vMaxSize = m_vMaxSize;
	if((m_SizePolicies[HYORIENT_Horizontal] & HY_SIZEFLAG_EXPAND) == 0)
		vMaxSize.x = vSizeHint.x;
	else
		vMaxSize.x = HyMath::Max(vMaxSize.x, vSizeHint.x);

	if((m_SizePolicies[HYORIENT_Vertical] & HY_SIZEFLAG_EXPAND) == 0)
		vMaxSize.y = vSizeHint.y;
	else
		vMaxSize.y = HyMath::Max(vMaxSize.y, vSizeHint.y);

	return vMaxSize;
}

void IHyEntityUi::SetMaxSize(uint32 uiMaxSizeX, uint32 uiMaxSizeY)
{
	if(m_vMaxSize.x == uiMaxSizeX && m_vMaxSize.y == uiMaxSizeY)
		return;

	HySetVec(m_vMaxSize, uiMaxSizeX, uiMaxSizeY);
	SetSizeDirty();
}

bool IHyEntityUi::IsSizeDirty() const
{
	return m_bSizeDirty;
}

glm::ivec2 IHyEntityUi::GetPreferredSize()
{
	if(m_bSizeDirty)
		m_vSizeHint = OnCalcPreferredSize();

	return m_vSizeHint;
}

glm::ivec2 IHyEntityUi::Resize(uint32 uiNewWidth, uint32 uiNewHeight)
{
	glm::ivec2 vSizeHint = GetPreferredSize();
	glm::ivec2 vDiff = glm::ivec2(uiNewWidth, uiNewHeight) - vSizeHint;

	if(uiNewWidth != 0)
	{
		//if(vSizeHint[0] != 0)
		{
			if(vDiff[0] >= 0)
				uiNewWidth = vSizeHint.x + ((m_SizePolicies[0] & HY_SIZEFLAG_EXPAND) * vDiff[0]);
			else
				uiNewWidth = vSizeHint.x + (((m_SizePolicies[0] & HY_SIZEFLAG_SHRINK) >> 1) * vDiff[0]);
		}
	}
	else
		uiNewWidth = vSizeHint.x;
	uiNewWidth = HyMath::Max(uiNewWidth, static_cast<uint32>(m_vMinSize.x));
	uiNewWidth = HyMath::Min(uiNewWidth, static_cast<uint32>(m_vMaxSize.x));

	if(uiNewHeight != 0)
	{
		//if(vSizeHint[1] != 0)
		{
			if(vDiff[1] >= 0)
				uiNewHeight = vSizeHint.y + ((m_SizePolicies[1] & HY_SIZEFLAG_EXPAND) * vDiff[1]);
			else
				uiNewHeight = vSizeHint.y + (((m_SizePolicies[1] & HY_SIZEFLAG_SHRINK) >> 1) * vDiff[1]);
		}
	}
	else
		uiNewHeight = vSizeHint.y;
	uiNewHeight = HyMath::Max(uiNewHeight, static_cast<uint32>(m_vMinSize.y));
	uiNewHeight = HyMath::Min(uiNewHeight, static_cast<uint32>(m_vMaxSize.y));

	if(m_bLockProportions && uiNewWidth != 0 && uiNewHeight != 0)
	{
		glm::ivec2 vProportionalSize = HyMath::LockAspectRatio(vSizeHint.x, vSizeHint.y, uiNewWidth, uiNewHeight);
		uiNewWidth = vProportionalSize.x;
		uiNewHeight = vProportionalSize.y;
	}

	m_vActualSize = OnResize(uiNewWidth, uiNewHeight);
	m_bSizeDirty = false;

	return m_vActualSize;
}

void IHyEntityUi::SetSizeDirty()
{
	SetDirty(IHyNode::DIRTY_SceneAABB);

	m_bSizeDirty = true;

	// This will propagate upward if *this is nested in another layout
	if(m_pParent && (m_pParent->GetInternalFlags() & NODETYPE_IsLayout) != 0)
		static_cast<HyLayout *>(m_pParent)->SetSizeDirty();
}
