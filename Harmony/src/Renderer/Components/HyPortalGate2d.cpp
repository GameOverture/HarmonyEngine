/**************************************************************************
*	HyPortalGate2d.cpp
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Renderer/Components/HyPortalGate2d.h"

HyPortalGate2d::HyPortalGate2d(const glm::vec2 &pt1, const glm::vec2 &pt2, bool bPositiveYNormal, float fDepthAmt, float fStencilCullExtents /*= 5000.0f*/) :	m_ptPOINT1(pt1),
																																								m_ptPOINT2(pt2),
																																								m_pStencil(HY_NEW HyStencil()),
																																								m_StencilShape(nullptr),
																																								m_BoundingVolume(nullptr),
																																								m_Transform(b2Vec2(0.0f, 0.0f), b2Rot(0.0f))
{
	m_ptMidPoint.x = (m_ptPOINT1.x + m_ptPOINT2.x) / 2;
	m_ptMidPoint.y = (m_ptPOINT1.y + m_ptPOINT2.y) / 2;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Calculate oriented boxes to use for both the bounding volume and stencil
	glm::vec2 boundingVertList[4];
	glm::vec2 stencilVertList[4];

	boundingVertList[0] = m_ptPOINT1;
	stencilVertList[0] = m_ptPOINT1;
	stencilVertList[0] += glm::normalize(m_ptPOINT1 - m_ptPOINT2) * fStencilCullExtents;

	boundingVertList[1] = m_ptPOINT2;
	stencilVertList[1] = m_ptPOINT2;
	stencilVertList[1] += glm::normalize(m_ptPOINT2 - m_ptPOINT1) * fStencilCullExtents;

	// If dx = x2 - x1 and dy = y2 - y1, then the normals are (-dy, dx) and (dy, -dx)
	glm::vec2 vNormal1(-(m_ptPOINT2.y - m_ptPOINT1.y), (m_ptPOINT2.x - m_ptPOINT1.x));

	boundingVertList[2] = m_ptPOINT2;
	boundingVertList[3] = m_ptPOINT1;
	stencilVertList[2] = m_ptPOINT2;
	stencilVertList[3] = m_ptPOINT1;
	if((bPositiveYNormal && vNormal1.y > 0) || (bPositiveYNormal == false && vNormal1.y <= 0))
	{
		boundingVertList[2] += vNormal1 * fDepthAmt;
		boundingVertList[3] += vNormal1 * fDepthAmt;

		stencilVertList[2] += vNormal1 * fStencilCullExtents;
		stencilVertList[3] += vNormal1 * fStencilCullExtents;
	}
	else
	{
		glm::vec2 vNormal2((m_ptPOINT2.y - m_ptPOINT1.y), -(m_ptPOINT2.x - m_ptPOINT1.x));

		boundingVertList[2] += vNormal2 * fDepthAmt;
		boundingVertList[3] += vNormal2 * fDepthAmt;

		stencilVertList[2] += vNormal2 * fStencilCullExtents;
		stencilVertList[3] += vNormal2 * fStencilCullExtents;
	}

	m_BoundingVolume.SetAsPolygon(boundingVertList, 4);

	m_StencilShape.GetShape().SetAsPolygon(stencilVertList, 4);
	m_pStencil->AddInstance(&m_StencilShape);
}

HyPortalGate2d::~HyPortalGate2d()
{
	m_pStencil->Destroy();
}

const glm::vec2 &HyPortalGate2d::GetPt1() const
{
	return m_ptPOINT1;
}

const glm::vec2 &HyPortalGate2d::GetPt2() const
{
	return m_ptPOINT2;
}

const glm::vec2 &HyPortalGate2d::Midpoint() const
{
	return m_ptMidPoint;
}

const b2PolygonShape *HyPortalGate2d::GetBV() const
{
	return static_cast<const b2PolygonShape *>(m_BoundingVolume.GetB2Shape());
}

const b2Transform &HyPortalGate2d::GetTransform() const
{
	return m_Transform;
}
