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

class HyEntity2d;
class HyRect;

class HyShape2d
{
	friend class HyEntity2d;
	friend class HyPrimitive2d;
	friend class HyPhysicsCtrl2d;
	friend class HyBox2dDestructListener;

	HyShapeType									m_eType;
	HyEntity2d *								m_pParent;

	b2Shape *									m_pShape;

	bool 										m_bIsFixtureAllowed;
	b2FixtureDef *								m_pInit;
	b2Fixture *									m_pFixture;
	bool										m_bFixtureDirty;

public:
	static const float							FloatSlop;

	HyShape2d(HyEntity2d *pParent = nullptr);
	HyShape2d(const HyShape2d &copyRef);
	virtual ~HyShape2d();

	const HyShape2d &operator=(const HyShape2d &rhs);

	HyShapeType GetType() const;
	bool IsValidShape() const;

	glm::vec2 ComputeSize() const;
	void GetCentroid(glm::vec2 &ptCentroidOut) const;
	float CalcArea() const; // Returns the area in meters squared
	
	const b2Shape *GetB2Shape() const;
	b2Shape *ClonePpmShape(float fPpmInverse) const;

	void ParentDetach();
	HyEntity2d *ParentGet() const;

	void SetAsNothing();

	void SetAsB2Shape(const b2Shape *pShape);

	// Set as an isolated edge.
	void SetAsLineSegment(const glm::vec2 &pt1, const glm::vec2 &pt2);
	void SetAsLineSegment(const b2Vec2 &pt1, const b2Vec2 &pt2);

	// Set as a line loop. This automatically connects last vertex to the first.
	// Passed in parameters are copied, and understood to be local coordinates
	void SetAsLineLoop(const glm::vec2 *pVertices, uint32 uiNumVerts);
	void SetAsLineLoop(const std::vector<glm::vec2> &verticesList);

	// Set as a line chain with isolated end vertices. Passed in parameters are 
	// copied, and understood to be local coordinates
	void SetAsLineChain(const glm::vec2 *pVertices, uint32 uiNumVerts);
	void SetAsLineChain(const std::vector<glm::vec2> &verticesList);

	// Set as a circle with the specified center and radius
	bool SetAsCircle(float fRadius);
	bool SetAsCircle(const glm::vec2 &ptCenter, float fRadius);
	bool SetAsCircle(const b2Vec2& center, float fRadius);

	// Set as a convex hull from the given array of local points.
	// uiCount must be in the range [3, b2_maxPolygonVertices].
	// The points may be re-ordered, even if they form a convex polygon
	// Collinear points are handled but not removed. Collinear points
	// may lead to poor stacking behavior in physics simulation.
	void SetAsPolygon(const glm::vec2 *pPointArray, uint32 uiCount);
	void SetAsPolygon(const b2Vec2 *pPointArray, uint32 uiCount);
	void SetAsPolygon(const std::vector<glm::vec2> &verticesList);

	bool SetAsBox(float fWidth, float fHeight); // Build vertices to represent an axis-aligned box, bottom left corner at 0,0
	bool SetAsBox(const HyRect &rect);			// Build vertices to represent an oriented box

	// Applies when attached to a physics body
	bool IsFixtureAllowed() const;
	void SetFixtureAllowed(bool bIsFixtureAllowed);	// The parent entity will ignore/remove this shape if 'bIsFixtureAllowed' == false
	void Setup(const b2FixtureDef &fixtureDefRef);
	float GetDensity() const;
	void SetDensity(float fDensity); // Usually in kg / m ^ 2.
	void SetDensityInKg(float fWeightKg); // Sets the density using the "weight" of currently set shape
	float GetFriction() const;
	void SetFriction(float fFriction);
	float GetRestitution() const;
	void SetRestitution(float fRestitution);
	float GetRestitutionThreshold() const;
	void SetRestitutionThreshold(float fRestitutionThreshold);
	b2Filter GetFilter() const;
	void SetFilter(const b2Filter &filter);
	bool IsSensor() const;
	void SetSensor(bool bIsSensor);

	bool TestPoint(const glm::mat4 &mtxSelfTransform, const glm::vec2 &ptTestPoint) const;
	//bool IsColliding(const glm::mat4 &mtxSelfTransform, const HyShape2d &testShape, const glm::mat4 &mtxTestTransform, b2WorldManifold &worldManifoldOut) const;

	bool ComputeAABB(b2AABB &aabbOut, const glm::mat4 &mtxTransform) const;

	b2Shape *CloneTransform(const glm::mat4 &mtxTransform) const;
	void TransformSelf(const glm::mat4 &mtxTransform);

protected:
	void CreateFixture(b2Body *pBody);
	void DestroyFixture();
	
	void ShapeChanged();
	bool IsFixtureDirty();
};

#endif /* HyShape2d_h__ */
