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

HyActor2d::HyActor2d(HyEntity2d *pParent /*= nullptr*/) :
	HyEntity2d(pParent),
	m_Locomotion(0.1f, 6.0f, 20.0f, 3.0f, 10.0f, 30.0f, 8.0f, 0.2f),
	m_ActorFixture(this)
{
	m_ActorFixture.SetPhysicsAllowed(false);
	m_ActorFixture.SetAsCapsule(glm::vec2(0.0f, -0.5f), glm::vec2(0.0f, 0.5f), 0.3f);
}

HyActor2d::HyActor2d(HyActor2d &&donor) noexcept :
	HyEntity2d(std::move(donor))
{
}

/*virtual*/ HyActor2d::~HyActor2d(void)
{
}

HyActor2d &HyActor2d::operator=(HyActor2d &&donor) noexcept
{
	HyEntity2d::operator=(std::move(donor));

	return *this;
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
	bool bOnGround = IsOnGround();
	b2QueryFilter pogoFilter = { HYCOLLISION_Actor, HYCOLLISION_Default | HYCOLLISION_Dynamic };
	b2QueryFilter collideFilter = { HYCOLLISION_Actor, HYCOLLISION_Default | HYCOLLISION_Dynamic | HYCOLLISION_Actor }; // Mover overlap filter
	b2QueryFilter castFilter = { HYCOLLISION_Actor, HYCOLLISION_Default | HYCOLLISION_Dynamic }; // Movers don't sweep against other movers, allows for soft collision
	m_Locomotion.UpdatePhysical(IHyNode::sm_pScene->GetPhysicsWorld(), ptPos, bOnGround, m_ActorFixture.GetAsCapsule(), pogoFilter, collideFilter, castFilter);

	pos.Set(ptPos);
	if(bOnGround)
		m_uiEntityAttribs &= ~ACTORATTRIB_IsAirborne;
	else
		m_uiEntityAttribs |= ACTORATTRIB_IsAirborne;

	HyEntity2d::Update();
}
