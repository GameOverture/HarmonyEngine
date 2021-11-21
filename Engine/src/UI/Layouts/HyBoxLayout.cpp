/**************************************************************************
*	HyBoxLayout.cpp
*
*	Harmony Engine
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "UI/Layouts/HyBoxLayout.h"
#include "UI/Widgets/IHyWidget.h"

HyBoxLayout::HyBoxLayout(HyOrientation eOrientation, HyEntity2d *pParent /*= nullptr*/) :
	IHyLayout(eOrientation == HYORIEN_Horizontal ? HYLAYOUT_Horizontal : HYLAYOUT_Vertical, pParent),
	m_eOrientation(eOrientation),
	m_bReverse(eOrientation == HYORIEN_Horizontal ? false : true) // 'm_bReverse' is defaulted ON when 'm_eOrientation' is HYORIEN_Vertical to achieve top->bottom as default
{
}

/*virtual*/ HyBoxLayout::~HyBoxLayout()
{
}

/*virtual*/ glm::ivec2 HyBoxLayout::GetSizeHint() /*override*/
{
	//if(ChildCount() != 0 && m_vSizeHint.x == 0 && m_vSizeHint.y == 0)
		OnSetLayoutItems();

	return m_vSizeHint;
}

void HyBoxLayout::ReverseOrder(bool bReverse)
{
	if(m_eOrientation == HYORIEN_Horizontal)
		m_bReverse = bReverse;
	else
		m_bReverse = !bReverse; // HYORIEN_Vertical needs this flipped to achieve top->bottom default

	SetLayoutItems();
}

/*virtual*/ void HyBoxLayout::OnSetLayoutItems() /*override*/
{
	uint32 uiNumChildren = ChildCount();
	if(uiNumChildren == 0)
		return;

	HyOrientation eInverseOrien = m_eOrientation ? HYORIEN_Horizontal : HYORIEN_Vertical;

	// Figure out m_vSizeHint while counting size policies
	m_vSizeHint.x = m_Margins.left + m_Margins.right;
	m_vSizeHint.y = m_Margins.top + m_Margins.bottom;
	m_vSizeHint[m_eOrientation] += (GetSpacing()[m_eOrientation] * (uiNumChildren - 1));
	glm::ivec2 vNumExpandItems;
	glm::ivec2 vNumShrinkItems;

	auto fpPreferredSize = [&](IHyNode2d *&pChildItem) // Lambda func used to iterate over 'm_ChildList'
	{
		// Children are guaranteed to be HyEntityUi
		HyEntityUi *pItem = static_cast<HyEntityUi *>(pChildItem);
		glm::ivec2 vItemSizeHint = pItem->GetSizeHint();
			
		if(pItem->GetUiType() == Ui_Widget)
		{
			IHyWidget *pWidget = static_cast<IHyWidget *>(pItem);

			vNumExpandItems.x += (pWidget->GetHorizontalPolicy() & HY_SIZEFLAG_EXPAND);			// Adds 1 or 0
			vNumExpandItems.y += (pWidget->GetVerticalPolicy() & HY_SIZEFLAG_EXPAND);			// Adds 1 or 0

			vNumShrinkItems.x += ((pWidget->GetHorizontalPolicy() & HY_SIZEFLAG_SHRINK) >> 1);	// Adds 1 or 0
			vNumShrinkItems.y += ((pWidget->GetVerticalPolicy() & HY_SIZEFLAG_SHRINK) >> 1);	// Adds 1 or 0
		}
		else // Ui_Layout
		{
			vNumExpandItems.x++; vNumExpandItems.y++;
			vNumShrinkItems.x++; vNumShrinkItems.y++;
		}

		m_vSizeHint[m_eOrientation] += vItemSizeHint[m_eOrientation];
		m_vSizeHint[eInverseOrien] = HyMax(m_vSizeHint[eInverseOrien], vItemSizeHint[eInverseOrien]);
	};
	if(m_bReverse)
		std::for_each(m_ChildList.rbegin(), m_ChildList.rend(), fpPreferredSize);
	else
		std::for_each(m_ChildList.begin(), m_ChildList.end(), fpPreferredSize);


	// Determine if preferred size (m_vSizeHint) fits within m_vSize (any '0' component of m_vSize means to use m_vSizeHint instead)
	glm::ivec2 vActualSize = m_vSize;
	if(vActualSize.x == 0)
		vActualSize.x = m_vSizeHint.x;
	if(vActualSize.y == 0)
		vActualSize.y = m_vSizeHint.y;

	glm::vec2 vDifference = vActualSize - m_vSizeHint;

	// Distrubute positive difference to all 'expanding' size policies
	// Distrubute negative difference to all 'shrink' size policies
	float fExpandAmt = 0.0f;
	float fShrinkAmt = 0.0f;
	if(vDifference[m_eOrientation] >= 0)
	{
		if(vNumExpandItems[m_eOrientation] > 0)
			fExpandAmt = vDifference[m_eOrientation] / vNumExpandItems[m_eOrientation];
	}
	else
	{
		if(vNumShrinkItems[m_eOrientation] > 0)
			fShrinkAmt = vDifference[m_eOrientation] / vNumShrinkItems[m_eOrientation];
	}

	// Finally go through each child and set its position and OnResize()
	glm::vec2 ptCurPos(m_Margins.left, m_Margins.bottom);

	auto fpPositionAndResize = [&](IHyNode2d *&pChildItem) // Lambda func used to iterate over 'm_ChildList'
	{
		// Children are guaranteed to be HyEntityUi
		HyEntityUi *pItem = static_cast<HyEntityUi *>(pChildItem);

		// Set item to the ptCurPos
		pItem->pos.Set(ptCurPos);

		glm::ivec2 vItemSize;
		if(pItem->GetUiType() == Ui_Widget)
		{
			IHyWidget *pWidget = static_cast<IHyWidget *>(pItem);
			vItemSize = pWidget->GetSizeHint();

			// Along 'm_eOrientation', distrubute either 'fExpandAmt' or 'fShrinkAmt' among those with those size policies
			HySizePolicy eSizePolicy = pWidget->GetSizePolicy(m_eOrientation);
			vItemSize[m_eOrientation] += static_cast<int32>(fExpandAmt * (eSizePolicy & HY_SIZEFLAG_EXPAND));
			vItemSize[m_eOrientation] += static_cast<int32>(fShrinkAmt * ((eSizePolicy & HY_SIZEFLAG_SHRINK) >> 1));

			// Along 'eInverseOrien', either clamp size or center within available space
			eSizePolicy = pWidget->GetSizePolicy(eInverseOrien);
			if(vItemSize[eInverseOrien] <= vActualSize[eInverseOrien])
			{
				if((eSizePolicy & HY_SIZEFLAG_EXPAND) != 0)
					vItemSize[eInverseOrien] += vActualSize[eInverseOrien] - vItemSize[eInverseOrien];
				else
					pItem->pos.GetAnimFloat(eInverseOrien).Offset(0.5f * (vActualSize[eInverseOrien] - vItemSize[eInverseOrien]));
			}
			else
			{
				if((eSizePolicy & HY_SIZEFLAG_SHRINK) != 0)
					vItemSize[eInverseOrien] += vActualSize[eInverseOrien] - vItemSize[eInverseOrien];
			}

			vItemSize = pWidget->OnResize(vItemSize.x, vItemSize.y); // This doesn't always set the size 

			// Apply its local offset
			pItem->pos.Offset(pWidget->GetPosOffset());
		}
		else // Ui_Layout
		{
			IHyLayout *pLayout = static_cast<IHyLayout *>(pItem);

			vItemSize = pLayout->GetSizeHint();

			//vItemSize[m_eOrientation] += static_cast<int32>(fGrowAmt);
			vItemSize[m_eOrientation] += static_cast<int32>(fExpandAmt);
			vItemSize[m_eOrientation] += static_cast<int32>(fShrinkAmt);

			if(vItemSize[eInverseOrien] > vActualSize[eInverseOrien])
				vItemSize[eInverseOrien] = vActualSize[eInverseOrien];

			vItemSize[eInverseOrien] += vActualSize[eInverseOrien] - vItemSize[eInverseOrien];
			pItem->pos.GetAnimFloat(eInverseOrien).Offset(0.5f * (vActualSize[eInverseOrien] - vItemSize[eInverseOrien]));

			HyInternal_LayoutSetSize(*pLayout, vItemSize.x, vItemSize.y);
		}

		ptCurPos[m_eOrientation] += vItemSize[m_eOrientation] + GetSpacing()[m_eOrientation];
	};
	if(m_bReverse)
		std::for_each(m_ChildList.rbegin(), m_ChildList.rend(), fpPositionAndResize);
	else
		std::for_each(m_ChildList.begin(), m_ChildList.end(), fpPositionAndResize);
}
