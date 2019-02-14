/**************************************************************************
*	IHyNode2d.cpp
*
*	Harmony Engine
*	Copyright (c) 2016 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/IHyNode2d.h"
#include "Scene/Nodes/Loadables/Visables/Objects/HyEntity2d.h"
#include "HyEngine.h"

IHyNode2d::IHyNode2d(HyType eNodeType, HyEntity2d *pParent) :	IHyNode(eNodeType),
																m_pParent(pParent),
																m_fRotation(0.0f),
																m_pPhysicsBody(nullptr),
																pos(*this, DIRTY_Transform | DIRTY_Scissor | DIRTY_WorldAABB),
																rot(m_fRotation, *this, DIRTY_Transform | DIRTY_Scissor | DIRTY_WorldAABB),
																rot_pivot(*this, DIRTY_Transform | DIRTY_Scissor | DIRTY_WorldAABB),
																scale(*this, DIRTY_Transform | DIRTY_Scissor | DIRTY_WorldAABB),
																scale_pivot(*this, DIRTY_Transform | DIRTY_Scissor | DIRTY_WorldAABB)
{
	m_uiExplicitAndTypeFlags |= NODETYPE_Is2d;
	scale.Set(1.0f);

	if(m_pParent)
		_CtorSetupNewChild(*m_pParent, *this);
}

IHyNode2d::IHyNode2d(const IHyNode2d &copyRef) :	IHyNode(copyRef),
													m_pParent(copyRef.m_pParent),
													m_mtxCached(copyRef.m_mtxCached),
													m_fRotation(copyRef.m_fRotation),
													m_pPhysicsBody(nullptr),
													pos(*this, DIRTY_Transform | DIRTY_Scissor | DIRTY_WorldAABB),
													rot(m_fRotation, *this, DIRTY_Transform | DIRTY_Scissor | DIRTY_WorldAABB),
													rot_pivot(*this, DIRTY_Transform | DIRTY_Scissor | DIRTY_WorldAABB),
													scale(*this, DIRTY_Transform | DIRTY_Scissor | DIRTY_WorldAABB),
													scale_pivot(*this, DIRTY_Transform | DIRTY_Scissor | DIRTY_WorldAABB),
													m_aabbCached(copyRef.m_aabbCached)
{
	if(copyRef.m_pParent)
		copyRef.m_pParent->ChildAppend(*this);

	pos.Set(copyRef.pos.Get());
	rot.Set(copyRef.rot.Get());
	rot_pivot.Set(copyRef.rot_pivot.Get());
	scale.Set(copyRef.scale.Get());
	scale_pivot.Set(copyRef.scale_pivot.Get());

	if(copyRef.m_pPhysicsBody)
	{
		b2BodyDef def;
		copyRef.PhysicsBodyDef(def);
		PhysicsInit(def);
	}
}

/*virtual*/ IHyNode2d::~IHyNode2d()
{
	if(m_pPhysicsBody)
	{
		Hy_Physics2d().DestroyBody(m_pPhysicsBody);
		m_pPhysicsBody = nullptr;
	}

	ParentDetach();
}

const IHyNode2d &IHyNode2d::operator=(const IHyNode2d &rhs)
{
	IHyNode::operator=(rhs);

	// Copying the parent is not done by design
	//if(rhs.m_pParent)
	//	rhs.m_pParent->ChildAppend(*this);

	m_mtxCached = rhs.m_mtxCached;
	m_fRotation = rhs.m_fRotation;
	
	if(rhs.m_pPhysicsBody)
	{
		b2BodyDef def;
		rhs.PhysicsBodyDef(def);
		PhysicsInit(def);
	}

	pos.Set(rhs.pos.Get());
	rot.Set(rhs.rot.Get());
	rot_pivot.Set(rhs.rot_pivot.Get());
	scale.Set(rhs.scale.Get());
	scale_pivot.Set(rhs.scale_pivot.Get());

	return *this;
}

void IHyNode2d::ParentDetach()
{
	if(m_pParent == nullptr)
		return;

	m_pParent->ChildRemove(this);
}

HyEntity2d *IHyNode2d::ParentGet() const
{
	return m_pParent;
}

void IHyNode2d::GetLocalTransform(glm::mat4 &outMtx) const
{
	outMtx = glm::mat4(1.0f);

	glm::vec3 ptPos = pos.Extrapolate();
	glm::vec3 vScale = scale.Extrapolate();
	vScale.z = 1.0f;
	glm::vec3 ptRotPivot = rot_pivot.Extrapolate();
	glm::vec3 ptScalePivot = scale_pivot.Extrapolate();
	
	outMtx = glm::translate(outMtx, ptPos);

	outMtx = glm::translate(outMtx, ptRotPivot);
	outMtx = glm::rotate(outMtx, glm::radians(rot.Get()), glm::vec3(0, 0, 1));
	outMtx = glm::translate(outMtx, ptRotPivot * -1.0f);

	outMtx = glm::translate(outMtx, ptScalePivot);
	outMtx = glm::scale(outMtx, vScale);
	outMtx = glm::translate(outMtx, ptScalePivot * -1.0f);
}

const glm::mat4 &IHyNode2d::GetWorldTransform()
{
	if(IsDirty(DIRTY_Transform))
	{
		if(m_pParent)
		{
			m_mtxCached = m_pParent->GetWorldTransform();

			glm::mat4 mtxLocal;
			GetLocalTransform(mtxLocal);

			m_mtxCached *= mtxLocal;
		}
		else
			GetLocalTransform(m_mtxCached);

		ClearDirty(DIRTY_Transform);
	}

	return m_mtxCached;
}

void IHyNode2d::PhysicsInit(b2BodyDef &bodyDefOut)
{
	b2World &worldRef = Hy_Physics2d();

	if(m_pPhysicsBody)
		worldRef.DestroyBody(m_pPhysicsBody);
	
	bodyDefOut.userData = this;
	bodyDefOut.position.Set(pos.X(), pos.Y());
	bodyDefOut.angle = rot.Get();

	m_pPhysicsBody = worldRef.CreateBody(&bodyDefOut);
}

b2Body *IHyNode2d::PhysicsBody()
{
	return m_pPhysicsBody;
}

void IHyNode2d::PhysicsBodyDef(b2BodyDef &defRefOut) const
{
	if(m_pPhysicsBody == nullptr)
		return;

	defRefOut.userData = m_pPhysicsBody->GetUserData();
	defRefOut.type = m_pPhysicsBody->GetType();
	defRefOut.position = m_pPhysicsBody->GetPosition();
	defRefOut.angle = m_pPhysicsBody->GetAngle();
	defRefOut.linearVelocity = m_pPhysicsBody->GetLinearVelocity();
	defRefOut.angularVelocity = m_pPhysicsBody->GetAngularVelocity();
	defRefOut.linearDamping = m_pPhysicsBody->GetLinearDamping();
	defRefOut.angularDamping = m_pPhysicsBody->GetAngularDamping();
	defRefOut.allowSleep = m_pPhysicsBody->IsSleepingAllowed();
	defRefOut.awake = m_pPhysicsBody->IsAwake();
	defRefOut.fixedRotation = m_pPhysicsBody->IsFixedRotation();
	defRefOut.bullet = m_pPhysicsBody->IsBullet();
	defRefOut.active = m_pPhysicsBody->IsActive();
	defRefOut.gravityScale = m_pPhysicsBody->GetGravityScale();
}

/*virtual*/ const b2AABB &IHyNode2d::GetWorldAABB()
{
	return m_aabbCached;
}

/*virtual*/ void IHyNode2d::Update() /*override*/
{
	IHyNode::Update();

	if(m_pPhysicsBody != nullptr && m_pPhysicsBody->IsActive())
	{
		pos.Set(m_pPhysicsBody->GetPosition().x, m_pPhysicsBody->GetPosition().y);
		rot.Set(glm::degrees(m_pPhysicsBody->GetAngle()));
	}
}

/*friend*/ void _CtorSetupNewChild(HyEntity2d &parentRef, IHyNode2d &childRef)
{
	_CtorChildAppend(parentRef, childRef);

	childRef._SetVisible(parentRef.IsVisible(), false);
	childRef._SetPauseUpdate(parentRef.IsPauseUpdate(), false);
}
