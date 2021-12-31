/**************************************************************************
*	HyScrollContainer.h
*
*	Harmony Engine
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyScrollContainer_h__
#define HyScrollContainer_h__

#include "Afx/HyStdAfx.h"
#include "UI/HyUiContainer.h"
#include "UI/Components/HyScrollBar.h"

class HyScrollContainer : public HyUiContainer
{
protected:
	bool					m_bUseVertBar;
	bool					m_bUseHorzBar;
	HyScrollBar				m_VertBar;
	HyScrollBar				m_HorzBar;

public:
	HyScrollContainer(HyLayoutType eRootLayout, const HyPanelInit &initRef, uint32 uiScrollBarDiameter, bool bUseVert, bool bUseHorz, HyEntity2d *pParent = nullptr);
	virtual ~HyScrollContainer();

	virtual void SetSize(int32 iNewWidth, int32 iNewHeight) override;

	void EnableScrollBars(bool bUseVert, bool bUseHorz);
	void SetScrollBarColor(HyColor color);
	void SetLineScrollAmt(float fLineScrollAmt);

protected:
	virtual void OnContainerUpdate() override;
	virtual void OnRootLayoutUpdate() override;

	static void OnScroll(HyScrollBar *pSelf, float fNewPosition, float fTotalRange, void *pData);
};

#endif /* HyScrollContainer_h__ */
