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
#include "Scene/Nodes/Loadables/Bodies/Drawables/Objects/HyPrimitive2d.h"

#if defined(HY_DEBUG) && !defined(HY_PLATFORM_GUI)
	//#define HY_USE_LAYOUT_DEBUG_BOXES
#endif

class IHyWidget;

class HyLayout : public IHyEntityUi
{
#ifdef HY_USE_LAYOUT_DEBUG_BOXES
	HyPrimitive2d						m_DebugBoxBoarder;
	HyPrimitive2d						m_DebugBoxMargins;
#endif

	bool								m_bIsRootLayout;
	HyOrientation						m_eLayoutType;
	bool								m_bReverse;			// By default items are appended left->right, or top->bottom (NOTE: 'm_bReverse' is defaulted ON when 'm_eOrientation' is HYORIEN_Vertical to achieve top->bottom as default)

	uint32								m_uiNumExpandItems;	// How many child items have a size policy of EXPAND
	uint32								m_uiNumShrinkItems;	// How many child items have a size policy of SHRINK

	int32								m_iWidgetSpacing;	// Spacing between widgets inside the layout
	HyMargins<int16>					m_Margins;			// Pixel margins

public:
	HyLayout(HyOrientation eLayoutType, int32 iWidgetSpacing, HyEntity2d *pParent = nullptr);
	virtual ~HyLayout();

	bool IsRootLayout() const;
	void SetAsRootLayout(bool bIsRootLayout);

	HyOrientation GetLayoutType() const;
	void SetLayoutType(HyOrientation eLayoutType);

	virtual HySizePolicy GetSizePolicy(HyOrientation eOrien) const override;
	virtual glm::vec2 GetBotLeftOffset() override;

	void AppendItem(IHyEntityUi &itemRef);
	bool RemoveItem(IHyEntityUi &itemRef);
	void DetachAllItems();

	bool IsReverseOrder();
	void ReverseOrder(bool bReverse);

	const HyMargins<int16> &GetMargins() const;
	void SetMargins(int16 iLeft, int16 iBottom, int16 iRight, int16 iTop, int32 iWidgetSpacing);
	void SetMargins(const HyMargins<int16> &newMargins, int32 iWidgetSpacing);
	int32 GetWidgetSpacing();

	bool RequestWidgetFocus(IHyWidget *pWidget);
	bool IsWidgetInputAllowed();

#ifdef HY_USE_LAYOUT_DEBUG_BOXES
	void ShowDebugBox(bool bShow);
#endif

protected:
#ifdef HY_USE_LAYOUT_DEBUG_BOXES
	virtual void OnUpdate() override;
	void OnSetDebugBox();
#endif
	virtual glm::ivec2 OnCalcPreferredSize() override;
	virtual glm::ivec2 OnResize(uint32 uiNewWidth, uint32 uiNewHeight) override;

	void GetDistributedScalingAmts(int32 iTargetLength, int32 iCurrLength, float &fExpandAmtOut, float &fShrinkAmtOut);

private:
	using HyEntity2d::ChildAppend;
	using HyEntity2d::ChildInsert;
	using IHyEntityUi::SetMinSize;			// Min Size is dictated by the margins and items within the layout
};

#endif /* HyLayout_h__ */
