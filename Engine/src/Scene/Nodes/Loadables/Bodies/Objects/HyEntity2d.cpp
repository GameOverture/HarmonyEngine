/**************************************************************************
 *	HyEntity2d.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Loadables/Bodies/Objects/HyEntity2d.h"
#include "Scene/HyScene.h"
#include "Renderer/Effects/HyStencil.h"
#include "Assets/Nodes/HyEntityData.h"
#include "HyEngine.h"

HyEntity2d::HyEntity2d(HyEntity2d *pParent /*= nullptr*/) :
	IHyBody2d(HYTYPE_Entity, "", "", pParent),
	m_uiEntAttribs(0),
	m_eMouseInputState(MOUSEINPUT_None),
	m_pPhysicsBody(nullptr)
{
}

HyEntity2d::HyEntity2d(HyEntity2d &&donor) noexcept :
	IHyBody2d(std::move(donor)),
	m_ChildList(std::move(donor.m_ChildList)),
	m_uiEntAttribs(std::move(donor.m_uiEntAttribs)),
	m_eMouseInputState(std::move(donor.m_eMouseInputState)),
	m_pPhysicsBody(std::move(donor.m_pPhysicsBody))
{
}

HyEntity2d::~HyEntity2d(void)
{
	while(m_ChildList.empty() == false)
		m_ChildList[m_ChildList.size() - 1]->ParentDetach();

	PhysRelease();
}

HyEntity2d &HyEntity2d::operator=(HyEntity2d &&donor) noexcept
{
	IHyBody2d::operator=(std::move(donor));

	m_ChildList = std::move(donor.m_ChildList);
	m_uiEntAttribs = std::move(donor.m_uiEntAttribs);
	m_eMouseInputState = std::move(donor.m_eMouseInputState);
	m_pPhysicsBody = std::move(m_pPhysicsBody);

	return *this;
}

/*virtual*/ void HyEntity2d::SetVisible(bool bEnabled) /*override*/
{
	SetVisible(bEnabled, false);
}

void HyEntity2d::SetVisible(bool bEnabled, bool bOverrideExplicitChildren)
{
	if(bEnabled)
		m_uiFlags |= SETTING_IsVisible;
	else
		m_uiFlags &= ~SETTING_IsVisible;

	m_uiFlags |= EXPLICIT_Visible;

	for(uint32 i = 0; i < m_ChildList.size(); ++i)
		m_ChildList[i]->_SetVisible(bEnabled, bOverrideExplicitChildren);
}

/*virtual*/ void HyEntity2d::SetPauseUpdate(bool bUpdateWhenPaused) /*override*/
{
	SetPauseUpdate(bUpdateWhenPaused, false);
}

void HyEntity2d::SetPauseUpdate(bool bUpdateWhenPaused, bool bOverrideExplicitChildren)
{
	if(bUpdateWhenPaused)
	{
		if(0 == (m_uiFlags & SETTING_IsPauseUpdate)) // false
			HyScene::AddNode_PauseUpdate(this);
	}
	else
	{
		if(0 != (m_uiFlags & SETTING_IsPauseUpdate)) // true
			HyScene::RemoveNode_PauseUpdate(this);
	}

	if(bUpdateWhenPaused)
		m_uiFlags |= SETTING_IsPauseUpdate;
	else
		m_uiFlags &= ~SETTING_IsPauseUpdate;

	m_uiFlags |= EXPLICIT_PauseUpdate;

	for(uint32 i = 0; i < m_ChildList.size(); ++i)
		m_ChildList[i]->_SetPauseUpdate(IsPauseUpdate(), bOverrideExplicitChildren);
}

/*virtual*/ void HyEntity2d::SetScissor(int32 uiLocalX, int32 uiLocalY, uint32 uiWidth, uint32 uiHeight) /*override*/
{
	SetScissor(uiLocalX, uiLocalY, uiWidth, uiHeight, false);
}

void HyEntity2d::SetScissor(int32 uiLocalX, int32 uiLocalY, uint32 uiWidth, uint32 uiHeight, bool bOverrideExplicitChildren)
{
	IHyBody2d::SetScissor(uiLocalX, uiLocalY, uiWidth, uiHeight);

	for(uint32 i = 0; i < m_ChildList.size(); ++i)
	{
		if(0 != (m_ChildList[i]->m_uiFlags & NODETYPE_IsBody))
			static_cast<IHyBody2d *>(m_ChildList[i])->_SetScissor(m_pScissor, bOverrideExplicitChildren);
	}
}

/*virtual*/ void HyEntity2d::ClearScissor(bool bUseParentScissor) /*override*/
{
	ClearScissor(bUseParentScissor, false);
}

void HyEntity2d::ClearScissor(bool bUseParentScissor, bool bOverrideExplicitChildren)
{
	IHyBody2d::ClearScissor(bUseParentScissor);

	for(uint32 i = 0; i < m_ChildList.size(); ++i)
	{
		if(0 != (m_ChildList[i]->m_uiFlags & NODETYPE_IsBody))
			static_cast<IHyBody2d *>(m_ChildList[i])->_SetScissor(m_pScissor, bOverrideExplicitChildren);
	}
}

/*virtual*/ void HyEntity2d::SetStencil(HyStencil *pStencil) /*override*/
{
	SetStencil(pStencil, false);
}

void HyEntity2d::SetStencil(HyStencil *pStencil, bool bOverrideExplicitChildren)
{
	IHyBody2d::SetStencil(pStencil);

	for(uint32 i = 0; i < m_ChildList.size(); ++i)
	{
		if(0 != (m_ChildList[i]->m_uiFlags & NODETYPE_IsBody))
			static_cast<IHyBody2d *>(m_ChildList[i])->_SetStencil(m_hStencil, bOverrideExplicitChildren);
	}
}

/*virtual*/ void HyEntity2d::ClearStencil(bool bUseParentStencil) /*override*/
{
	ClearStencil(bUseParentStencil, false);
}

void HyEntity2d::ClearStencil(bool bUseParentStencil, bool bOverrideExplicitChildren)
{
	IHyBody2d::ClearStencil(bUseParentStencil);

	for(uint32 i = 0; i < m_ChildList.size(); ++i)
	{
		if(0 != (m_ChildList[i]->m_uiFlags & NODETYPE_IsBody))
			static_cast<IHyBody2d *>(m_ChildList[i])->_SetStencil(m_hStencil, bOverrideExplicitChildren);
	}
}

/*virtual*/ void HyEntity2d::UseCameraCoordinates() /*override*/
{
	UseCameraCoordinates(false);
}

void HyEntity2d::UseCameraCoordinates(bool bOverrideExplicitChildren)
{
	IHyBody2d::UseCameraCoordinates();

	for(uint32 i = 0; i < m_ChildList.size(); ++i)
	{
		if(0 != (m_ChildList[i]->m_uiFlags & NODETYPE_IsBody))
			static_cast<IHyBody2d *>(m_ChildList[i])->_SetCoordinateSystem(m_iCoordinateSystem, bOverrideExplicitChildren);
	}
}

/*virtual*/ void HyEntity2d::UseWindowCoordinates(int32 iWindowIndex) /*override*/
{
	UseWindowCoordinates(iWindowIndex, false);
}

void HyEntity2d::UseWindowCoordinates(int32 iWindowIndex, bool bOverrideExplicitChildren)
{
	IHyBody2d::UseWindowCoordinates(iWindowIndex);

	for(uint32 i = 0; i < m_ChildList.size(); ++i)
	{
		if(0 != (m_ChildList[i]->m_uiFlags & NODETYPE_IsBody))
			static_cast<IHyBody2d *>(m_ChildList[i])->_SetCoordinateSystem(iWindowIndex, bOverrideExplicitChildren);
	}
}

/*virtual*/ void HyEntity2d::SetDisplayOrder(int32 iOrderValue) /*override*/
{
	SetDisplayOrder(iOrderValue, false);
}

void HyEntity2d::SetDisplayOrder(int32 iOrderValue, bool bOverrideExplicitChildren)
{
	IHyBody2d::SetDisplayOrder(iOrderValue);
	SetChildrenDisplayOrder(bOverrideExplicitChildren);
}

/*virtual*/ void HyEntity2d::ResetDisplayOrder() /*override*/
{
	IHyBody2d::ResetDisplayOrder();
	for(uint32 i = 0; i < m_ChildList.size(); ++i)
	{
		if(0 != (m_ChildList[i]->m_uiFlags & NODETYPE_IsBody))
			static_cast<IHyBody2d *>(m_ChildList[i])->ResetDisplayOrder();
	}
}

/*virtual*/ const b2AABB &HyEntity2d::GetSceneAABB() /*override*/
{
	HyMath::InvalidateAABB(m_SceneAABB);
	for(uint32 i = 0; i < m_ChildList.size(); ++i)
	{
		if((0 != (m_ChildList[i]->m_uiFlags & NODETYPE_IsDrawable) &&
			static_cast<IHyDrawable2d *>(m_ChildList[i])->IsLoadDataValid() == false) ||
			m_ChildList[i]->GetSceneAABB().IsValid() == false)
		{
			continue;
		}

		if(m_SceneAABB.IsValid() == false)
			m_SceneAABB = m_ChildList[i]->GetSceneAABB();
		else
			m_SceneAABB.Combine(m_ChildList[i]->GetSceneAABB());
	}

	return m_SceneAABB;
}

void HyEntity2d::ChildAppend(IHyNode2d &childRef)
{
	HyAssert(&childRef != this, "HyEntity2d::ChildAppend was passed a child that was itself!");

	childRef.ParentDetach();
	childRef.m_pParent = this;

	m_ChildList.push_back(&childRef);
	SetNewChildAttributes(childRef);
}

/*virtual*/ bool HyEntity2d::ChildInsert(IHyNode2d &insertBefore, IHyNode2d &childRef)
{
	for(auto iter = m_ChildList.begin(); iter != m_ChildList.end(); ++iter)
	{
		if((*iter) == &insertBefore || 
		   ((*iter)->GetType() == HYTYPE_Entity && static_cast<HyEntity2d *>(*iter)->ChildExists(insertBefore)))
		{
			childRef.ParentDetach();
			childRef.m_pParent = this;

			m_ChildList.insert(iter, &childRef);
			SetNewChildAttributes(childRef);

			return true;
		}
	}

	HyLogWarning("HyEntity2d::ChildInsert failed - could not find 'insertBefore' node");
	return false;
}

bool HyEntity2d::ChildExists(IHyNode2d &childRef)
{
	for(auto iter = m_ChildList.begin(); iter != m_ChildList.end(); ++iter)
	{
		if((*iter) == &childRef ||
		   ((*iter)->GetType() == HYTYPE_Entity && static_cast<HyEntity2d *>(*iter)->ChildExists(childRef)))
		{
			return true;
		}
	}

	return false;
}

/*virtual*/ bool HyEntity2d::ChildRemove(IHyNode2d *pChild)
{
	for(auto iter = m_ChildList.begin(); iter != m_ChildList.end(); ++iter)
	{
		if(*iter == pChild)
		{
			(*iter)->m_pParent = nullptr;
			m_ChildList.erase(iter);

			if(sm_pHyAssets)
				sm_pHyAssets->SetEntityLoaded(this);

			return true;
		}
	}

	return false;
}

/*virtual*/ void HyEntity2d::ChildrenTransfer(HyEntity2d &newParent)
{
	while(m_ChildList.empty() == false)
		newParent.ChildAppend(*m_ChildList[0]);

	if(sm_pHyAssets)
		sm_pHyAssets->SetEntityLoaded(this);
}

/*virtual*/ uint32 HyEntity2d::ChildCount()
{
	return static_cast<uint32>(m_ChildList.size());
}

/*virtual*/ IHyNode2d *HyEntity2d::ChildGet(uint32 uiIndex)
{
	HyAssert(uiIndex < static_cast<uint32>(m_ChildList.size()), "HyEntity2d::ChildGet passed an invalid index");
	return m_ChildList[uiIndex];
}

void HyEntity2d::ForEachChild(std::function<void(IHyNode2d *)> func)
{
	func(this);

	for(uint32 i = 0; i < m_ChildList.size(); ++i)
	{
		if(m_ChildList[i]->GetType() == HYTYPE_Entity)
			static_cast<HyEntity2d *>(m_ChildList[i])->ForEachChild(func);
	}
}

std::vector<IHyNode2d *> HyEntity2d::FindChildren(std::function<bool(IHyNode2d *)> func)
{
	std::vector<IHyNode2d *> foundList;
	std::copy_if(m_ChildList.begin(), m_ChildList.end(), std::back_inserter(foundList), func);

	return foundList;
}

void HyEntity2d::EnableMouseInput()
{
	m_uiEntAttribs |= ENT2DATTRIB_MouseInput;
}

void HyEntity2d::DisableMouseInput()
{
	m_uiEntAttribs &= ~ENT2DATTRIB_MouseInput;
}

bool HyEntity2d::IsMouseInBounds()
{
	if(GetCoordinateSystem() >= 0)
	{
		return HyEngine::Input().GetMouseWindowIndex() == GetCoordinateSystem() && HyTestPointAABB(GetSceneAABB(), HyEngine::Input().GetMousePos());
	}
	else
	{
		glm::vec2 ptWorldMousePos;
		if(HyEngine::Input().GetWorldMousePos(ptWorldMousePos))
			return HyTestPointAABB(GetSceneAABB(), ptWorldMousePos);
	}

	return false;
}

void HyEntity2d::PhysInit(HyPhysicsGrid2d& physGridRef,
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
	bodyDef.userData = this;
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

void HyEntity2d::PhysInit(HyPhysicsGrid2d& physGridRef,
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
	bodyDef.userData = this;
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

HyPhysicsType HyEntity2d::PhysGetType() const
{
	return m_pPhysicsBody ? static_cast<HyPhysicsType>(m_pPhysicsBody->GetType()) : HYPHYS_Unknown;
}

void HyEntity2d::PhysSetType(HyPhysicsType eType)
{
	if(m_pPhysicsBody)
		m_pPhysicsBody->SetType(static_cast<b2BodyType>(eType)); // static_assert guarantees this to match
}

bool HyEntity2d::PhysIsEnabled() const
{
	return m_pPhysicsBody && m_pPhysicsBody->IsEnabled();
}

void HyEntity2d::PhysSetEnabled(bool bEnable)
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

bool HyEntity2d::PhysIsFixedRotation() const
{
	return m_pPhysicsBody && m_pPhysicsBody->IsFixedRotation();
}

void HyEntity2d::PhysSetFixedRotation(bool bFixedRot)
{
	if(m_pPhysicsBody)
		m_pPhysicsBody->SetFixedRotation(bFixedRot);
}

bool HyEntity2d::PhysIsCcd() const
{
	return m_pPhysicsBody && m_pPhysicsBody->IsBullet();
}

void HyEntity2d::PhysSetCcd(bool bContinuousCollisionDetection)
{
	if(m_pPhysicsBody)
		m_pPhysicsBody->SetBullet(bContinuousCollisionDetection);
}

bool HyEntity2d::PhysIsAwake() const
{
	return m_pPhysicsBody && m_pPhysicsBody->IsAwake();
}

void HyEntity2d::PhysSetAwake(bool bAwake)
{
	if(m_pPhysicsBody)
		m_pPhysicsBody->SetAwake(bAwake);
}

bool HyEntity2d::PhysIsSleepingAllowed() const
{
	return m_pPhysicsBody && m_pPhysicsBody->IsSleepingAllowed();
}

void HyEntity2d::PhysSetSleepingAllowed(bool bAllowSleep)
{
	if(m_pPhysicsBody)
		m_pPhysicsBody->SetSleepingAllowed(bAllowSleep);
}

float HyEntity2d::PhysGetGravityScale() const
{
	return m_pPhysicsBody ? m_pPhysicsBody->GetGravityScale() : 0.0f;
}

void HyEntity2d::PhysSetGravityScale(float fGravityScale)
{
	if(m_pPhysicsBody)
		m_pPhysicsBody->SetGravityScale(fGravityScale);
}

glm::vec2 HyEntity2d::PhysWorldCenterMass() const
{
	if(m_pPhysicsBody)
		return glm::vec2(m_pPhysicsBody->GetWorldCenter().x, m_pPhysicsBody->GetWorldCenter().y);

	return glm::vec2();
}

glm::vec2 HyEntity2d::PhysLocalCenterMass() const
{
	if(m_pPhysicsBody)
		return glm::vec2(m_pPhysicsBody->GetLocalCenter().x, m_pPhysicsBody->GetLocalCenter().y);

	return glm::vec2();
}

glm::vec2 HyEntity2d::PhysGetLinearVelocity() const
{
	if(m_pPhysicsBody && m_pPhysicsBody->IsEnabled())
		return glm::vec2(m_pPhysicsBody->GetLinearVelocity().x, m_pPhysicsBody->GetLinearVelocity().y);

	return glm::vec2();
}

void HyEntity2d::PhysSetLinearVelocity(glm::vec2 vVelocity)
{
	if(m_pPhysicsBody)
		m_pPhysicsBody->SetLinearVelocity(b2Vec2(vVelocity.x, vVelocity.y));
}

float HyEntity2d::PhysGetAngularVelocity() const
{
	if(m_pPhysicsBody)
		return m_pPhysicsBody->GetAngularVelocity();

	return 0.0f;
}

void HyEntity2d::PhysSetAngularVelocity(float fOmega)
{
	if(m_pPhysicsBody)
		m_pPhysicsBody->SetAngularVelocity(fOmega);
}

void HyEntity2d::PhysApplyForce(const glm::vec2 &vForce, const glm::vec2 &ptPoint, bool bWake)
{
	if(m_pPhysicsBody)
		m_pPhysicsBody->ApplyForce(b2Vec2(vForce.x, vForce.y), b2Vec2(ptPoint.x, ptPoint.y), bWake);
}

void HyEntity2d::PhysApplyForceToCenter(const glm::vec2 &vForce, bool bWake)
{
	if(m_pPhysicsBody)
		m_pPhysicsBody->ApplyForceToCenter(b2Vec2(vForce.x, vForce.y), bWake);
}

void HyEntity2d::PhysApplyTorque(float fTorque, bool bWake)
{
	if(m_pPhysicsBody)
		m_pPhysicsBody->ApplyTorque(fTorque, bWake);
}

void HyEntity2d::PhysApplyLinearImpulse(const glm::vec2 &vImpulse, const glm::vec2 &ptPoint, bool bWake)
{
	if(m_pPhysicsBody)
		m_pPhysicsBody->ApplyLinearImpulse(b2Vec2(vImpulse.x, vImpulse.y), b2Vec2(ptPoint.x, ptPoint.y), bWake);
}

void HyEntity2d::PhysApplyLinearImpulseToCenter(const glm::vec2 &vImpulse, bool bWake)
{
	if(m_pPhysicsBody)
		m_pPhysicsBody->ApplyLinearImpulseToCenter(b2Vec2(vImpulse.x, vImpulse.y), bWake);
}

void HyEntity2d::PhysApplyAngularImpulse(float fImpulse, bool bWake)
{
	if(m_pPhysicsBody)
		m_pPhysicsBody->ApplyAngularImpulse(fImpulse, bWake);
}

float HyEntity2d::PhysGetMass() const
{
	if(m_pPhysicsBody)
		return m_pPhysicsBody->GetMass();

	return 0.0f;
}

std::unique_ptr<HyPhysicsCollider> HyEntity2d::PhysAddCollider(const HyShape2d &shapeRef, float fDensity, float fFriction, float fRestitution, bool bIsSensor, b2Filter collideFilter)
{
	if(m_pPhysicsBody == nullptr || shapeRef.IsValidShape() == false)
		return nullptr;

	b2Shape *pPpmShape = shapeRef.ClonePpmShape(static_cast<HyPhysicsGrid2d *>(m_pPhysicsBody->GetWorld())->GetPpmInverse());
	std::unique_ptr<HyPhysicsCollider> pCollider = std::make_unique<HyPhysicsCollider>(m_pPhysicsBody, pPpmShape, fDensity, fFriction, fRestitution, bIsSensor, collideFilter);
	delete pPpmShape;

	return pCollider;
}

std::unique_ptr<HyPhysicsCollider> HyEntity2d::PhysAddCircleCollider(float fRadius, float fDensity, float fFriction, float fRestitution, bool bIsSensor, b2Filter collideFilter)
{
	return PhysAddCircleCollider(glm::vec2(0.0f, 0.0), fRadius, fDensity, fFriction, fRestitution, bIsSensor, collideFilter);
}

std::unique_ptr<HyPhysicsCollider> HyEntity2d::PhysAddCircleCollider(const glm::vec2 &ptCenter, float fRadius, float fDensity, float fFriction, float fRestitution, bool bIsSensor, b2Filter collideFilter)
{
	if(m_pPhysicsBody == nullptr || fRadius <= 0.0f)
		return nullptr;

	float fPpmInverse = static_cast<HyPhysicsGrid2d *>(m_pPhysicsBody->GetWorld())->GetPpmInverse();
	b2CircleShape circleShape;
	circleShape.m_p.x = ptCenter.x * fPpmInverse;
	circleShape.m_p.y = ptCenter.y * fPpmInverse;
	circleShape.m_radius = fRadius * fPpmInverse;
	return std::make_unique<HyPhysicsCollider>(m_pPhysicsBody, &circleShape, fDensity, fFriction, fRestitution, bIsSensor, collideFilter);
}

std::unique_ptr<HyPhysicsCollider> HyEntity2d::PhysAddLineChainCollider(const glm::vec2 *pVerts, uint32 uiNumVerts, float fDensity, float fFriction, float fRestitution, bool bIsSensor, b2Filter collideFilter)
{
	if(m_pPhysicsBody == nullptr || pVerts == nullptr || uiNumVerts == 0)
		return nullptr;

	float fPpmInverse = static_cast<HyPhysicsGrid2d *>(m_pPhysicsBody->GetWorld())->GetPpmInverse();
	std::vector<b2Vec2> vertList;
	for(uint32 i = 0; i < uiNumVerts; ++i)
		vertList.emplace_back(pVerts[i].x * fPpmInverse, pVerts[i].y * fPpmInverse);

	b2ChainShape chainShape;
	chainShape.CreateChain(vertList.data(), uiNumVerts);
	return std::make_unique<HyPhysicsCollider>(m_pPhysicsBody, &chainShape, fDensity, fFriction, fRestitution, bIsSensor, collideFilter);
}

void HyEntity2d::PhysDestroyCollider(std::unique_ptr<HyPhysicsCollider> pCollider)
{
	if(m_pPhysicsBody)
		m_pPhysicsBody->DestroyFixture(pCollider->GetFixture());

	pCollider.release();
}

float HyEntity2d::PhysGetInertia() const
{
	if(m_pPhysicsBody)
		return m_pPhysicsBody->GetInertia();

	return 0.0f;
}

void HyEntity2d::PhysRelease()
{
	if(m_pPhysicsBody)
	{
		m_pPhysicsBody->GetWorld()->DestroyBody(m_pPhysicsBody);
		m_pPhysicsBody = nullptr;
	}
}

bool HyEntity2d::IsReverseDisplayOrder() const
{
	return (m_uiEntAttribs & ENT2DATTRIB_ReverseDisplayOrder);
}

void HyEntity2d::ReverseDisplayOrder(bool bReverse)
{
	if(bReverse)
		m_uiEntAttribs |= ENT2DATTRIB_ReverseDisplayOrder;
	else
		m_uiEntAttribs &= ~ENT2DATTRIB_ReverseDisplayOrder;

	SetDisplayOrder(m_iDisplayOrder, false);
}

int32 HyEntity2d::SetChildrenDisplayOrder(bool bOverrideExplicitChildren)
{
	int32 iOrderValue = m_iDisplayOrder + 1;

	if((m_uiEntAttribs & ENT2DATTRIB_ReverseDisplayOrder) == 0)
	{
		for(uint32 i = 0; i < m_ChildList.size(); ++i)
		{
			if(0 != (m_ChildList[i]->m_uiFlags & NODETYPE_IsBody))
				iOrderValue = static_cast<IHyBody2d *>(m_ChildList[i])->_SetDisplayOrder(iOrderValue, bOverrideExplicitChildren);
		}
	}
	else
	{
		for(int32 i = static_cast<int32>(m_ChildList.size()) - 1; i >= 0; --i)
		{
			if(0 != (m_ChildList[i]->m_uiFlags & NODETYPE_IsBody))
				iOrderValue = static_cast<IHyBody2d *>(m_ChildList[i])->_SetDisplayOrder(iOrderValue, bOverrideExplicitChildren);
		}
	}

	return iOrderValue;
}

/*virtual*/ void HyEntity2d::Load() /*override*/
{
	// Load any attached children
	for(uint32 i = 0; i < m_ChildList.size(); ++i)
	{
		if(0 != (m_ChildList[i]->m_uiFlags & NODETYPE_IsLoadable))
			static_cast<IHyLoadable2d *>(m_ChildList[i])->Load();
	}

	if(sm_pHyAssets)
		sm_pHyAssets->SetEntityLoaded(this);
}

/*virtual*/ void HyEntity2d::Unload() /*override*/
{
	// Unload any attached children
	for(uint32 i = 0; i < m_ChildList.size(); ++i)
	{
		if(0 != (m_ChildList[i]->m_uiFlags & NODETYPE_IsLoadable))
			static_cast<IHyLoadable2d *>(m_ChildList[i])->Unload();
	}

	if(sm_pHyAssets)
		sm_pHyAssets->SetEntityLoaded(this);
}

/*virtual*/ void HyEntity2d::Update() /*override final*/
{
	IHyBody2d::Update();

	if((m_uiEntAttribs & ENT2DATTRIB_MouseInput) != 0)
	{
		bool bMouseInBounds = IsMouseInBounds();
		bool bLeftClickDown = HyEngine::Input().IsMouseBtnDown(HYMOUSE_BtnLeft);

		switch(m_eMouseInputState)
		{
		case MOUSEINPUT_None:
			if(bMouseInBounds)
			{
				m_eMouseInputState = MOUSEINPUT_Hover;
				OnMouseEnter();

				if(bLeftClickDown)
				{
					m_eMouseInputState = MOUSEINPUT_Down;
					OnMouseDown();
				}
			}
			break;

		case MOUSEINPUT_Hover:
			if(bMouseInBounds == false)
			{
				m_eMouseInputState = MOUSEINPUT_None;
				OnMouseLeave();
			}
			else if(bLeftClickDown)
			{
				m_eMouseInputState = MOUSEINPUT_Down;
				OnMouseDown();
			}
			break;

		case MOUSEINPUT_Down:
			if(bMouseInBounds == false)
			{
				m_eMouseInputState = MOUSEINPUT_None;
				OnMouseLeave();
			}
			else if(bLeftClickDown == false)
			{
				m_eMouseInputState = MOUSEINPUT_Hover;
				OnMouseClicked();
			}
			break;
		}
	}

	if(m_pPhysicsBody && m_pPhysicsBody->IsEnabled())
	{
		if(pos.IsAnimating() == false)
		{
			pos.GetAnimFloat(0).Updater([&](float fUnused) { return (m_pPhysicsBody->GetPosition().x * static_cast<HyPhysicsGrid2d *>(m_pPhysicsBody->GetWorld())->GetPixelsPerMeter()); });
			pos.GetAnimFloat(1).Updater([&](float fUnused) { return (m_pPhysicsBody->GetPosition().y * static_cast<HyPhysicsGrid2d *>(m_pPhysicsBody->GetWorld())->GetPixelsPerMeter()); });
		}

		if(rot.IsAnimating() == false)
			rot.Updater([&](float fUnused) { return glm::degrees(m_pPhysicsBody->GetAngle()); });
	}

	OnUpdate();
}

/*virtual*/ bool HyEntity2d::IsChildrenLoaded() const /*override final*/
{
	for(uint32 i = 0; i < m_ChildList.size(); ++i)
	{
		if(0 != (m_ChildList[i]->m_uiFlags & NODETYPE_IsLoadable))
		{
			if(static_cast<IHyLoadable2d *>(m_ChildList[i])->IsLoadDataValid() != false && static_cast<IHyLoadable2d *>(m_ChildList[i])->IsLoaded() == false)
				return false;
		}
	}

	return true;
}

/*virtual*/ void HyEntity2d::OnDataAcquired() /*override*/
{
	const HyEntityData *pData = static_cast<const HyEntityData *>(UncheckedGetData());
	//pData->
}

void HyEntity2d::SetNewChildAttributes(IHyNode2d &childRef)
{
	SetDirty(DIRTY_ALL);
	childRef._SetVisible(IsVisible(), false);
	childRef._SetPauseUpdate(IsPauseUpdate(), false);

	if(childRef.GetInternalFlags() & NODETYPE_IsBody)
	{
		static_cast<IHyBody2d &>(childRef)._SetCoordinateSystem(GetCoordinateSystem(), false);

		if(IsScissorSet())
			static_cast<IHyBody2d &>(childRef)._SetScissor(m_pScissor, false);

		if(IsStencilSet())
			static_cast<IHyBody2d &>(childRef)._SetStencil(m_hStencil, false);

		SetChildrenDisplayOrder(false);
	}

	if(sm_pHyAssets)
		sm_pHyAssets->SetEntityLoaded(this);
}

/*virtual*/ void HyEntity2d::SetDirty(uint32 uiDirtyFlags)
{
	IHyNode2d::SetDirty(uiDirtyFlags);

	if(m_pPhysicsBody && (uiDirtyFlags & IHyNode::DIRTY_FromUpdater) == 0)
	{
		float fPpmInverse = static_cast<HyPhysicsGrid2d *>(m_pPhysicsBody->GetWorld())->GetPpmInverse();

		uint32 uiTransformFlags = (uiDirtyFlags & (DIRTY_Position | DIRTY_Rotation));
		if((DIRTY_Position | DIRTY_Rotation) == uiTransformFlags)
			m_pPhysicsBody->SetTransform(b2Vec2(pos.X() * fPpmInverse, pos.Y() * fPpmInverse), glm::radians(rot.Get()));
		else if(DIRTY_Position == uiTransformFlags)
			m_pPhysicsBody->SetTransform(b2Vec2(pos.X() * fPpmInverse, pos.Y() * fPpmInverse), m_pPhysicsBody->GetAngle());
		else if(DIRTY_Rotation == uiTransformFlags)
			m_pPhysicsBody->SetTransform(m_pPhysicsBody->GetPosition(), glm::radians(rot.Get()));

		ClearDirty(IHyNode::DIRTY_FromUpdater);
	}

	for(uint32 i = 0; i < m_ChildList.size(); ++i)
		m_ChildList[i]->SetDirty(uiDirtyFlags);
}

/*virtual*/ void HyEntity2d::_SetVisible(bool bEnabled, bool bIsOverriding) /*override final*/
{
	IHyNode::_SetVisible(bEnabled, bIsOverriding);

	if(0 == (m_uiFlags & EXPLICIT_Visible))
	{
		for(uint32 i = 0; i < m_ChildList.size(); ++i)
			m_ChildList[i]->_SetVisible(IsVisible(), bIsOverriding);
	}
}

/*virtual*/ void HyEntity2d::_SetPauseUpdate(bool bUpdateWhenPaused, bool bIsOverriding) /*override final*/
{
	IHyNode::_SetPauseUpdate(bUpdateWhenPaused, bIsOverriding);

	if(0 == (m_uiFlags & EXPLICIT_PauseUpdate))
	{
		for(uint32 i = 0; i < m_ChildList.size(); ++i)
			m_ChildList[i]->_SetPauseUpdate(IsPauseUpdate(), bIsOverriding);
	}
}

/*virtual*/ void HyEntity2d::_SetScissor(const ScissorRect *pParentScissor, bool bIsOverriding) /*override final*/
{
	IHyBody2d::_SetScissor(pParentScissor, bIsOverriding);
	
	if(0 == (m_uiFlags & EXPLICIT_Scissor))
	{
		for(uint32 i = 0; i < m_ChildList.size(); ++i)
		{
			if(0 != (m_ChildList[i]->m_uiFlags & NODETYPE_IsBody))
				static_cast<IHyBody2d *>(m_ChildList[i])->_SetScissor(m_pScissor, bIsOverriding);
		}
	}
}

/*virtual*/ void HyEntity2d::_SetStencil(HyStencilHandle hHandle, bool bIsOverriding) /*override final*/
{
	IHyBody2d::_SetStencil(hHandle, bIsOverriding);

	if(0 == (m_uiFlags & EXPLICIT_Stencil))
	{
		for(uint32 i = 0; i < m_ChildList.size(); ++i)
		{
			if(0 != (m_ChildList[i]->m_uiFlags & NODETYPE_IsBody))
				static_cast<IHyBody2d *>(m_ChildList[i])->_SetStencil(m_hStencil, bIsOverriding);
		}
	}
}

/*virtual*/ void HyEntity2d::_SetCoordinateSystem(int32 iWindowIndex, bool bIsOverriding) /*override final*/
{
	IHyBody2d::_SetCoordinateSystem(iWindowIndex, bIsOverriding);

	if(0 == (m_uiFlags & EXPLICIT_CoordinateSystem))
	{
		for(uint32 i = 0; i < m_ChildList.size(); ++i)
		{
			if(0 != (m_ChildList[i]->m_uiFlags & NODETYPE_IsBody))
				static_cast<IHyBody2d *>(m_ChildList[i])->_SetCoordinateSystem(iWindowIndex, bIsOverriding);
		}
	}
}

/*virtual*/ int32 HyEntity2d::_SetDisplayOrder(int32 iOrderValue, bool bIsOverriding) /*override final*/
{
	IHyBody2d::_SetDisplayOrder(iOrderValue, bIsOverriding);

	if(0 == (m_uiFlags & EXPLICIT_DisplayOrder))
		iOrderValue = SetChildrenDisplayOrder(bIsOverriding);

	return iOrderValue;
}

/*friend*/ void HyNodeCtorAppend(HyEntity2d *pEntity, IHyNode2d *pChildNode)
{
	pEntity->m_ChildList.push_back(pChildNode);
	pEntity->SetDirty(HyEntity2d::DIRTY_ALL);
}
