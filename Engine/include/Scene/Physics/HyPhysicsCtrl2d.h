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

struct HyPhysicsInit2d
{
	b2BodyDef				m_BodyDef;
	b2FixtureDef			m_FixtureDef;
};

class HyPhysicsCtrl2d
{
	friend class HyPhysicsGrid2d;			// To invoke Update
	friend class IHyBody2d;					// To invoke FlushTransform

	IHyBody2d &				m_NodeRef;
	HyPhysicsInit2d *		m_pInit;		// Dynamically allocated when physics simulation is desired. Simulation will then start if/once owner of *this is a child of a HyPhysicsGrid2d.
	HyPhysicsComponent2d *	m_pSimData;		// A pointer to the parent's concrete value in HyPhysicsGrid2d::m_PhysChildMap. Null if owner of *this is not yet a child of a HyPhysicsGrid2d
	bool					m_bEnabled;

public:
	HyPhysicsCtrl2d(IHyBody2d &nodeRef);
	~HyPhysicsCtrl2d();

	// Don't set the b2FixtureDef::shape, instead use .shape of 'm_NodeRef'
	void Init(const b2BodyDef &bodyDef, const b2FixtureDef &fixtureDef);
	void Init(HyBodyType eType,
		bool bIsEnabled = true,
		bool bIsFixedRotation = false,
		float fDensity = 1.0f,
		float fFriction = 0.2f,
		bool bIsAwake = true,
		glm::vec2 vLinearVelocity = glm::vec2(0.0f, 0.0f),
		float fAngularVelocity = 0.0f,
		float fLinearDamping = 0.0f,
		float fAngularDamping = 0.0f,
		float fRestitution = 0.4f,
		float fRestitutionThreshold = 1.0f,
		b2Filter filter = b2Filter(),
		bool bAllowSleep = true,
		bool bIsSensor = false,
		float fGravityScale = 1.0f,
		bool bIsCcd = false);

	void Uninit();

	HyBodyType GetType() const;
	void SetType(HyBodyType eType);
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

	// Applies to the fixture/shape
	float GetDensity() const;
	void SetDensity(float fDensity);
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

private:
	void Update();			// Should only be invoked by the parent HyPhysicsGrid2d
	void FlushTransform();	// Should only be invoked by the node IHyBody2d
};

#endif /* HyPhysicsCtrl2d_h__ */
