/**************************************************************************
*	HyPhysics2d.cpp
*	
*	Harmony Engine
*	Copyright (c) 2022 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Scene/Physics/HyPhysicsGrid2d.h"
#include "Scene/Physics/HyPhysics2d.h"

HyPhysics2d::HyPhysics2d(HyEntity2d &ownerRef) :
	m_OwnerRef(ownerRef),
	m_pPhysicsBody(nullptr),
	m_pFixture(nullptr)
{
	//b2Body *pBody, const b2Shape *pb2PpmShape, float fDensity, float fFriction, float fRestitution, bool bIsSensor, b2Filter collideFilter
	//b2FixtureDef def;
	//def.shape = pb2PpmShape;
	//def.userData = this;
	//def.friction = 0.2f;		// The friction coefficient, usually in the range [0,1].
	//def.restitution = 0.0f;		// The restitution (elasticity) usually in the range [0,1].
	//def.density = fDensity;		// The density, usually in kg/m^2.
	//def.isSensor = bIsSensor;
	//def.filter = collideFilter;

	//m_pFixture = pBody->CreateFixture(&def);
}

HyPhysics2d::~HyPhysics2d()
{
}

void HyPhysics2d::PhysInit(HyPhysicsGrid2d &physGridRef,
	HyPhysicsType eType,
	bool bIsEnabled /*= true*/,
	bool bIsFixedRotation /*= false*/,
	bool bIsCcd /*= false*/,
	bool bIsAwake /*= true*/,
	bool bAllowSleep /*= true*/,
	float fGravityScale /*= 1.0f*/)
{
	PhysRelease();

	b2BodyDef bodyDef;
	bodyDef.userData.pointer = reinterpret_cast<uintptr_t>(this);
	bodyDef.position.Set(pos.X() * physGridRef.GetPpmInverse(), pos.Y() * physGridRef.GetPpmInverse());
	bodyDef.angle = rot.Get();

	bodyDef.type = static_cast<b2BodyType>(eType); // static_assert guarantees this to match
	bodyDef.enabled = bIsEnabled;
	bodyDef.fixedRotation = bIsFixedRotation;
	bodyDef.bullet = bIsCcd;
	bodyDef.awake = bIsAwake;
	bodyDef.allowSleep = bAllowSleep;
	bodyDef.gravityScale = fGravityScale;

	m_pPhysicsBody = physGridRef.CreateBody(&bodyDef);
}

void HyPhysics2d::PhysInit(HyPhysicsGrid2d &physGridRef,
	HyPhysicsType eType,
	b2Vec2 vPos,
	bool bIsEnabled /*= true*/,
	bool bIsFixedRotation /*= false*/,
	bool bIsCcd /*= false*/,
	bool bIsAwake /*= true*/,
	bool bAllowSleep /*= true*/,
	float fGravityScale /*= 1.0f*/)
{
	PhysRelease();

	b2BodyDef bodyDef;
	bodyDef.userData.pointer = reinterpret_cast<uintptr_t>(this);
	bodyDef.position.Set(vPos.x * physGridRef.GetPpmInverse(), vPos.y * physGridRef.GetPpmInverse());
	bodyDef.angle = rot.Get();

	bodyDef.type = static_cast<b2BodyType>(eType); // static_assert guarantees this to match
	bodyDef.enabled = bIsEnabled;
	bodyDef.fixedRotation = bIsFixedRotation;
	bodyDef.bullet = bIsCcd;
	bodyDef.awake = bIsAwake;
	bodyDef.allowSleep = bAllowSleep;
	bodyDef.gravityScale = fGravityScale;

	m_pPhysicsBody = physGridRef.CreateBody(&bodyDef);
}

HyPhysicsType HyPhysics2d::PhysGetType() const
{
	return m_pPhysicsBody ? static_cast<HyPhysicsType>(m_pPhysicsBody->GetType()) : HYPHYS_Unknown;
}

void HyPhysics2d::PhysSetType(HyPhysicsType eType)
{
	if(m_pPhysicsBody)
		m_pPhysicsBody->SetType(static_cast<b2BodyType>(eType)); // static_assert guarantees this to match
}

bool HyPhysics2d::PhysIsEnabled() const
{
	return m_pPhysicsBody && m_pPhysicsBody->IsEnabled();
}

void HyPhysics2d::PhysSetEnabled(bool bEnable)
{
	if(m_pPhysicsBody)
	{
		if(bEnable)
		{
			float fPpmInverse = static_cast<HyPhysicsGrid2d *>(m_pPhysicsBody->GetWorld())->GetPpmInverse();
			m_pPhysicsBody->SetLinearVelocity(b2Vec2(0.0f, 0.0f));
			m_pPhysicsBody->SetAngularVelocity(0.0f);
			m_pPhysicsBody->SetTransform(b2Vec2(pos.X() * fPpmInverse, pos.Y() * fPpmInverse), glm::radians(rot.Get()));
		}

		m_pPhysicsBody->SetEnabled(bEnable);
	}
}

bool HyPhysics2d::PhysIsFixedRotation() const
{
	return m_pPhysicsBody && m_pPhysicsBody->IsFixedRotation();
}

void HyPhysics2d::PhysSetFixedRotation(bool bFixedRot)
{
	if(m_pPhysicsBody)
		m_pPhysicsBody->SetFixedRotation(bFixedRot);
}

bool HyPhysics2d::PhysIsCcd() const
{
	return m_pPhysicsBody && m_pPhysicsBody->IsBullet();
}

void HyPhysics2d::PhysSetCcd(bool bContinuousCollisionDetection)
{
	if(m_pPhysicsBody)
		m_pPhysicsBody->SetBullet(bContinuousCollisionDetection);
}

bool HyPhysics2d::PhysIsAwake() const
{
	return m_pPhysicsBody && m_pPhysicsBody->IsAwake();
}

void HyPhysics2d::PhysSetAwake(bool bAwake)
{
	if(m_pPhysicsBody)
		m_pPhysicsBody->SetAwake(bAwake);
}

bool HyPhysics2d::PhysIsSleepingAllowed() const
{
	return m_pPhysicsBody && m_pPhysicsBody->IsSleepingAllowed();
}

void HyPhysics2d::PhysSetSleepingAllowed(bool bAllowSleep)
{
	if(m_pPhysicsBody)
		m_pPhysicsBody->SetSleepingAllowed(bAllowSleep);
}

float HyPhysics2d::PhysGetGravityScale() const
{
	return m_pPhysicsBody ? m_pPhysicsBody->GetGravityScale() : 0.0f;
}

void HyPhysics2d::PhysSetGravityScale(float fGravityScale)
{
	if(m_pPhysicsBody)
		m_pPhysicsBody->SetGravityScale(fGravityScale);
}

glm::vec2 HyPhysics2d::PhysWorldCenterMass() const
{
	if(m_pPhysicsBody)
		return glm::vec2(m_pPhysicsBody->GetWorldCenter().x, m_pPhysicsBody->GetWorldCenter().y);

	return glm::vec2();
}

glm::vec2 HyPhysics2d::PhysLocalCenterMass() const
{
	if(m_pPhysicsBody)
		return glm::vec2(m_pPhysicsBody->GetLocalCenter().x, m_pPhysicsBody->GetLocalCenter().y);

	return glm::vec2();
}

glm::vec2 HyPhysics2d::PhysGetLinearVelocity() const
{
	if(m_pPhysicsBody && m_pPhysicsBody->IsEnabled())
		return glm::vec2(m_pPhysicsBody->GetLinearVelocity().x, m_pPhysicsBody->GetLinearVelocity().y);

	return glm::vec2();
}

void HyPhysics2d::PhysSetLinearVelocity(glm::vec2 vVelocity)
{
	if(m_pPhysicsBody)
		m_pPhysicsBody->SetLinearVelocity(b2Vec2(vVelocity.x, vVelocity.y));
}

float HyPhysics2d::PhysGetAngularVelocity() const
{
	if(m_pPhysicsBody)
		return m_pPhysicsBody->GetAngularVelocity();

	return 0.0f;
}

void HyPhysics2d::PhysSetAngularVelocity(float fOmega)
{
	if(m_pPhysicsBody)
		m_pPhysicsBody->SetAngularVelocity(fOmega);
}

void HyPhysics2d::PhysApplyForce(const glm::vec2 &vForce, const glm::vec2 &ptPoint, bool bWake)
{
	if(m_pPhysicsBody)
		m_pPhysicsBody->ApplyForce(b2Vec2(vForce.x, vForce.y), b2Vec2(ptPoint.x, ptPoint.y), bWake);
}

void HyPhysics2d::PhysApplyForceToCenter(const glm::vec2 &vForce, bool bWake)
{
	if(m_pPhysicsBody)
		m_pPhysicsBody->ApplyForceToCenter(b2Vec2(vForce.x, vForce.y), bWake);
}

void HyPhysics2d::PhysApplyTorque(float fTorque, bool bWake)
{
	if(m_pPhysicsBody)
		m_pPhysicsBody->ApplyTorque(fTorque, bWake);
}

void HyPhysics2d::PhysApplyLinearImpulse(const glm::vec2 &vImpulse, const glm::vec2 &ptPoint, bool bWake)
{
	if(m_pPhysicsBody)
		m_pPhysicsBody->ApplyLinearImpulse(b2Vec2(vImpulse.x, vImpulse.y), b2Vec2(ptPoint.x, ptPoint.y), bWake);
}

void HyPhysics2d::PhysApplyLinearImpulseToCenter(const glm::vec2 &vImpulse, bool bWake)
{
	if(m_pPhysicsBody)
		m_pPhysicsBody->ApplyLinearImpulseToCenter(b2Vec2(vImpulse.x, vImpulse.y), bWake);
}

void HyPhysics2d::PhysApplyAngularImpulse(float fImpulse, bool bWake)
{
	if(m_pPhysicsBody)
		m_pPhysicsBody->ApplyAngularImpulse(fImpulse, bWake);
}

void HyPhysics2d::PhysSetFilterData(b2Filter &Filter)
{
	for(b2Fixture *f = m_pPhysicsBody->GetFixtureList(); f; f = f->GetNext())
	{
		f->SetFilterData(Filter);
	}
}

const b2Filter &HyPhysics2d::PhysGetFilterData(int iIndex)
{
	return m_pPhysicsBody->GetFixtureList()[iIndex].GetFilterData();
}



float HyPhysics2d::PhysGetMass() const
{
	if(m_pPhysicsBody)
		return m_pPhysicsBody->GetMass();

	return 0.0f;
}

float HyPhysics2d::PhysGetInertia() const
{
	if(m_pPhysicsBody)
		return m_pPhysicsBody->GetInertia();

	return 0.0f;
}

void HyPhysics2d::Update()
{
	if(m_pPhysicsBody && m_pPhysicsBody->IsEnabled())
	{
		if(pos.IsAnimating() == false)
		{
			pos.GetAnimFloat(0).Updater([&](float fElapsedTime) { return (m_pPhysicsBody->GetPosition().x * static_cast<HyPhysicsGrid2d *>(m_pPhysicsBody->GetWorld())->GetPixelsPerMeter()); });
			pos.GetAnimFloat(1).Updater([&](float fElapsedTime) { return (m_pPhysicsBody->GetPosition().y * static_cast<HyPhysicsGrid2d *>(m_pPhysicsBody->GetWorld())->GetPixelsPerMeter()); });
		}

		if(rot.IsAnimating() == false)
			rot.Updater([&](float fElapsedTime) { return glm::degrees(m_pPhysicsBody->GetAngle()); });
	}
}

b2Fixture *HyPhysics2d::GetFixture()
{
	return m_pFixture;
}














std::unique_ptr<HyPhysics2d> HyPhysics2d::PhysAddCollider(const HyShape2d &shapeRef, float fDensity, float fFriction, float fRestitution, bool bIsSensor, b2Filter collideFilter)
{
	if(m_pPhysicsBody == nullptr || shapeRef.IsValidShape() == false)
		return nullptr;

	b2Shape *pPpmShape = shapeRef.ClonePpmShape(static_cast<HyPhysicsGrid2d *>(m_pPhysicsBody->GetWorld())->GetPpmInverse());
	std::unique_ptr<HyPhysics2d> pCollider = std::make_unique<HyPhysics2d>(m_pPhysicsBody, pPpmShape, fDensity, fFriction, fRestitution, bIsSensor, collideFilter);
	delete pPpmShape;

	return pCollider;
}

std::unique_ptr<HyPhysics2d> HyPhysics2d::PhysAddCircleCollider(float fRadius, float fDensity, float fFriction, float fRestitution, bool bIsSensor, b2Filter collideFilter)
{
	return PhysAddCircleCollider(glm::vec2(0.0f, 0.0), fRadius, fDensity, fFriction, fRestitution, bIsSensor, collideFilter);
}

std::unique_ptr<HyPhysics2d> HyPhysics2d::PhysAddCircleCollider(const glm::vec2 &ptCenter, float fRadius, float fDensity, float fFriction, float fRestitution, bool bIsSensor, b2Filter collideFilter)
{
	if(m_pPhysicsBody == nullptr || fRadius <= 0.0f)
		return nullptr;

	float fPpmInverse = static_cast<HyPhysicsGrid2d *>(m_pPhysicsBody->GetWorld())->GetPpmInverse();
	b2CircleShape circleShape;
	circleShape.m_p.x = ptCenter.x * fPpmInverse;
	circleShape.m_p.y = ptCenter.y * fPpmInverse;
	circleShape.m_radius = fRadius * fPpmInverse;
	return std::make_unique<HyPhysics2d>(m_pPhysicsBody, &circleShape, fDensity, fFriction, fRestitution, bIsSensor, collideFilter);
}

std::unique_ptr<HyPhysics2d> HyPhysics2d::PhysAddLineChainCollider(const glm::vec2 *pVerts, uint32 uiNumVerts, float fDensity, float fFriction, float fRestitution, bool bIsSensor, b2Filter collideFilter)
{
	if(m_pPhysicsBody == nullptr || pVerts == nullptr || uiNumVerts == 0)
		return nullptr;

	float fPpmInverse = static_cast<HyPhysicsGrid2d *>(m_pPhysicsBody->GetWorld())->GetPpmInverse();
	std::vector<b2Vec2> vertList;
	for(uint32 i = 0; i < uiNumVerts; ++i)
		vertList.emplace_back(pVerts[i].x * fPpmInverse, pVerts[i].y * fPpmInverse);

	b2ChainShape chainShape;
	chainShape.CreateChain(vertList.data(), uiNumVerts);
	return std::make_unique<HyPhysics2d>(m_pPhysicsBody, &chainShape, fDensity, fFriction, fRestitution, bIsSensor, collideFilter);
}

void HyPhysics2d::PhysDestroyCollider(std::unique_ptr<HyPhysics2d> pCollider)
{
	if(m_pPhysicsBody)
		m_pPhysicsBody->DestroyFixture(pCollider->GetFixture());

	pCollider.release();
}

void HyPhysics2d::PhysRelease()
{
	if(m_pPhysicsBody)
	{
		m_pPhysicsBody->GetWorld()->DestroyBody(m_pPhysicsBody);
		m_pPhysicsBody = nullptr;
	}
}
