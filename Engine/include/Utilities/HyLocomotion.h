/**************************************************************************
*	HyLocomotion.h
*
*	Harmony Engine
*	Copyright (c) 2024 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyLocomotion_h__
#define HyLocomotion_h__

#include "Afx/HyStdAfx.h"

class HyLocomotion2d
{
	glm::vec2				m_vThrottle;

	// Simple + Physical
	glm::vec2				m_vVelocity;
	float					m_fMaxSpeed;
	float					m_fMinSpeed;
	float					m_fAccel;
	float					m_fDecel;

	// Physical
	float					m_fPogoVelocity;
	float					m_fJumpSpeed;
	float					m_fMoverGravity;
	float					m_fFriction;			// Friction has units of 1/time
	float					m_fAirSteer;

public:
	static constexpr int	m_planeCapacity = 8;
	b2CollisionPlane		m_planes[m_planeCapacity] = {};
	int						m_planeCount;

public:
	HyLocomotion2d();
	HyLocomotion2d(float fMinSpeed, float fMaxSpeed, float fAcceleration, float fDeceleration);
	HyLocomotion2d(float fMinSpeed, float fMaxSpeed, float fAcceleration, float fDeceleration, float fJumpSpeed, float fMoverGravity, float fFriction, float fAirSteer);
	~HyLocomotion2d();

	bool IsMoving() const;
	glm::vec2 GetVelocity() const;
	void SetVelocity(glm::vec2 vVelocity);
	void SetVelocityX(float fVelocityX);
	void SetVelocityY(float fVelocityY);

	void SetupSimple(float fMinSpeed, float fMaxSpeed, float fAcceleration, float fDeceleration);
	void SetupPhysical(float fMinSpeed, float fMaxSpeed, float fAcceleration, float fDeceleration, float fJumpSpeed, float fMoverGravity, float fFriction, float fAirSteer);

	void GoUp();							// Should be invoked every frame going UP is desired. Called before Update()
	void GoDown();							// Should be invoked every frame going DOWN is desired. Called before Update()
	void GoLeft();							// Should be invoked every frame going LEFT is desired. Called before Update()
	void GoRight();							// Should be invoked every frame going RIGHT is desired. Called before Update()
	void SetThrottle(glm::vec2 vThrottle);

	void Jump();
	void StopX();
	void StopY();

	void UpdateSimple();					// Should be invoked every frame after all Go*() functions have been called
	void UpdatePhysical(b2WorldId hWorld, glm::vec2 &ptPosOut, bool &bOnGroundOut, const b2Capsule &moverRef, b2QueryFilter pogoFilter, b2QueryFilter collideFilter, b2QueryFilter castFilter);	// Should be invoked every frame after all Go*() functions have been called - Uses the active physics simulation

private:
	static bool PlaneResultFcn(b2ShapeId shapeId, const b2PlaneResult *planeResult, void *context);
};

#endif /* HyLocomotion_h__ */
