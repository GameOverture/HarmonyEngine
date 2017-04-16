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

HyEntity2d::HyEntity2d(HyEntity2d *pParent /*= nullptr*/) :	IHyNode2d(HYTYPE_Entity2d, pParent),
															m_uiAttributes(0),
															m_eMouseInputState(MOUSEINPUT_None),
															m_pMouseInputUserParam(nullptr),
															m_iDisplayOrder(0),
															color(*this),
															scissor(*this)
{
}

HyEntity2d::~HyEntity2d(void)
{
	while(m_ChildList.empty() == false)
		m_ChildList[m_ChildList.size() - 1]->ParentDetach();
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

int32 HyEntity2d::SetDisplayOrder(int32 iOrderValue, bool bOverrideExplicitChildren)
{
	m_iDisplayOrder = iOrderValue;
	m_uiExplicitFlags |= EXPLICIT_DisplayOrder;

	for(uint32 i = 0; i < m_ChildList.size(); ++i)
		iOrderValue = m_ChildList[i]->_SetDisplayOrder(iOrderValue, bOverrideExplicitChildren);
}

void HyEntity2d::ChildAppend(IHyNode2d &childInst)
{
	childInst.ParentDetach();
	childInst.m_pParent = this;

	m_ChildList.push_back(&childInst);
	SetNewChildAttributes(childInst);
}

bool HyEntity2d::ChildInsert(IHyNode2d &insertBefore, IHyNode2d &childInst)
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

bool HyEntity2d::ChildRemove(IHyNode2d *pChild)
{
	for(auto iter = m_ChildList.begin(); iter != m_ChildList.end(); ++iter)
	{
		if(*iter == this)
		{
			(*iter)->m_pParent = nullptr;
			m_ChildList.erase(iter);
			return true;
		}
	}

	return false;
}

void HyEntity2d::ChildrenTransfer(HyEntity2d &newParent)
{
	while(m_ChildList.empty() == false)
		newParent.ChildAppend(*m_ChildList[0]);
}

uint32 HyEntity2d::ChildCount()
{
	return static_cast<uint32>(m_ChildList.size());
}

IHyNode2d *HyEntity2d::ChildGet(uint32 uiIndex)
{
	HyAssert(uiIndex < static_cast<uint32>(m_ChildList.size()), "IHyNode2d::ChildGet passed an invalid index");
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

/*virtual*/ void HyEntity2d::SetDirty(HyNodeDirtyType eDirtyType)
{
	IHyNode2d::SetDirty(eDirtyType);

	for(uint32 i = 0; i < m_ChildList.size(); ++i)
		m_ChildList[i]->SetDirty(eDirtyType);
}

/*virtual*/ void HyEntity2d::SetNewChildAttributes(IHyNode2d &childInst)
{
	childInst._SetEnabled(m_bEnabled, false);
	childInst._SetPauseUpdate(m_bPauseOverride, false);
	childInst._SetScissor(scissor, false);
	//childInst._SetDisplayOrder(
}

/*virtual*/ void HyEntity2d::_SetEnabled(bool bEnabled, bool bIsOverriding)
{
	IHyNode::_SetEnabled(bEnabled, bIsOverriding);

	if(0 == (m_uiExplicitFlags & EXPLICIT_Enabled))
	{
		for(uint32 i = 0; i < m_ChildList.size(); ++i)
			m_ChildList[i]->_SetEnabled(m_bEnabled, bIsOverriding);
	}
}

/*virtual*/ void HyEntity2d::_SetPauseUpdate(bool bUpdateWhenPaused, bool bIsOverriding)
{
	IHyNode::_SetPauseUpdate(bUpdateWhenPaused, bIsOverriding);

	if(0 == (m_uiExplicitFlags & EXPLICIT_PauseUpdate))
	{
		for(uint32 i = 0; i < m_ChildList.size(); ++i)
			m_ChildList[i]->_SetPauseUpdate(m_bPauseOverride, bIsOverriding);
	}
}

/*virtual*/ void HyEntity2d::_SetScissor(HyScissor &scissorRef, bool bIsOverriding)
{
	if(bIsOverriding)
		m_uiExplicitFlags &= ~EXPLICIT_Scissor;

	if(0 == (m_uiExplicitFlags & EXPLICIT_Scissor))
	{
		scissor = scissorRef;

		for(uint32 i = 0; i < m_ChildList.size(); ++i)
			m_ChildList[i]->_SetScissor(scissor, bIsOverriding);
	}
}

/*virtual*/ int32 HyEntity2d::_SetDisplayOrder(int32 iOrderValue, bool bIsOverriding)
{
}
