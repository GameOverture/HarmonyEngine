/**************************************************************************
*	HyPortal2d.h
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyPortal2d_h__
#define HyPortal2d_h__

#include "Afx/HyStdAfx.h"

class HyPortalGate2d
{
public:
	HyPortalGate2d(const glm::vec2 &pt1, const glm::vec2 &pt2, bool bPositiveDirection)
	{ }
	~HyPortalGate2d()
	{ }
};

class HyPortal2d
{
	HyPortalGate2d		m_Gate1;
	HyPortalGate2d		m_Gate2;

public:
	HyPortal2d(const HyPortalGate2d &gate1Ref, const HyPortalGate2d &gate2Ref);
	~HyPortal2d();
};

#endif /* HyPortal2d_h__ */
