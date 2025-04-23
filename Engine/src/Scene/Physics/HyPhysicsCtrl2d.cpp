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
#include "Scene/Physics/HyPhysicsCtrl2d.h"
#include "Scene/HyScene.h"
#include "Scene/Nodes/Loadables/Bodies/IHyBody2d.h"
#include "Scene/Nodes/Loadables/Bodies/Objects/HyPhysicsGrid2d.h"
#include "Diagnostics/Console/IHyConsole.h"

HyPhysicsCtrl2d::HyPhysicsCtrl2d(HyEntity2d &entityRef) :
	m_EntityRef(entityRef),
	m_pInit(nullptr),
	m_hBody(b2_nullBodyId)
{
}

HyPhysicsCtrl2d::~HyPhysicsCtrl2d()
{
	delete m_pInit;

	if(B2_IS_NON_NULL(m_hBody))
		IHyBody2d::sm_pScene->RemoveNode_PhysBody(this);
}

void HyPhysicsCtrl2d::Activate()
{
	IHyBody2d::sm_pScene->AddNode_PhysBody(this);
}

void HyPhysicsCtrl2d::Deactivate()
{
	if(B2_IS_NON_NULL(m_hBody))
		b2Body_Disable(m_hBody);
}

bool HyPhysicsCtrl2d::IsActivated() const
{
	return B2_IS_NON_NULL(m_hBody) && b2Body_IsEnabled(m_hBody);
}

void HyPhysicsCtrl2d::Setup(const b2BodyDef &bodyDef)
{
	Setup(static_cast<HyBodyType>(bodyDef.type),
		bodyDef.isEnabled,
		bodyDef.gravityScale,
		bodyDef.fixedRotation,
		bodyDef.isAwake,
		glm::vec2(bodyDef.linearVelocity.x, bodyDef.linearVelocity.y),
		bodyDef.angularVelocity,
		bodyDef.linearDamping,
		bodyDef.angularDamping,
		bodyDef.enableSleep,
		bodyDef.isBullet);
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
	// If 'm_hBody' exists, and is already apart of an active simulation. Simply update the b2Body with all the incoming parameters
	if(b2Body_IsValid(m_hBody))
	{
		b2Body_SetType(m_hBody, static_cast<b2BodyType>(eType));
		b2Body_SetGravityScale(m_hBody, fGravityScale);
		b2Body_SetFixedRotation(m_hBody, bIsFixedRotation);
		b2Body_SetAwake(m_hBody, bIsAwake);
		b2Body_SetLinearVelocity(m_hBody, { vLinearVelocity.x, vLinearVelocity.y });
		b2Body_SetAngularVelocity(m_hBody, fAngularVelocity);
		b2Body_SetLinearDamping(m_hBody, fLinearDamping);
		b2Body_SetAngularDamping(m_hBody, fAngularDamping);
		b2Body_EnableSleep(m_hBody, bAllowSleep);
		b2Body_SetBullet(m_hBody, bIsCcd);
		b2Body_SetUserData(m_hBody, &m_EntityRef);
		if(bIsEnabled)
			b2Body_Enable(m_hBody);
		else
			b2Body_Disable(m_hBody);

		return;
	}

	if(m_pInit == nullptr)
	{
		m_pInit = HY_NEW b2BodyDef();
		*m_pInit = b2DefaultBodyDef();
	}
	
	m_pInit->type = static_cast<b2BodyType>(eType);
	m_pInit->isEnabled = bIsEnabled;
	m_pInit->gravityScale = fGravityScale;
	m_pInit->fixedRotation = bIsFixedRotation;
	m_pInit->isAwake = bIsAwake;
	m_pInit->linearVelocity = { vLinearVelocity.x, vLinearVelocity.y };
	m_pInit->angularVelocity = fAngularVelocity;
	m_pInit->linearDamping = fLinearDamping;
	m_pInit->angularDamping = fAngularDamping;
	m_pInit->enableSleep = bAllowSleep;
	m_pInit->isBullet = bIsCcd;
	m_pInit->userData = &m_EntityRef;
}

HyBodyType HyPhysicsCtrl2d::GetType() const
{
	if(B2_IS_NON_NULL(m_hBody))
		return static_cast<HyBodyType>(b2Body_GetType(m_hBody));
	else if(m_pInit)
		return static_cast<HyBodyType>(m_pInit->type);
	else
		return static_cast<HyBodyType>(b2DefaultBodyDef().type);
}

void HyPhysicsCtrl2d::SetType(HyBodyType eType)
{
	if(B2_IS_NON_NULL(m_hBody))
		b2Body_SetType(m_hBody, static_cast<b2BodyType>(eType));
	else if(m_pInit)
		m_pInit->type = static_cast<b2BodyType>(eType);
	else
	{
		b2BodyDef def = b2DefaultBodyDef();
		def.type = static_cast<b2BodyType>(eType);
		Setup(def);
	}
}

bool HyPhysicsCtrl2d::IsFixedRotation() const
{
	if(B2_IS_NON_NULL(m_hBody))
		return b2Body_IsFixedRotation(m_hBody);
	else if(m_pInit)
		return m_pInit->fixedRotation;
	else
		return b2DefaultBodyDef().fixedRotation;
}

void HyPhysicsCtrl2d::SetFixedRotation(bool bFixedRot)
{
	if(B2_IS_NON_NULL(m_hBody))
		b2Body_SetFixedRotation(m_hBody, bFixedRot);
	else if(m_pInit)
		m_pInit->fixedRotation = bFixedRot;
	else
	{
		b2BodyDef def = b2DefaultBodyDef();
		def.fixedRotation = bFixedRot;
		Setup(def);
	}
}

bool HyPhysicsCtrl2d::IsAwake() const
{
	if(B2_IS_NON_NULL(m_hBody))
		return b2Body_IsAwake(m_hBody);
	else if(m_pInit)
		return m_pInit->isAwake;
	else
		return b2DefaultBodyDef().isAwake;
}

void HyPhysicsCtrl2d::SetAwake(bool bAwake)
{
	if(B2_IS_NON_NULL(m_hBody))
		b2Body_SetAwake(m_hBody, bAwake);
	else if(m_pInit)
		m_pInit->isAwake = bAwake;
	else
	{
		b2BodyDef def = b2DefaultBodyDef();
		def.isAwake = bAwake;
		Setup(def);
	}
}

bool HyPhysicsCtrl2d::IsSleepingAllowed() const
{
	if(B2_IS_NON_NULL(m_hBody))
		return b2Body_IsSleepEnabled(m_hBody);
	else if(m_pInit)
		return m_pInit->enableSleep;
	else
		return b2DefaultBodyDef().enableSleep;
}

void HyPhysicsCtrl2d::SetSleepingAllowed(bool bAllowSleep)
{
	if(B2_IS_NON_NULL(m_hBody))
		b2Body_EnableSleep(m_hBody, bAllowSleep);
	else if(m_pInit)
		m_pInit->enableSleep = bAllowSleep;
	else
	{
		b2BodyDef def = b2DefaultBodyDef();
		def.enableSleep = bAllowSleep;
		Setup(def);
	}
}

float HyPhysicsCtrl2d::GetGravityScale() const
{
	if(B2_IS_NON_NULL(m_hBody))
		return b2Body_GetGravityScale(m_hBody);
	else if(m_pInit)
		return m_pInit->gravityScale;
	else
		return b2DefaultBodyDef().gravityScale;
}

void HyPhysicsCtrl2d::SetGravityScale(float fGravityScale)
{
	if(B2_IS_NON_NULL(m_hBody))
		b2Body_SetGravityScale(m_hBody, fGravityScale);
	else if(m_pInit)
		m_pInit->gravityScale = fGravityScale;
	else
	{
		b2BodyDef def = b2DefaultBodyDef();
		def.gravityScale = fGravityScale;
		Setup(def);
	}
}

bool HyPhysicsCtrl2d::IsCcd() const
{
	if(B2_IS_NON_NULL(m_hBody))
		return b2Body_IsBullet(m_hBody);
	else if(m_pInit)
		return m_pInit->isBullet;
	else
		return b2DefaultBodyDef().isBullet;
}

void HyPhysicsCtrl2d::SetCcd(bool bContinuousCollisionDetection)
{
	if(B2_IS_NON_NULL(m_hBody))
		b2Body_SetBullet(m_hBody, bContinuousCollisionDetection);
	else if(m_pInit)
		m_pInit->isBullet = bContinuousCollisionDetection;
	else
	{
		b2BodyDef def = b2DefaultBodyDef();
		def.isBullet = bContinuousCollisionDetection;
		Setup(def);
	}
}

glm::vec2 HyPhysicsCtrl2d::GetVel() const
{
	if(B2_IS_NON_NULL(m_hBody))
	{
		b2Vec2 vel = b2Body_GetLinearVelocity(m_hBody);
		return glm::vec2(vel.x, vel.y);
	}
	else if(m_pInit)
		return glm::vec2(m_pInit->linearVelocity.x, m_pInit->linearVelocity.y);
	else
	{
		b2BodyDef def = b2DefaultBodyDef();
		return glm::vec2(def.linearVelocity.x, def.linearVelocity.y);
	}
}

float HyPhysicsCtrl2d::GetVelX() const
{
	if(B2_IS_NON_NULL(m_hBody))
		return b2Body_GetLinearVelocity(m_hBody).x;
	else if(m_pInit)
		return m_pInit->linearVelocity.x;
	else
		return b2DefaultBodyDef().linearVelocity.x;
}

float HyPhysicsCtrl2d::GetVelY() const
{
	if(B2_IS_NON_NULL(m_hBody))
		return b2Body_GetLinearVelocity(m_hBody).y;
	else if(m_pInit)
		return m_pInit->linearVelocity.y;
	else
		return b2DefaultBodyDef().linearVelocity.y;
}

void HyPhysicsCtrl2d::SetVel(const glm::vec2 &vVelocity)
{
	SetVel(vVelocity.x, vVelocity.y);
}

void HyPhysicsCtrl2d::SetVel(float fVelocityX, float fVelocityY)
{
	if(B2_IS_NON_NULL(m_hBody))
		b2Body_SetLinearVelocity(m_hBody, { fVelocityX, fVelocityY });
	else if(m_pInit)
		m_pInit->linearVelocity = { fVelocityX, fVelocityY };
	else
	{
		b2BodyDef def = b2DefaultBodyDef();
		def.linearVelocity = { fVelocityX, fVelocityY };
		Setup(def);
	}
}

void HyPhysicsCtrl2d::SetVelX(float fVelocityX)
{
	if(B2_IS_NON_NULL(m_hBody))
		b2Body_SetLinearVelocity(m_hBody, { fVelocityX, b2Body_GetLinearVelocity(m_hBody).y });
	else if(m_pInit)
		m_pInit->linearVelocity = { fVelocityX, m_pInit->linearVelocity.y };
	else
	{
		b2BodyDef def = b2DefaultBodyDef();
		def.linearVelocity = { fVelocityX, def.linearVelocity.y };
		Setup(def);
	}
}

void HyPhysicsCtrl2d::SetVelY(float fVelocityY)
{
	if(B2_IS_NON_NULL(m_hBody))
		b2Body_SetLinearVelocity(m_hBody, { b2Body_GetLinearVelocity(m_hBody).x, fVelocityY });
	else if(m_pInit)
		m_pInit->linearVelocity = { m_pInit->linearVelocity.x, fVelocityY };
	else
	{
		b2BodyDef def = b2DefaultBodyDef();
		def.linearVelocity = { def.linearVelocity.x, fVelocityY };
		Setup(def);
	}
}

float HyPhysicsCtrl2d::GetAngVel() const
{
	if(B2_IS_NON_NULL(m_hBody))
		return b2Body_GetAngularVelocity(m_hBody);
	else if(m_pInit)
		return m_pInit->angularVelocity;
	else
		return b2DefaultBodyDef().angularVelocity;
}

void HyPhysicsCtrl2d::SetAngVel(float fOmega)
{
	if(B2_IS_NON_NULL(m_hBody))
		b2Body_SetAngularVelocity(m_hBody, fOmega);
	else if(m_pInit)
		m_pInit->angularVelocity = fOmega;
	else
	{
		b2BodyDef def = b2DefaultBodyDef();
		def.angularVelocity = fOmega;
		Setup(def);
	}
}

void HyPhysicsCtrl2d::AddForce(const glm::vec2 &vForce)
{
	if(B2_IS_NON_NULL(m_hBody))
		b2Body_ApplyForceToCenter(m_hBody, { vForce.x, vForce.y }, true);
	else
		HyLogWarning("HyPhysicsCtrl2d::ApplyForceToCenter invoked before physics component was set active");
}

void HyPhysicsCtrl2d::AddForceX(float fForceX)
{
	if(B2_IS_NON_NULL(m_hBody))
		b2Body_ApplyForceToCenter(m_hBody, { fForceX, 0.0f }, true);
	else
		HyLogWarning("HyPhysicsCtrl2d::AddForceX invoked before physics component was set active");
}

void HyPhysicsCtrl2d::AddForceY(float fForceY)
{
	if(B2_IS_NON_NULL(m_hBody))
		b2Body_ApplyForceToCenter(m_hBody, { 0.0f, fForceY }, true);
	else
		HyLogWarning("HyPhysicsCtrl2d::AddForceY invoked before physics component was set active");
}

void HyPhysicsCtrl2d::AddForceToPt(const glm::vec2 &vForce, const glm::vec2 &ptScenePoint)
{
	if(B2_IS_NON_NULL(m_hBody))
		b2Body_ApplyForce(m_hBody, { vForce.x, vForce.y }, { ptScenePoint.x * IHyNode::sm_pScene->GetPpmInverse(), ptScenePoint.y * IHyNode::sm_pScene->GetPpmInverse() }, true);
	else
		HyLogWarning("HyPhysicsCtrl2d::AddForceToPt invoked before physics component was set active");
}

void HyPhysicsCtrl2d::AddTorque(float fTorque)
{
	if(B2_IS_NON_NULL(m_hBody))
		b2Body_ApplyTorque(m_hBody, fTorque, true);
	else
		HyLogWarning("HyPhysicsCtrl2d::AddTorque invoked before physics component was set active");
}

void HyPhysicsCtrl2d::AddImpulse(const glm::vec2 &vImpulse)
{
	if(B2_IS_NON_NULL(m_hBody))
		b2Body_ApplyLinearImpulseToCenter(m_hBody, { vImpulse.x, vImpulse.y }, true);
	else
		HyLogWarning("HyPhysicsCtrl2d::AddImpulse invoked before physics component was set active");
}

void HyPhysicsCtrl2d::AddImpulseX(float fImpuseX)
{
	if(B2_IS_NON_NULL(m_hBody))
		b2Body_ApplyLinearImpulseToCenter(m_hBody, { fImpuseX, 0.0f }, true);
	else
		HyLogWarning("HyPhysicsCtrl2d::AddImpulseX invoked before physics component was set active");
}

void HyPhysicsCtrl2d::AddImpulseY(float fImpuseY)
{
	if(B2_IS_NON_NULL(m_hBody))
		b2Body_ApplyLinearImpulseToCenter(m_hBody, { 0.0f, fImpuseY }, true);
	else
		HyLogWarning("HyPhysicsCtrl2d::AddImpulseY invoked before physics component was set active");
}

void HyPhysicsCtrl2d::AddImpulseToPt(const glm::vec2 &vImpulse, const glm::vec2 &ptScenePoint)
{
	if(B2_IS_NON_NULL(m_hBody))
		b2Body_ApplyLinearImpulse(m_hBody, { vImpulse.x, vImpulse.y }, { ptScenePoint.x * IHyNode::sm_pScene->GetPpmInverse(), ptScenePoint.y * IHyNode::sm_pScene->GetPpmInverse() }, true);
	else
		HyLogWarning("HyPhysicsCtrl2d::AddImpulse invoked before physics component was set active");
}

void HyPhysicsCtrl2d::AddAngImpulse(float fImpulse)
{
	if(B2_IS_NON_NULL(m_hBody))
		b2Body_ApplyAngularImpulse(m_hBody, fImpulse, true);
	else
		HyLogWarning("HyPhysicsCtrl2d::AddAngImpulse invoked before physics component was set active");
}

float HyPhysicsCtrl2d::GetLinearDamping() const
{
	if(B2_IS_NON_NULL(m_hBody))
		return b2Body_GetLinearDamping(m_hBody);
	else if(m_pInit)
		return m_pInit->linearDamping;
	else
		return b2DefaultBodyDef().linearDamping;
}

void HyPhysicsCtrl2d::SetLinearDamping(float fLinearDamping)
{
	if(B2_IS_NON_NULL(m_hBody))
		b2Body_SetLinearDamping(m_hBody, fLinearDamping);
	else if(m_pInit)
		m_pInit->linearDamping = fLinearDamping;
	else
	{
		b2BodyDef def = b2DefaultBodyDef();
		def.linearDamping = fLinearDamping;
		Setup(def);
	}
}

float HyPhysicsCtrl2d::GetAngularDamping() const
{
	if(B2_IS_NON_NULL(m_hBody))
		return b2Body_GetAngularDamping(m_hBody);
	else if(m_pInit)
		return m_pInit->angularDamping;
	else
		return b2DefaultBodyDef().angularDamping;
}

void HyPhysicsCtrl2d::SetAngularDamping(float fAngularDamping)
{
	if(B2_IS_NON_NULL(m_hBody))
		b2Body_SetAngularDamping(m_hBody, fAngularDamping);
	else if(m_pInit)
		m_pInit->angularDamping = fAngularDamping;
	else
	{
		b2BodyDef def = b2DefaultBodyDef();
		def.angularDamping = fAngularDamping;
		Setup(def);
	}
}

//glm::vec2 HyPhysicsCtrl2d::GetSceneCenterMass() const
//{
//	if(B2_IS_NON_NULL(m_hBody))
//		return glm::vec2(m_pBody->GetWorldCenter().x, m_pBody->GetWorldCenter().y);
//
//	HyLogWarning("HyPhysicsCtrl2d::GetSceneCenter invoked before physics component was set active");
//	return glm::vec2(0.0f, 0.0f);
//}
//
//glm::vec2 HyPhysicsCtrl2d::GetLocalCenterMass() const
//{
//	if(B2_IS_NON_NULL(m_hBody))
//		return glm::vec2(m_pBody->GetLocalCenter().x, m_pBody->GetLocalCenter().y);
//
//	HyLogWarning("HyPhysicsCtrl2d::LocalCenterMass invoked before physics component was set active");
//	return glm::vec2(0.0f, 0.0f);
//}

float HyPhysicsCtrl2d::GetMass() const
{
	if(B2_IS_NON_NULL(m_hBody))
		return b2Body_GetMass(m_hBody);

	HyLogWarning("HyPhysicsCtrl2d::GetMass invoked before physics component was set active");
	return 0.0f;
}

float HyPhysicsCtrl2d::GetRotInertia() const
{
	if(B2_IS_NON_NULL(m_hBody))
		return b2Body_GetRotationalInertia(m_hBody);

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
