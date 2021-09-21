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
	HyScrollContainer(HyLayoutType eRootLayout, int32 iWidth, int32 iHeight, int32 iStroke, uint32 uiScrollBarDiameter, HyEntity2d *pParent = nullptr);
	virtual ~HyScrollContainer();

	virtual void OnSetLayoutItems();
};

#endif /* HyScrollContainer_h__ */
