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

class HyScrollContainer : public HyContainer
{
public:
	HyScrollContainer(HyLayoutType eRootLayout, HyEntity2d *pParent = nullptr);
	HyScrollContainer(HyLayoutType eRootLayout, int32 iWidth, int32 iHeight, int32 iStroke, HyEntity2d *pParent = nullptr);
	virtual ~HyScrollContainer();

protected:
	virtual void OnContainerUpdate() { }
};

#endif /* HyScrollContainer_h__ */
