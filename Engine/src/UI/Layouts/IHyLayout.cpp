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
#include "UI/Widgets/IHyWidget.h"

IHyLayout::IHyLayout(HyLayoutType eLayoutType, HyEntity2d *pParent /*= nullptr*/) :
	HyEntityUi(Ui_Layout, pParent),
	m_eLAYOUT_TYPE(eLayoutType)
{
}

/*virtual*/ IHyLayout::~IHyLayout()
{
}

glm::ivec2 IHyLayout::GetSize() const
{
	return m_vSize;
}

void IHyLayout::SetMargins(int32 iLeft, int32 iBottom, int32 iRight, int32 iTop, uint16 uiWidgetSpacingX, uint16 uiWidgetSpacingY)
{
	m_Margins.Set(iLeft, iBottom, iRight, iTop);
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

void IHyLayout::SetSize(int32 iNewWidth, int32 iNewHeight)
{
	HySetVec(m_vSize, iNewWidth, iNewHeight);
	OnSetLayoutItems();
}

/*virtual*/ glm::vec2 IHyLayout::GetPosOffset() /*override*/
{
	return glm::vec2(0.0f, 0.0f);
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
	for(uint32 iRow = 0; iRow < uiNumRows; ++iRow)
	{
		int32 iCurRowSize = 0;
		for(uint32 iCol = 0; iCol < uiNumCols; ++iCol)
		{
			// Children are guaranteed to be HyEntityUi
			HyEntityUi *pItem = static_cast<HyEntityUi *>(m_ChildList[iRow + (iCol * uiNumRows)]);
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

			iCurRowSize += vItemSizeHint.x;
			pCurColSizes[iCol] += vItemSizeHint.y;
		}
		
		iPrefRowSize = HyMax(iPrefRowSize, iCurRowSize);
	}

	for(int i = 0; i < uiNumCols; ++i)
		iPrefColSize = HyMax(iPrefColSize, pCurColSizes[i]);
	delete[] pCurColSizes;

	// m_vUiSizeHint represents the preferred size a layout would ideally be
	m_vUiSizeHint.x = m_Margins.left + m_Margins.right + ((uiNumCols - 1) * GetHorizontalSpacing());
	m_vUiSizeHint.x += iPrefRowSize;
	m_vUiSizeHint.y = m_Margins.top + m_Margins.bottom + ((uiNumRows - 1) * GetVerticalSpacing());
	m_vUiSizeHint.y += iPrefColSize;

	// Determine if preferred size (m_vUiSizeHint) fits within m_vSize
	//	- Distrubute positive difference to all 'expanding' then 'grow' size policies
	//	- Distrubute negative difference to all 'fill' then 'shrink' size policies
	glm::vec2 vDifference = m_vSize - m_vUiSizeHint;

	glm::vec2 vGrowAmt(0.0f, 0.0f);
	glm::vec2 vExpandAmt(0.0f, 0.0f);
	glm::vec2 vShrinkAmt(0.0f, 0.0f);
	for(int32 iDimen = HYORIEN_Horizontal; iDimen < HYNUM_ORIENTATIONS; ++iDimen)
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
	glm::vec2 ptCurPos(m_Margins.left, m_Margins.bottom);
	for(uint32 iRow = 0; iRow < uiNumRows; ++iRow)
	{
		float fRowHeight = 0.0f;
		for(uint32 iCol = 0; iCol < uiNumCols; ++iCol)
		{
			// Children are guaranteed to be HyEntityUi
			HyEntityUi *pItem = static_cast<HyEntityUi *>(m_ChildList[iRow + (iCol * uiNumRows)]);
			glm::ivec2 vItemSize = pItem->GetSizeHint();

			if(pItem->GetUiType() == Ui_Widget)
			{
				IHyWidget *pWidget = static_cast<IHyWidget *>(pItem);

				// Resize this item based on all the final calculations of size policies and size constraints
				vItemSize.x += vGrowAmt.x * static_cast<float>(pWidget->GetHorizontalPolicy() & HY_SIZEFLAG_GROW);
				vItemSize.y += vGrowAmt.y * static_cast<float>(pWidget->GetVerticalPolicy() & HY_SIZEFLAG_GROW);
				vItemSize.x += vExpandAmt.x * static_cast<float>((pWidget->GetHorizontalPolicy() & HY_SIZEFLAG_EXPAND) >> 1);
				vItemSize.y += vExpandAmt.y * static_cast<float>((pWidget->GetHorizontalPolicy() & HY_SIZEFLAG_EXPAND) >> 1);
				vItemSize.x += vShrinkAmt.x * static_cast<float>((pWidget->GetHorizontalPolicy() & HY_SIZEFLAG_SHRINK) >> 2);
				vItemSize.y += vShrinkAmt.y * static_cast<float>((pWidget->GetVerticalPolicy() & HY_SIZEFLAG_SHRINK) >> 2);

				pWidget->OnResize(vItemSize.x, vItemSize.y);
			}
			else // Ui_Layout
			{
				vItemSize.x += vGrowAmt.x;
				vItemSize.y += vGrowAmt.y;
				vItemSize.x += vExpandAmt.x;
				vItemSize.y += vExpandAmt.y;
				vItemSize.x += vShrinkAmt.x;
				vItemSize.y += vShrinkAmt.y;
				IHyLayout *pLayout = static_cast<IHyLayout *>(pItem);
				pLayout->SetSize(vItemSize.x, vItemSize.y);
			}

			

			// Set item to the ptCurPos + its local offset, then update ptCurPos for the next item
			pItem->pos.Set(ptCurPos + pItem->GetPosOffset());

			ptCurPos.x += vItemSize.x + GetHorizontalSpacing();
			fRowHeight = HyMax(fRowHeight, static_cast<float>(vItemSize.y));
		}

		ptCurPos.x = 0.0f;
		ptCurPos.y += fRowHeight + GetVerticalSpacing();
	}
}
