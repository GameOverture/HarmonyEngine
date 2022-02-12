/**************************************************************************
*	HyPhysics2d.h
*	
*	Harmony Engine
*	Copyright (c) 2022 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyPhysics2d_h__
#define HyPhysics2d_h__

#include "Afx/HyStdAfx.h"
#include "Scene/Physics/HyShape2d.h"

class HyPhysics2d
{
	HyEntity2d &	m_OwnerRef;

	b2Body *		m_pPhysicsBody;
	b2Fixture *		m_pFixture;

public:
	HyPhysics2d(HyEntity2d &ownerRef);
	~HyPhysics2d();

	void PhysInit(HyPhysicsGrid2d &physGridRef,
		HyPhysicsType eType,
		b2Vec2 pos,
		bool bIsEnabled = true,
		bool bIsFixedRotation = false,
		bool bIsCcd = false,
		bool bIsAwake = true,
		bool bAllowSleep = true,
		float fGravityScale = 1.0f);

	void PhysInit(HyPhysicsGrid2d &physGridRef,
		HyPhysicsType eType,
		bool bIsEnabled = true,
		bool bIsFixedRotation = false,
		bool bIsCcd = false,
		bool bIsAwake = true,
		bool bAllowSleep = true,
		float fGravityScale = 1.0f);

	HyPhysicsType PhysGetType() const;
	void PhysSetType(HyPhysicsType eType);
	bool PhysIsEnabled() const;
	void PhysSetEnabled(bool bEnable);
	bool PhysIsFixedRotation() const;
	void PhysSetFixedRotation(bool bFixedRot);
	bool PhysIsCcd() const;
	void PhysSetCcd(bool bContinuousCollisionDetection);
	bool PhysIsAwake() const;
	void PhysSetAwake(bool bAwake);
	bool PhysIsSleepingAllowed() const;
	void PhysSetSleepingAllowed(bool bAllowSleep);
	float PhysGetGravityScale() const;
	void PhysSetGravityScale(float fGravityScale);

	glm::vec2 PhysWorldCenterMass() const;
	glm::vec2 PhysLocalCenterMass() const;
	glm::vec2 PhysGetLinearVelocity() const;

	void PhysSetFilterData(b2Filter &Filter);
	const b2Filter &PhysGetFilterData(int iIndex);

	void PhysSetLinearVelocity(glm::vec2 vVelocity);
	float PhysGetAngularVelocity() const;
	void PhysSetAngularVelocity(float fOmega);
	void PhysApplyForce(const glm::vec2 &vForce, const glm::vec2 &ptPoint, bool bWake);
	void PhysApplyForceToCenter(const glm::vec2 &vForce, bool bWake);
	void PhysApplyTorque(float fTorque, bool bWake);
	void PhysApplyLinearImpulse(const glm::vec2 &vImpulse, const glm::vec2 &ptPoint, bool bWake);
	void PhysApplyLinearImpulseToCenter(const glm::vec2 &vImpulse, bool bWake);
	void PhysApplyAngularImpulse(float fImpulse, bool bWake);
	float PhysGetMass() const;
	float PhysGetInertia() const;




	// fFriction : The friction coefficient, usually in the range [0,1].
	// fRestitution : (elasticity) usually in the range [0,1].
	// fDensity : usually in kg/m^2.
	// bIsSensor : Is a sensor shape collects contact information but never generates a collision response.
	std::unique_ptr<HyPhysics2d> PhysAddCollider(const HyShape2d &shapeRef, float fDensity, float fFriction, float fRestitution, bool bIsSensor, b2Filter collideFilter);
	std::unique_ptr<HyPhysics2d> PhysAddCircleCollider(float fRadius, float fDensity, float fFriction, float fRestitution, bool bIsSensor, b2Filter collideFilter);
	std::unique_ptr<HyPhysics2d> PhysAddCircleCollider(const glm::vec2 &ptCenter, float fRadius, float fDensity, float fFriction, float fRestitution, bool bIsSensor, b2Filter collideFilter);
	std::unique_ptr<HyPhysics2d> PhysAddLineChainCollider(const glm::vec2 *pVerts, uint32 uiNumVerts, float fDensity, float fFriction, float fRestitution, bool bIsSensor, b2Filter collideFilter);
	void PhysDestroyCollider(std::unique_ptr<HyPhysics2d> pCollider);

	void PhysRelease();


	void Update();

private:
	b2Fixture *GetFixture();
};

#endif /* HyPhysics2d_h__ */
