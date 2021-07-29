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
	m_eOrientation(eOrientation)
{
}

/*virtual*/ HyBoxLayout::~HyBoxLayout()
{
}

void HyBoxLayout::AppendItem(HyEntityUi &itemRef)
{
	ChildAppend(itemRef);
	OnSetLayoutItems();
}

/*virtual*/ void HyBoxLayout::OnSetLayoutItems() /*override*/
{
	uint32 uiNumChildren = ChildCount();
	if(uiNumChildren == 0)
		return;

	int32 iInverseOrien = m_eOrientation ? 0 : 1;

	// Figure out m_vPreferredSize while counting size policies
	m_vPreferredSize.x = m_Margins.left + m_Margins.right;
	m_vPreferredSize.y = m_Margins.top + m_Margins.bottom;
	m_vPreferredSize[m_eOrientation] += (GetSpacing()[m_eOrientation] * (uiNumChildren - 1));
	glm::ivec2 vNumGrowItems;
	glm::ivec2 vNumExpandItems;
	glm::ivec2 vNumShrinkItems;
	glm::ivec2 vNumNoneItems;
	
	for(uint32 i = 0; i < uiNumChildren; ++i)
	{
		// Children are guaranteed to be HyEntityUi
		HyEntityUi *pItem = static_cast<HyEntityUi *>(m_ChildList[i]);
		glm::ivec2 vItemSizeHint;
			
		if(pItem->GetUiType() == Ui_Widget)
		{
			IHyWidget *pWidget = static_cast<IHyWidget *>(pItem);
			vItemSizeHint = pWidget->GetSizeHint();

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
			vItemSizeHint = static_cast<IHyLayout *>(pItem)->GetPreferredSize();
			vNumGrowItems.x++; vNumGrowItems.y++;
			vNumExpandItems.x++; vNumExpandItems.y++;
			vNumShrinkItems.x++; vNumShrinkItems.y++;
		}

		m_vPreferredSize[m_eOrientation] += vItemSizeHint[m_eOrientation];
		m_vPreferredSize[iInverseOrien] = HyMax(m_vPreferredSize[iInverseOrien], vItemSizeHint[iInverseOrien]);
	}

	// Determine if preferred size (m_vPreferredSize) fits within m_vSize
	//	- Distrubute positive difference to all 'expanding' then 'grow' size policies
	//	- Distrubute negative difference to all 'fill' then 'shrink' size policies
	glm::vec2 vDifference = m_vSize - m_vPreferredSize;

	float vGrowAmt = 0.0f;
	float vExpandAmt = 0.0f;
	float vShrinkAmt = 0.0f;
	if(vDifference[m_eOrientation] >= 0)
	{
		if(vNumExpandItems[m_eOrientation] > 0)
			vExpandAmt = vDifference[m_eOrientation] / vNumExpandItems[m_eOrientation];
		else if(vNumGrowItems[m_eOrientation] > 0)
			vGrowAmt = vDifference[m_eOrientation] / vNumGrowItems[m_eOrientation];
	}
	else
	{
		if(vNumShrinkItems[m_eOrientation] > 0)
			vShrinkAmt = vDifference[m_eOrientation] / vNumShrinkItems[m_eOrientation];
	}

	// Finally go through each child and set its position and OnResize()
	glm::vec2 ptCurPos(m_Margins.left, m_Margins.bottom);
	for(uint32 i = 0; i < uiNumChildren; ++i)
	{
		// Children are guaranteed to be HyEntityUi
		HyEntityUi *pItem = static_cast<HyEntityUi *>(m_ChildList[i]);

		// Set item to the ptCurPos
		pItem->pos.Set(ptCurPos);

		glm::ivec2 vItemSize;
		if(pItem->GetUiType() == Ui_Widget)
		{
			IHyWidget *pWidget = static_cast<IHyWidget *>(pItem);
			vItemSize = pWidget->GetSizeHint();

			// Resize this item based on all the final calculations of size policies and size constraints
			HySizePolicy eSizePolicy = pWidget->GetSizePolicy(static_cast<HyOrientation>(m_eOrientation));
			vItemSize[m_eOrientation] += static_cast<int32>(vGrowAmt * (eSizePolicy & HY_SIZEFLAG_GROW));
			vItemSize[m_eOrientation] += static_cast<int32>(vExpandAmt * ((eSizePolicy & HY_SIZEFLAG_EXPAND) >> 1));
			vItemSize[m_eOrientation] += static_cast<int32>(vShrinkAmt * ((eSizePolicy & HY_SIZEFLAG_SHRINK) >> 2));

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
			vItemSize = pLayout->GetPreferredSize();

			vItemSize[m_eOrientation] += static_cast<int32>(vGrowAmt);
			vItemSize[m_eOrientation] += static_cast<int32>(vExpandAmt);
			vItemSize[m_eOrientation] += static_cast<int32>(vShrinkAmt);

			vItemSize[iInverseOrien] += m_vSize[iInverseOrien] - vItemSize[iInverseOrien];

			pLayout->SetSize(vItemSize.x, vItemSize.y);
		}

		ptCurPos[m_eOrientation] += vItemSize[m_eOrientation] + GetSpacing()[m_eOrientation];
	}
}
