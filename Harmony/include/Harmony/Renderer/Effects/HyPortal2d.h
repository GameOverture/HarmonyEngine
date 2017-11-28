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
#include "Renderer/Effects/HyStencil.h"

class HyEntity2d;

class HyPortalGate2d
{
	HyStencil *			m_pStencil;
	HyPrimitive2d		m_StencilShape;
	HyShape2d			m_BoundingVolume;

public:
	HyPortalGate2d(const glm::vec2 &pt1, const glm::vec2 &pt2, bool bPositiveYNormal, float fStencilCullExtents /*= 2500.0f*/) :	m_pStencil(HY_NEW HyStencil()),
																																	m_StencilShape(nullptr),
																																	m_BoundingVolume(nullptr)
	{
		m_BoundingVolume.SetAsLineSegment(pt1, pt2);

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Setup the stencil (m_pStencil & m_StencilShape)
		glm::vec2 stencilVertList[4];
		stencilVertList[0] = pt1;
		stencilVertList[0] += glm::normalize(pt1 - pt2) * fStencilCullExtents;
		stencilVertList[1] = pt2;
		stencilVertList[1] += glm::normalize(pt2 - pt1) * fStencilCullExtents;

		// If dx = x2 - x1 and dy = y2 - y1, then the normals are (-dy, dx) and (dy, -dx)
		glm::vec2 vNormal1(-(pt2.y - pt1.y), (pt2.x - pt1.x));
		glm::vec2 vNormal2((pt2.y - pt1.y), -(pt2.x - pt1.x));

		stencilVertList[2] = pt2;
		stencilVertList[3] = pt1;
		if((bPositiveYNormal && vNormal1.y > 0) || (bPositiveYNormal == false && vNormal1.y <= 0))
		{
			stencilVertList[2] += vNormal1 * fStencilCullExtents;
			stencilVertList[3] += vNormal1 * fStencilCullExtents;
		}
		else
		{
			stencilVertList[2] += vNormal2 * fStencilCullExtents;
			stencilVertList[3] += vNormal2 * fStencilCullExtents;
		}

		m_StencilShape.GetShape().SetAsPolygon(stencilVertList, 4);
		m_pStencil->AddInstance(&m_StencilShape);
	}

	~HyPortalGate2d()
	{
		m_pStencil->Destroy();
	}
};

class HyPortal2d
{
	friend class IHyRenderer;

	static HyPortal2dHandle			sm_hHandleCount;
	const HyPortal2dHandle			m_hHANDLE;

	HyPortalGate2d					m_Gate1;
	HyPortalGate2d					m_Gate2;

	std::vector<IHyDrawInst2d *>	m_DrawInstList;

public:
	HyPortal2d(const HyPortalGate2d &gate1Ref, const HyPortalGate2d &gate2Ref);
private: ~HyPortal2d();
public:
	void Destroy();

	HyPortal2dHandle GetHandle() const;

private:
	void TestEntity(HyEntity2d *pEnt);
};

#endif /* HyPortal2d_h__ */
