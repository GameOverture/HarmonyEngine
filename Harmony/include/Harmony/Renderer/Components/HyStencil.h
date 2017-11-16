/**************************************************************************
*	HyStencil.h
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyStencil_h__
#define HyStencil_h__

#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Leafs/Draws/HyPrimitive2d.h"

class HyStencil
{
	static HyStencilHandle			sm_hHandleCount;

	const HyStencilHandle			m_hHANDLE;
	HyPrimitive2d					m_Shape;

public:
	HyStencil();
	~HyStencil();

	HyStencilHandle GetHandle();
	HyShape2d &GetShape();
};

#endif /* HyStencil_h__ */
