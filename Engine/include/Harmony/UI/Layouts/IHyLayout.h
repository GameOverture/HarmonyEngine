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
#include "UI/HyWidget.h"

class IHyLayout : protected IHyLayoutItem
{
protected:
	glm::ivec2			m_vSize;
	HyRectangle<int32>	m_Margins;		// Tag = Spacing between widgets inside the layout

public:
	IHyLayout(HyEntity2d *pParent = nullptr);
	virtual ~IHyLayout();

	void AddItem(IHyLayoutItem *pItem);
	void SetSize(int32 iWidth, int32 iHeight);
	void SetMargins(int32 iLeft, int32 iTop, int32 iRight, int32 iBottom, uint16 uiWidgetSpacingX, uint16 uiWidgetSpacingY);

	uint16 GetHorizontalSpacing();
	uint16 GetVerticalSpacing();

protected:
	virtual void OnDoLayout() = 0;
};

#endif /* IHyLayout_h__ */
