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

HyPhysicsCtrl2d::HyPhysicsCtrl2d(HyEntity2d &entityRef) :
	m_EntityRef(entityRef),
	m_pInit(nullptr),
	m_pBody(nullptr)
{
}

HyPhysicsCtrl2d::~HyPhysicsCtrl2d()
{
	delete m_pInit;

	if(m_pBody)
		IHyBody2d::sm_pScene->RemoveNode_PhysBody(&m_EntityRef);
}

void HyPhysicsCtrl2d::Activate()
{
	IHyBody2d::sm_pScene->AddNode_PhysBody(&m_EntityRef);
}

void HyPhysicsCtrl2d::Deactivate()
{
	if(m_pBody)
		m_pBody->SetEnabled(false);
}

bool HyPhysicsCtrl2d::IsActivated() const
{
	return m_pBody && m_pBody->IsEnabled();
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
		m_pBody->GetUserData().pointer = reinterpret_cast<uintptr_t>(&m_EntityRef);

		return;
	}

	if(m_pInit == nullptr)
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
	m_pInit->userData.pointer = reinterpret_cast<uintptr_t>(&m_EntityRef);
}

HyBodyType HyPhysicsCtrl2d::GetType() const
{
	if(m_pBody)
		return static_cast<HyBodyType>(m_pBody->GetType());
	else if(m_pInit)
		return static_cast<HyBodyType>(m_pInit->type);
	else
		return static_cast<HyBodyType>(b2BodyDef().type);
}

void HyPhysicsCtrl2d::SetType(HyBodyType eType)
{
	if(m_pBody)
		m_pBody->SetType(static_cast<b2BodyType>(eType));
	else if(m_pInit)
		m_pInit->type = static_cast<b2BodyType>(eType);
	else
	{
		b2BodyDef def;
		def.type = static_cast<b2BodyType>(eType);
		Setup(def);
	}
}

bool HyPhysicsCtrl2d::IsFixedRotation() const
{
	if(m_pBody)
		return m_pBody->IsFixedRotation();
	else if(m_pInit)
		return m_pInit->fixedRotation;
	else
		return b2BodyDef().fixedRotation;
}

void HyPhysicsCtrl2d::SetFixedRotation(bool bFixedRot)
{
	if(m_pBody)
		m_pBody->SetFixedRotation(bFixedRot);
	else if(m_pInit)
		m_pInit->fixedRotation = bFixedRot;
	else
	{
		b2BodyDef def;
		def.fixedRotation = bFixedRot;
		Setup(def);
	}
}

bool HyPhysicsCtrl2d::IsAwake() const
{
	if(m_pBody)
		return m_pBody->IsAwake();
	else if(m_pInit)
		return m_pInit->awake;
	else
		return b2BodyDef().awake;
}

void HyPhysicsCtrl2d::SetAwake(bool bAwake)
{
	if(m_pBody)
		m_pBody->SetAwake(bAwake);
	else if(m_pInit)
		m_pInit->awake = bAwake;
	else
	{
		b2BodyDef def;
		def.awake = bAwake;
		Setup(def);
	}
}

bool HyPhysicsCtrl2d::IsSleepingAllowed() const
{
	if(m_pBody)
		return m_pBody->IsSleepingAllowed();
	else if(m_pInit)
		return m_pInit->allowSleep;
	else
		return b2BodyDef().allowSleep;
}

void HyPhysicsCtrl2d::SetSleepingAllowed(bool bAllowSleep)
{
	if(m_pBody)
		m_pBody->SetSleepingAllowed(bAllowSleep);
	else if(m_pInit)
		m_pInit->allowSleep = bAllowSleep;
	else
	{
		b2BodyDef def;
		def.allowSleep = bAllowSleep;
		Setup(def);
	}
}

float HyPhysicsCtrl2d::GetGravityScale() const
{
	if(m_pBody)
		return m_pBody->GetGravityScale();
	else if(m_pInit)
		return m_pInit->gravityScale;
	else
		return b2BodyDef().gravityScale;
}

void HyPhysicsCtrl2d::SetGravityScale(float fGravityScale)
{
	if(m_pBody)
		m_pBody->SetGravityScale(fGravityScale);
	else if(m_pInit)
		m_pInit->gravityScale = fGravityScale;
	else
	{
		b2BodyDef def;
		def.gravityScale = fGravityScale;
		Setup(def);
	}
}

bool HyPhysicsCtrl2d::IsCcd() const
{
	if(m_pBody)
		return m_pBody->IsBullet();
	else if(m_pInit)
		return m_pInit->bullet;
	else
		return b2BodyDef().bullet;
}

void HyPhysicsCtrl2d::SetCcd(bool bContinuousCollisionDetection)
{
	if(m_pBody)
		m_pBody->SetBullet(bContinuousCollisionDetection);
	else if(m_pInit)
		m_pInit->bullet = bContinuousCollisionDetection;
	else
	{
		b2BodyDef def;
		def.bullet = bContinuousCollisionDetection;
		Setup(def);
	}
}

glm::vec2 HyPhysicsCtrl2d::GetVel() const
{
	if(m_pBody)
		return glm::vec2(m_pBody->GetLinearVelocity().x, m_pBody->GetLinearVelocity().y);
	else if(m_pInit)
		return glm::vec2(m_pInit->linearVelocity.x, m_pInit->linearVelocity.y);
	else
		return glm::vec2(b2BodyDef().linearVelocity.x, b2BodyDef().linearVelocity.y);
}

float HyPhysicsCtrl2d::GetVelX() const
{
	if(m_pBody)
		return m_pBody->GetLinearVelocity().x;
	else if(m_pInit)
		return m_pInit->linearVelocity.x;
	else
		return b2BodyDef().linearVelocity.x;
}

float HyPhysicsCtrl2d::GetVelY() const
{
	if(m_pBody)
		return m_pBody->GetLinearVelocity().y;
	else if(m_pInit)
		return m_pInit->linearVelocity.y;
	else
		return b2BodyDef().linearVelocity.y;
}

void HyPhysicsCtrl2d::SetVel(const glm::vec2 &vVelocity)
{
	if(m_pBody)
		m_pBody->SetLinearVelocity(b2Vec2(vVelocity.x, vVelocity.y));
	else if(m_pInit)
		m_pInit->linearVelocity = b2Vec2(vVelocity.x, vVelocity.y);
	else
	{
		b2BodyDef def;
		def.linearVelocity = b2Vec2(vVelocity.x, vVelocity.y);
		Setup(def);
	}
}

void HyPhysicsCtrl2d::SetVelX(float fVelocityX)
{
	if(m_pBody)
		m_pBody->SetLinearVelocity(b2Vec2(fVelocityX, m_pBody->GetLinearVelocity().y));
	else if(m_pInit)
		m_pInit->linearVelocity = b2Vec2(fVelocityX, m_pInit->linearVelocity.y);
	else
	{
		b2BodyDef def;
		def.linearVelocity = b2Vec2(fVelocityX, def.linearVelocity.y);
		Setup(def);
	}
}

void HyPhysicsCtrl2d::SetVelY(float fVelocityY)
{
	if(m_pBody)
		m_pBody->SetLinearVelocity(b2Vec2(m_pBody->GetLinearVelocity().x, fVelocityY));
	else if(m_pInit)
		m_pInit->linearVelocity = b2Vec2(m_pInit->linearVelocity.x, fVelocityY);
	else
	{
		b2BodyDef def;
		def.linearVelocity = b2Vec2(def.linearVelocity.x, fVelocityY);
		Setup(def);
	}
}

float HyPhysicsCtrl2d::GetAngVel() const
{
	if(m_pBody)
		return m_pBody->GetAngularVelocity();
	else if(m_pInit)
		return m_pInit->angularVelocity;
	else
		return b2BodyDef().angularVelocity;
}

void HyPhysicsCtrl2d::SetAngVel(float fOmega)
{
	if(m_pBody)
		m_pBody->SetAngularVelocity(fOmega);
	else if(m_pInit)
		m_pInit->angularVelocity = fOmega;
	else
	{
		b2BodyDef def;
		def.angularVelocity = fOmega;
		Setup(def);
	}
}

void HyPhysicsCtrl2d::AddForce(const glm::vec2 &vForce)
{
	if(m_pBody)
		m_pBody->ApplyForceToCenter(b2Vec2(vForce.x, vForce.y), true);
	else
		HyLogWarning("HyPhysicsCtrl2d::ApplyForceToCenter invoked before physics component was set active");
}

void HyPhysicsCtrl2d::AddForceX(float fForceX)
{
	if(m_pBody)
		m_pBody->ApplyForceToCenter(b2Vec2(fForceX, 0.0f), true);
	else
		HyLogWarning("HyPhysicsCtrl2d::AddForceX invoked before physics component was set active");
}

void HyPhysicsCtrl2d::AddForceY(float fForceY)
{
	if(m_pBody)
		m_pBody->ApplyForceToCenter(b2Vec2(0.0f, fForceY), true);
	else
		HyLogWarning("HyPhysicsCtrl2d::AddForceY invoked before physics component was set active");
}

void HyPhysicsCtrl2d::AddForceToPt(const glm::vec2 &vForce, const glm::vec2 &ptScenePoint)
{
	if(m_pBody)
		m_pBody->ApplyForce(b2Vec2(vForce.x, vForce.y), b2Vec2(ptScenePoint.x * IHyNode::sm_pScene->GetPpmInverse(), ptScenePoint.y * IHyNode::sm_pScene->GetPpmInverse()), true);
	else
		HyLogWarning("HyPhysicsCtrl2d::AddForceToPt invoked before physics component was set active");
}

void HyPhysicsCtrl2d::AddTorque(float fTorque)
{
	if(m_pBody)
		m_pBody->ApplyTorque(fTorque, true);
	else
		HyLogWarning("HyPhysicsCtrl2d::AddTorque invoked before physics component was set active");
}

void HyPhysicsCtrl2d::AddImpulse(const glm::vec2 &vImpulse)
{
	if(m_pBody)
		m_pBody->ApplyLinearImpulseToCenter(b2Vec2(vImpulse.x, vImpulse.y), true);
	else
		HyLogWarning("HyPhysicsCtrl2d::AddImpulse invoked before physics component was set active");
}

void HyPhysicsCtrl2d::AddImpulseX(float fImpuseX)
{
	if(m_pBody)
		m_pBody->ApplyLinearImpulseToCenter(b2Vec2(fImpuseX, 0.0f), true);
	else
		HyLogWarning("HyPhysicsCtrl2d::AddImpulseX invoked before physics component was set active");
}

void HyPhysicsCtrl2d::AddImpulseY(float fImpuseY)
{
	if(m_pBody)
		m_pBody->ApplyLinearImpulseToCenter(b2Vec2(0.0f, fImpuseY), true);
	else
		HyLogWarning("HyPhysicsCtrl2d::AddImpulseY invoked before physics component was set active");
}

void HyPhysicsCtrl2d::AddImpulseToPt(const glm::vec2 &vImpulse, const glm::vec2 &ptScenePoint)
{
	if(m_pBody)
		m_pBody->ApplyLinearImpulse(b2Vec2(vImpulse.x, vImpulse.y), b2Vec2(ptScenePoint.x * IHyNode::sm_pScene->GetPpmInverse(), ptScenePoint.y * IHyNode::sm_pScene->GetPpmInverse()), true);
	else
		HyLogWarning("HyPhysicsCtrl2d::AddImpulse invoked before physics component was set active");
}

void HyPhysicsCtrl2d::AddAngImpulse(float fImpulse)
{
	if(m_pBody)
		m_pBody->ApplyAngularImpulse(fImpulse, true);
	else
		HyLogWarning("HyPhysicsCtrl2d::AddAngImpulse invoked before physics component was set active");
}

float HyPhysicsCtrl2d::GetLinearDamping() const
{
	if(m_pBody)
		return m_pBody->GetLinearDamping();
	else if(m_pInit)
		return m_pInit->linearDamping;
	else
		return b2BodyDef().linearDamping;
}

void HyPhysicsCtrl2d::SetLinearDamping(float fLinearDamping)
{
	if(m_pBody)
		m_pBody->SetLinearDamping(fLinearDamping);
	else if(m_pInit)
		m_pInit->linearDamping = fLinearDamping;
	else
	{
		b2BodyDef def;
		def.linearDamping = fLinearDamping;
		Setup(def);
	}
}

float HyPhysicsCtrl2d::GetAngularDamping() const
{
	if(m_pBody)
		return m_pBody->GetAngularDamping();
	else if(m_pInit)
		return m_pInit->angularDamping;
	else
		return b2BodyDef().angularDamping;
}

void HyPhysicsCtrl2d::SetAngularDamping(float fAngularDamping)
{
	if(m_pBody)
		m_pBody->SetAngularDamping(fAngularDamping);
	else if(m_pInit)
		m_pInit->angularDamping = fAngularDamping;
	else
	{
		b2BodyDef def;
		def.angularDamping = fAngularDamping;
		Setup(def);
	}
}

glm::vec2 HyPhysicsCtrl2d::GetSceneCenterMass() const
{
	if(m_pBody)
		return glm::vec2(m_pBody->GetWorldCenter().x, m_pBody->GetWorldCenter().y);

	HyLogWarning("HyPhysicsCtrl2d::GetSceneCenter invoked before physics component was set active");
	return glm::vec2(0.0f, 0.0f);
}

glm::vec2 HyPhysicsCtrl2d::GetLocalCenterMass() const
{
	if(m_pBody)
		return glm::vec2(m_pBody->GetLocalCenter().x, m_pBody->GetLocalCenter().y);

	HyLogWarning("HyPhysicsCtrl2d::LocalCenterMass invoked before physics component was set active");
	return glm::vec2(0.0f, 0.0f);
}

float HyPhysicsCtrl2d::GetMass() const
{
	if(m_pBody)
		return m_pBody->GetMass();

	HyLogWarning("HyPhysicsCtrl2d::GetMass invoked before physics component was set active");
	return 0.0f;
}

float HyPhysicsCtrl2d::GetInertia() const
{
	if(m_pBody)
		return m_pBody->GetInertia();

	HyLogWarning("HyPhysicsCtrl2d::GetInertia invoked before physics component was set active");
	return 0.0f;
}

//// Should only be invoked by the parent HyPhysicsGrid2d
//void HyPhysicsCtrl2d::Update()
//{
//	// 'm_pBox2d' is guarenteed to be valid if Update() is invoked (via HyPhysicsGrid2d)
//	HyAssert(m_pBox2d, "HyPhysicsCtrl2d::Update() - m_pBox2d was null");
//	HyAssert(m_NodeRef.ParentGet(), "HyPhysicsCtrl2d::Update() - Node's parent is null"); // Node's parent must exist
//	HyAssert(m_NodeRef.ParentGet()->GetInternalFlags() & IHyNode::NODETYPE_IsPhysicsGrid, "HyPhysicsCtrl2d::Update() - Node's parent isn't a physics grid"); // and also be the HyPhysicsGrid2d that invoked this
//
//	if(m_pBox2d->m_pBody->GetType() != b2_staticBody && m_bEnabled)
//	{
//		// If any HyAnimFloat controlling the position or rotation are not animating, reset the below lambda that will have the b2Body set them respectively
//		if(m_NodeRef.pos.IsAnimating() == false)
//		{
//			auto fpUpdaterPosX = [&](float fElapsedTime) {
//				return (m_pBox2d->m_pBody->GetPosition().x * static_cast<HyPhysicsGrid2d *>(m_NodeRef.ParentGet())->GetPixelsPerMeter());
//			};
//			auto fpUpdaterPosY = [&](float fElapsedTime) {
//				return (m_pBox2d->m_pBody->GetPosition().y * static_cast<HyPhysicsGrid2d *>(m_NodeRef.ParentGet())->GetPixelsPerMeter());
//			};
//
//			m_NodeRef.pos.GetAnimFloat(0).Updater(fpUpdaterPosX);
//			m_NodeRef.pos.GetAnimFloat(1).Updater(fpUpdaterPosY);
//		}
//		if(m_NodeRef.rot.IsAnimating() == false)
//		{
//			m_NodeRef.rot.Updater([&](float fElapsedTime) {
//				return glm::degrees(m_pBox2d->m_pBody->GetAngle());
//				});
//		}
//	}
//}

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
