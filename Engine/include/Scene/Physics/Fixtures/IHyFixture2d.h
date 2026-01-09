/**************************************************************************
*	IHyFixture2d.h
*
*	Harmony Engine
*	Copyright (c) 2025 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef IHyFixture2d_h__
#define IHyFixture2d_h__

#include "Afx/HyStdAfx.h"

class HyEntity2d;
class HyRect;

class IHyFixture2d
{
	friend class HyEntity2d;
	friend class HyPrimitive2d;

protected:
	HyEntity2d *								m_pParent;

	HyFixtureType								m_eType;
	bool 										m_bPhysicsAllowed;
	bool										m_bPhysicsDirty;

	// Physics Collisions
	float										m_fMaxPush;			// Setting this to FLT_MAX makes it as rigid as possible. Lower values can make the plane collision soft. Usually in meters.
	bool										m_bClipVelocity;	// Indicates if b2ClipVector should clip against this plane. Should be false for soft collision.

public:
	IHyFixture2d(HyEntity2d *pParent = nullptr);
	IHyFixture2d(const IHyFixture2d &copyRef);
	virtual ~IHyFixture2d();

	const IHyFixture2d &operator=(const IHyFixture2d &rhs);

	HyFixtureType GetType() const;
	bool IsValid() const;

	virtual void TransformSelf(const glm::mat4 &mtxTransform) = 0;
	virtual std::vector<float> SerializeSelf() const = 0;
	virtual void DeserializeSelf(HyFixtureType eFixtureType, const std::vector<float> &floatList) = 0;

	glm::vec2 ComputeSize() const;

	void ParentDetach();
	HyEntity2d *ParentGet() const;

	// Applies when attached to a physics body
	bool IsPhysicsAllowed() const;
	void SetPhysicsAllowed(bool bIsPhysicsAllowed);	// Whether the parent entity will use this shape for physics simulation

	virtual bool TestPoint(const glm::vec2 &ptTestPoint, const glm::mat4 &mtxSelfTransform) const = 0;
	virtual b2CastOutput TestRay(const glm::vec2 &ptStart, const glm::vec2 &vDirection, const glm::mat4 &mtxSelfTransform) const = 0;

	virtual bool ComputeAABB(b2AABB &aabbOut, const glm::mat4 &mtxTransform) const = 0;

	// Setting this to FLT_MAX makes the plane as rigid as possible. Lower values can make the plane collision soft. Usually in meters.
	void SetSoftCollision(float fMaxPush);
	void SetHardCollision();
	void GetCollisionInfo(float &fPushLimitOut, bool &bClipVelocityOut) const;

protected:
	bool IsPhysicsDirty() const;
	virtual void PhysicsAttach() = 0;
	virtual void PhysicsRemove(bool bUpdateBodyMass) = 0;
};

#endif /* IHyFixture2d_h__ */
