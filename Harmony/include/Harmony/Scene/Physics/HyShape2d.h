/**************************************************************************
*	HyShape2d.h
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyShape2d_h__
#define HyShape2d_h__

#include "Afx/HyStdAfx.h"
#include "Box2D/Box2D.h"

class IHyNode2d;

class HyShape2d
{
	IHyNode2d &						m_OwnerRef;
	b2Shape *						m_pShape;

public:
	HyShape2d(IHyNode2d &ownerRef);
	virtual ~HyShape2d();

	bool IsValid();

	// Create a line loop. This automatically adjusts connectivity. Passed in parameters are copied
	void SetAsLineLoop(const glm::vec2 *pVertices, uint32 uiNumVerts);

	// Create a line chain with isolated end vertices. Passed in parameters are copied
	void SetAsLineChain(const glm::vec2 *pVertices, uint32 uiNumVerts);

	// Set as an isolated edge.
	void SetAsLineSegment(const glm::vec2 &pt1, const glm::vec2 &pt2);

	void SetAsCircle(float fRadius);

	// Create a convex hull from the given array of local points.
	// uiCount must be in the range [3, b2_maxPolygonVertices].
	// The points may be re-ordered, even if they form a convex polygon
	// Collinear points are handled but not removed. Collinear points
	// may lead to poor stacking behavior.
	void SetAsPolygon(const glm::vec2 *pPointArray, uint32 uiCount);

	// Build vertices to represent an axis-aligned box centered on the local origin.
	void SetAsBox(float fHalfWidth, float fHalfHeight);

	// Build vertices to represent an oriented box.
	// ptBoxCenter is the center of the box in local coordinates.
	// fRot the rotation of the box in local coordinates.
	void SetAsBox(float fHalfWidth, float fHalfHeight, const glm::vec2 &ptBoxCenter, float fRotDeg);

	bool TestPoint(glm::vec2 ptWorldPoint) const;
};

#endif /* HyShape2d_h__ */
