/**************************************************************************
*	HyPhysicsCtrl2d.h
*	
*	Harmony Engine
*	Copyright (c) 2022 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyPhysicsCtrl2d_h__
#define HyPhysicsCtrl2d_h__

#include "Afx/HyStdAfx.h"
#include "Scene/Physics/HyShape2d.h"

struct HyPhysicsComponent2d;

class HyPhysicsCtrl2d
{
	friend class HyPhysicsGrid2d;			// To invoke Update
	friend class IHyBody2d;					// To invoke FlushTransform

	b2BodyDef *				m_pInit;		// Dynamically allocated when physics simulation is desired. Simulation will then start if/once owner of *this is a child of a HyPhysicsGrid2d.
	HyPhysicsComponent2d *	m_pData;		// A pointer to the parent's concrete value in HyPhysicsGrid2d::m_PhysChildMap. Null if owner of *this is not yet a child of a HyPhysicsGrid2d

public:
	HyPhysicsCtrl2d();
	~HyPhysicsCtrl2d();

	void Init(const b2BodyDef &bodyDef);
	void Init(HyPhysicsType eType,
		bool bIsEnabled = true,
		bool bIsFixedRotation = false,
		bool bIsAwake = true,
		glm::vec2 vLinearVelocity = glm::vec2(0.0f, 0.0f),
		float fAngularVelocity = 0.0f,
		float fLinearDamping = 0.0f,
		float fAngularDamping = 0.0f,
		bool bAllowSleep = true,
		float fGravityScale = 1.0f,
		bool bIsCcd = false);

	void Uninit();
	bool IsSimulating() const;

	HyPhysicsType GetType() const;
	void SetType(HyPhysicsType eType);
	bool IsEnabled() const;
	void SetEnabled(bool bEnable);
	bool IsFixedRotation() const;
	void SetFixedRotation(bool bFixedRot);
	bool IsAwake() const;
	void SetAwake(bool bAwake);
	glm::vec2 GetLinearVelocity() const;
	void SetLinearVelocity(glm::vec2 vVelocity);
	float GetAngularVelocity() const;
	void SetAngularVelocity(float fOmega);
	float GetLinearDamping() const;
	void SetLinearDamping(float fLinearDamping);
	float GetAngularDamping() const;
	void SetAngularDamping(float fAngularDamping);
	bool IsSleepingAllowed() const;
	void SetSleepingAllowed(bool bAllowSleep);
	float GetGravityScale() const;
	void SetGravityScale(float fGravityScale);
	bool IsCcd() const;
	void SetCcd(bool bContinuousCollisionDetection);

	glm::vec2 GridCenterMass() const;
	glm::vec2 LocalCenterMass() const;
	void ApplyForce(const glm::vec2 &vForce, const glm::vec2 &ptPoint, bool bWake);
	void ApplyForceToCenter(const glm::vec2 &vForce, bool bWake);
	void ApplyTorque(float fTorque, bool bWake);
	void ApplyLinearImpulse(const glm::vec2 &vImpulse, const glm::vec2 &ptPoint, bool bWake);
	void ApplyLinearImpulseToCenter(const glm::vec2 &vImpulse, bool bWake);
	void ApplyAngularImpulse(float fImpulse, bool bWake);
	float GetMass() const;
	float GetInertia() const;

	void SetFilterData(b2Filter &Filter);
	const b2Filter &GetFilterData(int iIndex);

private:
	void Update();			// Should only be invoked by the parent HyPhysicsGrid2d
	void FlushTransform();	// Should only be invoked by the node IHyBody2d

	// fFriction : The friction coefficient, usually in the range [0,1].
	// fRestitution : (elasticity) usually in the range [0,1].
	// fDensity : usually in kg/m^2.
	// bIsSensor : Is a sensor shape collects contact information but never generates a collision response.
	std::unique_ptr<HyPhysicsCtrl2d> PhysAddCollider(const HyShape2d &shapeRef, float fDensity, float fFriction, float fRestitution, bool bIsSensor, b2Filter collideFilter);
	std::unique_ptr<HyPhysicsCtrl2d> PhysAddCircleCollider(float fRadius, float fDensity, float fFriction, float fRestitution, bool bIsSensor, b2Filter collideFilter);
	std::unique_ptr<HyPhysicsCtrl2d> PhysAddCircleCollider(const glm::vec2 &ptCenter, float fRadius, float fDensity, float fFriction, float fRestitution, bool bIsSensor, b2Filter collideFilter);
	std::unique_ptr<HyPhysicsCtrl2d> PhysAddLineChainCollider(const glm::vec2 *pVerts, uint32 uiNumVerts, float fDensity, float fFriction, float fRestitution, bool bIsSensor, b2Filter collideFilter);
	void PhysDestroyCollider(std::unique_ptr<HyPhysicsCtrl2d> pCollider);
};

#endif /* HyPhysicsCtrl2d_h__ */
