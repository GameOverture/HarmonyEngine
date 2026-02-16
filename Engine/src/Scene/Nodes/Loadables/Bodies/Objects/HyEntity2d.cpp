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
#include "Scene/HyScene.h"
#include "Scene/Nodes/Loadables/Bodies/Objects/HyEntity2d.h"
#include "Scene/Physics/Fixtures/IHyFixture2d.h"
#include "Renderer/Effects/HyStencil.h"
#include "HyEngine.h"

HyEntity2d::HyEntity2d(HyEntity2d *pParent /*= nullptr*/) :
	IHyBody2d(HYTYPE_Entity, HyNodePath(), pParent),
	m_uiEntityAttribs(0),
	m_fDeferAmt(0.0f),
	m_fpDeferFinishedFunc(nullptr),
	physics(*this)
{
}

HyEntity2d::HyEntity2d(HyEntity2d &&donor) noexcept :
	IHyBody2d(std::move(donor)),
	m_ChildList(std::move(donor.m_ChildList)),
	m_uiEntityAttribs(std::move(donor.m_uiEntityAttribs)),
	m_fDeferAmt(0.0f),
	m_fpDeferFinishedFunc(nullptr),
	physics(*this)
{
}

HyEntity2d::~HyEntity2d(void)
{
	if(IsRegisteredAssembleEntity())
		HyScene::RemoveEntNode_Assemble(this);

	while(m_ChildList.empty() == false)
		m_ChildList[m_ChildList.size() - 1]->ParentDetach();

	while(m_FixtureList.empty() == false)
		m_FixtureList[m_FixtureList.size() - 1]->ParentDetach();
	physics.Destroy(true); // Destroy physics body first to detach all shapes more efficiently

	ClearScissor(true);
}

HyEntity2d &HyEntity2d::operator=(HyEntity2d &&donor) noexcept
{
	IHyBody2d::operator=(std::move(donor));

	m_ChildList = std::move(donor.m_ChildList);
	m_uiEntityAttribs = std::move(donor.m_uiEntityAttribs);

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

/*virtual*/ float HyEntity2d::GetWidth(float fPercent /*= 1.0f*/) /*override*/
{
	HyShape2d shape;
	CalcLocalBoundingShape(shape);
	
	b2AABB aabb;
	shape.ComputeAABB(aabb, glm::mat4(1.0f));
	return (b2AABB_Extents(aabb).x * 2.0f) * fPercent;
}

/*virtual*/ float HyEntity2d::GetHeight(float fPercent /*= 1.0f*/) /*override*/
{
	HyShape2d shape;
	CalcLocalBoundingShape(shape);
	
	b2AABB aabb;
	shape.ComputeAABB(aabb, glm::mat4(1.0f));
	return (b2AABB_Extents(aabb).y * 2.0f) * fPercent;
}

/*virtual*/ void HyEntity2d::CalcLocalBoundingShape(HyShape2d &shapeOut) /*override*/
{
	shapeOut.SetAsNothing();

	b2AABB localAABB;
	HyMath::InvalidateAABB(localAABB);
	for(uint32 i = 0; i < m_ChildList.size(); ++i)
	{
		if(0 == (m_ChildList[i]->m_uiFlags & NODETYPE_IsBody) ||
			static_cast<IHyBody2d *>(m_ChildList[i])->IsLoadDataValid() == false)
		{
			continue;
		}

		static_cast<IHyBody2d *>(m_ChildList[i])->CalcLocalBoundingShape(shapeOut);
		glm::mat4 localMtx;
		m_ChildList[i]->GetLocalTransform(localMtx, 0.0f);

		if(b2IsValidAABB(localAABB) == false)
			shapeOut.ComputeAABB(localAABB, localMtx);
		else
		{
			b2AABB tmpAABB;
			shapeOut.ComputeAABB(tmpAABB, localMtx);
			localAABB = b2AABB_Union(localAABB, tmpAABB);
		}
	}

	if(b2IsValidAABB(localAABB))
		shapeOut.SetAsBox(HyRect(b2AABB_Extents(localAABB).x, b2AABB_Extents(localAABB).y, glm::vec2(b2AABB_Center(localAABB).x, b2AABB_Center(localAABB).y), 0.0f));
	else
		shapeOut.SetAsNothing();
}

/*virtual*/ const b2AABB &HyEntity2d::GetSceneAABB() /*override*/
{
	HyMath::InvalidateAABB(m_SceneAABB);
	for(uint32 i = 0; i < m_ChildList.size(); ++i)
	{
		if(0 == (m_ChildList[i]->m_uiFlags & NODETYPE_IsBody) ||
			static_cast<IHyBody2d *>(m_ChildList[i])->IsLoadDataValid() == false ||
			b2IsValidAABB(static_cast<IHyBody2d *>(m_ChildList[i])->GetSceneAABB()) == false)
		{
			continue;
		}

		if(b2IsValidAABB(m_SceneAABB) == false)
			m_SceneAABB = static_cast<IHyBody2d *>(m_ChildList[i])->GetSceneAABB();
		else
			m_SceneAABB = b2AABB_Union(m_SceneAABB, static_cast<IHyBody2d *>(m_ChildList[i])->GetSceneAABB());
	}

	return m_SceneAABB;
}

/*virtual*/ void HyEntity2d::SetScissor(const HyRect &scissorRect) /*override*/
{
	SetScissor(scissorRect, false);
}

void HyEntity2d::SetScissor(const HyRect &scissorRect, bool bOverrideExplicitChildren)
{
	IHyBody2d::SetScissor(scissorRect);

	for(uint32 i = 0; i < m_ChildList.size(); ++i)
	{
		if(0 != (m_ChildList[i]->m_uiFlags & NODETYPE_IsBody))
			static_cast<IHyBody2d *>(m_ChildList[i])->_SetScissorStencil(m_hScissorStencil, bOverrideExplicitChildren);
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
			static_cast<IHyBody2d *>(m_ChildList[i])->_SetScissorStencil(m_hScissorStencil, bOverrideExplicitChildren);
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

void HyEntity2d::ChildAppend(IHyNode2d &childRef)
{
	HyAssert(&childRef != this, "HyEntity2d::ChildAppend was passed a child that was itself!");

	childRef.ParentDetach();
	childRef.m_pParent = this;

	m_ChildList.push_back(&childRef);
	SetNewChildAttributes(childRef);
}

/*virtual*/ void HyEntity2d::ChildPrepend(IHyNode2d &childRef)
{
	HyAssert(&childRef != this, "HyEntity2d::ChildPrepend was passed a child that was itself!");

	childRef.ParentDetach();
	childRef.m_pParent = this;

	m_ChildList.insert(m_ChildList.begin(), &childRef);
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
			// Remove child/parent connection
			(*iter)->m_pParent = nullptr;
			m_ChildList.erase(iter);

			// Clear all of the child's inherited attributes
			pChild->m_uiFlags |= EXPLICIT_ParentsVisible;
			//if((pChild->GetInternalFlags() & NODETYPE_IsBody) != 0 && (pChild->GetInternalFlags() & EXPLICIT_ScissorStencil) == 0)
			//	static_cast<IHyBody2d *>(pChild)->ClearScissor(true);


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

void HyEntity2d::FixtureAppend(IHyFixture2d &fixtureRef)
{
	if(this == fixtureRef.ParentGet())
		return;

	fixtureRef.ParentDetach();
	m_FixtureList.push_back(&fixtureRef);
	fixtureRef.m_pParent = this;

	SyncPhysicsFixtures();
}

bool HyEntity2d::FixtureRemove(IHyFixture2d &childFixtureRef)
{
	for(auto iter = m_FixtureList.begin(); iter != m_FixtureList.end(); ++iter)
	{
		if(*iter == &childFixtureRef)
		{
			// Fixture found as a child, now also remove it from the physics simulation
			childFixtureRef.PhysicsRemove(true);

			m_FixtureList.erase(iter);
			childFixtureRef.m_pParent = nullptr;
			return true;
		}
	}

	return false;
}

uint32 HyEntity2d::FixtureCount() const
{
	return static_cast<uint32>(m_FixtureList.size());
}

IHyFixture2d *HyEntity2d::FixtureGet(uint32 uiIndex)
{
	HyAssert(uiIndex < static_cast<uint32>(m_FixtureList.size()), "HyEntity2d::FixtureGet passed an invalid index");
	return m_FixtureList[uiIndex];
}

bool HyEntity2d::IsMouseInputEnabled() const
{
	return (m_uiEntityAttribs & ENTITYATTRIB_MouseInputEnabled) != 0;
}

void HyEntity2d::SetMouseInput(bool bEnable)
{
	if(bEnable)
		EnableMouseInput();
	else
		DisableMouseInput();
}

void HyEntity2d::EnableMouseInput()
{
	m_uiEntityAttribs |= ENTITYATTRIB_MouseInputEnabled;

	if(CalcMouseInBounds())
	{
		m_uiEntityAttribs |= ENTITYATTRIB_MouseInputHover;
		OnMouseEnter();
	}
}

void HyEntity2d::DisableMouseInput()
{
	if(IsMouseHover())
		OnMouseLeave();
	if(IsMouseDown())
		OnMouseUp();
	
	m_uiEntityAttribs &= ~(ENTITYATTRIB_MouseInputEnabled | ENTITYATTRIB_MouseInputHover | ENTITYATTRIB_MouseInputDown | ENTITYATTRIB_MouseInputInvalid);
}

bool HyEntity2d::IsMouseHover()
{
	if(IsMouseInputEnabled())
		return (m_uiEntityAttribs & ENTITYATTRIB_MouseInputHover) != 0;
	else
		return CalcMouseInBounds();
}

bool HyEntity2d::IsMouseDown() const
{
	return (m_uiEntityAttribs & ENTITYATTRIB_MouseInputDown) != 0;
}

bool HyEntity2d::IsReverseDisplayOrder() const
{
	return (m_uiEntityAttribs & ENTITYATTRIB_ReverseDisplayOrder);
}

void HyEntity2d::ReverseDisplayOrder(bool bReverse)
{
	if(bReverse)
		m_uiEntityAttribs |= ENTITYATTRIB_ReverseDisplayOrder;
	else
		m_uiEntityAttribs &= ~ENTITYATTRIB_ReverseDisplayOrder;

	SetDisplayOrder(m_iDisplayOrder, false);
}

int32 HyEntity2d::SetChildrenDisplayOrder(bool bOverrideExplicitChildren)
{
	int32 iOrderValue = m_iDisplayOrder + 1;

	if((m_uiEntityAttribs & ENTITYATTRIB_ReverseDisplayOrder) == 0)
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

void HyEntity2d::Defer(float fTime, std::function<void(HyEntity2d *)> fpFunc)
{
	m_fDeferAmt = fTime;
	delete m_fpDeferFinishedFunc;
	m_fpDeferFinishedFunc = HY_NEW std::function<void(HyEntity2d *)>(fpFunc);
}

bool HyEntity2d::IsDeferring() const
{
	return m_fDeferAmt > 0.0f;
}

bool HyEntity2d::IsRegisteredAssembleEntity() const
{
	return (m_uiEntityAttribs & ENTITYATTRIB_IsRegisteredAssemble);
}

void HyEntity2d::RegisterAssembleEntity(bool bRegister /*= true*/)
{
	if(bRegister)
	{
		if(IsRegisteredAssembleEntity() == false)
		{
			m_uiEntityAttribs |= ENTITYATTRIB_IsRegisteredAssemble;
			HyScene::AddEntNode_Assemble(this);
		}
	}
	else
	{
		if(IsRegisteredAssembleEntity())
		{
			m_uiEntityAttribs &= ~ENTITYATTRIB_IsRegisteredAssemble;
			HyScene::RemoveEntNode_Assemble(this);
		}
	}
}

void HyEntity2d::SetAssembleNeeded()
{
	m_uiEntityAttribs |= ENTITYATTRIB_AssembleNeeded;
}

void HyEntity2d::Assemble()
{
	if((m_uiEntityAttribs & ENTITYATTRIB_AssembleNeeded) == 0)
		return;

	OnAssemble();
	m_uiEntityAttribs &= ~ENTITYATTRIB_AssembleNeeded;
}

/*virtual*/ void HyEntity2d::SetDirty(uint32 uiDirtyFlags)
{
	IHyBody2d::SetDirty(uiDirtyFlags);

	for(uint32 i = 0; i < m_ChildList.size(); ++i)
		m_ChildList[i]->SetDirty(uiDirtyFlags);

	// If this body is actively being simulated by Box2d, and has a dirty transform
	// AND this isn't coming from the Scene updating physics normally
	if(physics.IsActivated() && (uiDirtyFlags & DIRTY_Transform) && sm_pScene->IsPhysicsUpdating() == false)
	{
		SyncPhysicsBody();
	}
}

/*virtual*/ void HyEntity2d::Update() /*override*/
{
	IHyBody2d::Update();

	if(IsDeferring())
	{
		m_fDeferAmt -= HyEngine::DeltaTime();
		if(m_fDeferAmt <= 0.0f)
		{
			(*m_fpDeferFinishedFunc)(this);
			if(m_fDeferAmt <= 0.0f) // NOTE: Must recheck in case the 'm_fpDeferFinishedFunc' callback set a new Defer()
			{
				delete m_fpDeferFinishedFunc;
				m_fpDeferFinishedFunc = nullptr;
				m_fDeferAmt = 0.0f;
			}
		}
	}

	if(IsMouseInputEnabled())
	{
		bool bMouseInBounds = CalcMouseInBounds();
		bool bLeftClickDown = HyEngine::Input().IsMouseBtnDown(HYMOUSE_BtnLeft);

		if(IsMouseHover() == false)
		{
			if(bMouseInBounds)
			{
				m_uiEntityAttribs |= ENTITYATTRIB_MouseInputHover;
				OnMouseEnter();
	
				if(bLeftClickDown)
				{
					if((m_uiEntityAttribs & (ENTITYATTRIB_MouseInputInvalid | ENTITYATTRIB_MouseInputDown)) == 0)
					{
						m_uiEntityAttribs |= ENTITYATTRIB_MouseInputDown;
						OnMouseDown();
					}
				}
				else
				{
					if(IsMouseDown())
					{
						m_uiEntityAttribs &= ~(ENTITYATTRIB_MouseInputDown | ENTITYATTRIB_MouseInputInvalid);
						OnMouseUp();
						OnMouseClicked();
					}

					m_uiEntityAttribs &= ~(ENTITYATTRIB_MouseInputDown | ENTITYATTRIB_MouseInputInvalid);
				}
			}
			else
			{
				if(bLeftClickDown && IsMouseDown() == false && HyEngine::Input().IsUsingTouchScreen() == false)
					m_uiEntityAttribs |= ENTITYATTRIB_MouseInputInvalid;
				else if(bLeftClickDown == false && IsMouseDown())
				{
					m_uiEntityAttribs &= ~(ENTITYATTRIB_MouseInputDown | ENTITYATTRIB_MouseInputInvalid);
					OnMouseUp();
				}
			}
		}
		else // Is currently hovering
		{
			if(bMouseInBounds) // Still in bounds
			{
				if(bLeftClickDown)
				{
					if((m_uiEntityAttribs & (ENTITYATTRIB_MouseInputInvalid | ENTITYATTRIB_MouseInputDown)) == 0)
					{
						m_uiEntityAttribs |= ENTITYATTRIB_MouseInputDown;
						OnMouseDown();
					}
				}
				else
				{
					if(IsMouseDown())
					{
						m_uiEntityAttribs &= ~ENTITYATTRIB_MouseInputDown;
						OnMouseUp();
						OnMouseClicked();
					}

					m_uiEntityAttribs &= ~ENTITYATTRIB_MouseInputInvalid;
				}
			}
			else // Left bounds
			{
				m_uiEntityAttribs &= ~ENTITYATTRIB_MouseInputHover;
				OnMouseLeave();

				if(bLeftClickDown && IsMouseDown() == false && HyEngine::Input().IsUsingTouchScreen() == false)
					m_uiEntityAttribs |= ENTITYATTRIB_MouseInputInvalid;
				else if(bLeftClickDown == false && IsMouseDown())
				{
					m_uiEntityAttribs &= ~(ENTITYATTRIB_MouseInputDown | ENTITYATTRIB_MouseInputInvalid);
					OnMouseUp();
				}
			}
		}
	} // Mouse Input Enabled

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
			static_cast<IHyBody2d &>(childRef)._SetScissorStencil(m_hScissorStencil, false);

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

/*virtual*/ void HyEntity2d::_SetScissorStencil(HyStencilHandle hHandle, bool bIsOverriding) /*override final*/
{
	IHyBody2d::_SetScissorStencil(hHandle, bIsOverriding);
	
	if(0 == (m_uiFlags & EXPLICIT_ScissorStencil))
	{
		for(uint32 i = 0; i < m_ChildList.size(); ++i)
		{
			if(0 != (m_ChildList[i]->m_uiFlags & NODETYPE_IsBody))
				static_cast<IHyBody2d *>(m_ChildList[i])->_SetScissorStencil(m_hScissorStencil, bIsOverriding);
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

bool HyEntity2d::CalcMouseInBounds()
{
	glm::vec2 ptMouseInSceneCoords;
	if(GetCoordinateSystem() >= 0)
	{
		if(HyEngine::Input().GetMouseWindowIndex() != GetCoordinateSystem())
			return false;

		ptMouseInSceneCoords = HyEngine::Input().GetMousePos();
	}
	else if(GetCoordinateSystem() < 0)
	{
		if(HyEngine::Input().GetWorldMousePos(ptMouseInSceneCoords) == false)
			return false;
	}

	if(FixtureCount() > 0)
	{
		for(int32 i = 0; i < m_FixtureList.size(); ++i)
		{
			IHyFixture2d *pFixture = m_FixtureList[i];
			if(pFixture->TestPoint(ptMouseInSceneCoords, GetSceneTransform(0.0f)))
				return true;
		}
	}
	else
		return HyMath::TestPointAABB(GetSceneAABB(), ptMouseInSceneCoords);

	return false;
}

void HyEntity2d::SyncPhysicsFixtures()
{
	if(B2_IS_NULL(physics.GetHandle()))
		return;

	for(int32 i = 0; i < m_FixtureList.size(); ++i)
	{
		IHyFixture2d *pFixture = m_FixtureList[i];
		if(pFixture->IsPhysicsDirty())
		{
			if(pFixture->IsValid() && pFixture->IsPhysicsAllowed())
			{
				if(pFixture->GetType() != HYFIXTURE_LineChain)
				{
					HyAssert(false == (physics.GetType() == HYBODY_Dynamic && static_cast<HyShape2d *>(pFixture)->GetDensity() == 0.0f), "HyEntity2d::SyncPhysicsFixtures - Attempting to create a fixture with zero density on a dynamic body");
				}
				pFixture->PhysicsAttach();
			}
			else
				pFixture->PhysicsRemove(true);
		}
	}
}

void HyEntity2d::SyncPhysicsBody()
{
	HyAssert(B2_IS_NON_NULL(physics.GetHandle()), "HyEntity2d::SyncPhysicsBody invoked with null physics body");

	// TODO: SCALE NOT SUPPORTED - If scale is different, modify all shapes in fixtures (cannot change num of vertices in shape says Box2d)
	const glm::mat4 &mtxSceneRef = GetSceneTransform(0.0f);
	glm::vec3 ptTranslation = mtxSceneRef[3];
	glm::vec3 vRotations = glm::eulerAngles(glm::quat_cast(mtxSceneRef));

	float fPpmInverse = sm_pScene->GetPpmInverse();
	b2Body_SetTransform(physics.GetHandle(), { ptTranslation.x * fPpmInverse, ptTranslation.y * fPpmInverse }, b2MakeRot(vRotations.z));
	b2Body_SetAwake(physics.GetHandle(), true);
}

/*friend*/ void HyNodeCtorAppend(HyEntity2d *pEntity, IHyNode2d *pChildNode)
{
	pEntity->m_ChildList.push_back(pChildNode);
	pEntity->SetDirty(HyEntity2d::DIRTY_ALL);
}
