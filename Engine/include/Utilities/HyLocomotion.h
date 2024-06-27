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

// NOTE: All values in pixels per second.
class HyLocomotion2d
{
	glm::vec2				m_vVelocity;
	float					m_fMaxSpeed;
	float					m_fAccel;
	float					m_fDecel;
	
	enum DirectionFlags
	{
		DIRECTION_UP		= 1 << 0,
		DIRECTION_DOWN		= 1 << 1,
		DIRECTION_LEFT		= 1 << 2,
		DIRECTION_RIGHT		= 1 << 3
	};
	uint32					m_uiDirectionFlags;

public:
	HyLocomotion2d();
	HyLocomotion2d(float fMaxSpeed, float fAcceleration, float fDeceleration);
	~HyLocomotion2d();

	bool IsMoving() const;
	glm::vec2 GetVelocity() const;

	void Setup(float fMaxSpeed, float fAcceleration, float fDeceleration);

	void GoUp();		// Should be invoked every frame going UP is desired. Called before Update()
	void GoDown();		// Should be invoked every frame going DOWN is desired. Called before Update()
	void GoLeft();		// Should be invoked every frame going LEFT is desired. Called before Update()
	void GoRight();		// Should be invoked every frame going RIGHT is desired. Called before Update()

	void StopX();
	void StopY();

	void Update();		// Should be invoked every frame after all Go*() functions have been called
};

#endif /* HyLocomotion_h__ */
