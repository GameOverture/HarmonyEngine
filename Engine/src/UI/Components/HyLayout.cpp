/**************************************************************************
*	HyLayout.cpp
*
*	Harmony Engine
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "UI/Components/HyLayout.h"
#include "UI/HyUiContainer.h"
#include "UI/Widgets/IHyWidget.h"
#include "Diagnostics/Console/IHyConsole.h"

HyLayout::HyLayout(HyOrientation eLayoutType, HyEntity2d *pParent /*= nullptr*/) :
	IHyEntityUi(pParent),
	m_eLayoutType(eLayoutType),
	m_vActualSize(0, 0),
	m_bReverse(m_eLayoutType == HYORIEN_Horizontal ? false : true), // 'm_bReverse' is defaulted ON when 'm_eLayoutType' is HYORIEN_Vertical to achieve top->bottom as default
	m_bLayoutDirty(false)
{
	m_uiFlags |= NODETYPE_IsLayout;

	m_SizePolicies[HYORIEN_Horizontal] = HYSIZEPOLICY_Flexible;
	m_SizePolicies[HYORIEN_Vertical] = HYSIZEPOLICY_Flexible;
}

/*virtual*/ HyLayout::~HyLayout()
{
}

/*virtual*/ float HyLayout::GetWidth(float fPercent /*= 1.0f*/) /*override*/
{
	return m_vActualSize.x * fPercent;
}

/*virtual*/ float HyLayout::GetHeight(float fPercent /*= 1.0f*/) /*override*/
{
	return m_vActualSize.y * fPercent;
}

HyOrientation HyLayout::GetLayoutType() const
{
	return m_eLayoutType;
}

void HyLayout::SetLayoutType(HyOrientation eLayoutType)
{
	if(m_eLayoutType == eLayoutType)
		return;

	m_eLayoutType = eLayoutType;
	m_bReverse = (m_eLayoutType == HYORIEN_Horizontal ? false : true), // 'm_bReverse' is defaulted ON when 'm_eLayoutType' is HYORIEN_Vertical to achieve top->bottom as default
	SetLayoutDirty();
}

glm::ivec2 HyLayout::GetActualSize() const
{
	return m_vActualSize;
}

/*virtual*/ HySizePolicy HyLayout::GetSizePolicy(HyOrientation eOrien) /*override*/
{
	uint32 uiSizePolicy = 0;// HYSIZEPOLICY_Fixed;
	bool bShrinkValid = true;
	for(int32 i = 0; i < static_cast<int32>(m_ChildList.size()); ++i)
	{
		// Children are guaranteed to be IHyEntityUi
		IHyEntityUi *pItem = static_cast<IHyEntityUi *>(m_ChildList[i]);

		if(pItem->GetSizePolicy(eOrien) & HY_SIZEFLAG_EXPAND)
			uiSizePolicy |= HY_SIZEFLAG_EXPAND;
		if((pItem->GetSizePolicy(eOrien) & HY_SIZEFLAG_SHRINK) >> 1)
			uiSizePolicy |= HY_SIZEFLAG_SHRINK;
	}

	return static_cast<HySizePolicy>(uiSizePolicy);
}

/*virtual*/ glm::vec2 HyLayout::GetPosOffset() /*override*/
{
	return glm::vec2(0, 0);
}

void HyLayout::AppendItem(IHyEntityUi &itemRef)
{
	ChildAppend(itemRef);
	SetLayoutDirty();
}

void HyLayout::DetachAllItems()
{
	while(m_ChildList.empty() == false)
		m_ChildList[m_ChildList.size() - 1]->ParentDetach();

	SetLayoutDirty();
}

bool HyLayout::IsReverseOrder()
{
	if(m_eLayoutType == HYORIEN_Vertical)
		return !m_bReverse; // HYORIEN_Vertical needs this flipped to achieve top->bottom default
	else
		return m_bReverse;
}

void HyLayout::ReverseOrder(bool bReverse)
{
	if(m_eLayoutType == HYORIEN_Vertical)
		m_bReverse = !bReverse; // HYORIEN_Vertical needs this flipped to achieve top->bottom default
	else
		m_bReverse = bReverse;

	SetLayoutDirty();
}

const HyRectangle<int16> &HyLayout::GetMargins() const
{
	return m_Margins;
}

void HyLayout::SetMargins(int16 iLeft, int16 iBottom, int16 iRight, int16 iTop, int32 iWidgetSpacing)
{
	m_Margins.Set(iLeft, iBottom, iRight, iTop);
	m_Margins.iTag = iWidgetSpacing;
	
	SetLayoutDirty();
}

int32 HyLayout::GetWidgetSpacing()
{
	return m_Margins.iTag;
}

bool HyLayout::IsLayoutDirty() const
{
	return m_bLayoutDirty;
}

void HyLayout::SetLayoutDirty()
{
	m_bSizeHintDirty = true;
	m_bLayoutDirty = true;

	// Propagate upward if this is nested in another layout
	if(m_pParent && (m_pParent->GetInternalFlags() & NODETYPE_IsLayout) != 0)
		static_cast<HyLayout *>(m_pParent)->SetLayoutDirty();
}

bool HyLayout::RequestWidgetFocus(IHyWidget *pWidget)
{
	// Propagate upward if this is nested in another layout
	if(m_pParent)
	{
		if((m_pParent->GetInternalFlags() & NODETYPE_IsLayout) != 0)
			return static_cast<HyLayout *>(m_pParent)->RequestWidgetFocus(pWidget);
		else // m_pParent must be the container (and *this is the root layout)
			return static_cast<HyUiContainer *>(m_pParent)->RequestWidgetFocus(pWidget);
	}
	return false;
}

bool HyLayout::IsWidgetInputAllowed()
{
	// Propagate upward if this is nested in another layout
	if(m_pParent)
	{
		if((m_pParent->GetInternalFlags() & NODETYPE_IsLayout) != 0)
			return static_cast<HyLayout *>(m_pParent)->IsWidgetInputAllowed();
		else // m_pParent must be the container (and *this is the root layout)
			return static_cast<HyUiContainer *>(m_pParent)->IsInputAllowed();
	}

	// This shouldn't happen
	return false;
}

/*virtual*/ void HyLayout::OnSetSizeHint() /*override*/
{
	HySetVec(m_vMinSize, m_Margins.left, m_Margins.bottom);
	m_vSizeHint = m_vMinSize;

	uint32 uiNumChildren = ChildCount();
	if(uiNumChildren == 0)
		return;

	HyOrientation eOrientation, eInverseOrien;
	if(m_eLayoutType == HYORIEN_Horizontal)
	{
		eOrientation = HYORIEN_Horizontal;
		eInverseOrien = HYORIEN_Vertical;

		// HACK NOTE: I have no idea why I need to half these margin values in the 'orientation' dimension, otherwise they come out double
		m_vSizeHint.x += (m_Margins.right /** 0.5f*/);
		m_vMinSize.x += (m_Margins.right /** 0.5f*/);
		m_vSizeHint.y += m_Margins.top;
		m_vMinSize.y += m_Margins.top;
	}
	else
	{
		eOrientation = HYORIEN_Vertical;
		eInverseOrien = HYORIEN_Horizontal;

		// HACK NOTE: I have no idea why I need to half these margin values in the 'orientation' dimension, otherwise they come out double
		m_vSizeHint.x += m_Margins.right;
		m_vMinSize.x += m_Margins.right;
		m_vSizeHint.y += (m_Margins.top /** 0.5f*/);
		m_vMinSize.y += (m_Margins.top /** 0.5f*/);
	}

	// Figure out m_vSizeHint while counting size policies
	m_vSizeHint[eOrientation] += (GetWidgetSpacing() * (uiNumChildren - 1));
	m_vMinSize[eOrientation] += (GetWidgetSpacing() * (uiNumChildren - 1));
	m_uiNumExpandItems = m_uiNumShrinkItems = 0;

	for(uint32 i = 0; i < static_cast<uint32>(m_ChildList.size()); ++i)
	{
		IHyNode2d *pChildItem = m_ChildList[i];

		// Children are guaranteed to be IHyEntityUi
		IHyEntityUi *pItem = static_cast<IHyEntityUi *>(pChildItem);

		glm::ivec2 vItemMinSize = pItem->GetMinSize();
		m_vMinSize[eOrientation] += vItemMinSize[eOrientation];
		m_vMinSize[eInverseOrien] = HyMax(m_vMinSize[eInverseOrien], vItemMinSize[eInverseOrien]);

		m_uiNumExpandItems += (pItem->GetSizePolicy(eOrientation) & HY_SIZEFLAG_EXPAND);		// Adds 1 or 0
		m_uiNumShrinkItems += ((pItem->GetSizePolicy(eOrientation) & HY_SIZEFLAG_SHRINK) >> 1);	// Adds 1 or 0

		glm::ivec2 vItemSizeHint = pItem->GetSizeHint();
		m_vSizeHint[eOrientation] += vItemSizeHint[eOrientation];
		m_vSizeHint[eInverseOrien] = HyMax(m_vSizeHint[eInverseOrien], vItemSizeHint[eInverseOrien]);
	}
}

/*virtual*/ glm::ivec2 HyLayout::OnResize(uint32 uiNewWidth, uint32 uiNewHeight) /*override*/
{
	glm::ivec2 vTargetSize(uiNewWidth, uiNewHeight);
	if(m_vActualSize == vTargetSize)
	{
		m_bLayoutDirty = false;
		return m_vActualSize;
	}

	HyOrientation eOrientation, eInverseOrien;
	int32 iInverseOrienMargin;
	if(m_eLayoutType == HYORIEN_Horizontal)
	{
		eOrientation = HYORIEN_Horizontal;
		eInverseOrien = HYORIEN_Vertical;
		iInverseOrienMargin = m_Margins.top + m_Margins.bottom;
	}
	else
	{
		eOrientation = HYORIEN_Vertical;
		eInverseOrien = HYORIEN_Horizontal;
		iInverseOrienMargin = m_Margins.left + m_Margins.right;
	}

	float fExpandAmt = 0.0f, fShrinkAmt = 0.0f;
	if(vTargetSize[eOrientation] != 0)
		GetDistributedScalingAmts(vTargetSize[eOrientation], GetSizeHint()[eOrientation], fExpandAmt, fShrinkAmt);

	// Go through each child and set its position and OnResize()
	glm::vec2 ptCurPos(m_Margins.left, m_Margins.bottom);
	int32 iMaxInverse = 0; // Used to determine the m_vActualSize when the inverse dimension was specified as '0'
	bool bNeedsResize = false;

	auto fpPositionAndResize = [&](IHyNode2d *&pChildItem) // Lambda func used to iterate over 'm_ChildList'
	{
		// Children are guaranteed to be IHyEntityUi
		IHyEntityUi *pItem = static_cast<IHyEntityUi *>(pChildItem);

		// Set item to the ptCurPos
		pItem->pos.Set(ptCurPos);

		glm::ivec2 vItemSize = pItem->GetSizeHint();

		if(vTargetSize[eOrientation] != 0)
		{
			HySizePolicy eSizePolicy = pItem->GetSizePolicy(eOrientation);
			// Along 'eOrientation', distribute either 'fExpandAmt' or 'fShrinkAmt' among those with those size policies
			vItemSize[eOrientation] += static_cast<int32>(fExpandAmt * (eSizePolicy & HY_SIZEFLAG_EXPAND));
			vItemSize[eOrientation] += static_cast<int32>(fShrinkAmt * ((eSizePolicy & HY_SIZEFLAG_SHRINK) >> 1));
		}

		if(vTargetSize[eInverseOrien] != 0)
		{
			// Along 'eInverseOrien', either clamp size or center within available space
			HySizePolicy eSizePolicy = pItem->GetSizePolicy(eInverseOrien);
			if(vItemSize[eInverseOrien] <= vTargetSize[eInverseOrien])
			{
				if((eSizePolicy & HY_SIZEFLAG_EXPAND) != 0)
					vItemSize[eInverseOrien] += (vTargetSize[eInverseOrien] - iInverseOrienMargin) - vItemSize[eInverseOrien];
				else
					pItem->pos.GetAnimFloat(eInverseOrien).Offset(0.5f * ((vTargetSize[eInverseOrien] - iInverseOrienMargin) - vItemSize[eInverseOrien]));
			}
			else
			{
				if((eSizePolicy & HY_SIZEFLAG_SHRINK) != 0)
					vItemSize[eInverseOrien] += (vTargetSize[eInverseOrien] - iInverseOrienMargin) - vItemSize[eInverseOrien];
			}
		}
		else
			iMaxInverse = HyMax(iMaxInverse, vItemSize[eInverseOrien]);

		// If 'pItem' is a nested layout, it should pass '0' to Resize where appropriate
		glm::ivec2 vResize = vItemSize;
		if((pItem->GetInternalFlags() & NODETYPE_IsLayout) != 0)
		{
			if(vTargetSize.x == 0)
				vResize.x = 0;
			if(vTargetSize.y == 0)
				vResize.y = 0;
		}
		glm::ivec2 vActualItemSize = pItem->Resize(vResize.x, vResize.y);
		if(vTargetSize[eOrientation] != 0 && vActualItemSize != vItemSize)
			bNeedsResize = true;

		// After resizing, apply offset to get 'pItem' oriented to its bottom left
		pItem->pos.Offset(pItem->GetPosOffset());

		ptCurPos[eOrientation] += vActualItemSize[eOrientation] + GetWidgetSpacing();
	};
	if(m_bReverse)
		std::for_each(m_ChildList.rbegin(), m_ChildList.rend(), fpPositionAndResize);
	else
		std::for_each(m_ChildList.begin(), m_ChildList.end(), fpPositionAndResize);

	if(vTargetSize[eInverseOrien] == 0)
	{
		vTargetSize[eInverseOrien] = iMaxInverse;
		bNeedsResize = true;
	}

	if(bNeedsResize)
	{
		vTargetSize[eOrientation] = static_cast<int32>(ptCurPos[eOrientation] - GetWidgetSpacing());
		HySetVec(ptCurPos, m_Margins.left, m_Margins.bottom);

		GetDistributedScalingAmts(vTargetSize[eOrientation], GetSizeHint()[eOrientation], fExpandAmt, fShrinkAmt);

		if(m_bReverse)
			std::for_each(m_ChildList.rbegin(), m_ChildList.rend(), fpPositionAndResize);
		else
			std::for_each(m_ChildList.begin(), m_ChildList.end(), fpPositionAndResize);
	}

	m_bLayoutDirty = false;

	m_vActualSize[eOrientation] = static_cast<int32>(ptCurPos[eOrientation] - GetWidgetSpacing());
	m_vActualSize[eInverseOrien] = vTargetSize[eInverseOrien];
	return m_vActualSize;
}

void HyLayout::GetDistributedScalingAmts(int32 iTargetLength, int32 iCurrLength, float &fExpandAmtOut, float &fShrinkAmtOut)
{
	// Determine if current length fits within iTargetLength
	float fDifference = static_cast<float>(iTargetLength) - static_cast<float>(iCurrLength);
	
	fExpandAmtOut = 0.0f;
	fShrinkAmtOut = 0.0f;

	// Distribute positive difference to all 'expanding' size policies
	if(fDifference >= 0)
	{
		if(m_uiNumExpandItems > 0)
			fExpandAmtOut = fDifference / m_uiNumExpandItems;
	}
	else // Distribute negative difference to all 'shrink' size policies
	{
		if(m_uiNumShrinkItems > 0)
			fShrinkAmtOut = fDifference / m_uiNumShrinkItems;
	}
}
