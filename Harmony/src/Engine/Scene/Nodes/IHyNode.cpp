/**************************************************************************
*	IHyNode.cpp
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/IHyNode.h"
#include "Scene/HyScene.h"
#include "Scene/Tweens/HyTweenFloat.h"

IHyNode::IHyNode(HyType eNodeType) :
	m_eTYPE(eNodeType),
	m_uiDirtyFlags(0),
	m_uiExplicitAndTypeFlags(0),
	m_bVisible(true),
	m_bPauseOverride(false),
	m_iTag(0)
{
	HyScene::AddNode(this);
}

IHyNode::IHyNode(const IHyNode &copyRef) :
	m_eTYPE(copyRef.m_eTYPE),
	m_uiDirtyFlags(copyRef.m_uiDirtyFlags),
	m_uiExplicitAndTypeFlags(copyRef.m_uiExplicitAndTypeFlags),
	m_bVisible(copyRef.m_bVisible),
	m_bPauseOverride(copyRef.m_bPauseOverride),
	m_iTag(copyRef.m_iTag)
{
	HyScene::AddNode(this);

	if(m_bPauseOverride)
		HyScene::AddNode_PauseUpdate(this);
}

/*virtual*/ IHyNode::~IHyNode()
{
	if(m_bPauseOverride)
		HyScene::RemoveNode_PauseUpdate(this);

	HyScene::RemoveNode(this);
}

const IHyNode &IHyNode::operator=(const IHyNode &rhs)
{
	HyAssert(m_eTYPE == rhs.m_eTYPE, "IHyNode::operator= cannot assign from a different HyType");

	m_uiDirtyFlags = rhs.m_uiDirtyFlags;
	m_uiExplicitAndTypeFlags = rhs.m_uiExplicitAndTypeFlags;
	m_bVisible = rhs.m_bVisible;

	if(m_bPauseOverride != rhs.m_bPauseOverride)
	{
		m_bPauseOverride = rhs.m_bPauseOverride;
		if(m_bPauseOverride)
			HyScene::AddNode_PauseUpdate(this);
		else
			HyScene::RemoveNode_PauseUpdate(this);
	}
	
	m_iTag = rhs.m_iTag;

	return *this;
}

HyType IHyNode::GetType() const
{
	return m_eTYPE;
}

bool IHyNode::Is2D() const
{
	return 0 != (m_uiExplicitAndTypeFlags & NODETYPE_Is2d);
}

uint32 IHyNode::GetExplicitAndTypeFlags() const
{
	return m_uiExplicitAndTypeFlags;
}

bool IHyNode::IsVisible() const
{
	return m_bVisible;
}

/*virtual*/ void IHyNode::SetVisible(bool bEnabled)
{
	m_bVisible = bEnabled;
	m_uiExplicitAndTypeFlags |= EXPLICIT_Visible;
}

bool IHyNode::IsPauseUpdate() const
{
	return m_bPauseOverride;
}

/*virtual*/ void IHyNode::SetPauseUpdate(bool bUpdateWhenPaused)
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
}

int64 IHyNode::GetTag() const
{
	return m_iTag;
}

void IHyNode::SetTag(int64 iTag)
{
	m_iTag = iTag;
}

/*virtual*/ void IHyNode::Update()
{
	// Update any currently active AnimFloat associated with this transform, and remove any of them that are finished.
	for(std::vector<HyTweenFloat *>::iterator iter = m_ActiveTweenFloatsList.begin(); iter != m_ActiveTweenFloatsList.end();)
	{
		if((*iter)->UpdateFloat())
		{
			(*iter)->m_bAddedToOwnerUpdate = false;
			iter = m_ActiveTweenFloatsList.erase(iter);
		}
		else
			++iter;
	}
}

/*virtual*/ void IHyNode::_SetVisible(bool bEnabled, bool bIsOverriding)
{
	if(bIsOverriding)
		m_uiExplicitAndTypeFlags &= ~EXPLICIT_Visible;

	if(0 == (m_uiExplicitAndTypeFlags & EXPLICIT_Visible))
		m_bVisible = bEnabled;
}

/*virtual*/ void IHyNode::_SetPauseUpdate(bool bUpdateWhenPaused, bool bIsOverriding)
{
	if(bIsOverriding)
		m_uiExplicitAndTypeFlags &= ~EXPLICIT_PauseUpdate;

	if(0 == (m_uiExplicitAndTypeFlags & EXPLICIT_PauseUpdate))
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
	}
}

/*virtual*/ void IHyNode::SetDirty(uint32 uiDirtyFlags)
{
	m_uiDirtyFlags |= uiDirtyFlags;
}

bool IHyNode::IsDirty(NodeDirtyFlag eDirtyType)
{
	return ((m_uiDirtyFlags & eDirtyType) != 0);
}

void IHyNode::ClearDirty(NodeDirtyFlag eDirtyType)
{
	m_uiDirtyFlags &= ~eDirtyType;
}

void IHyNode::InsertActiveTweenFloat(HyTweenFloat *pTweenFloat)
{
	if(pTweenFloat->m_bAddedToOwnerUpdate == false)
	{
		pTweenFloat->m_bAddedToOwnerUpdate = true;
		m_ActiveTweenFloatsList.push_back(pTweenFloat);
	}
}
