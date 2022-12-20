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

class HyEntity2d;

class HyPhysicsCtrl2d
{
	friend class HyScene;
	friend class HyEntity2d;

	HyEntity2d &				m_EntityRef;
	b2BodyDef *					m_pInit;	// Dynamically allocated when physics simulation is desired. Simulation will then start if/once owner of *this is a child of a HyPhysicsGrid2d.
	b2Body *					m_pBody;	// A pointer to the concrete value in HyScene::m_NodeMap_Collision, nullptr otherwise

public:
	HyPhysicsCtrl2d(HyEntity2d &entityRef);
	~HyPhysicsCtrl2d();

	void Activate();
	void Deactivate();
	bool IsActivated() const;

	void Setup(const b2BodyDef &bodyDef);
	void Setup(HyBodyType eType,
		bool bIsEnabled = true,
		float fGravityScale = 1.0f,
		bool bIsFixedRotation = false,
		bool bIsAwake = true,
		glm::vec2 vLinearVelocity = glm::vec2(0.0f, 0.0f),
		float fAngularVelocity = 0.0f,
		float fLinearDamping = 0.0f,
		float fAngularDamping = 0.0f,
		bool bAllowSleep = true,
		bool bIsCcd = false);

	HyBodyType GetType() const;
	void SetType(HyBodyType eType);
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
};

#endif /* HyPhysicsCtrl2d_h__ */
