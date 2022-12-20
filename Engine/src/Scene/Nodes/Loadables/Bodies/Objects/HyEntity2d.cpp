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
#include "HyEngine.h"

HyEntity2d::HyEntity2d(HyEntity2d *pParent /*= nullptr*/) :
	IHyBody2d(HYTYPE_Entity, "", "", pParent),
	m_uiEntAttribs(0),
	physics(*this)
{
}

HyEntity2d::HyEntity2d(HyEntity2d &&donor) noexcept :
	IHyBody2d(std::move(donor)),
	m_ChildList(std::move(donor.m_ChildList)),
	m_uiEntAttribs(std::move(donor.m_uiEntAttribs)),
	physics(*this)
{
}

HyEntity2d::~HyEntity2d(void)
{
	while(m_ChildList.empty() == false)
		m_ChildList[m_ChildList.size() - 1]->ParentDetach();

	physics.Deactivate();
}

HyEntity2d &HyEntity2d::operator=(HyEntity2d &&donor) noexcept
{
	IHyBody2d::operator=(std::move(donor));

	m_ChildList = std::move(donor.m_ChildList);
	m_uiEntAttribs = std::move(donor.m_uiEntAttribs);

	return *this;
}

void HyEntity2d::InitChildren()
{
	for(auto iter = m_ChildList.begin(); iter != m_ChildList.end(); ++iter)
		SetNewChildAttributes(*(*iter));
}

/*virtual*/ void HyEntity2d::SetVisible(bool bEnabled) /*override*/
{
	IHyNode::SetVisible(bEnabled);

	bool bPropagateVisible = IsVisible() && (GetInternalFlags() & EXPLICIT_ParentsVisible);
	for(uint32 i = 0; i < m_ChildList.size(); ++i)
		m_ChildList[i]->SetParentsVisible(bPropagateVisible);
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
		if(0 == (m_ChildList[i]->m_uiFlags & NODETYPE_IsBody) ||
			static_cast<IHyBody2d *>(m_ChildList[i])->IsLoadDataValid() == false ||
			static_cast<IHyBody2d *>(m_ChildList[i])->GetSceneAABB().IsValid() == false)
		{
			continue;
		}

		if(m_SceneAABB.IsValid() == false)
			m_SceneAABB = static_cast<IHyBody2d *>(m_ChildList[i])->GetSceneAABB();
		else
			m_SceneAABB.Combine(static_cast<IHyBody2d *>(m_ChildList[i])->GetSceneAABB());
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
			(*iter)->m_uiFlags |= EXPLICIT_ParentsVisible;

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
	m_uiEntAttribs |= ENT2DATTRIB_MouseInputEnabled;
}

void HyEntity2d::DisableMouseInput()
{
	m_uiEntAttribs &= ~ENT2DATTRIB_MouseInputEnabled;
}

bool HyEntity2d::IsMouseInBounds()
{
	glm::vec2 ptMouseInSceneCoords;
	if(GetCoordinateSystem() >= 0 && HyEngine::Input().GetMouseWindowIndex() == GetCoordinateSystem())
	{
		ptMouseInSceneCoords = HyEngine::Input().GetMousePos();
	}
	else if(GetCoordinateSystem() < 0)
	{
		if(HyEngine::Input().GetWorldMousePos(ptMouseInSceneCoords) == false)
			return false;
	}

	if(ShapeCount() > 0)
	{
		for(int32 i = 0; i < m_ShapeList.size(); ++i)
		{
			HyShape2d *pHyShape = m_ShapeList[i];
			if(pHyShape->TestPoint(GetSceneTransform(0.0f), ptMouseInSceneCoords))
				return true;
		}
	}
	else
		return HyTestPointAABB(GetSceneAABB(), ptMouseInSceneCoords);

	return false;
}

void HyEntity2d::ShapeAppend(HyShape2d &shapeRef)
{
	if(this == shapeRef.ParentGet())
		return;

	shapeRef.ParentDetach();
	m_ShapeList.push_back(&shapeRef);

	SyncPhysicsFixtures();

	//// A Box2d Body needs to exist in order to create/attach fixtures.
	//// HyScene::AddNode_PhysBody() will create a m_pBody when one doesn't exist
	//if(physics.m_pBody == nullptr)
	//	IHyNode::sm_pScene->AddNode_PhysBody(this, false);

	// ... and then set the density so it doesn't affect the previous body
	//shapeRef.SetDensity(fDensity);

	// Finally register the connection between the body and fixture (shape)
	
}

bool HyEntity2d::ShapeRemove(HyShape2d &childShapeRef)
{
	if(physics.m_pBody == nullptr)
		return false;

	for(b2Fixture *pFixture = physics.m_pBody->GetFixtureList(); pFixture != nullptr; pFixture = pFixture->GetNext())
	{
		HyShape2d *pShape = reinterpret_cast<HyShape2d *>(pFixture->GetUserData().pointer);
		if(pShape == &childShapeRef)
		{
			physics.m_pBody->DestroyFixture(pFixture);
			childShapeRef.m_pFixture = nullptr;
			return true;
		}
	}

	return false;
}

uint32 HyEntity2d::ShapeCount() const
{
	return static_cast<uint32>(m_ShapeList.size());
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

/*virtual*/ void HyEntity2d::SetDirty(uint32 uiDirtyFlags)
{
	IHyBody2d::SetDirty(uiDirtyFlags);

	for(uint32 i = 0; i < m_ChildList.size(); ++i)
		m_ChildList[i]->SetDirty(uiDirtyFlags);

	// If this body is actively being simulated by Box2d, and has a dirty transform
	// AND this isn't coming from the Scene updating physics normally
	if(physics.m_pBody && (uiDirtyFlags & DIRTY_Transform) && sm_pScene->IsPhysicsUpdating() == false)
	{
		SyncPhysicsBody();
	}
}

/*virtual*/ void HyEntity2d::Update() /*override*/
{
	IHyBody2d::Update();

	if((m_uiEntAttribs & ENT2DATTRIB_MouseInputEnabled) != 0)
	{
		bool bMouseInBounds = IsMouseInBounds();
		bool bLeftClickDown = HyEngine::Input().IsMouseBtnDown(HYMOUSE_BtnLeft);

		if((m_uiEntAttribs & ENT2DATTRIB_MouseInputHover) == 0) // Not currently hovering
		{
			if(bMouseInBounds)
			{
				m_uiEntAttribs |= ENT2DATTRIB_MouseInputHover;
				OnMouseEnter();
	
				if(bLeftClickDown)
				{
					if((m_uiEntAttribs & (ENT2DATTRIB_MouseInputInvalid | ENT2DATTRIB_MouseInputDown)) == 0)
					{
						m_uiEntAttribs |= ENT2DATTRIB_MouseInputDown;
						OnMouseDown();
					}
				}
				else
				{
					m_uiEntAttribs &= ~(ENT2DATTRIB_MouseInputDown | ENT2DATTRIB_MouseInputInvalid);
				}
			}
			else
			{
				if(HyEngine::Input().IsUsingTouchScreen() == false && bLeftClickDown && (m_uiEntAttribs & ENT2DATTRIB_MouseInputDown) == 0)
					m_uiEntAttribs |= ENT2DATTRIB_MouseInputInvalid;
			}
		}
		else // Is currently hovering
		{
			if(bMouseInBounds) // Still in bounds
			{
				if(bLeftClickDown)
				{
					if((m_uiEntAttribs & (ENT2DATTRIB_MouseInputInvalid | ENT2DATTRIB_MouseInputDown)) == 0)
					{
						m_uiEntAttribs |= ENT2DATTRIB_MouseInputDown;
						OnMouseDown();
					}
				}
				else
				{
					if((m_uiEntAttribs & ENT2DATTRIB_MouseInputDown) != 0)
					{
						m_uiEntAttribs &= ~ENT2DATTRIB_MouseInputDown;
						OnMouseClicked();
					}

					m_uiEntAttribs &= ~ENT2DATTRIB_MouseInputInvalid;
				}
			}
			else // Left bounds
			{
				m_uiEntAttribs &= ~ENT2DATTRIB_MouseInputHover;
				OnMouseLeave();
			}
		}
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
		
void HyEntity2d::SetNewChildAttributes(IHyNode2d &childRef)
{
	SetDirty(DIRTY_ALL);
	childRef.SetParentsVisible(IsVisible() && (GetInternalFlags() & EXPLICIT_ParentsVisible));
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

/*virtual*/ void HyEntity2d::SetParentsVisible(bool bParentsVisible) /*override final*/
{
	IHyNode::SetParentsVisible(bParentsVisible);
	for(uint32 i = 0; i < m_ChildList.size(); ++i)
		m_ChildList[i]->SetParentsVisible(IsVisible() && bParentsVisible);
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

void HyEntity2d::SyncPhysicsFixtures()
{
	if(physics.m_pBody == nullptr)
		return;

	for(int32 i = 0; i < m_ShapeList.size(); ++i)
	{
		HyShape2d *pShape = m_ShapeList[i];
		if(pShape->IsFixtureDirty())
		{
			if(pShape->IsValidShape())
				pShape->CreateFixture(physics.m_pBody);
			else
				pShape->DestroyFixture();
		}
	}
}

void HyEntity2d::SyncPhysicsBody()
{
	HyAssert(physics.m_pBody, "HyEntity2d::SyncPhysicsBody invoked with null physics body");

	// TODO: SCALE NOT SUPPORTED - If scale is different, modify all shapes in fixtures (cannot change num of vertices in shape says Box2d)
	const glm::mat4 &mtxSceneRef = GetSceneTransform(0.0f);
	glm::vec3 ptTranslation = mtxSceneRef[3];
	glm::vec3 vRotations = glm::eulerAngles(glm::quat_cast(mtxSceneRef));

	float fPpmInverse = sm_pScene->GetPpmInverse();
	physics.m_pBody->SetTransform(b2Vec2(ptTranslation.x * fPpmInverse, ptTranslation.y * fPpmInverse), vRotations.z);
	physics.m_pBody->SetLinearVelocity(b2Vec2(0, 0));
	physics.m_pBody->SetAngularVelocity(0.0f);
	physics.m_pBody->SetAwake(true);
}

/*friend*/ void HyNodeCtorAppend(HyEntity2d *pEntity, IHyNode2d *pChildNode)
{
	pEntity->m_ChildList.push_back(pChildNode);
	pEntity->SetDirty(HyEntity2d::DIRTY_ALL);
}
