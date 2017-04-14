/**************************************************************************
*	IHyNode2d.cpp
*
*	Harmony Engine
*	Copyright (c) 2016 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Scene/Nodes/IHyNode2d.h"
#include "Scene/Nodes/Transforms/Tweens/HyTweenFloat.h"

IHyNode2d::IHyNode2d(HyType eNodeType, IHyNode2d *pParent) :	m_eTYPE(eNodeType),
														m_bDirty(false),
														m_bIsDraw2d(false),
														m_bEnabled(true),
														m_bPauseOverride(false),
														m_pParent(nullptr),
														m_uiExplicitFlags(0),
														m_iTag(0)
{
	HyScene::AddNode(this);
	
	if(pParent)
		pParent->ChildAppend(*this);
}

/*virtual*/ IHyNode2d::~IHyNode2d()
{
	if(m_ChildList.empty() == false)
	{
		//HyLogWarning("Deleting Scene Node with '" << m_ChildList.size() << "' children attached. These children are now orphaned");
		for(uint32 i = 0; i < m_ChildList.size(); ++i)
			m_ChildList[i]->ParentDetach();
	}

	ParentDetach();
	HyScene::RemoveNode(this);

	if(m_bPauseOverride)
		HyScene::RemoveNode_PauseUpdate(this);
}

HyType IHyNode2d::GetType()
{
	return m_eTYPE;
}

bool IHyNode2d::IsDraw2d()
{
	return m_bIsDraw2d;
}

bool IHyNode2d::IsEnabled()
{
	return m_bEnabled;
}

void IHyNode2d::SetEnabled(bool bEnabled, bool bOverrideExplicitChildren /*= true*/)
{
	m_bEnabled = bEnabled;
	m_uiExplicitFlags |= EXPLICIT_Enabled;

	for(uint32 i = 0; i < m_ChildList.size(); ++i)
		m_ChildList[i]->_SetEnabled(bEnabled, bOverrideExplicitChildren);
}

void IHyNode2d::SetPauseUpdate(bool bUpdateWhenPaused, bool bOverrideExplicitChildren /*= true*/)
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

int64 IHyNode2d::GetTag()
{
	return m_iTag;
}

void IHyNode2d::SetTag(int64 iTag)
{
	m_iTag = iTag;
}

void IHyNode2d::ChildAppend(IHyNode2d &childInst)
{
	childInst.ParentDetach();
	childInst.m_pParent = this;

	m_ChildList.push_back(&childInst);
	SetNewChildAttributes(childInst);
}

bool IHyNode2d::ChildInsert(IHyNode2d &insertBefore, IHyNode2d &childInst)
{
	for(auto iter = m_ChildList.begin(); iter != m_ChildList.end(); ++iter)
	{
		if((*iter) == &insertBefore || (*iter)->ChildFind(insertBefore))
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

bool IHyNode2d::ChildFind(IHyNode2d &childInst)
{
	for(auto iter = m_ChildList.begin(); iter != m_ChildList.end(); ++iter)
	{
		if((*iter) == &childInst)
			return true;
		else if((*iter)->m_ChildList.empty() == false)
		{
			if((*iter)->ChildFind(childInst))
				return true;
		}
	}

	return false;
}

void IHyNode2d::ChildrenTransfer(IHyNode2d &newParent)
{
	while(m_ChildList.empty() == false)
		newParent.ChildAppend(*m_ChildList[0]);
}

uint32 IHyNode2d::ChildCount()
{
	return static_cast<uint32>(m_ChildList.size());
}

IHyNode2d *IHyNode2d::ChildGet(uint32 uiIndex)
{
	HyAssert(uiIndex < static_cast<uint32>(m_ChildList.size()), "IHyNode2d::ChildGet passed an invalid index");
	return m_ChildList[uiIndex];
}

void IHyNode2d::ParentDetach()
{
	if(m_pParent == nullptr)
		return;

	for(std::vector<IHyNode2d *>::iterator iter = m_pParent->m_ChildList.begin(); iter != m_pParent->m_ChildList.end(); ++iter)
	{
		if(*iter == this)
		{
			m_pParent->m_ChildList.erase(iter);
			m_pParent = nullptr;
			return;
		}
	}

	HyError("IHyNode2d::ParentDetach() could not find itself in parent's child list");
}

bool IHyNode2d::ParentExists()
{
	return m_pParent != nullptr;
}

void IHyNode2d::ForEachChild(std::function<void(IHyNode2d *)> func)
{
	func(this);

	for(uint32 i = 0; i < m_ChildList.size(); ++i)
		m_ChildList[i]->ForEachChild(func);
}

void IHyNode2d::Update()
{
	// Update any currently active AnimFloat associated with this transform, and remove any of them that are finished.
	for(std::vector<HyTweenFloat *>::iterator iter = m_ActiveAnimFloatsList.begin(); iter != m_ActiveAnimFloatsList.end();)
	{
		if((*iter)->UpdateFloat())
		{
			(*iter)->m_bAddedToOwnerUpdate = false;
			iter = m_ActiveAnimFloatsList.erase(iter);
		}
		else
			++iter;
	}

	// TODO: Process the action queue

	InstUpdate();
}

void IHyNode2d::SetDirty()
{
	m_bDirty = true;

	for(uint32 i = 0; i < m_ChildList.size(); ++i)
		m_ChildList[i]->SetDirty();
}

void IHyNode2d::InsertActiveAnimFloat(HyTweenFloat *pAnimFloat)
{
	if(pAnimFloat->m_bAddedToOwnerUpdate == false)
	{
		pAnimFloat->m_bAddedToOwnerUpdate = true;
		m_ActiveAnimFloatsList.push_back(pAnimFloat);
	}
}

/*virtual*/ void IHyNode2d::SetNewChildAttributes(IHyNode2d &childInst)
{
	//childInst.m_uiExplicitFlags = 0;

	childInst._SetEnabled(m_bEnabled, false);
	childInst._SetPauseUpdate(m_bPauseOverride, false);
}

void IHyNode2d::_SetEnabled(bool bEnabled, bool bOverrideExplicitChildren)
{
	if(bOverrideExplicitChildren)
		m_uiExplicitFlags &= ~EXPLICIT_Enabled;

	if(0 == (m_uiExplicitFlags & EXPLICIT_Enabled))
	{
		m_bEnabled = bEnabled;

		for(uint32 i = 0; i < m_ChildList.size(); ++i)
			m_ChildList[i]->_SetEnabled(m_bEnabled, bOverrideExplicitChildren);
	}
}

void IHyNode2d::_SetPauseUpdate(bool bUpdateWhenPaused, bool bOverrideExplicitChildren)
{
	if(bOverrideExplicitChildren)
		m_uiExplicitFlags &= ~EXPLICIT_PauseUpdate;

	if(0 == (m_uiExplicitFlags & EXPLICIT_PauseUpdate))
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
}
