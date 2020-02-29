/**************************************************************************
*	HyShape2d.h
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyShape2d_h__
#define HyShape2d_h__

#include "Afx/HyStdAfx.h"

class IHyNode2d;

class HyShape2d
{
	IHyNode2d *						m_pOwnerNode;

	HyShapeType						m_eType;
	b2Shape *						m_pShape;

public:
	HyShape2d(IHyNode2d *pOwnerNode);
	HyShape2d(const HyShape2d &copyRef) = delete;
	HyShape2d(IHyNode2d *pOwnerNode, const HyShape2d &copyRef);
	virtual ~HyShape2d();

	const HyShape2d &operator=(const HyShape2d &rhs);

	HyShapeType GetType() const;
	void GetCentroid(glm::vec2 &ptCentroidOut) const;
	const b2Shape *GetB2Shape() const;
	b2Shape *GetB2Shape();

	bool IsValid() const;

	void SetAsNothing();

	// Set as an isolated edge.
	void SetAsLineSegment(const glm::vec2 &pt1, const glm::vec2 &pt2);
	void SetAsLineSegment(const b2Vec2 &pt1, const b2Vec2 &pt2);

	// Set as a line loop. This automatically connects last vertex to the first.
	// Passed in parameters are copied, and understood to be local coordinates
	void SetAsLineLoop(const glm::vec2 *pVertices, uint32 uiNumVerts);

	// Set as a line chain with isolated end vertices. Passed in parameters are 
	// copied, and understood to be local coordinates
	void SetAsLineChain(const glm::vec2 *pVertices, uint32 uiNumVerts);

	// Set as a circle with the specified center and radius
	void SetAsCircle(float fRadius);
	void SetAsCircle(const glm::vec2 &ptCenter, float fRadius);
	void SetAsCircle(const b2Vec2& center, float fRadius);

	// Set as a convex hull from the given array of local points.
	// uiCount must be in the range [3, b2_maxPolygonVertices].
	// The points may be re-ordered, even if they form a convex polygon
	// Collinear points are handled but not removed. Collinear points
	// may lead to poor stacking behavior in physics simulation.
	void SetAsPolygon(const glm::vec2 *pPointArray, uint32 uiCount);
	void SetAsPolygon(const b2Vec2 *pPointArray, uint32 uiCount);


	// Build vertices to represent an axis-aligned box
	void SetAsBox(int32 iWidth, int32 iHeight);
	void SetAsBox(float fWidth, float fHeight);

	// Build vertices to represent an oriented box.
	// ptBoxCenter is the center of the box in local coordinates.
	// fRot the rotation of the box in local coordinates.
	void SetAsBox(float fHalfWidth, float fHalfHeight, const glm::vec2 &ptBoxCenter, float fRotDeg);

	// Returns true if the world point intersects within this bounding volume's world transformation
	bool TestPoint(const glm::vec2 &ptWorldPointRef) const;

	bool IsColliding(HyShape2d &shapeRef, b2WorldManifold &worldManifoldOut);
};

#endif /* HyShape2d_h__ */
