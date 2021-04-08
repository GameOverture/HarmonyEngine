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

class IHyLayout : public HyEntityUi
{
protected:
	const HyLayoutType					m_eLAYOUT_TYPE;

	glm::ivec2							m_vSize;
	HyRectangle<int32>					m_Margins;		// Tag = Spacing between widgets inside the layout

public:
	IHyLayout(HyLayoutType eLayoutType, HyEntity2d *pParent = nullptr);
	virtual ~IHyLayout();

	glm::ivec2 GetSize() const;
	void SetSize(int32 iNewWidth, int32 iNewHeight);

	void SetMargins(int32 iLeft, int32 iTop, int32 iRight, int32 iBottom, uint16 uiWidgetSpacingX, uint16 uiWidgetSpacingY);

	uint16 GetHorizontalSpacing();
	uint16 GetVerticalSpacing();

protected:
	virtual void OnSetLayoutItems() = 0;
	void SetLayoutItems(uint32 uiNumRows, uint32 uiNumCols);

private:
	// Prevent adding any child that isn't of IHyLayoutItem type
	using HyEntity2d::ChildAppend;
	using HyEntity2d::ChildInsert;
};

#endif /* IHyLayout_h__ */
