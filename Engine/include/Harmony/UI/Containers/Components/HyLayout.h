/**************************************************************************
*	HyLayout.h
*
*	Harmony Engine
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyLayout_h__
#define HyLayout_h__

#include "Afx/HyStdAfx.h"
#include "UI/IHyEntityUi.h"

class HyLayout : public IHyEntityUi
{
	HyLayoutType						m_eLayoutType;
	glm::ivec2							m_vActualSize;

	glm::ivec2							m_vNumExpandItems;
	glm::ivec2							m_vNumShrinkItems;
	
	bool								m_bReverse;			// By default items are appended left->right, or top->bottom (NOTE: 'm_bReverse' is defaulted ON when 'm_eOrientation' is HYORIEN_Vertical to achieve top->bottom as default)

	HyRectangle<int16>					m_Margins;			// Tag = Spacing between widgets inside the layout

	bool								m_bLayoutDirty;

public:
	HyLayout(HyLayoutType eLayoutType, HyEntity2d *pParent = nullptr);
	virtual ~HyLayout();

	virtual glm::vec2 GetPosOffset() override;	// What offset is needed to get *this oriented to its bottom left

	void AppendItem(IHyEntityUi &itemRef);
	void ClearItems();

	bool IsReverseOrder();
	void ReverseOrder(bool bReverse);

	const HyRectangle<int16> &GetMargins() const;
	void SetMargins(int16 iLeft, int16 iBottom, int16 iRight, int16 iTop, uint16 uiWidgetSpacingX, uint16 uiWidgetSpacingY);

	uint16 GetHorizontalSpacing();
	uint16 GetVerticalSpacing();
	glm::ivec2 GetSpacing();

	bool IsLayoutDirty() const;
	void SetLayoutDirty();
	void SetLayoutItems();

protected:
	virtual void OnSetSizeHint() override;
	virtual glm::ivec2 OnResize(uint32 uiNewWidth, uint32 uiNewHeight) override;

private:
	using HyEntity2d::ChildAppend;
	using HyEntity2d::ChildInsert;
	using IHyEntityUi::SetMinSize;			// Min Size is dictated by the margins and items within the layout
};

#endif /* HyLayout_h__ */
