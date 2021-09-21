/**************************************************************************
*	HyGridLayout.h
*
*	Harmony Engine
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyGridLayout_h__
#define HyGridLayout_h__

#include "Afx/HyStdAfx.h"
#include "UI/Layouts/IHyLayout.h"

class HySpacer;

class HyGridLayout : public IHyLayout
{
	HY_UILAYOUT

	glm::ivec2							m_vGridSize;
	std::vector<uint32>					m_ColMinWidthList;
	std::vector<uint32>					m_RowMinHeightList;
	std::vector<uint32>					m_ColumnStretchPriorityList;
	std::vector<uint32>					m_RowStretchPriorityList;

	struct Cell
	{
		HyEntityUi *					m_pItem;
		glm::ivec2						m_vOriginIndex;	// Could "point" to the beginning (top-left) grid index if iRowSpan or iColumnSpan was specified
														// Otherwise 'm_vOriginIndex' will equal this GridSpot's index

		glm::ivec2						m_vEndIndex;	// Could "point" to the last (bottom-right) grid index if iRowSpan or iColumnSpan was specified
														// Otherwise 'm_vEndIndex' will equal this GridSpot's index

		Cell() :
			m_pItem(nullptr)
		{ }
	};
	std::vector<std::vector<Cell>>		m_GridCells;

	glm::ivec2							m_vSizeHint;		// The total preferred size

public:
	HyGridLayout(HyEntity2d *pParent = nullptr);
	virtual ~HyGridLayout();

	virtual glm::ivec2 GetSizeHint() override;

	uint32 GetNumColumns() const;
	uint32 GetNumRows() const;

	uint32 GetColumnMinimumWidth(uint32 uiColumnIndex);
	void SetColumnMinimumWidth(uint32 uiColumnIndex, uint32 uiMinWidth);

	uint32 GetRowMinimumHeight(uint32 uiRowIndex);
	void SetRowMinimumHeight(uint32 uiRowIndex, uint32 uiMinHeight);

	uint32 GetColumnStretchPriority(uint32 uiColumnIndex);
	void SetColumnStretchPriority(uint32 uiColumnIndex, uint32 uiColumnStretchPriority);

	uint32 GetRowStretchPriority(uint32 uiRowIndex);
	void SetRowStretchPriority(uint32 uiRowIndex, uint32 uiRowStretchPriority);

	// Item indices are laid out using (if
	// [0][0], [1][0], [2][0]
	// [0][1], [1][1], [2][1]
	// [0][2], [1][2], [2][2]
	void InsertItem(HyEntityUi *pItem, uint32 uiX, uint32 uiY, uint32 uiColumnSpan = 0, uint32 uiRowSpan = 0);

protected:
	virtual void OnClearItems() override;
	virtual void OnSetLayoutItems() override;

	void EmbiggenGrid(uint32 uiNumColumns, uint32 uiNumRows);
	glm::ivec2 DetermineSpan(uint32 uiX, uint32 uiY);
};

#endif /* HyGridLayout_h__ */
