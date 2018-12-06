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
#include "Scene/Nodes/Loadables/Visables/Objects/HyEntity2d.h"
#include "Scene/HyScene.h"
#include "Renderer/Effects/HyStencil.h"
#include "Assets/Nodes/HyEntityData.h"
#include "HyEngine.h"

HyEntity2d::HyEntity2d(const char *szPrefix, const char *szName, HyEntity2d *pParent /*= nullptr*/) :	IHyVisable2d(HYTYPE_Entity, szPrefix, szName, pParent),
																										m_uiAttributes(0),
																										m_eMouseInputState(MOUSEINPUT_None),
																										m_pMouseInputUserParam(nullptr)
{
}

HyEntity2d::HyEntity2d(HyEntity2d *pParent /*= nullptr*/) :	IHyVisable2d(HYTYPE_Entity, nullptr, nullptr, pParent),
															m_uiAttributes(0),
															m_eMouseInputState(MOUSEINPUT_None),
															m_pMouseInputUserParam(nullptr)
{
}

HyEntity2d::~HyEntity2d(void)
{
	while(m_ChildList.empty() == false)
		m_ChildList[m_ChildList.size() - 1]->ParentDetach();
}

/*virtual*/ void HyEntity2d::SetEnabled(bool bEnabled) /*override*/
{
	SetEnabled(bEnabled, false);
}

void HyEntity2d::SetEnabled(bool bEnabled, bool bOverrideExplicitChildren)
{
	m_bEnabled = bEnabled;
	m_uiExplicitAndTypeFlags |= EXPLICIT_Enabled;

	for(uint32 i = 0; i < m_ChildList.size(); ++i)
		m_ChildList[i]->_SetEnabled(bEnabled, bOverrideExplicitChildren);
}

/*virtual*/ void HyEntity2d::SetPauseUpdate(bool bUpdateWhenPaused) /*override*/
{
	SetPauseUpdate(bUpdateWhenPaused, false);
}

void HyEntity2d::SetPauseUpdate(bool bUpdateWhenPaused, bool bOverrideExplicitChildren)
{
	if(bUpdateWhenPaused)
	{
		if(m_bPauseOverride == false)
			HyScene::AddNode_PauseUpdate(this);
	}
	else
	{
		if(m_bPauseOverride == true)
			HyScene::RemoveNode_PauseUpdate(this);
	}

	m_bPauseOverride = bUpdateWhenPaused;
	m_uiExplicitAndTypeFlags |= EXPLICIT_PauseUpdate;

	for(uint32 i = 0; i < m_ChildList.size(); ++i)
		m_ChildList[i]->_SetPauseUpdate(m_bPauseOverride, bOverrideExplicitChildren);
}

/*virtual*/ void HyEntity2d::SetScissor(int32 uiLocalX, int32 uiLocalY, uint32 uiWidth, uint32 uiHeight) /*override*/
{
	SetScissor(uiLocalX, uiLocalY, uiWidth, uiHeight, false);
}

void HyEntity2d::SetScissor(int32 uiLocalX, int32 uiLocalY, uint32 uiWidth, uint32 uiHeight, bool bOverrideExplicitChildren)
{
	IHyVisable2d::SetScissor(uiLocalX, uiLocalY, uiWidth, uiHeight);

	for(uint32 i = 0; i < m_ChildList.size(); ++i)
	{
		if(0 != (m_ChildList[i]->m_uiExplicitAndTypeFlags & NODETYPE_IsVisable))
			static_cast<IHyVisable2d *>(m_ChildList[i])->_SetScissor(m_pScissor, bOverrideExplicitChildren);
	}
}

/*virtual*/ void HyEntity2d::ClearScissor(bool bUseParentScissor) /*override*/
{
	ClearScissor(bUseParentScissor, false);
}

void HyEntity2d::ClearScissor(bool bUseParentScissor, bool bOverrideExplicitChildren)
{
	IHyVisable2d::ClearScissor(bUseParentScissor);

	for(uint32 i = 0; i < m_ChildList.size(); ++i)
	{
		if(0 != (m_ChildList[i]->m_uiExplicitAndTypeFlags & NODETYPE_IsVisable))
			static_cast<IHyVisable2d *>(m_ChildList[i])->_SetScissor(m_pScissor, bOverrideExplicitChildren);
	}
}

/*virtual*/ void HyEntity2d::SetStencil(HyStencil *pStencil) /*override*/
{
	SetStencil(pStencil, false);
}

void HyEntity2d::SetStencil(HyStencil *pStencil, bool bOverrideExplicitChildren)
{
	IHyVisable2d::SetStencil(pStencil);

	for(uint32 i = 0; i < m_ChildList.size(); ++i)
	{
		if(0 != (m_ChildList[i]->m_uiExplicitAndTypeFlags & NODETYPE_IsVisable))
			static_cast<IHyVisable2d *>(m_ChildList[i])->_SetStencil(m_hStencil, bOverrideExplicitChildren);
	}
}

/*virtual*/ void HyEntity2d::ClearStencil(bool bUseParentStencil) /*override*/
{
	ClearStencil(bUseParentStencil, false);
}

void HyEntity2d::ClearStencil(bool bUseParentStencil, bool bOverrideExplicitChildren)
{
	IHyVisable2d::ClearStencil(bUseParentStencil);

	for(uint32 i = 0; i < m_ChildList.size(); ++i)
	{
		if(0 != (m_ChildList[i]->m_uiExplicitAndTypeFlags & NODETYPE_IsVisable))
			static_cast<IHyVisable2d *>(m_ChildList[i])->_SetStencil(m_hStencil, bOverrideExplicitChildren);
	}
}

/*virtual*/ void HyEntity2d::UseCameraCoordinates() /*override*/
{
	UseCameraCoordinates(false);
}

void HyEntity2d::UseCameraCoordinates(bool bOverrideExplicitChildren /*= false*/)
{
	IHyVisable2d::UseCameraCoordinates();

	for(uint32 i = 0; i < m_ChildList.size(); ++i)
	{
		if(0 != (m_ChildList[i]->m_uiExplicitAndTypeFlags & NODETYPE_IsVisable))
			static_cast<IHyVisable2d *>(m_ChildList[i])->_SetCoordinateSystem(m_iCoordinateSystem, bOverrideExplicitChildren);
	}
}

/*virtual*/ void HyEntity2d::UseWindowCoordinates(int32 iWindowIndex) /*override*/
{
	UseWindowCoordinates(iWindowIndex, false);
}

void HyEntity2d::UseWindowCoordinates(int32 iWindowIndex, bool bOverrideExplicitChildren)
{
	IHyVisable2d::UseWindowCoordinates(iWindowIndex);

	for(uint32 i = 0; i < m_ChildList.size(); ++i)
	{
		if(0 != (m_ChildList[i]->m_uiExplicitAndTypeFlags & NODETYPE_IsVisable))
			static_cast<IHyVisable2d *>(m_ChildList[i])->_SetCoordinateSystem(iWindowIndex, bOverrideExplicitChildren);
	}
}

/*virtual*/ void HyEntity2d::SetDisplayOrder(int32 iOrderValue) /*override*/
{
	SetDisplayOrder(iOrderValue, false);
}

void HyEntity2d::SetDisplayOrder(int32 iOrderValue, bool bOverrideExplicitChildren)
{
	IHyVisable2d::SetDisplayOrder(iOrderValue);
	SetChildrenDisplayOrder(bOverrideExplicitChildren);
}

/*virtual*/ void HyEntity2d::ResetDisplayOrder() /*override*/
{
	IHyVisable2d::ResetDisplayOrder();
	for(uint32 i = 0; i < m_ChildList.size(); ++i)
	{
		if(0 != (m_ChildList[i]->m_uiExplicitAndTypeFlags & NODETYPE_IsVisable))
			static_cast<IHyVisable2d *>(m_ChildList[i])->ResetDisplayOrder();
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

/*virtual*/ bool HyEntity2d::ChildInsert(IHyNode2d &insertBefore, IHyNode2d &childRef)
{
	childRef.ParentDetach();

	for(auto iter = m_ChildList.begin(); iter != m_ChildList.end(); ++iter)
	{
		if((*iter) == &insertBefore || 
		   ((*iter)->GetType() == HYTYPE_Entity && static_cast<HyEntity2d *>(*iter)->ChildExists(insertBefore)))
		{
			childRef.m_pParent = this;

			m_ChildList.insert(iter, &childRef);
			SetNewChildAttributes(childRef);

			return true;
		}
	}

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

			return true;
		}
	}

	return false;
}

/*virtual*/ void HyEntity2d::ChildrenTransfer(HyEntity2d &newParent)
{
	while(m_ChildList.empty() == false)
		newParent.ChildAppend(*m_ChildList[0]);
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

bool HyEntity2d::EnableMouseInput(void *pUserParam /*= nullptr*/)
{
	m_pMouseInputUserParam = pUserParam;
	m_uiAttributes |= ATTRIBFLAG_MouseInput;

	return true;
}

void HyEntity2d::DisableMouseInput()
{
	m_uiAttributes &= ~ATTRIBFLAG_MouseInput;
}

bool HyEntity2d::IsReverseDisplayOrder() const
{
	return (m_uiAttributes & ATTRIBFLAG_ReverseDisplayOrder);
}

void HyEntity2d::ReverseDisplayOrder(bool bReverse)
{
	if(bReverse)
		m_uiAttributes |= ATTRIBFLAG_ReverseDisplayOrder;
	else
		m_uiAttributes &= ~ATTRIBFLAG_ReverseDisplayOrder;

	SetDisplayOrder(m_iDisplayOrder, false);
}

int32 HyEntity2d::SetChildrenDisplayOrder(bool bOverrideExplicitChildren)
{
	int32 iOrderValue = m_iDisplayOrder + 1;

	if((m_uiAttributes & ATTRIBFLAG_ReverseDisplayOrder) == 0)
	{
		for(uint32 i = 0; i < m_ChildList.size(); ++i)
		{
			if(0 != (m_ChildList[i]->m_uiExplicitAndTypeFlags & NODETYPE_IsVisable))
				iOrderValue = static_cast<IHyVisable2d *>(m_ChildList[i])->_SetDisplayOrder(iOrderValue, bOverrideExplicitChildren);
		}
	}
	else
	{
		for(int32 i = static_cast<int32>(m_ChildList.size()) - 1; i >= 0; --i)
		{
			if(0 != (m_ChildList[i]->m_uiExplicitAndTypeFlags & NODETYPE_IsVisable))
				iOrderValue = static_cast<IHyVisable2d *>(m_ChildList[i])->_SetDisplayOrder(iOrderValue, bOverrideExplicitChildren);
		}
	}

	return iOrderValue;
}

/*virtual*/ const b2AABB &HyEntity2d::GetWorldAABB() /*override*/
{
	m_aabbCached.lowerBound = m_aabbCached.upperBound = b2Vec2(0.0f, 0.0f);
	for(uint32 i = 0; i < m_ChildList.size(); ++i)
	{
		if(m_ChildList[i]->GetWorldAABB().IsValid() == false)
			continue;

		if(i == 0)
			m_aabbCached = m_ChildList[i]->GetWorldAABB();
		else
			m_aabbCached.Combine(m_ChildList[i]->GetWorldAABB());
	}

	return m_aabbCached;
}

/*virtual*/ void HyEntity2d::Load() /*override*/
{
	// Load any attached children
	for(uint32 i = 0; i < m_ChildList.size(); ++i)
	{
		if(0 != (m_ChildList[i]->m_uiExplicitAndTypeFlags & NODETYPE_IsLoadable))
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
		if(0 != (m_ChildList[i]->m_uiExplicitAndTypeFlags & NODETYPE_IsLoadable))
			static_cast<IHyLoadable2d *>(m_ChildList[i])->Unload();
	}

	if(sm_pHyAssets)
		sm_pHyAssets->SetEntityLoaded(this);
}

/*virtual*/ void HyEntity2d::Update() /*override final*/
{
	IHyVisable2d::Update();

	if((m_uiAttributes & ATTRIBFLAG_MouseInput) != 0)
	{
		glm::vec2 ptMousePt;
		bool bMouseInBounds;
		if(GetCoordinateSystem() >= 0)
		{
			ptMousePt = Hy_Input().GetMousePos();
			bMouseInBounds = Hy_Input().GetMouseWindowIndex() == GetCoordinateSystem() && HyTestPointAABB(GetWorldAABB(), ptMousePt);
		}
		else
		{
			ptMousePt = Hy_Input().GetWorldMousePos();
			bMouseInBounds = HyTestPointAABB(GetWorldAABB(), ptMousePt);
		}

		bool bLeftClickDown = Hy_Input().IsMouseBtnDown(HYMOUSE_BtnLeft);

		switch(m_eMouseInputState)
		{
		case MOUSEINPUT_None:
			if(bMouseInBounds)
			{
				m_eMouseInputState = MOUSEINPUT_Hover;
				OnMouseEnter(m_pMouseInputUserParam);

				if(bLeftClickDown)
				{
					m_eMouseInputState = MOUSEINPUT_Down;
					OnMouseDown(m_pMouseInputUserParam);
				}
			}
			break;

		case MOUSEINPUT_Hover:
			if(bMouseInBounds == false)
			{
				m_eMouseInputState = MOUSEINPUT_None;
				OnMouseLeave(m_pMouseInputUserParam);
			}
			else if(bLeftClickDown)
			{
				m_eMouseInputState = MOUSEINPUT_Down;
				OnMouseDown(m_pMouseInputUserParam);
			}
			break;

		case MOUSEINPUT_Down:
			if(bMouseInBounds == false)
			{
				m_eMouseInputState = MOUSEINPUT_None;
				OnMouseLeave(m_pMouseInputUserParam);
			}
			else if(bLeftClickDown == false)
			{
				m_eMouseInputState = MOUSEINPUT_Hover;
				OnMouseUp(m_pMouseInputUserParam);
				OnMouseClicked(m_pMouseInputUserParam);
			}
			break;
		}
	}

	OnUpdate();
}

/*virtual*/ bool HyEntity2d::IsChildrenLoaded() const /*override final*/
{
	for(uint32 i = 0; i < m_ChildList.size(); ++i)
	{
		if(0 != (m_ChildList[i]->m_uiExplicitAndTypeFlags & NODETYPE_IsLoadable))
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
	childRef._SetEnabled(m_bEnabled, false);
	childRef._SetPauseUpdate(m_bPauseOverride, false);

	if(childRef.GetExplicitAndTypeFlags() & NODETYPE_IsVisable)
		SetupNewChild(*this, static_cast<IHyVisable2d &>(childRef));

	if(sm_pHyAssets)
		sm_pHyAssets->SetEntityLoaded(this);
}

/*virtual*/ void HyEntity2d::SetDirty(uint32 uiDirtyFlags)
{
	IHyNode2d::SetDirty(uiDirtyFlags);

	for(uint32 i = 0; i < m_ChildList.size(); ++i)
		m_ChildList[i]->SetDirty(uiDirtyFlags);
}

/*virtual*/ void HyEntity2d::_SetEnabled(bool bEnabled, bool bIsOverriding) /*override final*/
{
	IHyNode::_SetEnabled(bEnabled, bIsOverriding);

	if(0 == (m_uiExplicitAndTypeFlags & EXPLICIT_Enabled))
	{
		for(uint32 i = 0; i < m_ChildList.size(); ++i)
			m_ChildList[i]->_SetEnabled(m_bEnabled, bIsOverriding);
	}
}

/*virtual*/ void HyEntity2d::_SetPauseUpdate(bool bUpdateWhenPaused, bool bIsOverriding) /*override final*/
{
	IHyNode::_SetPauseUpdate(bUpdateWhenPaused, bIsOverriding);

	if(0 == (m_uiExplicitAndTypeFlags & EXPLICIT_PauseUpdate))
	{
		for(uint32 i = 0; i < m_ChildList.size(); ++i)
			m_ChildList[i]->_SetPauseUpdate(m_bPauseOverride, bIsOverriding);
	}
}

/*virtual*/ void HyEntity2d::_SetScissor(const ScissorRect *pParentScissor, bool bIsOverriding) /*override final*/
{
	IHyVisable2d::_SetScissor(pParentScissor, bIsOverriding);
	
	if(0 == (m_uiExplicitAndTypeFlags & EXPLICIT_Scissor))
	{
		for(uint32 i = 0; i < m_ChildList.size(); ++i)
		{
			if(0 != (m_ChildList[i]->m_uiExplicitAndTypeFlags & NODETYPE_IsVisable))
				static_cast<IHyVisable2d *>(m_ChildList[i])->_SetScissor(m_pScissor, bIsOverriding);
		}
	}
}

/*virtual*/ void HyEntity2d::_SetStencil(HyStencilHandle hHandle, bool bIsOverriding) /*override final*/
{
	IHyVisable2d::_SetStencil(hHandle, bIsOverriding);

	if(0 == (m_uiExplicitAndTypeFlags & EXPLICIT_Stencil))
	{
		for(uint32 i = 0; i < m_ChildList.size(); ++i)
		{
			if(0 != (m_ChildList[i]->m_uiExplicitAndTypeFlags & NODETYPE_IsVisable))
				static_cast<IHyVisable2d *>(m_ChildList[i])->_SetStencil(m_hStencil, bIsOverriding);
		}
	}
}

/*virtual*/ void HyEntity2d::_SetCoordinateSystem(int32 iWindowIndex, bool bIsOverriding) /*override final*/
{
	IHyVisable2d::_SetCoordinateSystem(iWindowIndex, bIsOverriding);

	if(0 == (m_uiExplicitAndTypeFlags & EXPLICIT_CoordinateSystem))
	{
		for(uint32 i = 0; i < m_ChildList.size(); ++i)
		{
			if(0 != (m_ChildList[i]->m_uiExplicitAndTypeFlags & NODETYPE_IsVisable))
				static_cast<IHyVisable2d *>(m_ChildList[i])->_SetCoordinateSystem(iWindowIndex, bIsOverriding);
		}
	}
}

/*virtual*/ int32 HyEntity2d::_SetDisplayOrder(int32 iOrderValue, bool bIsOverriding) /*override final*/
{
	IHyVisable2d::_SetDisplayOrder(iOrderValue, bIsOverriding);

	if(0 == (m_uiExplicitAndTypeFlags & EXPLICIT_DisplayOrder))
		iOrderValue = SetChildrenDisplayOrder(bIsOverriding);

	return iOrderValue;
}

/*friend*/ void _CtorChildAppend(HyEntity2d &entityRef, IHyNode2d &childRef)
{
	entityRef.m_ChildList.push_back(&childRef);
	entityRef.SetDirty(HyEntity2d::DIRTY_ALL);
}
