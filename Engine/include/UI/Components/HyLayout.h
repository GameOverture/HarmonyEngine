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

class IHyWidget;

class HyLayout : public IHyEntityUi
{
	HyOrientation						m_eLayoutType;
	glm::ivec2							m_vActualSize;

	uint32								m_uiNumExpandItems;	// How many child items have a size policy of EXPAND
	uint32								m_uiNumShrinkItems;	// How many child items have a size policy of SHRINK
	
	bool								m_bReverse;			// By default items are appended left->right, or top->bottom (NOTE: 'm_bReverse' is defaulted ON when 'm_eOrientation' is HYORIEN_Vertical to achieve top->bottom as default)

	HyMargins<int16>					m_Margins;			// Pixel margins
	int32								m_iWidgetSpacing;	// Spacing between widgets inside the layout

	bool								m_bLayoutDirty;

public:
	HyLayout(HyOrientation eLayoutType, int32 iWidgetSpacing, HyEntity2d *pParent = nullptr);
	virtual ~HyLayout();

	virtual float GetWidth(float fPercent = 1.0f) override;
	virtual float GetHeight(float fPercent = 1.0f) override;

	HyOrientation GetLayoutType() const;
	void SetLayoutType(HyOrientation eLayoutType);

	glm::ivec2 GetActualSize() const;

	virtual HySizePolicy GetSizePolicy(HyOrientation eOrien) override;
	virtual glm::vec2 GetPosOffset() override;	// What offset is needed to get *this oriented to its bottom left

	void AppendItem(IHyEntityUi &itemRef);
	bool RemoveItem(IHyEntityUi &itemRef);
	void DetachAllItems();

	bool IsReverseOrder();
	void ReverseOrder(bool bReverse);

	const HyMargins<int16> &GetMargins() const;
	void SetMargins(int16 iLeft, int16 iBottom, int16 iRight, int16 iTop, int32 iWidgetSpacing);
	int32 GetWidgetSpacing();

	bool IsLayoutDirty() const;
	void SetLayoutDirty();

	bool RequestWidgetFocus(IHyWidget *pWidget);
	bool IsWidgetInputAllowed();

protected:
	virtual void OnSetSizeHint() override;
	virtual glm::ivec2 OnResize(uint32 uiNewWidth, uint32 uiNewHeight) override;

	void GetDistributedScalingAmts(int32 iTargetLength, int32 iCurrLength, float &fExpandAmtOut, float &fShrinkAmtOut);

private:
	using HyEntity2d::ChildAppend;
	using HyEntity2d::ChildInsert;
	using IHyEntityUi::SetMinSize;			// Min Size is dictated by the margins and items within the layout
};

#endif /* HyLayout_h__ */
