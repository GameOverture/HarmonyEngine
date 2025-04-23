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

	HyEntity2d *								m_pParent;

	HyShapeType									m_eType;
	struct ChainData
	{
		glm::vec2 *		pPointList; // Dynamically allocated
		int				iCount;
		bool			bLoop; // If true, pPointList/iCount is guaranteed to not include the "final" point (a repeat of the first point)
	};
	union ShapeData
	{
		b2Capsule		capsule;
		b2Circle		circle;
		b2Polygon		polygon;
		b2Segment		segment;
		ChainData		chain;
	}											m_Data;		// NOTE: This shape is stored in pixel units like everything else. It is converted to pixel-per-meters when sent to Box2d

	bool 										m_bPhysicsAllowed;
	b2ShapeId									m_hPhysicsShape;
	b2ShapeDef *								m_pPhysicsInit;
	bool										m_bPhysicsDirty;

public:
	static const float							FloatSlop;

	HyShape2d(HyEntity2d *pParent = nullptr);
	HyShape2d(const HyShape2d &copyRef);
	virtual ~HyShape2d();

	const HyShape2d &operator=(const HyShape2d &rhs);

	HyShapeType GetType() const;
	bool IsValidShape() const;

	void TransformSelf(const glm::mat4 &mtxTransform);

	glm::vec2 ComputeSize() const;
	void GetCentroid(glm::vec2 &ptCentroidOut) const;
	float CalcArea() const; // Returns the area in meters squared
	
	//b2ShapeId GetB2Shape() const;
	//b2ShapeId ClonePpmShape(float fPpmInverse) const;

	void ParentDetach();
	HyEntity2d *ParentGet() const;

	void SetAsNothing();

	//void SetAsB2Shape(b2ShapeId hShape, const b2ShapeDef *pPhysicsInit = nullptr);

	// Set as an isolated edge/line. When used in a physics simulation, these segments have double sided collision
	void SetAsLineSegment(const glm::vec2 &pt1, const glm::vec2 &pt2, const b2ShapeDef *pPhysicsInit = nullptr);

	// A series of line segments chained to gether. 'bLoop' is whether to automatically connects last vertex to the first.
	// Passed in parameters are copied, and understood to be local coordinates.
	// When used in a physics simulation, the line chain only has right-side collision.
	void SetAsLineChain(const glm::vec2 *pVertices, uint32 uiNumVerts, bool bLoop, const b2ShapeDef *pPhysicsInit = nullptr);
	void SetAsLineChain(const std::vector<glm::vec2> &verticesList, bool bLoop, const b2ShapeDef *pPhysicsInit = nullptr);

	// Set as a circle with the specified center and radius
	bool SetAsCircle(float fRadius, const b2ShapeDef *pPhysicsInit = nullptr);
	bool SetAsCircle(const glm::vec2 &ptCenter, float fRadius, const b2ShapeDef *pPhysicsInit = nullptr);

	// Set as a convex hull from the given array of local points.
	// uiCount must be in the range [3, b2_maxPolygonVertices].
	// The points may be re-ordered, even if they form a convex polygon
	// Collinear points are handled but not removed. Collinear points
	// may lead to poor stacking behavior in physics simulation.
	bool SetAsPolygon(const glm::vec2 *pPointArray, uint32 uiCount, const b2ShapeDef *pPhysicsInit = nullptr);
	bool SetAsPolygon(const std::vector<glm::vec2> &verticesList, const b2ShapeDef *pPhysicsInit = nullptr);
	// TODO: Support rounded polygons

	bool SetAsBox(float fWidth, float fHeight, const b2ShapeDef *pPhysicsInit = nullptr); // Build vertices to represent an axis-aligned box, bottom left corner at 0,0
	bool SetAsBox(const HyRect &rect, const b2ShapeDef *pPhysicsInit = nullptr);			// Build vertices to represent an oriented box
	// TODO: Support rounded boxes

	bool SetAsCapsule(const glm::vec2 &pt1, const glm::vec2 &pt2, float fRadius, const b2ShapeDef *pPhysicsInit = nullptr);

	// Applies when attached to a physics body
	bool IsPhysicsAllowed() const;
	void SetPhysicsAllowed(bool bIsPhysicsAllowed);	// Whether the parent entity will use this shape for physics simulation
	void Setup(const b2ShapeDef &fixtureDefRef);
	float GetDensity() const;
	void SetDensity(float fDensity, bool bUpdateBodyMass = true); // Usually in kg / m ^ 2.
	void SetDensityInKg(float fWeightKg, bool bUpdateBodyMass = true); // Sets the density using the "weight" of currently set shape
	float GetFriction() const;
	void SetFriction(float fFriction);
	float GetRestitution() const;
	void SetRestitution(float fRestitution);
	b2Filter GetFilter() const;
	void SetFilter(const b2Filter &filter);
	bool IsSensor() const;

	bool TestPoint(const glm::vec2 &ptTestPoint, const glm::mat4 &mtxSelfTransform) const;
	//bool IsColliding(const glm::mat4 &mtxSelfTransform, const HyShape2d &testShape, const glm::mat4 &mtxTestTransform, b2WorldManifold &worldManifoldOut) const;

	bool ComputeAABB(b2AABB &aabbOut, const glm::mat4 &mtxTransform) const;

protected:
	void ClearShapeData();

	void CreateFixture(b2BodyId hBody);
	void DestroyFixture();
	
	void ShapeChanged();
	bool IsPhysicsDirty();

	// NOTE: Assumes 'shapeDataOut' starts as zeroed-out ShapeData. Will newly dynamically allocate for chain types
	bool TransformShapeData(ShapeData &shapeDataOut, const glm::mat4 &mtxTransform) const;
};

#endif /* HyShape2d_h__ */
