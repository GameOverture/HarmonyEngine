/**************************************************************************
 *	HyEntity2d.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Scene/Nodes/Entities/HyEntity2d.h"
#include "Scene/HyScene.h"
#include "HyEngine.h"

HyEntity2d::HyEntity2d(HyEntity2d *pParent /*= nullptr*/) :	IHyNodeDraw2d(HYTYPE_Entity2d, pParent),
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

void HyEntity2d::SetEnabled(bool bEnabled, bool bOverrideExplicitChildren /*= true*/)
{
	m_bEnabled = bEnabled;
	m_uiExplicitFlags |= EXPLICIT_Enabled;

	for(uint32 i = 0; i < m_ChildList.size(); ++i)
		m_ChildList[i]->_SetEnabled(bEnabled, bOverrideExplicitChildren);
}

void HyEntity2d::SetPauseUpdate(bool bUpdateWhenPaused, bool bOverrideExplicitChildren /*= true*/)
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

void HyEntity2d::SetScissor(int32 uiLocalX, int32 uiLocalY, uint32 uiWidth, uint32 uiHeight, bool bOverrideExplicitChildren /*= true*/)
{
	m_LocalScissorRect.x = uiLocalX;
	m_LocalScissorRect.y = uiLocalY;
	m_LocalScissorRect.width = uiWidth;
	m_LocalScissorRect.height = uiHeight;

	m_LocalScissorRect.iTag = 1;
	m_uiExplicitFlags |= EXPLICIT_Scissor;

	glm::mat4 mtx;
	GetWorldTransform(mtx);
	m_WorldScissorRect.x = static_cast<int32>(mtx[3].x + m_LocalScissorRect.x);
	m_WorldScissorRect.y = static_cast<int32>(mtx[3].y + m_LocalScissorRect.y);
	m_WorldScissorRect.width = static_cast<int32>(mtx[0].x * m_LocalScissorRect.width);
	m_WorldScissorRect.height = static_cast<int32>(mtx[1].y * m_LocalScissorRect.height);
	m_WorldScissorRect.iTag = 1;

	for(uint32 i = 0; i < m_ChildList.size(); ++i)
		m_ChildList[i]->_SetScissor(m_WorldScissorRect, bOverrideExplicitChildren);
}

void HyEntity2d::ClearScissor(bool bUseParentScissor, bool bOverrideExplicitChildren /*= true*/)
{
	m_LocalScissorRect.iTag = 0;

	if(bUseParentScissor == false)
	{
		m_uiExplicitFlags |= EXPLICIT_Scissor;

		m_WorldScissorRect.iTag = 0;

		for(uint32 i = 0; i < m_ChildList.size(); ++i)
			m_ChildList[i]->_SetScissor(m_WorldScissorRect, bOverrideExplicitChildren);
	}
	else
		m_uiExplicitFlags &= ~EXPLICIT_Scissor;
}

void HyEntity2d::SetDisplayOrder(int32 iOrderValue, bool bOverrideExplicitChildren /*= true*/)
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
		for(size_t i = m_ChildList.size() - 1; i >= 0; --i)
			iOrderValue = m_ChildList[i]->_SetDisplayOrder(iOrderValue, bOverrideExplicitChildren);
	}
}

void HyEntity2d::ChildAppend(IHyNode2d &childInst)
{
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

void HyEntity2d::EnableMouseInput(bool bEnable, void *pUserParam /*= NULL*/)
{
	if(bEnable)
		m_uiAttributes |= (ATTRIBFLAG_MouseInput | ATTRIBFLAG_BoundingVolumeDirty);
	else
		m_uiAttributes &= ~ATTRIBFLAG_MouseInput;

	m_pMouseInputUserParam = pUserParam;
}

void HyEntity2d::EnableCollider(bool bEnable)
{
	if(bEnable)
		m_uiAttributes |= (ATTRIBFLAG_HasBoundingVolume | ATTRIBFLAG_BoundingVolumeDirty);
	else
		m_uiAttributes &= ~ATTRIBFLAG_HasBoundingVolume;
}

void HyEntity2d::EnablePhysics(bool bEnable)
{
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
	if((m_uiExplicitFlags & EXPLICIT_Scissor) != 0)
	{
		if(m_LocalScissorRect.iTag == 1)
		{
			glm::mat4 mtx;
			GetWorldTransform(mtx);

			m_WorldScissorRect.x = static_cast<int32>(mtx[3].x + m_LocalScissorRect.x);
			m_WorldScissorRect.y = static_cast<int32>(mtx[3].y + m_LocalScissorRect.y);
			m_WorldScissorRect.width = static_cast<int32>(mtx[0].x * m_LocalScissorRect.width);
			m_WorldScissorRect.height = static_cast<int32>(mtx[1].y * m_LocalScissorRect.height);
			m_WorldScissorRect.iTag = 1;
		}
		else
			m_WorldScissorRect.iTag = 0;

		for(uint32 i = 0; i < m_ChildList.size(); ++i)
			m_ChildList[i]->_SetScissor(m_WorldScissorRect, false);
	}

	if((m_uiAttributes & (ATTRIBFLAG_HasBoundingVolume | ATTRIBFLAG_MouseInput)) != 0)
	{
		if(m_uiAttributes & ATTRIBFLAG_BoundingVolumeDirty)
		{
			OnCalcBoundingVolume();
			m_uiAttributes &= ~ATTRIBFLAG_BoundingVolumeDirty;
		}

		if((m_uiAttributes & ATTRIBFLAG_MouseInput) != 0)
		{
			bool bLeftClickDown = IHyInputMap::IsMouseLeftDown();
			bool bMouseInBounds = m_BoundingVolume.IsWorldPointCollide(IHyInputMap::GetWorldMousePos());

			switch(m_eMouseInputState)
			{
			case MOUSEINPUT_None:
				if(bLeftClickDown == false && bMouseInBounds)
				{
					m_eMouseInputState = MOUSEINPUT_Hover;
					OnMouseEnter(m_pMouseInputUserParam);
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
				if(bLeftClickDown == false)
				{
					m_eMouseInputState = MOUSEINPUT_None;
					OnMouseUp(m_pMouseInputUserParam);

					if(bMouseInBounds)
						OnMouseClicked(m_pMouseInputUserParam);
				}
				break;
			}
		}
	}

	OnUpdate();
}

/*virtual*/ void HyEntity2d::SetDirty(HyNodeDirtyType eDirtyType)
{
	IHyNode2d::SetDirty(eDirtyType);

	for(uint32 i = 0; i < m_ChildList.size(); ++i)
		m_ChildList[i]->SetDirty(eDirtyType);
}

/*virtual*/ void HyEntity2d::SetNewChildAttributes(IHyNode2d &childInst)
{
	SetDirty(HYNODEDIRTY_Transform);
	SetDirty(HYNODEDIRTY_Color);

	childInst._SetEnabled(m_bEnabled, false);
	childInst._SetPauseUpdate(m_bPauseOverride, false);
	childInst._SetScissor(m_WorldScissorRect, false);

	int32 iOrderValue = m_iDisplayOrder;
	for(uint32 i = 0; i < m_ChildList.size(); ++i)
		iOrderValue = m_ChildList[i]->_SetDisplayOrder(iOrderValue, false);
}

/*virtual*/ void HyEntity2d::_SetEnabled(bool bEnabled, bool bIsOverriding) /*override*/
{
	IHyNode::_SetEnabled(bEnabled, bIsOverriding);

	if(0 == (m_uiExplicitFlags & EXPLICIT_Enabled))
	{
		for(uint32 i = 0; i < m_ChildList.size(); ++i)
			m_ChildList[i]->_SetEnabled(m_bEnabled, bIsOverriding);
	}
}

/*virtual*/ void HyEntity2d::_SetPauseUpdate(bool bUpdateWhenPaused, bool bIsOverriding) /*override*/
{
	IHyNode::_SetPauseUpdate(bUpdateWhenPaused, bIsOverriding);

	if(0 == (m_uiExplicitFlags & EXPLICIT_PauseUpdate))
	{
		for(uint32 i = 0; i < m_ChildList.size(); ++i)
			m_ChildList[i]->_SetPauseUpdate(m_bPauseOverride, bIsOverriding);
	}
}

/*virtual*/ void HyEntity2d::_SetScissor(const HyScreenRect<int32> &worldScissorRectRef, bool bIsOverriding) /*override*/
{
	if(bIsOverriding)
		m_uiExplicitFlags &= ~EXPLICIT_Scissor;

	if(0 == (m_uiExplicitFlags & EXPLICIT_Scissor))
	{
		m_WorldScissorRect = worldScissorRectRef;

		for(uint32 i = 0; i < m_ChildList.size(); ++i)
			m_ChildList[i]->_SetScissor(worldScissorRectRef, bIsOverriding);
	}
}

/*virtual*/ int32 HyEntity2d::_SetDisplayOrder(int32 iOrderValue, bool bIsOverriding) /*override*/
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
