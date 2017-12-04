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
#include "Scene/Nodes/Draws/Entities/HyEntity2d.h"
#include "Renderer/Effects/HyStencil.h"

class HyPortalGate2d
{
	friend class HyPortal2d;

	const glm::vec2		m_ptPOINT1;
	const glm::vec2		m_ptPOINT2;
	glm::vec2			m_ptMidPoint;
	glm::vec2			m_vEntranceNormal;

	HyShape2d			m_BoundingVolume;
	b2Transform			m_Transform;

	HyStencil *			m_pStencil;
	HyPrimitive2d		m_StencilShape;

	// Debug drawing
	class DebugDraw : public HyEntity2d
	{
		HyPortalGate2d &m_Ref;

		HyPrimitive2d	m_BV;
		HyPrimitive2d	m_Gate;
		HyPrimitive2d	m_EntranceNormal;
		HyPrimitive2d	m_Stencil;

	public:
		DebugDraw(HyPortalGate2d &gateRef) :	m_Ref(gateRef),
												m_BV(this),
												m_Gate(this),
												m_EntranceNormal(this),
												m_Stencil(this)
		{
			m_Gate.GetShape().SetAsLineSegment(m_Ref.GetPt1(), m_Ref.GetPt2());
			m_Gate.SetTint(0xFF0000FF);
			m_Gate.SetLineThickness(5.0f);

			m_EntranceNormal.GetShape().SetAsLineSegment(m_Ref.Midpoint(), m_Ref.Midpoint() + (m_Ref.EntranceNormal() * 25.0f));
			m_EntranceNormal.SetTint(0xFF0000FF);
			m_EntranceNormal.SetLineThickness(5.0f);

			m_BV.GetShape().SetAsPolygon(reinterpret_cast<const glm::vec2 *>(m_Ref.GetBV()->m_vertices), m_Ref.GetBV()->m_count);
			m_BV.SetTint(0xFFFF0000);
			m_BV.SetWireframe(true);

			m_Stencil.GetShape().SetAsPolygon(reinterpret_cast<const glm::vec2 *>(static_cast<b2PolygonShape *>(m_Ref.m_StencilShape.GetShape().GetB2Shape())->m_vertices),
											  static_cast<b2PolygonShape *>(m_Ref.m_StencilShape.GetShape().GetB2Shape())->m_count);
			m_Stencil.SetTint(0xFFFFFF00);
			m_Stencil.SetWireframe(true);

			Load();
		}
	};
	DebugDraw *			m_pDebugDraw;

public:
	HyPortalGate2d(const glm::vec2 &pt1, const glm::vec2 &pt2, const glm::vec2 &ptEntrance, float fDepthAmt, float fStencilCullExtents);
	~HyPortalGate2d();

	const glm::vec2 &GetPt1() const;
	const glm::vec2 &GetPt2() const;
	const glm::vec2 &Midpoint() const;
	const glm::vec2 &EntranceNormal() const;

	const b2PolygonShape *GetBV() const;
	const b2Transform &GetTransform() const;
	HyStencil *GetStencil() const;

	void EnableDebugDraw(bool bEnable);
};

#endif /* HyPortalGate2d_h__ */
