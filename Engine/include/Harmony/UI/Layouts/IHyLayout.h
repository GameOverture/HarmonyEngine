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

class HyContainer;

#define HY_UILAYOUT \
	private:\
		using HyEntity2d::ChildAppend; \
		using HyEntity2d::ChildInsert;

class IHyLayout : public HyEntityUi
{
	HY_UILAYOUT
	friend class HyContainer;

protected:
	HyContainer *						m_pContainerParent;
	const HyLayoutType					m_eLAYOUT_TYPE;
	glm::ivec2							m_vSize;		// If 'x' or 'y' are zero, use corresponding SizeHint().x/y instead
	HyRectangle<int32>					m_Margins;		// Tag = Spacing between widgets inside the layout

public:
	IHyLayout(HyLayoutType eLayoutType, HyEntity2d *pParent = nullptr);
	virtual ~IHyLayout();

	void AppendItem(HyEntityUi &itemRef);
	void ClearItems();

	glm::ivec2 GetSize();
	virtual glm::ivec2 GetSizeHint() = 0;	// The preferred size of *this

	void SetMargins(int32 iLeft, int32 iBottom, int32 iRight, int32 iTop, uint16 uiWidgetSpacingX, uint16 uiWidgetSpacingY);

	uint16 GetHorizontalSpacing();
	uint16 GetVerticalSpacing();
	glm::ivec2 GetSpacing();

protected:
	void SetLayoutItems();

	virtual void OnClearItems() { }
	virtual void OnSetLayoutItems() = 0;
	void SetSize(int32 iNewWidth, int32 iNewHeight);

private:
	void SetContainerParent(HyContainer *pContainerParent);

	friend void HyInternal_LayoutSetSize(IHyLayout &layoutRef, int32 iNewWidth, int32 iNewHeight);
};

#endif /* IHyLayout_h__ */
