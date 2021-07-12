/**************************************************************************
*	HyTextUi.h
*
*	Harmony Engine
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyTextUi_h__
#define HyTextUi_h__

#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Loadables/Bodies/Objects/HyEntity2d.h"

class HyTextUi : public HyEntity2d
{
public:
	HyTextUi(HyEntity2d *pParent = nullptr);
	virtual ~HyTextUi();
};

#endif /* HyTextUi_h__ */
