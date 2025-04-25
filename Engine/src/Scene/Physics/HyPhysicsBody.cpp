/**************************************************************************
*	HyPhysicsBody.cpp
*	
*	Harmony Engine
*	Copyright (c) 2022 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Scene/Physics/HyPhysicsBody.h"
#include "Scene/HyScene.h"
#include "Scene/Nodes/Loadables/Bodies/IHyBody2d.h"
#include "Scene/Nodes/Loadables/Bodies/Objects/HyPhysicsGrid2d.h"
#include "Diagnostics/Console/IHyConsole.h"
#include "Scene/Nodes/Loadables/Bodies/Objects/HyEntity2d.h"

HyPhysicsBody::HyPhysicsBody(HyEntity2d &entityRef) :
	m_EntityRef(entityRef),
	m_pInit(nullptr),
	m_hBody(b2_nullBodyId)
{
}

HyPhysicsBody::~HyPhysicsBody()
{
	Destroy(true);
}

b2BodyId HyPhysicsBody::GetHandle() const
{
	return m_hBody;
}

void HyPhysicsBody::Setup(const b2BodyDef &bodyDef)
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

void HyPhysicsBody::Setup(HyBodyType eType,
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

HyBodyType HyPhysicsBody::GetType() const
{
	if(B2_IS_NON_NULL(m_hBody))
		return static_cast<HyBodyType>(b2Body_GetType(m_hBody));
	else if(m_pInit)
		return static_cast<HyBodyType>(m_pInit->type);
	else
		return static_cast<HyBodyType>(b2DefaultBodyDef().type);
}

void HyPhysicsBody::SetType(HyBodyType eType)
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

bool HyPhysicsBody::IsFixedRotation() const
{
	if(B2_IS_NON_NULL(m_hBody))
		return b2Body_IsFixedRotation(m_hBody);
	else if(m_pInit)
		return m_pInit->fixedRotation;
	else
		return b2DefaultBodyDef().fixedRotation;
}

void HyPhysicsBody::SetFixedRotation(bool bFixedRot)
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

bool HyPhysicsBody::IsAwake() const
{
	if(B2_IS_NON_NULL(m_hBody))
		return b2Body_IsAwake(m_hBody);
	else if(m_pInit)
		return m_pInit->isAwake;
	else
		return b2DefaultBodyDef().isAwake;
}

void HyPhysicsBody::SetAwake(bool bAwake)
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

bool HyPhysicsBody::IsSleepingAllowed() const
{
	if(B2_IS_NON_NULL(m_hBody))
		return b2Body_IsSleepEnabled(m_hBody);
	else if(m_pInit)
		return m_pInit->enableSleep;
	else
		return b2DefaultBodyDef().enableSleep;
}

void HyPhysicsBody::SetSleepingAllowed(bool bAllowSleep)
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

float HyPhysicsBody::GetGravityScale() const
{
	if(B2_IS_NON_NULL(m_hBody))
		return b2Body_GetGravityScale(m_hBody);
	else if(m_pInit)
		return m_pInit->gravityScale;
	else
		return b2DefaultBodyDef().gravityScale;
}

void HyPhysicsBody::SetGravityScale(float fGravityScale)
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

bool HyPhysicsBody::IsCcd() const
{
	if(B2_IS_NON_NULL(m_hBody))
		return b2Body_IsBullet(m_hBody);
	else if(m_pInit)
		return m_pInit->isBullet;
	else
		return b2DefaultBodyDef().isBullet;
}

void HyPhysicsBody::SetCcd(bool bContinuousCollisionDetection)
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

glm::vec2 HyPhysicsBody::GetVel() const
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

float HyPhysicsBody::GetVelX() const
{
	if(B2_IS_NON_NULL(m_hBody))
		return b2Body_GetLinearVelocity(m_hBody).x;
	else if(m_pInit)
		return m_pInit->linearVelocity.x;
	else
		return b2DefaultBodyDef().linearVelocity.x;
}

float HyPhysicsBody::GetVelY() const
{
	if(B2_IS_NON_NULL(m_hBody))
		return b2Body_GetLinearVelocity(m_hBody).y;
	else if(m_pInit)
		return m_pInit->linearVelocity.y;
	else
		return b2DefaultBodyDef().linearVelocity.y;
}

void HyPhysicsBody::SetVel(const glm::vec2 &vVelocity)
{
	SetVel(vVelocity.x, vVelocity.y);
}

void HyPhysicsBody::SetVel(float fVelocityX, float fVelocityY)
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

void HyPhysicsBody::SetVelX(float fVelocityX)
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

void HyPhysicsBody::SetVelY(float fVelocityY)
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

float HyPhysicsBody::GetAngVel() const
{
	if(B2_IS_NON_NULL(m_hBody))
		return b2Body_GetAngularVelocity(m_hBody);
	else if(m_pInit)
		return m_pInit->angularVelocity;
	else
		return b2DefaultBodyDef().angularVelocity;
}

void HyPhysicsBody::SetAngVel(float fOmega)
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

void HyPhysicsBody::AddForce(const glm::vec2 &vForce)
{
	if(B2_IS_NON_NULL(m_hBody))
		b2Body_ApplyForceToCenter(m_hBody, { vForce.x, vForce.y }, true);
	else
		HyLogWarning("HyPhysicsBody::ApplyForceToCenter invoked before physics component was set active");
}

void HyPhysicsBody::AddForceX(float fForceX)
{
	if(B2_IS_NON_NULL(m_hBody))
		b2Body_ApplyForceToCenter(m_hBody, { fForceX, 0.0f }, true);
	else
		HyLogWarning("HyPhysicsBody::AddForceX invoked before physics component was set active");
}

void HyPhysicsBody::AddForceY(float fForceY)
{
	if(B2_IS_NON_NULL(m_hBody))
		b2Body_ApplyForceToCenter(m_hBody, { 0.0f, fForceY }, true);
	else
		HyLogWarning("HyPhysicsBody::AddForceY invoked before physics component was set active");
}

void HyPhysicsBody::AddForceToPt(const glm::vec2 &vForce, const glm::vec2 &ptScenePoint)
{
	if(B2_IS_NON_NULL(m_hBody))
		b2Body_ApplyForce(m_hBody, { vForce.x, vForce.y }, { ptScenePoint.x * IHyNode::sm_pScene->GetPpmInverse(), ptScenePoint.y * IHyNode::sm_pScene->GetPpmInverse() }, true);
	else
		HyLogWarning("HyPhysicsBody::AddForceToPt invoked before physics component was set active");
}

void HyPhysicsBody::AddTorque(float fTorque)
{
	if(B2_IS_NON_NULL(m_hBody))
		b2Body_ApplyTorque(m_hBody, fTorque, true);
	else
		HyLogWarning("HyPhysicsBody::AddTorque invoked before physics component was set active");
}

void HyPhysicsBody::AddImpulse(const glm::vec2 &vImpulse)
{
	if(B2_IS_NON_NULL(m_hBody))
		b2Body_ApplyLinearImpulseToCenter(m_hBody, { vImpulse.x, vImpulse.y }, true);
	else
		HyLogWarning("HyPhysicsBody::AddImpulse invoked before physics component was set active");
}

void HyPhysicsBody::AddImpulseX(float fImpuseX)
{
	if(B2_IS_NON_NULL(m_hBody))
		b2Body_ApplyLinearImpulseToCenter(m_hBody, { fImpuseX, 0.0f }, true);
	else
		HyLogWarning("HyPhysicsBody::AddImpulseX invoked before physics component was set active");
}

void HyPhysicsBody::AddImpulseY(float fImpuseY)
{
	if(B2_IS_NON_NULL(m_hBody))
		b2Body_ApplyLinearImpulseToCenter(m_hBody, { 0.0f, fImpuseY }, true);
	else
		HyLogWarning("HyPhysicsBody::AddImpulseY invoked before physics component was set active");
}

void HyPhysicsBody::AddImpulseToPt(const glm::vec2 &vImpulse, const glm::vec2 &ptScenePoint)
{
	if(B2_IS_NON_NULL(m_hBody))
		b2Body_ApplyLinearImpulse(m_hBody, { vImpulse.x, vImpulse.y }, { ptScenePoint.x * IHyNode::sm_pScene->GetPpmInverse(), ptScenePoint.y * IHyNode::sm_pScene->GetPpmInverse() }, true);
	else
		HyLogWarning("HyPhysicsBody::AddImpulse invoked before physics component was set active");
}

void HyPhysicsBody::AddAngImpulse(float fImpulse)
{
	if(B2_IS_NON_NULL(m_hBody))
		b2Body_ApplyAngularImpulse(m_hBody, fImpulse, true);
	else
		HyLogWarning("HyPhysicsBody::AddAngImpulse invoked before physics component was set active");
}

float HyPhysicsBody::GetLinearDamping() const
{
	if(B2_IS_NON_NULL(m_hBody))
		return b2Body_GetLinearDamping(m_hBody);
	else if(m_pInit)
		return m_pInit->linearDamping;
	else
		return b2DefaultBodyDef().linearDamping;
}

void HyPhysicsBody::SetLinearDamping(float fLinearDamping)
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

float HyPhysicsBody::GetAngularDamping() const
{
	if(B2_IS_NON_NULL(m_hBody))
		return b2Body_GetAngularDamping(m_hBody);
	else if(m_pInit)
		return m_pInit->angularDamping;
	else
		return b2DefaultBodyDef().angularDamping;
}

void HyPhysicsBody::SetAngularDamping(float fAngularDamping)
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

glm::vec2 HyPhysicsBody::GetSceneCenterMass() const
{
	if(B2_IS_NON_NULL(m_hBody))
	{
		b2Vec2 ptCenter = b2Body_GetWorldCenterOfMass(m_hBody);
		return glm::vec2(ptCenter.x, ptCenter.y);
	}

	HyLogWarning("HyPhysicsBody::GetSceneCenter invoked before physics component was set active");
	return glm::vec2(0.0f, 0.0f);
}

glm::vec2 HyPhysicsBody::GetLocalCenterMass() const
{
	if(B2_IS_NON_NULL(m_hBody))
	{
		b2Vec2 ptCenter = b2Body_GetLocalCenterOfMass(m_hBody);
		return glm::vec2(ptCenter.x, ptCenter.y);
	}

	HyLogWarning("HyPhysicsBody::LocalCenterMass invoked before physics component was set active");
	return glm::vec2(0.0f, 0.0f);
}

float HyPhysicsBody::GetMass() const
{
	if(B2_IS_NON_NULL(m_hBody))
		return b2Body_GetMass(m_hBody);

	HyLogWarning("HyPhysicsBody::GetMass invoked before physics component was set active");
	return 0.0f;
}

float HyPhysicsBody::GetRotInertia() const
{
	if(B2_IS_NON_NULL(m_hBody))
		return b2Body_GetRotationalInertia(m_hBody);

	HyLogWarning("HyPhysicsBody::GetInertia invoked before physics component was set active");
	return 0.0f;
}

void HyPhysicsBody::Activate()
{
	if(b2Body_IsValid(m_hBody))
	{
		if(b2Body_IsEnabled(m_hBody) == false)
		{
			m_EntityRef.SyncPhysicsBody();
			m_EntityRef.SyncPhysicsShapes();
			b2Body_Enable(m_hBody);
		}
	}
	else
	{
		if(m_pInit == nullptr)
			Setup(b2DefaultBodyDef());

		const glm::mat4 &mtxSceneRef = m_EntityRef.GetSceneTransform(0.0f);
		glm::vec3 ptTranslation = mtxSceneRef[3];
		glm::vec3 vRotations = glm::eulerAngles(glm::quat_cast(mtxSceneRef));

		m_pInit->position.x = ptTranslation.x * IHyNode::sm_pScene->GetPpmInverse();
		m_pInit->position.y = ptTranslation.y * IHyNode::sm_pScene->GetPpmInverse();
		m_pInit->rotation = b2MakeRot(vRotations.z);

		m_hBody = b2CreateBody(IHyNode::sm_pScene->GetPhysicsWorld(), m_pInit);
		m_EntityRef.SyncPhysicsShapes();

		if(b2Body_IsEnabled(m_hBody) == false)
			b2Body_Enable(m_hBody);
	}
}

void HyPhysicsBody::Deactivate()
{
	if(B2_IS_NON_NULL(m_hBody))
		b2Body_Disable(m_hBody);
}

bool HyPhysicsBody::IsActivated() const
{
	return B2_IS_NON_NULL(m_hBody) && b2Body_IsEnabled(m_hBody);
}

void HyPhysicsBody::Destroy(bool bClearInitCache)
{
	if(B2_IS_NON_NULL(m_hBody) && b2Body_IsValid(m_hBody))
	{
		b2DestroyBody(m_hBody);
		m_hBody = b2_nullBodyId;
	}

	if(bClearInitCache)
	{
		delete m_pInit;
		m_pInit = nullptr;
	}
}
