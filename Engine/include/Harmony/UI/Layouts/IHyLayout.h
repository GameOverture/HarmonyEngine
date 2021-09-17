/**************************************************************************
*	IHyLayout.h
*
*	Harmony Engine
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef IHyLayout_h__
#define IHyLayout_h__

#include "Afx/HyStdAfx.h"
#include "UI/HyEntityUi.h"

#define HY_UILAYOUT \
	private:\
		using HyEntity2d::ChildAppend; \
		using HyEntity2d::ChildInsert;

class IHyLayout : public HyEntityUi
{
	HY_UILAYOUT

protected:
	const HyLayoutType					m_eLAYOUT_TYPE;
	glm::ivec2							m_vSize;
	HyRectangle<int32>					m_Margins;		// Tag = Spacing between widgets inside the layout

public:
	IHyLayout(HyLayoutType eLayoutType, HyEntity2d *pParent = nullptr);
	virtual ~IHyLayout();

	void Clear();

	glm::ivec2 GetSize() const;
	virtual glm::ivec2 GetSizeHint() = 0;	// The preferred size of *this

	void SetMargins(int32 iLeft, int32 iBottom, int32 iRight, int32 iTop, uint16 uiWidgetSpacingX, uint16 uiWidgetSpacingY);

	uint16 GetHorizontalSpacing();
	uint16 GetVerticalSpacing();
	glm::ivec2 GetSpacing();

protected:
	virtual void OnClear() { }
	virtual void OnSetLayoutItems() = 0;
	void SetSize(int32 iNewWidth, int32 iNewHeight);

	friend void HyInternal_LayoutSetSize(IHyLayout &layoutRef, int32 iNewWidth, int32 iNewHeight);
};

#endif /* IHyLayout_h__ */
