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
#include "UI/HyUI.h"

class IHyLayout : public HyUI
{
protected:
	glm::ivec2							m_vSize;
	HyRectangle<int32>					m_Margins;		// Tag = Spacing between widgets inside the layout

public:
	IHyLayout(HyEntity2d *pParent = nullptr);
	virtual ~IHyLayout();

	void SetSize(int32 iWidth, int32 iHeight);
	void SetMargins(int32 iLeft, int32 iTop, int32 iRight, int32 iBottom, uint16 uiWidgetSpacingX, uint16 uiWidgetSpacingY);

	uint16 GetHorizontalSpacing();
	uint16 GetVerticalSpacing();

protected:
	virtual void OnResize(int32 iNewWidth, int32 iNewHeight) override;

	virtual void OnSetLayoutItems() = 0;
	void SetLayoutItems(uint32 uiNumRows, uint32 uiNumCols);
};

#endif /* IHyLayout_h__ */
