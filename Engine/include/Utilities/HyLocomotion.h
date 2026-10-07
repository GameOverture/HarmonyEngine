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

struct HyLocomotionParams
{
	// Simple
	float				m_fMaxSpeed;		// Meters per second
	float				m_fMinSpeed;		// Meters per second
	float				m_fAccel;			// Meters per second squared
	float				m_fDecel;			// Meters per second squared

	// Physical
	float				m_fJumpSpeed;		// Meters per second
	float				m_fMoverGravity;	// Meters per second squared
	float				m_fFriction;		// Friction has units of 1/time
	float				m_fAirSteer;

	HyLocomotionParams();
	HyLocomotionParams(float fMinSpeed, float fMaxSpeed, float fAcceleration, float fDeceleration);
	HyLocomotionParams(float fMinSpeed, float fMaxSpeed, float fAcceleration, float fDeceleration, float fJumpSpeed, float fMoverGravity, float fFriction, float fAirSteer);

	void Setup(float fMinSpeed, float fMaxSpeed, float fAcceleration, float fDeceleration);
	void SetupPhys(float fMinSpeed, float fMaxSpeed, float fAcceleration, float fDeceleration, float fJumpSpeed, float fMoverGravity, float fFriction, float fAirSteer);
};

class HyLocomotion2d
{
protected:
	float					m_fMaxSpeed;		// Meters per second
	float					m_fMinSpeed;		// Meters per second
	float					m_fAccel;			// Meters per second squared
	float					m_fDecel;			// Meters per second squared

	glm::vec2				m_vThrottle;
	glm::vec2				m_vVelocity;

public:
	HyLocomotion2d();
	HyLocomotion2d(const HyLocomotionParams &initRef);
	HyLocomotion2d(float fMinSpeed, float fMaxSpeed, float fAcceleration, float fDeceleration);
	virtual ~HyLocomotion2d();

	bool IsMoving() const;
	glm::vec2 GetVelocity() const;
	void SetVelocity(glm::vec2 vVelocity);
	void SetVelocityX(float fVelocityX);
	void SetVelocityY(float fVelocityY);

	virtual void Setup(const HyLocomotionParams &initRef);

	void GoUp();							// Should be invoked every frame going UP is desired. Called before Update()
	void GoDown();							// Should be invoked every frame going DOWN is desired. Called before Update()
	void GoLeft();							// Should be invoked every frame going LEFT is desired. Called before Update()
	void GoRight();							// Should be invoked every frame going RIGHT is desired. Called before Update()
	void SetThrottle(glm::vec2 vThrottle);

	void StopX();
	void StopY();

	virtual void Update();					// Should be invoked every frame after all Go*() functions have been called
};

#endif /* HyLocomotion_h__ */
