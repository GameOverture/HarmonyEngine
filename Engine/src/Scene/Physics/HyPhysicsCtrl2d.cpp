/**************************************************************************
*	HyPhysicsCtrl2d.cpp
*	
*	Harmony Engine
*	Copyright (c) 2022 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Scene/HyScene.h"
#include "Scene/Physics/HyPhysicsCtrl2d.h"
#include "Scene/Nodes/Loadables/Bodies/Objects/HyPhysicsGrid2d.h"
#include "Diagnostics/Console/IHyConsole.h"

HyPhysicsCtrl2d::HyPhysicsCtrl2d(IHyBody2d &nodeRef) :
	m_NodeRef(nodeRef),
	m_pInit(nullptr),
	m_bEnabled(true)
{
}

HyPhysicsCtrl2d::~HyPhysicsCtrl2d()
{
	delete m_pInit;
}

void HyPhysicsCtrl2d::Activate()
{
	IHyNode::sm_pScene->AddNode_PhysBody(&m_NodeRef);
}

void HyPhysicsCtrl2d::Deactivate()
{
	if(m_NodeRef.IsSimulating())
	{
		HyAssert(m_NodeRef.ParentGet() && (m_NodeRef.ParentGet()->GetInternalFlags() & IHyNode::NODETYPE_IsPhysicsGrid), "HyPhysicsCtrl2d::Uninit() - node didn't have proper parent");
		if(m_NodeRef.ParentGet() && (m_NodeRef.ParentGet()->GetInternalFlags() & IHyNode::NODETYPE_IsPhysicsGrid))
			static_cast<HyPhysicsGrid2d *>(m_NodeRef.ParentGet())->UninitChildPhysics(m_NodeRef);

		m_NodeRef.pos.GetAnimFloat(0).StopAnim();
		m_NodeRef.pos.GetAnimFloat(1).StopAnim();
		m_NodeRef.rot.StopAnim();
	}
}

void HyPhysicsCtrl2d::Setup(const b2BodyDef &bodyDef)
{
	Setup(static_cast<HyBodyType>(bodyDef.type),
		bodyDef.enabled,
		bodyDef.gravityScale,
		bodyDef.fixedRotation,
		bodyDef.awake,
		glm::vec2(bodyDef.linearVelocity.x, bodyDef.linearVelocity.y),
		bodyDef.angularVelocity,
		bodyDef.linearDamping,
		bodyDef.angularDamping,
		bodyDef.allowSleep,
		bodyDef.bullet);
}

void HyPhysicsCtrl2d::Setup(HyBodyType eType,
	bool bIsEnabled /*= true*/,
	float fGravityScale /*= 1.0f*/,
	bool bIsFixedRotation /*= false*/,
	bool bIsAwake /*= true*/,
	glm::vec2 vLinearVelocity /*= glm::vec2(0.0f, 0.0f)*/,
	float fAngularVelocity /*= 0.0f*/,
	float fLinearDamping /*= 0.0f*/,
	float fAngularDamping /*= 0.0f*/,
	bool bAllowSleep /*= true*/,
	bool bIsCcd /*= false*/)
{
	HyAssert(eType != HYBODY_Unknown, "HyPhysicsCtrl2d::Init was passed 'HYBODY_Unknown'");

	m_bEnabled = bIsEnabled;

	// If 'm_pBody' exists, then this node is already apart of an active simulation. Simply update the m_pBody with all the incoming parameters
	if(m_pBody)
	{
		// Box2d already checks for a redundant values getting set, and avoids most computation
		m_pBody->SetType(static_cast<b2BodyType>(eType));
		m_pBody->SetEnabled(bIsEnabled);
		m_pBody->SetGravityScale(fGravityScale);
		m_pBody->SetFixedRotation(bIsFixedRotation);
		m_pBody->SetAwake(bIsAwake);
		m_pBody->SetLinearVelocity(b2Vec2(vLinearVelocity.x, vLinearVelocity.y));
		m_pBody->SetAngularVelocity(fAngularVelocity);
		m_pBody->SetLinearDamping(fLinearDamping);
		m_pBody->SetAngularDamping(fAngularDamping);
		m_pBody->SetSleepingAllowed(bAllowSleep);
		m_pBody->SetBullet(bIsCcd);
		m_pBody->GetUserData().pointer = reinterpret_cast<uintptr_t>(&m_NodeRef);

		return;
	}

	delete m_pInit;
	m_pInit = HY_NEW b2BodyDef();
	
	m_pInit->type = static_cast<b2BodyType>(eType);
	m_pInit->enabled = bIsEnabled;
	m_pInit->gravityScale = fGravityScale;
	m_pInit->fixedRotation = bIsFixedRotation;
	m_pInit->awake = bIsAwake;
	m_pInit->linearVelocity = b2Vec2(vLinearVelocity.x, vLinearVelocity.y);
	m_pInit->angularVelocity = fAngularVelocity;
	m_pInit->linearDamping = fLinearDamping;
	m_pInit->angularDamping = fAngularDamping;
	m_pInit->allowSleep = bAllowSleep;
	m_pInit->bullet = bIsCcd;
	m_pInit->userData.pointer = reinterpret_cast<uintptr_t>(&m_NodeRef);

	m_NodeRef.sm_pScene->AddNode_Collidable(&m_NodeRef);
}

HyBodyType HyPhysicsCtrl2d::GetType() const
{
	if(m_pBox2d)
		return static_cast<HyBodyType>(m_pBox2d->m_pBody->GetType());
	else if(m_pInit)
		return static_cast<HyBodyType>(m_pInit->m_BodyDef.type);
	else
		return HYBODY_Unknown;
}

void HyPhysicsCtrl2d::SetType(HyBodyType eType)
{
	if(m_pBox2d)
		m_pBox2d->m_pBody->SetType(static_cast<b2BodyType>(eType));
	else if(m_pInit)
		m_pInit->m_BodyDef.type = static_cast<b2BodyType>(eType);
	else
	{
		b2BodyDef def;
		def.type = static_cast<b2BodyType>(eType);
		Init(def, b2FixtureDef());
	}
}

bool HyPhysicsCtrl2d::IsEnabled() const
{
	if(m_pBox2d)
		return m_pBox2d->m_pBody->IsEnabled();
	else if(m_pInit)
		return m_pInit->m_BodyDef.enabled;
	else
		return b2BodyDef().enabled;
}

void HyPhysicsCtrl2d::SetEnabled(bool bEnable)
{
	if(m_pBox2d)
		m_pBox2d->m_pBody->SetEnabled(bEnable);
	else if(m_pInit)
		m_pInit->m_BodyDef.enabled = bEnable;
	else
	{
		b2BodyDef def;
		def.enabled = bEnable;
		Init(def, b2FixtureDef());
	}

	m_bEnabled = bEnable;

	if(m_bEnabled == false && m_NodeRef.pos.IsAnimating())
	{
		m_NodeRef.pos.GetAnimFloat(0).StopAnim();
		m_NodeRef.pos.GetAnimFloat(1).StopAnim();
		m_NodeRef.rot.StopAnim();
	}
}

bool HyPhysicsCtrl2d::IsFixedRotation() const
{
	if(m_pBox2d)
		return m_pBox2d->m_pBody->IsFixedRotation();
	else if(m_pInit)
		return m_pInit->m_BodyDef.fixedRotation;
	else
		return b2BodyDef().fixedRotation;
}

void HyPhysicsCtrl2d::SetFixedRotation(bool bFixedRot)
{
	if(m_pBox2d)
		m_pBox2d->m_pBody->SetFixedRotation(bFixedRot);
	else if(m_pInit)
		m_pInit->m_BodyDef.fixedRotation = bFixedRot;
	else
	{
		b2BodyDef def;
		def.fixedRotation = bFixedRot;
		Init(def, b2FixtureDef());
	}
}

bool HyPhysicsCtrl2d::IsAwake() const
{
	if(m_pBox2d)
		return m_pBox2d->m_pBody->IsAwake();
	else if(m_pInit)
		return m_pInit->m_BodyDef.awake;
	else
		return b2BodyDef().awake;
}

void HyPhysicsCtrl2d::SetAwake(bool bAwake)
{
	if(m_pBox2d)
		m_pBox2d->m_pBody->SetAwake(bAwake);
	else if(m_pInit)
		m_pInit->m_BodyDef.awake = bAwake;
	else
	{
		b2BodyDef def;
		def.awake = bAwake;
		Init(def, b2FixtureDef());
	}
}

glm::vec2 HyPhysicsCtrl2d::GetLinearVelocity() const
{
	if(m_pBox2d)
		return glm::vec2(m_pBox2d->m_pBody->GetLinearVelocity().x, m_pBox2d->m_pBody->GetLinearVelocity().y);
	else if(m_pInit)
		return glm::vec2(m_pInit->m_BodyDef.linearVelocity.x, m_pInit->m_BodyDef.linearVelocity.y);
	else
		return glm::vec2(b2BodyDef().linearVelocity.x, b2BodyDef().linearVelocity.y);
}

void HyPhysicsCtrl2d::SetLinearVelocity(glm::vec2 vVelocity)
{
	if(m_pBox2d)
		m_pBox2d->m_pBody->SetLinearVelocity(b2Vec2(vVelocity.x, vVelocity.y));
	else if(m_pInit)
		m_pInit->m_BodyDef.linearVelocity = b2Vec2(vVelocity.x, vVelocity.y);
	else
	{
		b2BodyDef def;
		def.linearVelocity = b2Vec2(vVelocity.x, vVelocity.y);
		Init(def, b2FixtureDef());
	}
}

float HyPhysicsCtrl2d::GetAngularVelocity() const
{
	if(m_pBox2d)
		return m_pBox2d->m_pBody->GetAngularVelocity();
	else if(m_pInit)
		return m_pInit->m_BodyDef.angularVelocity;
	else
		return b2BodyDef().angularVelocity;
}

void HyPhysicsCtrl2d::SetAngularVelocity(float fOmega)
{
	if(m_pBox2d)
		m_pBox2d->m_pBody->SetAngularVelocity(fOmega);
	else if(m_pInit)
		m_pInit->m_BodyDef.angularVelocity = fOmega;
	else
	{
		b2BodyDef def;
		def.angularVelocity = fOmega;
		Init(def, b2FixtureDef());
	}
}

float HyPhysicsCtrl2d::GetLinearDamping() const
{
	if(m_pBox2d)
		return m_pBox2d->m_pBody->GetLinearDamping();
	else if(m_pInit)
		return m_pInit->m_BodyDef.linearDamping;
	else
		return b2BodyDef().linearDamping;
}

void HyPhysicsCtrl2d::SetLinearDamping(float fLinearDamping)
{
	if(m_pBox2d)
		m_pBox2d->m_pBody->SetLinearDamping(fLinearDamping);
	else if(m_pInit)
		m_pInit->m_BodyDef.linearDamping = fLinearDamping;
	else
	{
		b2BodyDef def;
		def.linearDamping = fLinearDamping;
		Init(def, b2FixtureDef());
	}
}

float HyPhysicsCtrl2d::GetAngularDamping() const
{
	if(m_pBox2d)
		return m_pBox2d->m_pBody->GetAngularDamping();
	else if(m_pInit)
		return m_pInit->m_BodyDef.angularDamping;
	else
		return b2BodyDef().angularDamping;
}

void HyPhysicsCtrl2d::SetAngularDamping(float fAngularDamping)
{
	if(m_pBox2d)
		m_pBox2d->m_pBody->SetAngularDamping(fAngularDamping);
	else if(m_pInit)
		m_pInit->m_BodyDef.angularDamping = fAngularDamping;
	else
	{
		b2BodyDef def;
		def.angularDamping = fAngularDamping;
		Init(def, b2FixtureDef());
	}
}

bool HyPhysicsCtrl2d::IsSleepingAllowed() const
{
	if(m_pBox2d)
		return m_pBox2d->m_pBody->IsSleepingAllowed();
	else if(m_pInit)
		return m_pInit->m_BodyDef.allowSleep;
	else
		return b2BodyDef().allowSleep;
}

void HyPhysicsCtrl2d::SetSleepingAllowed(bool bAllowSleep)
{
	if(m_pBox2d)
		m_pBox2d->m_pBody->SetSleepingAllowed(bAllowSleep);
	else if(m_pInit)
		m_pInit->m_BodyDef.allowSleep = bAllowSleep;
	else
	{
		b2BodyDef def;
		def.allowSleep = bAllowSleep;
		Init(def, b2FixtureDef());
	}
}

float HyPhysicsCtrl2d::GetGravityScale() const
{
	if(m_pBox2d)
		return m_pBox2d->m_pBody->GetGravityScale();
	else if(m_pInit)
		return m_pInit->m_BodyDef.gravityScale;
	else
		return b2BodyDef().gravityScale;
}

void HyPhysicsCtrl2d::SetGravityScale(float fGravityScale)
{
	if(m_pBox2d)
		m_pBox2d->m_pBody->SetGravityScale(fGravityScale);
	else if(m_pInit)
		m_pInit->m_BodyDef.gravityScale = fGravityScale;
	else
	{
		b2BodyDef def;
		def.gravityScale = fGravityScale;
		Init(def, b2FixtureDef());
	}
}

bool HyPhysicsCtrl2d::IsCcd() const
{
	if(m_pBox2d)
		return m_pBox2d->m_pBody->IsBullet();
	else if(m_pInit)
		return m_pInit->m_BodyDef.bullet;
	else
		return b2BodyDef().bullet;
}

void HyPhysicsCtrl2d::SetCcd(bool bContinuousCollisionDetection)
{
	if(m_pBox2d)
		m_pBox2d->m_pBody->SetBullet(bContinuousCollisionDetection);
	else if(m_pInit)
		m_pInit->m_BodyDef.bullet = bContinuousCollisionDetection;
	else
	{
		b2BodyDef def;
		def.bullet = bContinuousCollisionDetection;
		Init(def, b2FixtureDef());
	}
}

void HyPhysicsCtrl2d::AddShape();

void HyPhysicsCtrl2d::AddShape(HyShape2d &shapeRef)
{
}

glm::vec2 HyPhysicsCtrl2d::GridCenterMass() const
{
	if(m_pBox2d)
		return glm::vec2(m_pBox2d->m_pBody->GetWorldCenter().x, m_pBox2d->m_pBody->GetWorldCenter().y);

	//HyLogWarning("HyPhysicsCtrl2d::GridCenterMass invoked before physics component is set");
	return glm::vec2(0.0f, 0.0f);
}

glm::vec2 HyPhysicsCtrl2d::LocalCenterMass() const
{
	if(m_pBox2d)
		return glm::vec2(m_pBox2d->m_pBody->GetLocalCenter().x, m_pBox2d->m_pBody->GetLocalCenter().y);

	return glm::vec2(0.0f, 0.0f);
}

void HyPhysicsCtrl2d::ApplyForce(const glm::vec2 &vForce, const glm::vec2 &ptPoint, bool bWake)
{
	if(m_pBox2d)
		m_pBox2d->m_pBody->ApplyForce(b2Vec2(vForce.x, vForce.y), b2Vec2(ptPoint.x, ptPoint.y), bWake);
}

void HyPhysicsCtrl2d::ApplyForceToCenter(const glm::vec2 &vForce, bool bWake)
{
	if(m_pBox2d)
		m_pBox2d->m_pBody->ApplyForceToCenter(b2Vec2(vForce.x, vForce.y), bWake);
}

void HyPhysicsCtrl2d::ApplyTorque(float fTorque, bool bWake)
{
	if(m_pBox2d)
		m_pBox2d->m_pBody->ApplyTorque(fTorque, bWake);
}

void HyPhysicsCtrl2d::ApplyLinearImpulse(const glm::vec2 &vImpulse, const glm::vec2 &ptPoint, bool bWake)
{
	if(m_pBox2d)
		m_pBox2d->m_pBody->ApplyLinearImpulse(b2Vec2(vImpulse.x, vImpulse.y), b2Vec2(ptPoint.x, ptPoint.y), bWake);
}

void HyPhysicsCtrl2d::ApplyLinearImpulseToCenter(const glm::vec2 &vImpulse, bool bWake)
{
	if(m_pBox2d)
		m_pBox2d->m_pBody->ApplyLinearImpulseToCenter(b2Vec2(vImpulse.x, vImpulse.y), bWake);
}

void HyPhysicsCtrl2d::ApplyAngularImpulse(float fImpulse, bool bWake)
{
	if(m_pBox2d)
		m_pBox2d->m_pBody->ApplyAngularImpulse(fImpulse, bWake);
}

float HyPhysicsCtrl2d::GetMass() const
{
	if(m_pBox2d)
		return m_pBox2d->m_pBody->GetMass();

	return 0.0f;
}

float HyPhysicsCtrl2d::GetInertia() const
{
	if(m_pBox2d)
		return m_pBox2d->m_pBody->GetInertia();

	return 0.0f;
}

// Should only be invoked by the parent HyPhysicsGrid2d
void HyPhysicsCtrl2d::Update()
{
	// 'm_pBox2d' is guarenteed to be valid if Update() is invoked (via HyPhysicsGrid2d)
	HyAssert(m_pBox2d, "HyPhysicsCtrl2d::Update() - m_pBox2d was null");
	HyAssert(m_NodeRef.ParentGet(), "HyPhysicsCtrl2d::Update() - Node's parent is null"); // Node's parent must exist
	HyAssert(m_NodeRef.ParentGet()->GetInternalFlags() & IHyNode::NODETYPE_IsPhysicsGrid, "HyPhysicsCtrl2d::Update() - Node's parent isn't a physics grid"); // and also be the HyPhysicsGrid2d that invoked this

	if(m_pBox2d->m_pBody->GetType() != b2_staticBody && m_bEnabled)
	{
		// If any HyAnimFloat controlling the position or rotation are not animating, reset the below lambda that will have the b2Body set them respectively
		if(m_NodeRef.pos.IsAnimating() == false)
		{
			auto fpUpdaterPosX = [&](float fElapsedTime) {
				return (m_pBox2d->m_pBody->GetPosition().x * static_cast<HyPhysicsGrid2d *>(m_NodeRef.ParentGet())->GetPixelsPerMeter());
			};
			auto fpUpdaterPosY = [&](float fElapsedTime) {
				return (m_pBox2d->m_pBody->GetPosition().y * static_cast<HyPhysicsGrid2d *>(m_NodeRef.ParentGet())->GetPixelsPerMeter());
			};

			m_NodeRef.pos.GetAnimFloat(0).Updater(fpUpdaterPosX);
			m_NodeRef.pos.GetAnimFloat(1).Updater(fpUpdaterPosY);
		}
		if(m_NodeRef.rot.IsAnimating() == false)
		{
			m_NodeRef.rot.Updater([&](float fElapsedTime) {
				return glm::degrees(m_pBox2d->m_pBody->GetAngle());
				});
		}
	}
}

//
//std::unique_ptr<HyPhysicsCtrl2d> HyPhysicsCtrl2d::PhysAddCollider(const HyShape2d &shapeRef, float fDensity, float fFriction, float fRestitution, bool bIsSensor, b2Filter collideFilter)
//{
//	if(m_pPhysicsBody == nullptr || shapeRef.IsValidShape() == false)
//		return nullptr;
//
//	b2Shape *pPpmShape = shapeRef.ClonePpmShape(static_cast<HyPhysicsGrid2d *>(m_pPhysicsBody->GetWorld())->GetPpmInverse());
//	std::unique_ptr<HyPhysicsCtrl2d> pCollider = std::make_unique<HyPhysicsCtrl2d>(m_pPhysicsBody, pPpmShape, fDensity, fFriction, fRestitution, bIsSensor, collideFilter);
//	delete pPpmShape;
//
//	return pCollider;
//}
//
//std::unique_ptr<HyPhysicsCtrl2d> HyPhysicsCtrl2d::PhysAddCircleCollider(float fRadius, float fDensity, float fFriction, float fRestitution, bool bIsSensor, b2Filter collideFilter)
//{
//	return PhysAddCircleCollider(glm::vec2(0.0f, 0.0), fRadius, fDensity, fFriction, fRestitution, bIsSensor, collideFilter);
//}
//
//std::unique_ptr<HyPhysicsCtrl2d> HyPhysicsCtrl2d::PhysAddCircleCollider(const glm::vec2 &ptCenter, float fRadius, float fDensity, float fFriction, float fRestitution, bool bIsSensor, b2Filter collideFilter)
//{
//	if(m_pPhysicsBody == nullptr || fRadius <= 0.0f)
//		return nullptr;
//
//	float fPpmInverse = static_cast<HyPhysicsGrid2d *>(m_pPhysicsBody->GetWorld())->GetPpmInverse();
//	b2CircleShape circleShape;
//	circleShape.m_p.x = ptCenter.x * fPpmInverse;
//	circleShape.m_p.y = ptCenter.y * fPpmInverse;
//	circleShape.m_radius = fRadius * fPpmInverse;
//	return std::make_unique<HyPhysicsCtrl2d>(m_pPhysicsBody, &circleShape, fDensity, fFriction, fRestitution, bIsSensor, collideFilter);
//}
//
//std::unique_ptr<HyPhysicsCtrl2d> HyPhysicsCtrl2d::PhysAddLineChainCollider(const glm::vec2 *pVerts, uint32 uiNumVerts, float fDensity, float fFriction, float fRestitution, bool bIsSensor, b2Filter collideFilter)
//{
//	if(m_pPhysicsBody == nullptr || pVerts == nullptr || uiNumVerts == 0)
//		return nullptr;
//
//	float fPpmInverse = static_cast<HyPhysicsGrid2d *>(m_pPhysicsBody->GetWorld())->GetPpmInverse();
//	std::vector<b2Vec2> vertList;
//	for(uint32 i = 0; i < uiNumVerts; ++i)
//		vertList.emplace_back(pVerts[i].x * fPpmInverse, pVerts[i].y * fPpmInverse);
//
//	b2ChainShape chainShape;
//	chainShape.CreateChain(vertList.data(), uiNumVerts);
//	return std::make_unique<HyPhysicsCtrl2d>(m_pPhysicsBody, &chainShape, fDensity, fFriction, fRestitution, bIsSensor, collideFilter);
//}
//
//void HyPhysicsCtrl2d::PhysDestroyCollider(std::unique_ptr<HyPhysicsCtrl2d> pCollider)
//{
//	if(m_pPhysicsBody)
//		m_pPhysicsBody->DestroyFixture(pCollider->GetFixture());
//
//	pCollider.release();
//}
