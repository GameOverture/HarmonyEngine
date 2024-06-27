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

HyLocomotion2d::HyLocomotion2d() :
	m_vVelocity(0.0f),
	m_fMaxSpeed(250.0f),
	m_fAccel(100.0f),
	m_fDecel(300.0f),
	m_uiDirectionFlags(0)
{
}

HyLocomotion2d::HyLocomotion2d(float fMaxSpeed, float fAcceleration, float fDeceleration) :
	m_vVelocity(0.0f),
	m_fMaxSpeed(fMaxSpeed),
	m_fAccel(fAcceleration),
	m_fDecel(fDeceleration),
	m_uiDirectionFlags(0)
{
}

HyLocomotion2d::~HyLocomotion2d()
{
}

bool HyLocomotion2d::IsMoving() const
{
	return m_uiDirectionFlags || m_vVelocity.x != 0.0f || m_vVelocity.y != 0.0f;
}

glm::vec2 HyLocomotion2d::GetVelocity() const
{
	return m_vVelocity;
}

void HyLocomotion2d::Setup(float fMaxSpeed, float fAcceleration, float fDeceleration)
{
	m_fMaxSpeed = fMaxSpeed;
	m_fAccel = fAcceleration;
	m_fDecel = fDeceleration;
}

void HyLocomotion2d::GoUp()
{
	m_uiDirectionFlags |= DIRECTION_UP;
}

void HyLocomotion2d::GoDown()
{
	m_uiDirectionFlags |= DIRECTION_DOWN;
}

void HyLocomotion2d::GoLeft()
{
	m_uiDirectionFlags |= DIRECTION_LEFT;
}

void HyLocomotion2d::GoRight()
{
	m_uiDirectionFlags |= DIRECTION_RIGHT;
}

void HyLocomotion2d::StopX()
{
	m_vVelocity.x = 0.0f;
}

void HyLocomotion2d::StopY()
{
	m_vVelocity.y = 0.0f;
}

void HyLocomotion2d::Update()
{
	if(m_uiDirectionFlags & DIRECTION_LEFT)
		m_vVelocity.x -= m_fAccel * HyEngine::DeltaTime();
	else if(m_vVelocity.x < 0.0f)
		m_vVelocity.x = HyMath::Min(0.0f, m_vVelocity.x + (m_fDecel * HyEngine::DeltaTime()));
	if(m_uiDirectionFlags & DIRECTION_RIGHT)
		m_vVelocity.x += m_fAccel * HyEngine::DeltaTime();
	else if(m_vVelocity.x > 0.0f)
		m_vVelocity.x = HyMath::Max(0.0f, m_vVelocity.x - (m_fDecel * HyEngine::DeltaTime()));
	if(m_uiDirectionFlags & DIRECTION_UP)
		m_vVelocity.y += m_fAccel * HyEngine::DeltaTime();
	else if(m_vVelocity.y > 0.0f)
		m_vVelocity.y = HyMath::Max(0.0f, m_vVelocity.y - (m_fDecel * HyEngine::DeltaTime()));
	if(m_uiDirectionFlags & DIRECTION_DOWN)
		m_vVelocity.y -= m_fAccel * HyEngine::DeltaTime();
	else if(m_vVelocity.y < 0.0f)
		m_vVelocity.y = HyMath::Min(0.0f, m_vVelocity.y + (m_fDecel * HyEngine::DeltaTime()));

	m_vVelocity.x = HyMath::Clamp(m_vVelocity.x, -m_fMaxSpeed, m_fMaxSpeed);
	m_vVelocity.y = HyMath::Clamp(m_vVelocity.y, -m_fMaxSpeed, m_fMaxSpeed);

	m_uiDirectionFlags = 0;
}
