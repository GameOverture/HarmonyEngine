/**************************************************************************
*	HyLocomotion.cpp
*
*	Harmony Engine
*	Copyright (c) 2024 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Utilities/HyLocomotion.h"
#include "HyEngine.h"

HyLocomotionParams::HyLocomotionParams() :
	m_fMinSpeed(0.0f),
	m_fMaxSpeed(10.0f),
	m_fAccel(30.0f),
	m_fDecel(50.0f),
	m_fJumpSpeed(10.0f),
	m_fMoverGravity(30.0f),
	m_fFriction(8.0f),
	m_fAirSteer(0.2f)
{ }

HyLocomotionParams::HyLocomotionParams(float fMinSpeed, float fMaxSpeed, float fAcceleration, float fDeceleration) :
	m_fMinSpeed(fMinSpeed),
	m_fMaxSpeed(fMaxSpeed),
	m_fAccel(fAcceleration),
	m_fDecel(fDeceleration),
	m_fJumpSpeed(10.0f),
	m_fMoverGravity(30.0f),
	m_fFriction(8.0f),
	m_fAirSteer(0.2f)
{ }

HyLocomotionParams::HyLocomotionParams(float fMinSpeed, float fMaxSpeed, float fAcceleration, float fDeceleration, float fJumpSpeed, float fMoverGravity, float fFriction, float fAirSteer) :
	m_fMinSpeed(fMinSpeed),
	m_fMaxSpeed(fMaxSpeed),
	m_fAccel(fAcceleration),
	m_fDecel(fDeceleration),
	m_fJumpSpeed(fJumpSpeed),
	m_fMoverGravity(fMoverGravity),
	m_fFriction(fFriction),
	m_fAirSteer(fAirSteer)
{ }

void HyLocomotionParams::Setup(float fMinSpeed, float fMaxSpeed, float fAcceleration, float fDeceleration)
{
	*this = HyLocomotionParams(fMinSpeed, fMaxSpeed, fAcceleration, fDeceleration);
}

void HyLocomotionParams::SetupPhys(float fMinSpeed, float fMaxSpeed, float fAcceleration, float fDeceleration, float fJumpSpeed, float fMoverGravity, float fFriction, float fAirSteer)
{
	*this = HyLocomotionParams(fMinSpeed, fMaxSpeed, fAcceleration, fDeceleration, fJumpSpeed, fMoverGravity, fFriction, fAirSteer);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

HyLocomotion2d::HyLocomotion2d() :
	HyLocomotion2d(HyLocomotionParams())
{
}

HyLocomotion2d::HyLocomotion2d(const HyLocomotionParams &initRef) :
	m_vThrottle(0.0f),
	m_vVelocity(0.0f)
{
	Setup(initRef);
}

HyLocomotion2d::HyLocomotion2d(float fMinSpeed, float fMaxSpeed, float fAcceleration, float fDeceleration) :
	HyLocomotion2d(HyLocomotionParams(fMinSpeed, fMaxSpeed, fAcceleration, fDeceleration))
{
}

HyLocomotion2d::~HyLocomotion2d()
{
}

bool HyLocomotion2d::IsMoving() const
{
	return m_vThrottle.x != 0.0f || m_vThrottle.y != 0.0f || m_vVelocity.x != 0.0f || m_vVelocity.y != 0.0f;
}

glm::vec2 HyLocomotion2d::GetVelocity() const
{
	return m_vVelocity;
}

void HyLocomotion2d::SetVelocity(glm::vec2 vVelocity)
{
	m_vVelocity = vVelocity;
}

void HyLocomotion2d::SetVelocityX(float fVelocityX)
{
	m_vVelocity.x = fVelocityX;
}

void HyLocomotion2d::SetVelocityY(float fVelocityY)
{
	m_vVelocity.y = fVelocityY;
}

/*virtual*/ void HyLocomotion2d::Setup(const HyLocomotionParams &initRef)
{
	m_fMaxSpeed = initRef.m_fMaxSpeed;
	m_fMinSpeed = initRef.m_fMinSpeed;
	m_fAccel = initRef.m_fAccel;
	m_fDecel = initRef.m_fDecel;
}

void HyLocomotion2d::GoUp()
{
	m_vThrottle.y = 1.0f;
}

void HyLocomotion2d::GoDown()
{
	m_vThrottle.y = -1.0f;
}

void HyLocomotion2d::GoLeft()
{
	m_vThrottle.x = -1.0f;
}

void HyLocomotion2d::GoRight()
{
	m_vThrottle.x = 1.0f;
}

void HyLocomotion2d::SetThrottle(glm::vec2 vThrottle)
{
	m_vThrottle.x = HyMath::Clamp(vThrottle.x, -1.0f, 1.0f);
	m_vThrottle.y = HyMath::Clamp(vThrottle.y, -1.0f, 1.0f);
}

void HyLocomotion2d::StopX()
{
	m_vVelocity.x = 0.0f;
}

void HyLocomotion2d::StopY()
{
	m_vVelocity.y = 0.0f;
}

/*virtual*/ void HyLocomotion2d::Update()
{
	const float fSpeedLimitX = m_fMaxSpeed * m_vThrottle.x;
	const float fSpeedLimitY = m_fMaxSpeed * m_vThrottle.y;

	// LEFT/RIGHT
	if(m_vThrottle.x < 0.0f) // LEFT
	{
		if(fSpeedLimitX <= m_vVelocity.x)
			m_vVelocity.x = HyMath::Max(fSpeedLimitX, m_vVelocity.x - (m_fAccel * HyEngine::DeltaTime()));
		else
			m_vVelocity.x = HyMath::Min(0.0f, m_vVelocity.x + (m_fDecel * HyEngine::DeltaTime()));
	}
	else if(m_vThrottle.x > 0.0f) // RIGHT
	{
		if(fSpeedLimitX >= m_vVelocity.x)
			m_vVelocity.x = HyMath::Min(fSpeedLimitX, m_vVelocity.x + (m_fAccel * HyEngine::DeltaTime()));
		else
			m_vVelocity.x = HyMath::Max(0.0f, m_vVelocity.x - (m_fDecel * HyEngine::DeltaTime()));
	}
	else // m_vThrottle.x == 0.0f
	{
		if(m_vVelocity.x > 0.0f)
			m_vVelocity.x = HyMath::Max(0.0f, m_vVelocity.x - (m_fDecel * HyEngine::DeltaTime()));
		else if(m_vVelocity.x < 0.0f)
			m_vVelocity.x = HyMath::Min(0.0f, m_vVelocity.x + (m_fDecel * HyEngine::DeltaTime()));
	}

	// UP/DOWN
	if(m_vThrottle.y > 0.0f) // UP
	{
		if(fSpeedLimitY >= m_vVelocity.y)
			m_vVelocity.y = HyMath::Min(fSpeedLimitY, m_vVelocity.y + (m_fAccel * HyEngine::DeltaTime()));
		else
			m_vVelocity.y = HyMath::Max(0.0f, m_vVelocity.y - (m_fDecel * HyEngine::DeltaTime()));
	}
	else if(m_vThrottle.y < 0.0f) // DOWN
	{
		if(fSpeedLimitY <= m_vVelocity.y)
			m_vVelocity.y = HyMath::Max(fSpeedLimitY, m_vVelocity.y - (m_fAccel * HyEngine::DeltaTime()));
		else
			m_vVelocity.y = HyMath::Min(0.0f, m_vVelocity.y + (m_fDecel * HyEngine::DeltaTime()));
	}
	else // m_vThrottle.y == 0.0f
	{
		if(m_vVelocity.y > 0.0f)
			m_vVelocity.y = HyMath::Max(0.0f, m_vVelocity.y - (m_fDecel * HyEngine::DeltaTime()));
		else if(m_vVelocity.y < 0.0f)
			m_vVelocity.y = HyMath::Min(0.0f, m_vVelocity.y + (m_fDecel * HyEngine::DeltaTime()));
	}

	if(fabs(m_vVelocity.x) < m_fMinSpeed)
		m_vVelocity.x = 0.0f;
	if(fabs(m_vVelocity.y) < m_fMinSpeed)
		m_vVelocity.y = 0.0f;

	m_vThrottle.x = m_vThrottle.y = 0.0f;
}
