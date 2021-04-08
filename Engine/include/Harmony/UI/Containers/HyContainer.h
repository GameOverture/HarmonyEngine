/**************************************************************************
*	HyContainer.h
*
*	Harmony Engine
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyContainer_h__
#define HyContainer_h__

#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Loadables/Bodies/Objects/HyEntity2d.h"
#include "UI/Layouts/IHyLayout.h"

class HyContainer : public HyEntity2d
{
	glm::vec2				m_vSize;
	IHyLayout *				m_pRootLayout;

public:
	HyContainer();
	virtual ~HyContainer();
};

#endif /* HyContainer_h__ */
