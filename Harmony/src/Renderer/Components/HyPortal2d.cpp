/**************************************************************************
*	HyPortal2d.cpp
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Renderer/Components/HyPortal2d.h"

HyPortal2d::HyPortal2d(const HyPortalGate2d &gate1Ref, const HyPortalGate2d &gate2Ref) :	m_Gate1(gate1Ref),
																							m_Gate2(gate2Ref)
{
}

HyPortal2d::~HyPortal2d()
{
}
