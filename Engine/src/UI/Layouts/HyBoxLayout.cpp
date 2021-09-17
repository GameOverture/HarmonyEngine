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
	return m_vSizeHint;
}

void HyBoxLayout::AppendItem(HyEntityUi &itemRef)
{
	ChildAppend(itemRef);
	OnSetLayoutItems();
}

void HyBoxLayout::ReverseOrder(bool bReverse)
{
	if(m_eOrientation == HYORIEN_Horizontal)
		m_bReverse = bReverse;
	else
		m_bReverse = !bReverse; // HYORIEN_Vertical needs this flipped to achieve top->bottom default

	OnSetLayoutItems();
}

/*virtual*/ void HyBoxLayout::OnSetLayoutItems() /*override*/
{
	uint32 uiNumChildren = ChildCount();
	if(uiNumChildren == 0)
		return;

	int32 iInverseOrien = m_eOrientation ? 0 : 1;

	// Figure out m_vSizeHint while counting size policies
	m_vSizeHint.x = m_Margins.left + m_Margins.right;
	m_vSizeHint.y = m_Margins.top + m_Margins.bottom;
	m_vSizeHint[m_eOrientation] += (GetSpacing()[m_eOrientation] * (uiNumChildren - 1));
	glm::ivec2 vNumGrowItems;
	glm::ivec2 vNumExpandItems;
	glm::ivec2 vNumShrinkItems;
	glm::ivec2 vNumNoneItems;

	auto fpPreferredSize = [&](IHyNode2d *&pChildItem) // Lambda func used to iterate over 'm_ChildList'
	{
		// Children are guaranteed to be HyEntityUi
		HyEntityUi *pItem = static_cast<HyEntityUi *>(pChildItem);
		glm::ivec2 vItemSizeHint = pItem->GetSizeHint();
			
		if(pItem->GetUiType() == Ui_Widget)
		{
			IHyWidget *pWidget = static_cast<IHyWidget *>(pItem);

			vNumGrowItems.x += (pWidget->GetHorizontalPolicy() & HY_SIZEFLAG_GROW);				// Adds 1 or 0
			vNumGrowItems.y += (pWidget->GetVerticalPolicy() & HY_SIZEFLAG_GROW);				// Adds 1 or 0

			vNumExpandItems.x += ((pWidget->GetHorizontalPolicy() & HY_SIZEFLAG_EXPAND) >> 1);	// Adds 1 or 0
			vNumExpandItems.y += ((pWidget->GetVerticalPolicy() & HY_SIZEFLAG_EXPAND) >> 1);	// Adds 1 or 0

			vNumShrinkItems.x += ((pWidget->GetHorizontalPolicy() & HY_SIZEFLAG_SHRINK) >> 2);	// Adds 1 or 0
			vNumShrinkItems.y += ((pWidget->GetVerticalPolicy() & HY_SIZEFLAG_SHRINK) >> 2);	// Adds 1 or 0

			vNumNoneItems.x += ((pWidget->GetHorizontalPolicy() & HY_SIZEFLAG_IGNORE) >> 3);	// Adds 1 or 0
			vNumNoneItems.y += ((pWidget->GetVerticalPolicy() & HY_SIZEFLAG_IGNORE) >> 3);		// Adds 1 or 0

			// Zero out vItemSizeHint dimensions if that size policy has IGNORE flag
			vItemSizeHint.x *= (((pWidget->GetHorizontalPolicy() & HY_SIZEFLAG_IGNORE) >> 3) ^ 1);	// Multiply by 1 or 0
			vItemSizeHint.y *= (((pWidget->GetVerticalPolicy() & HY_SIZEFLAG_IGNORE) >> 3) ^ 1);	// Multiply by 1 or 0
		}
		else // Ui_Layout
		{
			vNumGrowItems.x++; vNumGrowItems.y++;
			vNumExpandItems.x++; vNumExpandItems.y++;
			vNumShrinkItems.x++; vNumShrinkItems.y++;
		}

		m_vSizeHint[m_eOrientation] += vItemSizeHint[m_eOrientation];
		m_vSizeHint[iInverseOrien] = HyMax(m_vSizeHint[iInverseOrien], vItemSizeHint[iInverseOrien]);
	};
	if(m_bReverse)
		std::for_each(m_ChildList.rbegin(), m_ChildList.rend(), fpPreferredSize);
	else
		std::for_each(m_ChildList.begin(), m_ChildList.end(), fpPreferredSize);


	// Determine if preferred size (m_vSizeHint) fits within m_vSize
	//	- Distrubute positive difference to all 'expanding' then 'grow' size policies
	//	- Distrubute negative difference to all 'fill' then 'shrink' size policies
	glm::vec2 vDifference = m_vSize - m_vSizeHint;

	float fGrowAmt = 0.0f;
	float fExpandAmt = 0.0f;
	float fShrinkAmt = 0.0f;
	if(vDifference[m_eOrientation] >= 0)
	{
		if(vNumExpandItems[m_eOrientation] > 0)
			fExpandAmt = vDifference[m_eOrientation] / vNumExpandItems[m_eOrientation];
		else if(vNumGrowItems[m_eOrientation] > 0)
			fGrowAmt = vDifference[m_eOrientation] / vNumGrowItems[m_eOrientation];
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

			// Resize this item based on all the final calculations of size policies and size constraints
			HySizePolicy eSizePolicy = pWidget->GetSizePolicy(static_cast<HyOrientation>(m_eOrientation));
			vItemSize[m_eOrientation] += static_cast<int32>(fGrowAmt * (eSizePolicy & HY_SIZEFLAG_GROW));
			vItemSize[m_eOrientation] += static_cast<int32>(fExpandAmt * ((eSizePolicy & HY_SIZEFLAG_EXPAND) >> 1));
			vItemSize[m_eOrientation] += static_cast<int32>(fShrinkAmt * ((eSizePolicy & HY_SIZEFLAG_SHRINK) >> 2));

			HySizePolicy eInverseSizePolicy = pWidget->GetSizePolicy(static_cast<HyOrientation>(iInverseOrien));
			if(vDifference[iInverseOrien] >= 0)
			{
				if((eInverseSizePolicy & HY_SIZEFLAG_EXPAND) != 0)
					vItemSize[iInverseOrien] += m_vSize[iInverseOrien] - vItemSize[iInverseOrien];
				else
					pItem->pos.GetAnimFloat(iInverseOrien).Offset(0.5f * (m_vSize[iInverseOrien] - vItemSize[iInverseOrien]));
			}
			else
			{
				if((eInverseSizePolicy & HY_SIZEFLAG_SHRINK) != 0)
					vItemSize[iInverseOrien] += m_vSize[iInverseOrien] - vItemSize[iInverseOrien];
			}

			pWidget->OnResize(vItemSize.x, vItemSize.y);

			// Apply its local offset
			pItem->pos.Offset(pWidget->GetPosOffset());
		}
		else // Ui_Layout
		{
			IHyLayout *pLayout = static_cast<IHyLayout *>(pItem);
			vItemSize = pLayout->GetSizeHint();

			vItemSize[m_eOrientation] += static_cast<int32>(fGrowAmt);
			vItemSize[m_eOrientation] += static_cast<int32>(fExpandAmt);
			vItemSize[m_eOrientation] += static_cast<int32>(fShrinkAmt);

			vItemSize[iInverseOrien] += m_vSize[iInverseOrien] - vItemSize[iInverseOrien];

			HyInternal_LayoutSetSize(*pLayout, vItemSize.x, vItemSize.y);
		}

		ptCurPos[m_eOrientation] += vItemSize[m_eOrientation] + GetSpacing()[m_eOrientation];
	};
	if(m_bReverse)
		std::for_each(m_ChildList.rbegin(), m_ChildList.rend(), fpPositionAndResize);
	else
		std::for_each(m_ChildList.begin(), m_ChildList.end(), fpPositionAndResize);
}
