/**************************************************************************
 *	HyEntity2d.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Scene/Nodes/Draws/Entities/HyEntity2d.h"
#include "Scene/HyScene.h"
#include "Renderer/Effects/HyStencil.h"
#include "Renderer/Effects/HyPortal2d.h"
#include "HyEngine.h"

HyEntity2d::HyEntity2d(HyEntity2d *pParent /*= nullptr*/) :	IHyDraw2d(HYTYPE_Entity2d, pParent),
															m_uiAttributes(0),
															m_eMouseInputState(MOUSEINPUT_None),
															m_pMouseInputUserParam(nullptr),
															m_pMouseInputNode(nullptr)
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

/*virtual*/ void HyEntity2d::SetPauseUpdate(bool bUpdateWhenPaused) /*override*/
{
	SetPauseUpdate(bUpdateWhenPaused, false);
}

void HyEntity2d::SetEnabled(bool bEnabled, bool bOverrideExplicitChildren)
{
	m_bEnabled = bEnabled;
	m_uiExplicitFlags |= EXPLICIT_Enabled;

	for(uint32 i = 0; i < m_ChildList.size(); ++i)
		m_ChildList[i]->_SetEnabled(bEnabled, bOverrideExplicitChildren);
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
	m_uiExplicitFlags |= EXPLICIT_PauseUpdate;

	for(uint32 i = 0; i < m_ChildList.size(); ++i)
		m_ChildList[i]->_SetPauseUpdate(m_bPauseOverride, bOverrideExplicitChildren);
}

void HyEntity2d::SetScissor(int32 uiLocalX, int32 uiLocalY, uint32 uiWidth, uint32 uiHeight, bool bOverrideExplicitChildren /*= false*/)
{
	if(m_pScissor == nullptr)
		m_pScissor = HY_NEW ScissorRect();

	m_pScissor->m_LocalScissorRect.x = uiLocalX;
	m_pScissor->m_LocalScissorRect.y = uiLocalY;
	m_pScissor->m_LocalScissorRect.width = uiWidth;
	m_pScissor->m_LocalScissorRect.height = uiHeight;
	m_pScissor->m_LocalScissorRect.iTag = SCISSORTAG_Enabled;

	m_uiExplicitFlags |= EXPLICIT_Scissor;

	GetWorldScissor(m_pScissor->m_WorldScissorRect);

	for(uint32 i = 0; i < m_ChildList.size(); ++i)
		m_ChildList[i]->_SetScissor(m_pScissor->m_WorldScissorRect, bOverrideExplicitChildren);
}

void HyEntity2d::ClearScissor(bool bUseParentScissor, bool bOverrideExplicitChildren /*= false*/)
{
	if(m_pScissor == nullptr)
		return;

	m_pScissor->m_LocalScissorRect.iTag = SCISSORTAG_Disabled;
	m_pScissor->m_WorldScissorRect.iTag = SCISSORTAG_Disabled;

	if(bUseParentScissor == false)
		m_uiExplicitFlags |= EXPLICIT_Scissor;
	else
	{
		m_uiExplicitFlags &= ~EXPLICIT_Scissor;
		if(m_pParent)
			m_pParent->GetWorldScissor(m_pScissor->m_WorldScissorRect);
	}

	for(uint32 i = 0; i < m_ChildList.size(); ++i)
		m_ChildList[i]->_SetScissor(m_pScissor->m_WorldScissorRect, bOverrideExplicitChildren);
}

void HyEntity2d::SetStencil(HyStencil *pStencil, bool bOverrideExplicitChildren /*= false*/)
{
	if(pStencil == nullptr)
		m_hStencil = HY_UNUSED_HANDLE;
	else
		m_hStencil = pStencil->GetHandle();

	m_uiExplicitFlags |= EXPLICIT_Stencil;

	for(uint32 i = 0; i < m_ChildList.size(); ++i)
		m_ChildList[i]->_SetStencil(m_hStencil, bOverrideExplicitChildren);
}

void HyEntity2d::ClearStencil(bool bUseParentStencil, bool bOverrideExplicitChildren /*= false*/)
{
}

void HyEntity2d::UseCameraCoordinates(bool bOverrideExplicitChildren /*= false*/)
{
	m_iCoordinateSystem = -1;
	m_uiExplicitFlags |= EXPLICIT_CoordinateSystem;

	for(uint32 i = 0; i < m_ChildList.size(); ++i)
		m_ChildList[i]->_SetCoordinateSystem(m_iCoordinateSystem, bOverrideExplicitChildren);
}

void HyEntity2d::UseWindowCoordinates(int32 iWindowIndex /*= 0*/, bool bOverrideExplicitChildren /*= false*/)
{
	m_iCoordinateSystem = iWindowIndex;
	m_uiExplicitFlags |= EXPLICIT_CoordinateSystem;

	for(uint32 i = 0; i < m_ChildList.size(); ++i)
		m_ChildList[i]->_SetCoordinateSystem(iWindowIndex, bOverrideExplicitChildren);
}

void HyEntity2d::SetDisplayOrder(int32 iOrderValue, bool bOverrideExplicitChildren /*= false*/)
{
	m_iDisplayOrder = iOrderValue;
	m_uiExplicitFlags |= EXPLICIT_DisplayOrder;

	if((m_uiAttributes & ATTRIBFLAG_ReverseDisplayOrder) == 0)
	{
		for(uint32 i = 0; i < m_ChildList.size(); ++i)
			iOrderValue = m_ChildList[i]->_SetDisplayOrder(iOrderValue, bOverrideExplicitChildren);
	}
	else
	{
		for(int32 i = static_cast<int32>(m_ChildList.size()) - 1; i >= 0; --i)
			iOrderValue = m_ChildList[i]->_SetDisplayOrder(iOrderValue, bOverrideExplicitChildren);
	}
}

void HyEntity2d::ChildAppend(IHyNode2d &childInst)
{
	HyAssert(&childInst != this, "HyEntity2d::ChildAppend was passed a child that was itself!");

	childInst.ParentDetach();
	childInst.m_pParent = this;

	m_ChildList.push_back(&childInst);
	SetNewChildAttributes(childInst);
}

/*virtual*/ bool HyEntity2d::ChildInsert(IHyNode2d &insertBefore, IHyNode2d &childInst)
{
	for(auto iter = m_ChildList.begin(); iter != m_ChildList.end(); ++iter)
	{
		if((*iter) == &insertBefore || 
		   ((*iter)->GetType() == HYTYPE_Entity2d && static_cast<HyEntity2d *>(*iter)->ChildExists(insertBefore)))
		{
			childInst.ParentDetach();
			childInst.m_pParent = this;

			m_ChildList.insert(iter, &childInst);
			SetNewChildAttributes(childInst);

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
		   ((*iter)->GetType() == HYTYPE_Entity2d && static_cast<HyEntity2d *>(*iter)->ChildExists(childRef)))
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
			if(m_pMouseInputNode == pChild)
			{
				m_uiAttributes &= ~ATTRIBFLAG_MouseInput;
				m_pMouseInputNode = nullptr;
			}

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
	HyAssert(uiIndex < static_cast<uint32>(m_ChildList.size()), "HyEntityLeaf2d::ChildGet passed an invalid index");
	return m_ChildList[uiIndex];
}

void HyEntity2d::ForEachChild(std::function<void(IHyNode2d *)> func)
{
	func(this);

	for(uint32 i = 0; i < m_ChildList.size(); ++i)
	{
		if(m_ChildList[i]->GetType() == HYTYPE_Entity2d)
			static_cast<HyEntity2d *>(m_ChildList[i])->ForEachChild(func);
	}
}

bool HyEntity2d::EnableMouseInput(IHyDrawInst2d *pInputChildNode, void *pUserParam /*= nullptr*/)
{
	if(pInputChildNode == nullptr || ChildExists(*pInputChildNode) == false)
		return false;

	m_pMouseInputNode = pInputChildNode;
	m_pMouseInputUserParam = pUserParam;
	m_uiAttributes |= ATTRIBFLAG_MouseInput;

	return true;
}

void HyEntity2d::DisableMouseInput()
{
	m_uiAttributes &= ~ATTRIBFLAG_MouseInput;
}

void HyEntity2d::ReverseDisplayOrder(bool bReverse)
{
	if(bReverse)
		m_uiAttributes |= ATTRIBFLAG_ReverseDisplayOrder;
	else
		m_uiAttributes &= ~ATTRIBFLAG_ReverseDisplayOrder;

	SetDisplayOrder(m_iDisplayOrder, false);
}

/*virtual*/ bool HyEntity2d::IsLoaded() const /*override*/
{
	for(uint32 i = 0; i < m_ChildList.size(); ++i)
	{
		if(m_ChildList[i]->IsLoaded() == false)
			return false;
	}

	return true;
}

/*virtual*/ void HyEntity2d::Load() /*override*/
{
	// Load any attached children
	for(uint32 i = 0; i < m_ChildList.size(); ++i)
		m_ChildList[i]->Load();
}

/*virtual*/ void HyEntity2d::Unload() /*override*/
{
	// Unload any attached children
	for(uint32 i = 0; i < m_ChildList.size(); ++i)
		m_ChildList[i]->Unload();
}

/*virtual*/ void HyEntity2d::NodeUpdate() /*override final*/
{
	if((m_uiAttributes & ATTRIBFLAG_MouseInput) != 0)
	{
		bool bLeftClickDown = Hy_Input().IsMouseBtnDown(HYMOUSE_BtnLeft);
		bool bMouseInBounds = m_pMouseInputNode->GetBoundingVolume().TestPoint(Hy_Input().GetWorldMousePos());

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

void HyEntity2d::SetNewChildAttributes(IHyNode2d &childInst)
{
	SetDirty(DIRTY_ALL);

	childInst._SetEnabled(m_bEnabled, false);
	childInst._SetPauseUpdate(m_bPauseOverride, false);
	childInst._SetCoordinateSystem(m_iCoordinateSystem, false);

	if(m_pScissor != nullptr)
		childInst._SetScissor(m_pScissor->m_WorldScissorRect, false);

	int32 iOrderValue = m_iDisplayOrder;
	for(uint32 i = 0; i < m_ChildList.size(); ++i)
		iOrderValue = m_ChildList[i]->_SetDisplayOrder(iOrderValue, false);
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

	if(0 == (m_uiExplicitFlags & EXPLICIT_Enabled))
	{
		for(uint32 i = 0; i < m_ChildList.size(); ++i)
			m_ChildList[i]->_SetEnabled(m_bEnabled, bIsOverriding);
	}
}

/*virtual*/ void HyEntity2d::_SetPauseUpdate(bool bUpdateWhenPaused, bool bIsOverriding) /*override final*/
{
	IHyNode::_SetPauseUpdate(bUpdateWhenPaused, bIsOverriding);

	if(0 == (m_uiExplicitFlags & EXPLICIT_PauseUpdate))
	{
		for(uint32 i = 0; i < m_ChildList.size(); ++i)
			m_ChildList[i]->_SetPauseUpdate(m_bPauseOverride, bIsOverriding);
	}
}

/*virtual*/ void HyEntity2d::_SetScissor(const HyScreenRect<int32> &worldScissorRectRef, bool bIsOverriding) /*override final*/
{
	if(bIsOverriding)
		m_uiExplicitFlags &= ~EXPLICIT_Scissor;

	if(0 == (m_uiExplicitFlags & EXPLICIT_Scissor))
	{
		if(m_pScissor == nullptr)
			m_pScissor = HY_NEW ScissorRect();

		m_pScissor->m_WorldScissorRect = worldScissorRectRef;

		for(uint32 i = 0; i < m_ChildList.size(); ++i)
			m_ChildList[i]->_SetScissor(m_pScissor->m_WorldScissorRect, bIsOverriding);
	}
}

/*virtual*/ void HyEntity2d::_SetStencil(HyStencilHandle hHandle, bool bIsOverriding) /*override final*/
{
	if(bIsOverriding)
		m_uiExplicitFlags &= ~EXPLICIT_Stencil;

	if(0 == (m_uiExplicitFlags & EXPLICIT_Stencil))
	{
		m_hStencil = hHandle;

		for(uint32 i = 0; i < m_ChildList.size(); ++i)
			m_ChildList[i]->_SetStencil(m_hStencil, bIsOverriding);
	}
}

/*virtual*/ void HyEntity2d::_SetCoordinateSystem(int32 iWindowIndex, bool bIsOverriding) /*override final*/
{
	if(bIsOverriding)
		m_uiExplicitFlags &= ~EXPLICIT_CoordinateSystem;

	if(0 == (m_uiExplicitFlags & EXPLICIT_CoordinateSystem))
	{
		m_iCoordinateSystem = iWindowIndex;

		for(uint32 i = 0; i < m_ChildList.size(); ++i)
			m_ChildList[i]->_SetCoordinateSystem(iWindowIndex, bIsOverriding);
	}
}

/*virtual*/ int32 HyEntity2d::_SetDisplayOrder(int32 iOrderValue, bool bIsOverriding) /*override final*/
{
	if(bIsOverriding)
		m_uiExplicitFlags &= ~EXPLICIT_DisplayOrder;

	if(0 == (m_uiExplicitFlags & EXPLICIT_DisplayOrder))
	{
		m_iDisplayOrder = iOrderValue;

		for(uint32 i = 0; i < m_ChildList.size(); ++i)
			iOrderValue = m_ChildList[i]->_SetDisplayOrder(iOrderValue, bIsOverriding);
	}

	return iOrderValue;
}
