/**************************************************************************
*	IHyNode.cpp
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Scene/Nodes/IHyNode.h"
#include "Scene/HyScene.h"
#include "Scene/Nodes/Tweens/HyTweenFloat.h"

IHyNode::IHyNode(HyType eNodeType) :	m_eTYPE(eNodeType),
										m_uiExplicitFlags(0),
										m_uiTweenDirtyFlags(0),
										m_bEnabled(true),
										m_bPauseOverride(false),
										m_iTag(0)
{
}

IHyNode::~IHyNode()
{
}

HyType IHyNode::GetType()
{
	return m_eTYPE;
}

bool IHyNode::IsEnabled()
{
	return m_bEnabled;
}

int64 IHyNode::GetTag()
{
	return m_iTag;
}

void IHyNode::SetTag(int64 iTag)
{
	m_iTag = iTag;
}

void IHyNode::_SetEnabled(bool bEnabled, bool bIsOverriding)
{
	if(bIsOverriding)
		m_uiExplicitFlags &= ~EXPLICIT_Enabled;

	if(0 == (m_uiExplicitFlags & EXPLICIT_Enabled))
		m_bEnabled = bEnabled;
}

void IHyNode::_SetPauseUpdate(bool bUpdateWhenPaused, bool bIsOverriding)
{
	if(bIsOverriding)
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
	}
}

/*virtual*/ void IHyNode::SetDirty(HyNodeDirtyType eDirtyType)
{
	m_uiTweenDirtyFlags |= static_cast<uint32>(eDirtyType);
}

bool IHyNode::IsDirty(HyNodeDirtyType eDirtyType)
{
	return ((m_uiTweenDirtyFlags & eDirtyType) != 0);
}

void IHyNode::ClearDirty(HyNodeDirtyType eDirtyType)
{
	m_uiTweenDirtyFlags &= ~eDirtyType;
}

void IHyNode::InsertActiveTweenFloat(HyTweenFloat *pTweenFloat)
{
	if(pTweenFloat->m_bAddedToOwnerUpdate == false)
	{
		pTweenFloat->m_bAddedToOwnerUpdate = true;
		m_ActiveTweenFloatsList.push_back(pTweenFloat);
	}
}

void IHyNode::Update()
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

	// TODO: Process the action queue

	NodeUpdate();
}
