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
#include "UI/Containers/Components/HyLayout.h"

HyLayout::HyLayout(HyLayoutType eLayoutType, HyEntity2d *pParent /*= nullptr*/) :
	IHyEntityUi(pParent),
	m_eLayoutType(eLayoutType),
	m_vActualSize(0, 0),
	m_bReverse(m_eLayoutType == HYLAYOUT_Horizontal ? false : true), // 'm_bReverse' is defaulted ON when 'm_eLayoutType' is HYLAYOUT_Vertical to achieve top->bottom as default
	m_bLayoutDirty(false)
{
	m_uiFlags |= NODETYPE_IsLayout;

	m_SizePolicies[HYORIEN_Horizontal] = HYSIZEPOLICY_Flexible;
	m_SizePolicies[HYORIEN_Vertical] = HYSIZEPOLICY_Flexible;
}

/*virtual*/ HyLayout::~HyLayout()
{
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

/*virtual*/ void HyLayout::ClearItems() /*override*/
{
	while(m_ChildList.empty() == false)
		m_ChildList[m_ChildList.size() - 1]->ParentDetach();

	SetLayoutDirty();
}

bool HyLayout::IsReverseOrder()
{
	if(m_eLayoutType == HYLAYOUT_Vertical)
		return !m_bReverse; // HYLAYOUT_Vertical needs this flipped to achieve top->bottom default
	else
		return m_bReverse;
}

void HyLayout::ReverseOrder(bool bReverse)
{
	if(m_eLayoutType == HYLAYOUT_Vertical)
		m_bReverse = !bReverse; // HYLAYOUT_Vertical needs this flipped to achieve top->bottom default
	else
		m_bReverse = bReverse;

	SetLayoutDirty();
}

const HyRectangle<int16> &HyLayout::GetMargins() const
{
	return m_Margins;
}

void HyLayout::SetMargins(int16 iLeft, int16 iBottom, int16 iRight, int16 iTop, uint16 uiWidgetSpacingX, uint16 uiWidgetSpacingY)
{
	m_Margins.Set(iLeft, iBottom, iRight, iTop);
	m_Margins.iTag = uiWidgetSpacingX | (uiWidgetSpacingY << 16);
	
	SetLayoutDirty();
}

uint16 HyLayout::GetHorizontalSpacing()
{
	return m_Margins.iTag & 0x0000FFFF;
}

uint16 HyLayout::GetVerticalSpacing()
{
	return (m_Margins.iTag & 0xFFFF0000) >> 16;
}

glm::ivec2 HyLayout::GetSpacing()
{
	return glm::ivec2(GetHorizontalSpacing(), GetVerticalSpacing());
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

void HyLayout::SetLayoutItems()
{
	GetSizeHint(); // Updates m_vSizeHint (should already be updated at this point)

	HyOrientation eOrientation, eInverseOrien;
	if(m_eLayoutType == HYLAYOUT_Horizontal)
	{
		eOrientation = HYORIEN_Horizontal;
		eInverseOrien = HYORIEN_Vertical;
	}
	else
	{
		eOrientation = HYORIEN_Vertical;
		eInverseOrien = HYORIEN_Horizontal;
	}

	// Determine if preferred size (m_vSizeHint) fits within m_vActualSize
	glm::vec2 vDifference = m_vActualSize - m_vSizeHint;

	// Distrubute positive difference to all 'expanding' size policies
	// Distrubute negative difference to all 'shrink' size policies
	float fExpandAmt = 0.0f;
	float fShrinkAmt = 0.0f;
	if(vDifference[eOrientation] >= 0)
	{
		if(m_vNumExpandItems[eOrientation] > 0)
			fExpandAmt = vDifference[eOrientation] / m_vNumExpandItems[eOrientation];
	}
	else
	{
		if(m_vNumShrinkItems[eOrientation] > 0)
			fShrinkAmt = vDifference[eOrientation] / m_vNumShrinkItems[eOrientation];
	}

	// Finally go through each child and set its position and OnResize()
	glm::vec2 ptCurPos(m_Margins.left, m_Margins.bottom);

	auto fpPositionAndResize = [&](IHyNode2d *&pChildItem) // Lambda func used to iterate over 'm_ChildList'
	{
		// Children are guaranteed to be IHyEntityUi
		IHyEntityUi *pItem = static_cast<IHyEntityUi *>(pChildItem);

		// Set item to the ptCurPos
		pItem->pos.Set(ptCurPos);

		glm::ivec2 vItemSize = pItem->GetSizeHint();
		HySizePolicy eSizePolicy = pItem->GetSizePolicy(eOrientation);

		// Along 'eOrientation', distrubute either 'fExpandAmt' or 'fShrinkAmt' among those with those size policies
		vItemSize[eOrientation] += static_cast<int32>(fExpandAmt * (eSizePolicy & HY_SIZEFLAG_EXPAND));
		vItemSize[eOrientation] += static_cast<int32>(fShrinkAmt * ((eSizePolicy & HY_SIZEFLAG_SHRINK) >> 1));

		// Along 'eInverseOrien', either clamp size or center within available space
		eSizePolicy = pItem->GetSizePolicy(eInverseOrien);
		if(vItemSize[eInverseOrien] <= m_vActualSize[eInverseOrien])
		{
			if((eSizePolicy & HY_SIZEFLAG_EXPAND) != 0)
				vItemSize[eInverseOrien] += m_vActualSize[eInverseOrien] - vItemSize[eInverseOrien];
			else
				pItem->pos.GetAnimFloat(eInverseOrien).Offset(0.5f * (m_vActualSize[eInverseOrien] - vItemSize[eInverseOrien]));
		}
		else
		{
			if((eSizePolicy & HY_SIZEFLAG_SHRINK) != 0)
				vItemSize[eInverseOrien] += m_vActualSize[eInverseOrien] - vItemSize[eInverseOrien];
		}

		vItemSize = pItem->Resize(vItemSize.x, vItemSize.y);

		// After resizing, apply offset to get 'pItem' oriented to its bottom left
		pItem->pos.Offset(pItem->GetPosOffset());

		ptCurPos[eOrientation] += vItemSize[eOrientation] + GetSpacing()[eOrientation];
	};
	if(m_bReverse)
		std::for_each(m_ChildList.rbegin(), m_ChildList.rend(), fpPositionAndResize);
	else
		std::for_each(m_ChildList.begin(), m_ChildList.end(), fpPositionAndResize);

	m_bLayoutDirty = false;
}

/*virtual*/ void HyLayout::OnSetSizeHint() /*override*/
{
	HySetVec(m_vMinSize, m_Margins.left + m_Margins.right, m_Margins.top + m_Margins.bottom);

	m_vSizeHint = m_vMinSize;

	uint32 uiNumChildren = ChildCount();
	if(uiNumChildren == 0)
	{
		m_SizePolicies[HYORIEN_Horizontal] = HYSIZEPOLICY_Flexible;
		m_SizePolicies[HYORIEN_Vertical] = HYSIZEPOLICY_Flexible;
		return;
	}

	HyOrientation eOrientation, eInverseOrien;
	if(m_eLayoutType == HYLAYOUT_Horizontal)
	{
		eOrientation = HYORIEN_Horizontal;
		eInverseOrien = HYORIEN_Vertical;
	}
	else
	{
		eOrientation = HYORIEN_Vertical;
		eInverseOrien = HYORIEN_Horizontal;
	}

	// Figure out m_vSizeHint while counting size policies
	m_vSizeHint[eOrientation] += (GetSpacing()[eOrientation] * (uiNumChildren - 1));
	HySetVec(m_vNumExpandItems, 0, 0);
	HySetVec(m_vNumShrinkItems, 0, 0);

	auto fpPreferredSize = [&](IHyNode2d *&pChildItem) // Lambda func used to iterate over 'm_ChildList'
	{
		// Children are guaranteed to be IHyEntityUi
		IHyEntityUi *pItem = static_cast<IHyEntityUi *>(pChildItem);

		glm::ivec2 vItemMinSize = pItem->GetMinSize();
		m_vMinSize[eOrientation] += vItemMinSize[eOrientation];
		m_vMinSize[eInverseOrien] = HyMax(m_vMinSize[eInverseOrien], vItemMinSize[eInverseOrien]);

		m_vNumExpandItems.x += (pItem->GetHorizontalPolicy() & HY_SIZEFLAG_EXPAND);			// Adds 1 or 0
		m_vNumExpandItems.y += (pItem->GetVerticalPolicy() & HY_SIZEFLAG_EXPAND);			// Adds 1 or 0

		m_vNumShrinkItems.x += ((pItem->GetHorizontalPolicy() & HY_SIZEFLAG_SHRINK) >> 1);	// Adds 1 or 0
		m_vNumShrinkItems.y += ((pItem->GetVerticalPolicy() & HY_SIZEFLAG_SHRINK) >> 1);	// Adds 1 or 0

		glm::ivec2 vItemSizeHint = pItem->GetSizeHint();
		m_vSizeHint[eOrientation] += vItemSizeHint[eOrientation];
		m_vSizeHint[eInverseOrien] = HyMax(m_vSizeHint[eInverseOrien], vItemSizeHint[eInverseOrien]);
	};
	std::for_each(m_ChildList.begin(), m_ChildList.end(), fpPreferredSize);

	m_SizePolicies[eOrientation] = m_vNumShrinkItems[eOrientation] == 0 ? HYSIZEPOLICY_Expanding : HYSIZEPOLICY_Flexible;
	m_SizePolicies[eInverseOrien] = m_vNumShrinkItems[eInverseOrien] == 0 ? HYSIZEPOLICY_Expanding : HYSIZEPOLICY_Flexible;
}

/*virtual*/ glm::ivec2 HyLayout::OnResize(uint32 uiNewWidth, uint32 uiNewHeight) /*override*/
{
	if(m_vActualSize.x == uiNewWidth && m_vActualSize.y == uiNewHeight)
		return;

	HySetVec(m_vActualSize, static_cast<int32>(uiNewWidth), static_cast<int32>(uiNewHeight));
	SetLayoutItems();

	return m_vActualSize;
}
