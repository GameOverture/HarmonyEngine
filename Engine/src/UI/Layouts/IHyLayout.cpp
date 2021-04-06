/**************************************************************************
*	IHyLayout.cpp
*
*	Harmony Engine
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "UI/Layouts/IHyLayout.h"

IHyLayout::IHyLayout(HyEntity2d *pParent /*= nullptr*/) :
	HyUI(pParent)
{
}

/*virtual*/ IHyLayout::~IHyLayout()
{
}

void IHyLayout::SetSize(int32 iWidth, int32 iHeight)
{
	HySetVec(m_vSize, iWidth, iHeight);
	OnSetLayoutItems();
}

void IHyLayout::SetMargins(int32 iLeft, int32 iTop, int32 iRight, int32 iBottom, uint16 uiWidgetSpacingX, uint16 uiWidgetSpacingY)
{
	m_Margins.Set(iLeft, iTop, iRight, iBottom);
	m_Margins.iTag = uiWidgetSpacingX | (uiWidgetSpacingY << 16);
	OnSetLayoutItems();
}

uint16 IHyLayout::GetHorizontalSpacing()
{
	return m_Margins.iTag & 0x0000FFFF;
}

uint16 IHyLayout::GetVerticalSpacing()
{
	return (m_Margins.iTag & 0xFFFF0000) >> 16;
}

/*virtual*/ void IHyLayout::OnResize(int32 iNewWidth, int32 iNewHeight) /*override*/
{
	SetSize(iNewWidth, iNewHeight);
}

void IHyLayout::SetLayoutItems(uint32 uiNumRows, uint32 uiNumCols)
{
	// First determine the preferred length of all the layout children together, and tally their size policies
	glm::ivec2 vNumGrowItems;
	glm::ivec2 vNumExpandItems;
	glm::ivec2 vNumShrinkItems;
	glm::ivec2 vNumNoneItems;
	
	int32 iPrefRowSize = 0;
	int32 iPrefColSize = 0;
	int32 *pCurColSizes = HY_NEW int32[uiNumCols];
	memset(pCurColSizes, 0, sizeof(int32) * uiNumCols);
	for(uint32 iCol = 0; iCol < uiNumCols; ++iCol)
	{
		int32 iCurRowSize = 0;
		for(uint32 iRow = 0; iRow < uiNumRows; ++iRow)
		{
			// Children are guaranteed to be HyUI
			HyUI *pItem = static_cast<HyUI *>(m_ChildList[iRow + (iCol * uiNumRows)]);

			glm::ivec2 vItemSizeHint = pItem->GetSizeHint();
			iCurRowSize += vItemSizeHint.x;
			pCurColSizes[iCol] += vItemSizeHint.y;

			vNumGrowItems.x += (pItem->GetHorizontalPolicy() & Grow);				// Adds 1 or 0
			vNumGrowItems.y += (pItem->GetVerticalPolicy() & Grow);					// Adds 1 or 0

			vNumExpandItems.x += ((pItem->GetHorizontalPolicy() & Expand) >> 1);	// Adds 1 or 0
			vNumExpandItems.y += ((pItem->GetVerticalPolicy() & Expand) >> 1);		// Adds 1 or 0

			vNumShrinkItems.x += ((pItem->GetHorizontalPolicy() & Shrink) >> 2);	// Adds 1 or 0
			vNumShrinkItems.y += ((pItem->GetVerticalPolicy() & Shrink) >> 2);		// Adds 1 or 0

			vNumNoneItems.x += ((pItem->GetHorizontalPolicy() & None) >> 3);		// Adds 1 or 0
			vNumNoneItems.y += ((pItem->GetVerticalPolicy() & None) >> 3);			// Adds 1 or 0
		}
		
		iPrefRowSize = HyMax(iPrefRowSize, iCurRowSize);
	}

	for(int i = 0; i < uiNumCols; ++i)
		iPrefColSize = HyMax(iPrefColSize, pCurColSizes[i]);
	delete[] pCurColSizes;

	m_vSizeHint.x = m_Margins.left + m_Margins.right + ((uiNumCols - 1) * GetHorizontalSpacing());
	m_vSizeHint.x += iPrefRowSize;
	m_vSizeHint.y = m_Margins.top + m_Margins.bottom + ((uiNumRows - 1) * GetVerticalSpacing());
	m_vSizeHint.y += iPrefColSize;

	// Adjust this layout's size if allowed
	if(m_vSize.x < m_vSizeHint.x && (GetHorizontalPolicy() & (Grow | Expand)) != 0)
		m_vSize.x = m_vSizeHint.x;
	else if(m_vSize.x > m_vSizeHint.x && (GetHorizontalPolicy() & Shrink) != 0)
		m_vSize.x = m_vSizeHint.x;

	if(m_vSize.y < m_vSizeHint.y && (GetVerticalPolicy() & (Grow | Expand)) != 0)
		m_vSize.y = m_vSizeHint.y;
	else if(m_vSize.y > m_vSizeHint.y && (GetVerticalPolicy() & Shrink) != 0)
		m_vSize.y = m_vSizeHint.y;

	// Determine if preferred length fits within m_vSize
	//	- Distrubute positive difference to all 'expanding' then 'grow' size policies
	//	- Distrubute negative difference to all 'fill' then 'shrink' size policies
	glm::vec2 vDifference = m_vSize - m_vSizeHint;

	glm::vec2 vGrowAmt(0.0f, 0.0f);
	glm::vec2 vExpandAmt(0.0f, 0.0f);
	glm::vec2 vShrinkAmt(0.0f, 0.0f);
	for(int32 iDimen = Horizontal; iDimen < NumOrientations; ++iDimen)
	{
		if(vDifference[iDimen] >= 0)
		{
			if(vNumExpandItems[iDimen] > 0)
				vExpandAmt[iDimen] = vDifference[iDimen] / vNumExpandItems[iDimen];
			else if(vNumGrowItems[iDimen] > 0)
				vGrowAmt[iDimen] = vDifference[iDimen] / vNumGrowItems[iDimen];
		}
		else
		{
			if(vNumShrinkItems[iDimen] > 0)
				vShrinkAmt[iDimen] = vDifference[iDimen] / vNumShrinkItems[iDimen];
		}
	}

	// Finally go through each child layout item and set its position and OnResize()
	glm::vec2 ptCurPos(0.0f, 0.0f);
	for(uint32 iRow = 0; iRow < uiNumRows; ++iRow)
	{
		float fRowHeight = 0.0f;
		for(uint32 iCol = 0; iCol < uiNumCols; ++iCol)
		{
			HyUI *pItem = static_cast<HyUI *>(m_ChildList[iRow + (iCol * uiNumRows)]);

			glm::ivec2 vItemSize = pItem->GetSizeHint();
			vItemSize.x += vGrowAmt.x * static_cast<float>(pItem->GetHorizontalPolicy() & Grow);
			vItemSize.y += vGrowAmt.y * static_cast<float>(pItem->GetVerticalPolicy() & Grow);
			vItemSize.x += vExpandAmt.x * static_cast<float>((pItem->GetHorizontalPolicy() & Expand) >> 1);
			vItemSize.y += vExpandAmt.y * static_cast<float>((pItem->GetHorizontalPolicy() & Expand) >> 1);
			vItemSize.x += vShrinkAmt.x * static_cast<float>((pItem->GetHorizontalPolicy() & Shrink) >> 2);
			vItemSize.y += vShrinkAmt.y * static_cast<float>((pItem->GetVerticalPolicy() & Shrink) >> 2);

			pItem->OnResize(vItemSize.x, vItemSize.y);

			pItem->pos.Set(ptCurPos);

			ptCurPos.x += vItemSize.x + GetHorizontalSpacing();
			fRowHeight = HyMax(fRowHeight, static_cast<float>(vItemSize.y));
		}

		ptCurPos.x = 0.0f;
		ptCurPos.y += fRowHeight + GetVerticalSpacing();
	}
}
