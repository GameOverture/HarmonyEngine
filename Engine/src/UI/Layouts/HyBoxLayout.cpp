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

HyBoxLayout::HyBoxLayout(Orientation eOrientation, HyEntity2d *pParent /*= nullptr*/) :
	IHyLayout(pParent),
	m_eOrientation(eOrientation)
{
}

/*virtual*/ HyBoxLayout::~HyBoxLayout()
{
}

/*virtual*/ void HyBoxLayout::OnDoLayout() /*override*/
{
	uint32 uiNumChildren = ChildCount();
	if(uiNumChildren == 0)
		return;

	uint32 uiNumCols = 0;
	uint32 uiNumRows = 0;
	if(m_eOrientation == Horizontal)
	{
		uiNumCols = uiNumChildren;
		uiNumRows = 1;
	}
	else
	{
		uiNumCols = 1;
		uiNumRows = uiNumChildren;
	}

	// First determine the preferred length of all the widgets together, and tally their size policies
	glm::ivec2 vPreferredSize(m_Margins.left + m_Margins.right + ((uiNumCols-1) * GetHorizontalSpacing()),
								m_Margins.top + m_Margins.bottom + ((uiNumRows-1) * GetHorizontalSpacing()));
	glm::ivec2 vNumGrowItems;
	glm::ivec2 vNumExpandItems;
	glm::ivec2 vNumShrinkItems;
	glm::ivec2 vNumFillItems;
	for(uint32 i = 0; i < uiNumChildren; ++i)
	{
		// Children are guaranteed to be IHyLayoutItem
		IHyLayoutItem *pItem = static_cast<IHyLayoutItem *>(m_ChildList[i]);

		vPreferredSize.x += pItem->GetSizeHint().x;
		vPreferredSize.y = HyMax(vPreferredSize.y, pItem->GetSizeHint().y);

		vNumGrowItems.x += (pItem->GetHorizontalPolicy() & Grow);
		vNumGrowItems.y += (pItem->GetVerticalPolicy() & Grow);

		vNumExpandItems.x += ((pItem->GetHorizontalPolicy() & Expand) >> 1);
		vNumExpandItems.y += ((pItem->GetVerticalPolicy() & Expand) >> 1);

		vNumShrinkItems.x += ((pItem->GetHorizontalPolicy() & Shrink) >> 2);
		vNumShrinkItems.y += ((pItem->GetVerticalPolicy() & Shrink) >> 2);
			
		vNumFillItems.x += ((pItem->GetHorizontalPolicy() & Fill) >> 3);
		vNumFillItems.y += ((pItem->GetVerticalPolicy() & Fill) >> 3);
	}

	// Determine if preferred length fits within m_vSize
	//	- Distrubute positive difference to all 'expanding' then 'fill' size policies
	//	- Distrubute negative difference to all 'fill' then 'shrink' size policies
	glm::ivec2 vDifference = m_vSize - vPreferredSize;

		if(iDifference > 0)
		{
			int32 iExpandAmt = iDifference;
			if(expandItemList.empty() == false)
			{
				iExpandAmt /= expandItemList.size();
				int32 iRemainder = iDifference % expandItemList.size();

				for(uint32 i = 0; i < iNumChildren; ++i)
				{
					IHyLayoutItem *pItem = static_cast<IHyLayoutItem *>(m_ChildList[i]);
					auto vSize = pItem->GetSizeHint();
					vSize.x += (pItem->GetHorizontalPolicy() & Expand) ? iExpandAmt + iRemainder : 0;
					iRemainder = 0;

					pItem->OnResize(
				}
			}
		}
	}
}
