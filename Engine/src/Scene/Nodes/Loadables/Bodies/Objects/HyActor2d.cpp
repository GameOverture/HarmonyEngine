/**************************************************************************
 *	HyActor2d.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2025 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Loadables/Bodies/Objects/HyActor2d.h"
#include "Scene/HyScene.h"
#include "HyEngine.h"

HyActor2d::HyActor2d(HyEntity2d *pParent /*= nullptr*/) :
	HyActor2d(HyLocomotionParams(), 0.0f, 0.0f, pParent)
{ }

HyActor2d::HyActor2d(const HyLocomotionParams &locomotionInit, float fWidth, float fHeight, HyEntity2d *pParent /*= nullptr*/) :
	HyEntity2d(pParent),
	m_Locomotion(locomotionInit),
	m_ActorFixture(nullptr) // This is simulated externally to the world via 'm_Locomotion'::UpdatePhysical
{
	SetSize(fWidth, fHeight);
}

HyActor2d::HyActor2d(HyActor2d &&donor) noexcept :
	HyEntity2d(std::move(donor)),
	m_Locomotion(donor.m_Locomotion),
	m_ActorFixture(nullptr) // This is simulated externally to the world via 'm_Locomotion'::UpdatePhysical
{
	m_ActorFixture = donor.m_ActorFixture;
	m_ActorFixture.SetPhysicsAllowed(false);
}

/*virtual*/ HyActor2d::~HyActor2d(void)
{
}

HyActor2d &HyActor2d::operator=(HyActor2d &&donor) noexcept
{
	HyEntity2d::operator=(std::move(donor));

	return *this;
}

void HyActor2d::SetSize(float fWidth, float fHeight)
{
	if(fWidth <= 0.0f)
		fWidth = HyEngine::InitValues().fPixelsPerMeter * 0.3f;
	if(fHeight <= 0.0f)
		fHeight = HyEngine::InitValues().fPixelsPerMeter * 2.0f;

	// NOTE: explicitly set 'm_ActorFixture' in meters instead of pixels
	fWidth *= sm_pScene->GetPpmInverse();
	fHeight *= sm_pScene->GetPpmInverse();

	float fRadius = (fWidth * 0.5f);
	m_ActorFixture.SetAsCapsule(glm::vec2(0.0f, fRadius), glm::vec2(0.0f, fHeight - (fRadius * 2.0f)), fWidth * 0.5f);
	m_ActorFixture.SetPhysicsAllowed(false); // Simulated externally to the world via 'm_Locomotion'::UpdatePhysical
}

bool HyActor2d::IsOnGround() const
{
	return (m_uiEntityAttribs & ACTORATTRIB_IsAirborne) == 0;
}

void HyActor2d::SetThrottle(glm::vec2 vThrottle)
{
	m_Locomotion.SetThrottle(vThrottle);
}

void HyActor2d::Jump()
{
	m_Locomotion.Jump();
	m_uiEntityAttribs |= ACTORATTRIB_IsAirborne;
}

/*virtual*/ void HyActor2d::Update() /*override*/
{
	glm::vec2 ptPos = pos.Get();
	ptPos *= sm_pScene->GetPpmInverse();

	bool bOnGround = IsOnGround();
	b2QueryFilter pogoFilter = { HYCOLLISION_Actor, HYCOLLISION_Default | HYCOLLISION_Dynamic };
	b2QueryFilter collideFilter = { HYCOLLISION_Actor, HYCOLLISION_Default | HYCOLLISION_Dynamic | HYCOLLISION_Actor }; // Mover overlap filter
	b2QueryFilter castFilter = { HYCOLLISION_Actor, HYCOLLISION_Default | HYCOLLISION_Dynamic }; // Movers don't sweep against other movers, allows for soft collision
	m_Locomotion.UpdatePhysical(IHyNode::sm_pScene->GetPhysicsWorld(), ptPos, bOnGround, m_ActorFixture.GetAsCapsule(), pogoFilter, collideFilter, castFilter);

	ptPos *= sm_pScene->GetPixelsPerMeter();
	pos.Set(ptPos);

	if(bOnGround)
		m_uiEntityAttribs &= ~ACTORATTRIB_IsAirborne;
	else
		m_uiEntityAttribs |= ACTORATTRIB_IsAirborne;

	HyEntity2d::Update();
}
