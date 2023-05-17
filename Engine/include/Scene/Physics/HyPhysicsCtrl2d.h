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
	b2BodyDef *					m_pInit;					// Dynamically allocated when physics simulation is getting initialized. Simulation will then start when Activate() is invoked. It is more optimal to initalize before calling Activate().
	b2Body *					m_pBody;					// A pointer to the concrete value in HyScene::m_NodeMap_Collision, nullptr otherwise

public:
	HyPhysicsCtrl2d(HyEntity2d &entityRef);
	~HyPhysicsCtrl2d();

	void Activate();										// Enables physics simulation when invoked. It is more optimal to initalize values and append shapes to this entity before calling Activate().
	void Deactivate();										// Disables physics simulation when invoked. This does not delete the body/fixtures under the hood in Box2d
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
	bool IsSleepingAllowed() const;
	void SetSleepingAllowed(bool bAllowSleep);
	float GetGravityScale() const;
	void SetGravityScale(float fGravityScale);
	bool IsCcd() const;
	void SetCcd(bool bContinuousCollisionDetection);

	glm::vec2 GetVel() const;								// Retrieve the linear velocity of this body's center of mass in m/s
	float GetVelX() const;									// Retrieve the X linear velocity of this body's center of mass in m/s
	float GetVelY() const;									// Retrieve the Y linear velocity of this body's center of mass in m/s
	void SetVel(const glm::vec2 &vVelocity);				// Set the linear velocity of this body's center of mass in m/s
	void SetVel(float fVelocityX, float fVelocityY);		// Set the linear velocity of this body's center of mass in m/s
	void SetVelX(float fVelocityX);							// Set the X linear velocity of this body's center of mass in m/s
	void SetVelY(float fVelocityY);							// Set the Y linear velocity of this body's center of mass in m/s

	float GetAngVel() const;								// Retrieve the angular velocity of this body in radians/second
	void SetAngVel(float fOmega);							// Set the angular velocity of this body in radians/second

	void AddForce(const glm::vec2 &vForce);
	void AddForceX(float fForceX);
	void AddForceY(float fForceY);

	void AddForceToPt(const glm::vec2 &vForce, const glm::vec2 &ptScenePoint);
	void AddTorque(float fTorque);

	void AddImpulse(const glm::vec2 &vImpulse);
	void AddImpulseX(float fImpuseX);
	void AddImpulseY(float fImpuseY);

	void AddImpulseToPt(const glm::vec2 &vImpulse, const glm::vec2 &ptScenePoint);
	void AddAngImpulse(float fImpulse);

	float GetLinearDamping() const;
	void SetLinearDamping(float fLinearDamping);
	float GetAngularDamping() const;
	void SetAngularDamping(float fAngularDamping);

	glm::vec2 GetSceneCenterMass() const;
	glm::vec2 GetLocalCenterMass() const;

	float GetMass() const;
	float GetInertia() const;
};

#endif /* HyPhysicsCtrl2d_h__ */
