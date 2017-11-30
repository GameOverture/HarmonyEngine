/**************************************************************************
*	HyPortalGate2d.h
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyPortalGate2d_h__
#define HyPortalGate2d_h__

#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Draws/Instances/HyPrimitive2d.h"
#include "Renderer/Effects/HyStencil.h"

class HyPortalGate2d
{
	const glm::vec2		m_ptPOINT1;
	const glm::vec2		m_ptPOINT2;
	glm::vec2			m_ptMidPoint;

	HyShape2d			m_BoundingVolume;
	b2Transform			m_Transform;

	HyStencil *			m_pStencil;
	HyPrimitive2d		m_StencilShape;

public:
	HyPortalGate2d(const glm::vec2 &pt1, const glm::vec2 &pt2, const glm::vec2 &ptEntrance, float fDepthAmt, float fStencilCullExtents);
	~HyPortalGate2d();

	const glm::vec2 &GetPt1() const;
	const glm::vec2 &GetPt2() const;
	const glm::vec2 &Midpoint() const;

	const b2PolygonShape *GetBV() const;
	const b2Transform &GetTransform() const;
};

#endif /* HyPortalGate2d_h__ */
