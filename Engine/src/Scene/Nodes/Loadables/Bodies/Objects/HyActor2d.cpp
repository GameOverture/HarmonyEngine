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
	HyActor2d(0.0f, 0.0f, HyLocomotionParams(), pParent)
{ }

HyActor2d::HyActor2d(float fWidth, float fHeight, const HyLocomotionParams &locomotionInit, HyEntity2d *pParent /*= nullptr*/) :
	HyEntity2d(pParent),
	m_ActorMover(glm::ivec2(fWidth, fHeight), locomotionInit)
{
}

HyActor2d::HyActor2d(HyActor2d &&donor) noexcept :
	HyEntity2d(std::move(donor)),
	m_ActorMover(donor.m_ActorMover)
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

bool HyActor2d::IsSimulated() const
{
	return (m_uiEntityAttribs & ACTORATTRIB_HaltSimulatation) == 0;
}

void HyActor2d::EnableSimulation()
{
	m_uiEntityAttribs &= ~ACTORATTRIB_HaltSimulatation;
}

void HyActor2d::DisableSimulation()
{
	m_uiEntityAttribs |= ACTORATTRIB_HaltSimulatation;
}

void HyActor2d::SetSize(float fWidth, float fHeight)
{
	m_ActorMover.SetSize(glm::ivec2(fWidth, fHeight));
}

const b2Capsule &HyActor2d::GetMover() const
{
	return m_ActorMover.GetMover();
}

bool HyActor2d::IsOnGround() const
{
	return (m_uiEntityAttribs & ACTORATTRIB_IsAirborne) == 0;
}

void HyActor2d::SetThrottle(glm::vec2 vThrottle)
{
	m_ActorMover.SetThrottle(vThrottle);
}

void HyActor2d::Jump()
{
	m_ActorMover.Jump();
	m_uiEntityAttribs |= ACTORATTRIB_IsAirborne;
}

/*virtual*/ void HyActor2d::Update() /*override*/
{
	if((m_uiEntityAttribs & ACTORATTRIB_HaltSimulatation) == 0)
	{
		glm::vec2 ptPos = pos.Get();
		ptPos *= sm_pScene->GetPpmInverse();

		bool bOnGround = IsOnGround();
		b2QueryFilter pogoFilter = { HYCOLLISION_Actor, HYCOLLISION_Default | HYCOLLISION_Dynamic };
		b2QueryFilter collideFilter = { HYCOLLISION_Actor, HYCOLLISION_Default | HYCOLLISION_Dynamic | HYCOLLISION_Actor }; // Mover overlap filter
		b2QueryFilter castFilter = { HYCOLLISION_Actor, HYCOLLISION_Default | HYCOLLISION_Dynamic }; // Movers don't sweep against other movers, allows for soft collision
		m_ActorMover.Update(IHyNode::sm_pScene->GetPhysicsWorld(), ptPos, bOnGround, pogoFilter, collideFilter, castFilter);

		ptPos *= sm_pScene->GetPixelsPerMeter();
		pos.Set(ptPos);

		if(bOnGround)
			m_uiEntityAttribs &= ~ACTORATTRIB_IsAirborne;
		else
			m_uiEntityAttribs |= ACTORATTRIB_IsAirborne;
	}

	HyEntity2d::Update();
}
