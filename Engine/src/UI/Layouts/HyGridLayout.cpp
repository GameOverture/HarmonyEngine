/**************************************************************************
*	HyGridLayout.cpp
*
*	Harmony Engine
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "UI/Layouts/HyGridLayout.h"
#include "UI/Widgets/HySpacer.h"
#include "Diagnostics/Console/IHyConsole.h"
#include "Utilities/HyMath.h"

HyGridLayout::HyGridLayout(HyEntity2d *pParent /*= nullptr*/) :
	IHyLayout(HYLAYOUT_Grid, pParent)
{
}

/*virtual*/ HyGridLayout::~HyGridLayout()
{
}

/*virtual*/ glm::ivec2 HyGridLayout::GetSizeHint() /*override*/
{
	return m_vSizeHint;
}

uint32 HyGridLayout::GetNumColumns() const
{
	return m_vGridSize.x;
}

uint32 HyGridLayout::GetNumRows() const
{
	return m_vGridSize.y;
}

uint32 HyGridLayout::GetColumnMinimumWidth(uint32 uiColumnIndex)
{
	if(uiColumnIndex >= GetNumColumns())
		EmbiggenGrid(uiColumnIndex + 1, GetNumRows());

	return m_ColMinWidthList[uiColumnIndex];
}

void HyGridLayout::SetColumnMinimumWidth(uint32 uiColumnIndex, uint32 uiMinWidth)
{
	if(uiColumnIndex >= GetNumColumns())
		EmbiggenGrid(uiColumnIndex + 1, GetNumRows());

	m_ColMinWidthList[uiColumnIndex] = uiMinWidth;
	SetLayoutItems();
}

uint32 HyGridLayout::GetRowMinimumHeight(uint32 uiRowIndex)
{
	if(uiRowIndex >= GetNumRows())
		EmbiggenGrid(GetNumColumns(), uiRowIndex + 1);

	return m_RowMinHeightList[uiRowIndex];
}

void HyGridLayout::SetRowMinimumHeight(uint32 uiRowIndex, uint32 uiMinHeight)
{
	if(uiRowIndex >= GetNumRows())
		EmbiggenGrid(GetNumColumns(), uiRowIndex + 1);

	m_RowMinHeightList[uiRowIndex] = uiMinHeight;
	SetLayoutItems();
}


uint32 HyGridLayout::GetColumnStretchPriority(uint32 uiColumnIndex)
{
	if(uiColumnIndex >= GetNumColumns())
		EmbiggenGrid(uiColumnIndex + 1, GetNumRows());

	return m_ColumnStretchPriorityList[uiColumnIndex];
}

void HyGridLayout::SetColumnStretchPriority(uint32 uiColumnIndex, uint32 uiColumnStretchPriority)
{
	if(uiColumnIndex >= GetNumColumns())
		EmbiggenGrid(uiColumnIndex + 1, GetNumRows());

	m_ColumnStretchPriorityList[uiColumnIndex] = uiColumnStretchPriority;
	SetLayoutItems();
}

uint32 HyGridLayout::GetRowStretchPriority(uint32 uiRowIndex)
{
	if(uiRowIndex >= GetNumRows())
		EmbiggenGrid(GetNumColumns(), uiRowIndex + 1);

	return m_RowStretchPriorityList[uiRowIndex];
}

void HyGridLayout::SetRowStretchPriority(uint32 uiRowIndex, uint32 uiRowStretchPriority)
{
	if(uiRowIndex >= GetNumRows())
		EmbiggenGrid(GetNumColumns(), uiRowIndex + 1);

	m_RowStretchPriorityList[uiRowIndex] = uiRowStretchPriority;
	SetLayoutItems();
}

// Item indices are laid out using row-major
// [0] [1] [2]
// [3] [4] [5]
void HyGridLayout::InsertItem(HyEntityUi *pItem, uint32 uiX, uint32 uiY, uint32 uiColumnSpan /*= 0*/, uint32 uiRowSpan /*= 0*/)
{
	if(pItem == nullptr)
		return;

	// First determine this insert is valid and doesn't overlap any existing items in the grid
	if(m_GridCells.empty() == false)
	{
		for(uint32 i = 0; i < uiColumnSpan+1; ++i)
		{
			for(uint32 j = 0; j < uiRowSpan+1; ++j)
			{
				uint32 iTestX = uiX + i;
				uint32 iTestY = uiY + j;

				if(iTestX < m_GridCells.size() &&
					iTestY < m_GridCells[iTestX].size() &&
					m_GridCells[iTestX][iTestY].m_pItem)
				{
					HyLogWarning("HyGridLayout::InsertItem() passed invalid coordinates and/or span parameters. Item already exists at [" << iTestX << ", " << iTestY << "]");
					return;
				}
			}
		}
	}

	// Valid parameters, continue with insert
	ChildAppend(*pItem);
	EmbiggenGrid((uiX+1) + uiColumnSpan, (uiY+1) + uiRowSpan);

	for(uint32 i = 0; i < uiColumnSpan+1; ++i)
	{
		for(uint32 j = 0; j < uiRowSpan+1; ++j)
		{
			uint32 iGridX = uiX + i;
			uint32 iGridY = uiY + j;

			m_GridCells[iGridX][iGridY].m_pItem = pItem;
			HySetVec(m_GridCells[iGridX][iGridY].m_vOriginIndex, uiX, uiY);
			HySetVec(m_GridCells[iGridX][iGridY].m_vEndIndex, uiX + uiColumnSpan, uiY + uiRowSpan);
		}
	}

	SetLayoutItems();
}

/*virtual*/ void HyGridLayout::OnClearItems() /*override*/
{
	HySetVec(m_vGridSize, 0, 0);
	m_ColMinWidthList.clear();
	m_RowMinHeightList.clear();
	m_ColumnStretchPriorityList.clear();
	m_RowStretchPriorityList.clear();
	m_GridCells.clear();
}

/*virtual*/ void HyGridLayout::OnSetLayoutItems() /*override*/
{
	if(m_vGridSize.x == 0 || m_vGridSize.y == 0)
		return;

	// Iterate through and determine the preferred size (size hint) of every column in grid
	std::vector<float> colWidthList(GetNumColumns());
	for(int32 i = 0; i < GetNumColumns(); ++i)
	{
		for(int32 j = 0; j < GetNumRows(); ++j)
		{
			float fColWidth = 0.0f;
			if(m_GridCells[i][j].m_pItem == nullptr)
				fColWidth = m_ColMinWidthList[i];
			else
			{
				fColWidth = m_GridCells[i][j].m_pItem->GetSizeHint().x;

				// Divide 'uiColWidth' by how many grid cells this item spans in 'X'
				fColWidth /= static_cast<float>(1 + m_GridCells[i][j].m_vEndIndex.x - m_GridCells[i][j].m_vOriginIndex.x);
			}

			colWidthList[i] = HyMax(colWidthList[i], fColWidth);
		}
	}

	// Now do row heights
	std::vector<float> rowHeightList(GetNumRows());
	for(int32 j = 0; j < GetNumRows(); ++j)
	{
		for(int32 i = 0; i < GetNumColumns(); ++i)
		{
			float fRowHeight = 0.0f;
			if(m_GridCells[i][j].m_pItem == nullptr)
				fRowHeight = m_RowMinHeightList[i];
			else
			{
				fRowHeight = m_GridCells[i][j].m_pItem->GetSizeHint().y;

				// Divide 'fRowHeight' by how many grid cells this item spans in 'Y'
				fRowHeight /= static_cast<float>(1 + m_GridCells[i][j].m_vEndIndex.y - m_GridCells[i][j].m_vOriginIndex.y);
			}

			rowHeightList[i] = HyMax(rowHeightList[i], fRowHeight);
		}
	}

	// Then assemble all sizeHintMap and determine 'm_vSizeHint'
	float fTotalWidth = 0.0f;
	float fTotalHeight = 0.0f;
	for(int32 i = 0; i < GetNumColumns(); ++i)
		fTotalWidth += colWidthList[i];
	for(int32 j = 0; j < GetNumRows(); ++j)
		fTotalHeight += rowHeightList[j];
	HySetVec(m_vSizeHint, fTotalWidth, fTotalHeight);

	// Determine if preferred size (m_vSizeHint) fits within m_vSize
	//	- Distrubute positive difference to all 'expanding' then 'grow' size policies
	//	- Distrubute negative difference to all 'fill' then 'shrink' size policies
	glm::vec2 vDifference = m_vSize - m_vSizeHint;

	if(vDifference.x >= 0)
	{
		for(int32 i = 0; i < GetNumColumns(); ++i)
		{
			// Go through every item in this column, and see if they can expand
			bool bExpand = true;
			for(int32 j = 0; j < GetNumRows(); ++j)
			{
				if(m_GridCells[i][j].m_pItem != nullptr && m_GridCells[i][j].m_pItem->GetUiType() == Ui_Widget)
				{
					IHyWidget *pWidget = static_cast<IHyWidget *>(m_GridCells[i][j].m_pItem);
					if((pWidget->GetHorizontalPolicy() & HY_SIZEFLAG_EXPAND) == 0)
						bExpand = false;

					// TODO: figure out newly scaled width and heights of column/rows
				}
			}
		}
	}



	// Finally position and scale all the grid items into place
	for(int32 i = 0; i < GetNumColumns(); ++i)
	{
		for(int32 j = 0; j < GetNumRows(); ++j)
		{
			uint32 uiRowHeight = 0;
			uint32 uiColWidth = 0;
			//if(m_GridCells[i][j].m_pItem == nullptr)
		}
	}
}

void HyGridLayout::EmbiggenGrid(uint32 uiNumColumns, uint32 uiNumRows)
{
	m_vGridSize.x = HyMax(m_vGridSize.x, static_cast<int32>(uiNumColumns));
	m_vGridSize.y = HyMax(m_vGridSize.y, static_cast<int32>(uiNumRows));

	// First ensure other member variables are mapped properly to 'm_vGridSize'
	while(m_ColMinWidthList.size() < m_vGridSize.x)
		m_ColMinWidthList.push_back(10);

	while(m_RowMinHeightList.size() < m_vGridSize.y)
		m_RowMinHeightList.push_back(10);

	while(m_ColumnStretchPriorityList.size() < m_vGridSize.x)
		m_ColumnStretchPriorityList.push_back(0);

	while(m_RowStretchPriorityList.size() < m_vGridSize.y)
		m_RowStretchPriorityList.push_back(0);

	while(m_GridCells.size() < m_vGridSize.x)
		m_GridCells.emplace_back();
	for(int32 i = 0; i < m_GridCells.size(); ++i)
	{
		while(m_GridCells[i].size() < m_vGridSize.y)
			m_GridCells[i].emplace_back();
	}
}

glm::ivec2 HyGridLayout::DetermineSpan(uint32 uiOriginX, uint32 uiOriginY)
{
	if(uiOriginX >= GetNumColumns() || uiOriginY >= GetNumRows())
		return glm::ivec2(-1, -1);

	HyEntityUi *pRootItem = m_GridCells[uiOriginX][uiOriginY].m_pItem;
	glm::ivec2 vSpan(0,0);
	for(int32 i = uiOriginX; i < GetNumColumns(); ++i)
	{
		for(int32 j = uiOriginY; j < GetNumRows(); ++j)
		{
			if(pRootItem == m_GridCells[i][j].m_pItem)
				HySetVec(vSpan, i - uiOriginX, j - uiOriginY);
			else
				break;
		}
	}

	return vSpan;
}
