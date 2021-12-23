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
#include "UI/Containers/HyContainer.h"
#include "UI/Widgets/IHyWidget.h"
#include "Diagnostics/Console/IHyConsole.h"

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

HyLayoutType HyLayout::GetLayoutType() const
{
	return m_eLayoutType;
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

bool HyLayout::RequestWidgetFocus(IHyWidget *pWidget)
{
	// Propagate upward if this is nested in another layout
	if(m_pParent)
	{
		if((m_pParent->GetInternalFlags() & NODETYPE_IsLayout) != 0)
			return static_cast<HyLayout *>(m_pParent)->RequestWidgetFocus(pWidget);
		else // m_pParent must be the container (and *this is the root layout)
			return static_cast<HyContainer *>(m_pParent)->RequestWidgetFocus(pWidget);
	}
}

bool HyLayout::IsWidgetInputAllowed()
{
	// Propagate upward if this is nested in another layout
	if(m_pParent)
	{
		if((m_pParent->GetInternalFlags() & NODETYPE_IsLayout) != 0)
			return static_cast<HyLayout *>(m_pParent)->IsWidgetInputAllowed();
		else // m_pParent must be the container (and *this is the root layout)
			return static_cast<HyContainer *>(m_pParent)->IsInputAllowed();
	}

	// This shouldn't happen
	return false;
}

/*virtual*/ void HyLayout::OnSetSizeHint() /*override*/
{
	HySetVec(m_vMinSize, m_Margins.left + m_Margins.right, m_Margins.top + m_Margins.bottom);
	m_vSizeHint = m_vMinSize;

	uint32 uiNumChildren = ChildCount();
	if(uiNumChildren == 0)
		return;

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
	m_uiNumExpandItems = m_uiNumShrinkItems = 0;

	auto fpPreferredSize = [&](IHyNode2d *&pChildItem) // Lambda func used to iterate over 'm_ChildList'
	{
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
	};
	std::for_each(m_ChildList.begin(), m_ChildList.end(), fpPreferredSize);
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
	int32 iOrienMargin, iInverseOrienMargin;
	if(m_eLayoutType == HYLAYOUT_Horizontal)
	{
		eOrientation = HYORIEN_Horizontal;
		eInverseOrien = HYORIEN_Vertical;
		
		iOrienMargin = m_Margins.left + m_Margins.right;
		iInverseOrienMargin = m_Margins.top + m_Margins.bottom;
	}
	else
	{
		eOrientation = HYORIEN_Vertical;
		eInverseOrien = HYORIEN_Horizontal;
		
		iOrienMargin = m_Margins.top + m_Margins.bottom;
		iInverseOrienMargin = m_Margins.left + m_Margins.right;
	}


	float fExpandAmt = 0.0f, fShrinkAmt = 0.0f;
	if(vTargetSize[eOrientation] != 0)
		GetDistributedScalingAmts(vTargetSize[eOrientation], GetSizeHint()[eOrientation], fExpandAmt, fShrinkAmt);

	// Go through each child and set its position and OnResize()
	glm::vec2 ptCurPos(m_Margins.left, m_Margins.bottom);
	int32 iMaxInverse = 0; // Used to determine the m_vActualSize when the inverse demension was specified as '0'
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
			// Along 'eOrientation', distrubute either 'fExpandAmt' or 'fShrinkAmt' among those with those size policies
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
			//switch(static_cast<HyLayout *>(pItem)->GetLayoutType())
			//{
			//case HYLAYOUT_Horizontal:
			//	
			//	break;
			//	
			//case HYLAYOUT_Vertical:
			//	vResize.y = 0;
			//	break;

			//default:
			//	HyLogWarning("Layout type not handled");
			//	break;
			//}
		}
		glm::ivec2 vActualItemSize = pItem->Resize(vResize.x, vResize.y);
		if(vTargetSize[eOrientation] != 0 && vActualItemSize != vItemSize)
			bNeedsResize = true;

		// After resizing, apply offset to get 'pItem' oriented to its bottom left
		pItem->pos.Offset(pItem->GetPosOffset());

		ptCurPos[eOrientation] += vActualItemSize[eOrientation] + GetSpacing()[eOrientation];
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
		vTargetSize[eOrientation] = ptCurPos[eOrientation] - GetSpacing()[eOrientation];
		HySetVec(ptCurPos, m_Margins.left, m_Margins.bottom);

		GetDistributedScalingAmts(vTargetSize[eOrientation], GetSizeHint()[eOrientation], fExpandAmt, fShrinkAmt);

		if(m_bReverse)
			std::for_each(m_ChildList.rbegin(), m_ChildList.rend(), fpPositionAndResize);
		else
			std::for_each(m_ChildList.begin(), m_ChildList.end(), fpPositionAndResize);
	}

	m_bLayoutDirty = false;

	m_vActualSize[eOrientation] = ptCurPos[eOrientation] - GetSpacing()[eOrientation];
	m_vActualSize[eInverseOrien] = vTargetSize[eInverseOrien];
	return m_vActualSize;
}

void HyLayout::GetDistributedScalingAmts(int32 iTargetLength, int32 iCurrLength, float &fExpandAmtOut, float &fShrinkAmtOut)
{
	// Determine if current length fits within iTargetLength
	float fDifference = static_cast<float>(iTargetLength) - static_cast<float>(iCurrLength);
	
	fExpandAmtOut = 0.0f;
	fShrinkAmtOut = 0.0f;

	// Distrubute positive difference to all 'expanding' size policies
	if(fDifference >= 0)
	{
		if(m_uiNumExpandItems > 0)
			fExpandAmtOut = fDifference / m_uiNumExpandItems;
	}
	else // Distrubute negative difference to all 'shrink' size policies
	{
		if(m_uiNumShrinkItems > 0)
			fShrinkAmtOut = fDifference / m_uiNumShrinkItems;
	}
}
