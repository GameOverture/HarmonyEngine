/**************************************************************************
*	HyLocomotionPhys.h
*
*	Harmony Engine
*	Copyright (c) 2026 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyLocomotionPhys_h__
#define HyLocomotionPhys_h__

#include "Afx/HyStdAfx.h"
#include "Utilities/HyLocomotion.h"

class HyLocomotionPhys2d : public HyLocomotion2d
{
protected:
	float					m_fJumpSpeed;		// Meters per second
	float					m_fMoverGravity;	// Meters per second squared
	float					m_fFriction;		// Friction has units of 1/time
	float					m_fAirSteer;

	float					m_fPogoVelocity;

	static constexpr int	m_NUM_PLANES = 8;
	b2CollisionPlane		m_planes[m_NUM_PLANES] = {};
	int						m_iPlaneCount;

	b2Capsule				m_Mover;

public:
	HyLocomotionPhys2d();
	HyLocomotionPhys2d(glm::vec2 vSize, float fMinSpeed, float fMaxSpeed, float fAcceleration, float fDeceleration, float fJumpSpeed, float fMoverGravity, float fFriction, float fAirSteer);
	HyLocomotionPhys2d(glm::vec2 vSize, const HyLocomotionParams &initRef);
	virtual ~HyLocomotionPhys2d();

	void Setup(glm::vec2 vSize, const HyLocomotionParams &initRef);
	void SetSize(glm::vec2 vSize);
	const b2Capsule &GetMover() const;

	void Jump();

	// Should be invoked every frame after all Go*() functions have been called - Uses the active physics simulation
	void Update(b2WorldId hWorld, glm::vec2 &ptPosInOut, bool &bOnGroundInOut, b2QueryFilter pogoFilter, b2QueryFilter collideFilter, b2QueryFilter castFilter);

private:
	static bool PlaneResultFcn(b2ShapeId shapeId, const b2PlaneResult *planeResult, void *pContext);
};

#endif /* HyLocomotion_h__ */
