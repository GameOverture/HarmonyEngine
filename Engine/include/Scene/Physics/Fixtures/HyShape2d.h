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
#include "Scene/Physics/Fixtures/IHyFixture2d.h"

class HyEntity2d;
class HyRect;

class HyShape2d : public IHyFixture2d
{
	union ShapeData
	{
		b2Circle		circle;
		b2Segment		segment;
		b2Polygon		polygon;
		b2Capsule		capsule;
	}											m_Data;		// NOTE: This shape is stored in pixel units like everything else. It is converted to pixel-per-meters when sent to Box2d
	
	b2ShapeId									m_hPhysics;
	b2ShapeDef *								m_pPhysicsInit;

public:
	HyShape2d(HyEntity2d *pParent = nullptr);
	HyShape2d(const HyShape2d &copyRef);
	virtual ~HyShape2d();

	const HyShape2d &operator=(const HyShape2d &rhs);

	b2Circle GetAsCircle() const;
	b2Segment GetAsSegment() const;
	b2Polygon GetAsPolygon() const;
	b2Capsule GetAsCapsule() const;

	virtual void TransformSelf(const glm::mat4 &mtxTransform) override;
	virtual std::vector<float> SerializeSelf() const override;
	virtual std::vector<glm::vec2> DeserializeSelf(HyFixtureType eFixtureType, const std::vector<float> &floatList) override;

	bool GetCentroid(glm::vec2 &ptCentroidOut) const;
	float CalcArea() const; // Returns the area in meters squared

	void SetAsNothing();

	// Set as an isolated edge/line. When used in a physics simulation, these segments have double sided collision
	void SetAsLineSegment(const glm::vec2 &pt1, const glm::vec2 &pt2, const b2ShapeDef *pPhysicsInit = nullptr);

	// Set as a circle with the specified center and radius
	bool SetAsCircle(float fRadius, const b2ShapeDef *pPhysicsInit = nullptr);
	bool SetAsCircle(const glm::vec2 &ptCenter, float fRadius, const b2ShapeDef *pPhysicsInit = nullptr);

	// Set as a convex hull from the given array of local points.
	// uiCount must be in the range [3, B2_MAX_POLYGON_VERTICES].
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
	void Setup(const b2ShapeDef &shapeDefRef);
	float GetDensity() const;
	void SetDensity(float fDensity, bool bUpdateBodyMass = true); // Usually in kg / m ^ 2.
	void SetDensityInKg(float fWeightKg, bool bUpdateBodyMass = true); // Sets the density using the "weight" of currently set shape
	float GetFriction() const;
	void SetFriction(float fFriction);
	float GetRestitution() const;
	void SetRestitution(float fRestitution);
	b2Filter GetFilter() const; // If this shape is a chain, it will return the filter for the first edge
	void SetFilter(const b2Filter &filter); // WARNING: This function is potentially expensive if this shape is a line chain and its parent entity has already been physics.Activate()
	bool IsSensor() const;

	virtual bool TestPoint(const glm::vec2 &ptTestPoint, const glm::mat4 &mtxSelfTransform) const override;
	virtual b2CastOutput TestRay(const glm::vec2 &ptStart, const glm::vec2 &vDirection, const glm::mat4 &mtxSelfTransform) const override;
	virtual bool IsColliding(const IHyFixture2d &testShape, b2Manifold *pManifoldOut = nullptr) const override;

	virtual bool ComputeAABB(b2AABB &aabbOut, const glm::mat4 &mtxTransform) const override;

protected:
	void ClearShapeData();
	void ShapeChanged();

	bool IsPhysicsRegistered() const;
	bool IsPhysicsInitialized() const;
	virtual void PhysicsAttach() override;
	virtual void PhysicsRemove(bool bUpdateBodyMass) override;

	// NOTE: Assumes 'shapeDataOut' starts as zeroed-out ShapeData
	bool TransformShapeData(ShapeData &shapeDataOut, const glm::mat4 &mtxTransform) const;
	bool TransformShapeData(ShapeData &shapeDataOut, float fPpmInverse) const;
};

#endif /* HyShape2d_h__ */
