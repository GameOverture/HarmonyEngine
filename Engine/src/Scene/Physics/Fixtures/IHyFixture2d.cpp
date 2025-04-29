/**************************************************************************
*	IHyFixture2d.cpp
*
*	Harmony Engine
*	Copyright (c) 2025 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Scene/Physics/Fixtures/IHyFixture2d.h"
#include "Scene/Nodes/Loadables/Bodies/Objects/HyEntity2d.h"

IHyFixture2d::IHyFixture2d(HyEntity2d *pParent /*= nullptr*/) :
	m_pParent(nullptr),
	m_eType(HYFIXTURE_Nothing),
	m_bPhysicsAllowed(true),
	m_bPhysicsDirty(false),
	m_fMaxPush(FLT_MAX),
	m_bClipVelocity(true)
{
	if(pParent)
		pParent->FixtureAppend(*this);
}

IHyFixture2d::IHyFixture2d(const IHyFixture2d &copyRef) :
	m_pParent(nullptr),
	m_eType(HYFIXTURE_Nothing),
	m_bPhysicsAllowed(copyRef.m_bPhysicsAllowed),
	m_bPhysicsDirty(false),
	m_fMaxPush(copyRef.m_fMaxPush),
	m_bClipVelocity(copyRef.m_bClipVelocity)
{
	*this = copyRef;
}

/*virtual*/ IHyFixture2d::~IHyFixture2d()
{
	HyAssert(m_pParent == nullptr, "IHyFixture2d::~IHyFixture2d() - Fixture still has a parent. Parent class should take care of this.");
}

const IHyFixture2d &IHyFixture2d::operator=(const IHyFixture2d &rhs)
{
	m_eType = rhs.m_eType;
	m_bPhysicsAllowed = rhs.m_bPhysicsAllowed;
	
	if(rhs.m_pParent)
		rhs.m_pParent->FixtureAppend(*this);

	m_fMaxPush = rhs.m_fMaxPush;
	m_bClipVelocity = rhs.m_bClipVelocity;

	m_bPhysicsDirty = true;
	return *this;
}

HyFixtureType IHyFixture2d::GetType() const
{
	return m_eType;
}

bool IHyFixture2d::IsValid() const
{
	return m_eType != HYFIXTURE_Nothing;
}

glm::vec2 IHyFixture2d::ComputeSize() const
{
	b2AABB aabb;
	ComputeAABB(aabb, glm::mat4(1.0f));

	b2Vec2 vExtents = b2AABB_Extents(aabb);
	return glm::vec2(vExtents.x, vExtents.y) * 2.0f;
}

void IHyFixture2d::ParentDetach()
{
	HyEntity2d *pParent = ParentGet();
	if(pParent == nullptr)
		return;

	pParent->FixtureRemove(*this);
}

HyEntity2d *IHyFixture2d::ParentGet() const
{
	return m_pParent;
}

bool IHyFixture2d::IsPhysicsAllowed() const
{
	return m_bPhysicsAllowed;
}

void IHyFixture2d::SetPhysicsAllowed(bool bIsPhysicsAllowed)
{
	m_bPhysicsAllowed = bIsPhysicsAllowed;
	m_bPhysicsDirty = true;
}

void IHyFixture2d::SetSoftCollision(float fMaxPush)
{
	m_fMaxPush = fMaxPush;
	m_bClipVelocity = false;
}

void IHyFixture2d::SetHardCollision()
{
	m_fMaxPush = FLT_MAX;
	m_bClipVelocity = true;
}

void IHyFixture2d::GetCollisionInfo(float &fPushLimitOut, bool &bClipVelocityOut) const
{
	fPushLimitOut = m_fMaxPush;
	bClipVelocityOut = m_bClipVelocity;
}

bool IHyFixture2d::IsPhysicsDirty() const
{
	return m_bPhysicsDirty;
}
