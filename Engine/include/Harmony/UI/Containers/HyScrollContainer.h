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
#include "UI/Containers/HyContainer.h"
#include "UI/Containers/HyScrollBar.h"

class HyScrollContainer : public HyContainer
{
	enum
	{
		USE_VERT = 1 << 0,
		USE_HORZ = 1 << 1,
		USE_BOTH = USE_VERT | USE_HORZ
	};
	uint32					m_uiScrollFlags;
	uint32					m_uiScrollBarDiameter;

	HyScrollBar				m_VertBar;
	HyScrollBar				m_HorzBar;

public:
	HyScrollContainer(HyLayoutType eRootLayout, HyEntity2d *pParent = nullptr);
	HyScrollContainer(HyLayoutType eRootLayout, const HyPrimitivePanelInit &initRef, uint32 uiScrollBarDiameter, HyEntity2d *pParent = nullptr);
	virtual ~HyScrollContainer();

	void SetScrollBarColor(HyColor color);

protected:
	virtual void OnContainerUpdate() override;
	virtual void OnSetLayoutItems() override;

	static void OnScroll(HyScrollBar *pSelf, uint32 uiNewPosition, void *pData);
};

#endif /* HyScrollContainer_h__ */
