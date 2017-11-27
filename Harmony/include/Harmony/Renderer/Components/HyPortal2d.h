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
#include "Scene/Nodes/Draws/Instances/HyPrimitive2d.h"
#include "Renderer/Components/HyStencil.h"

class HyPortalGate2d
{
	bool				m_bPOSITIVE_NORMAL;
	HyStencil *			m_pStencil;
	HyPrimitive2d		m_StencilShape;
	HyShape2d			m_BoundingVolume;

public:
	HyPortalGate2d(const glm::vec2 &pt1, const glm::vec2 &pt2, bool bPositiveNormal) :	m_bPOSITIVE_NORMAL(bPositiveNormal),
																						m_pStencil(HY_NEW HyStencil()),
																						m_StencilShape(nullptr),
																						m_BoundingVolume(nullptr)
	{
		//m_StencilShape.GetShape().SetAsBox(2500.0f, 2500.0f, 

		m_BoundingVolume.SetAsLineSegment(pt1, pt2);
	}

	~HyPortalGate2d()
	{
		m_pStencil->Destroy();
	}
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
