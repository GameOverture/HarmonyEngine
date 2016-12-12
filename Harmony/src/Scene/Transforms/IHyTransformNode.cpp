/**************************************************************************
*	IHyTransformNode.cpp
*
*	Harmony Engine
*	Copyright (c) 2016 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Scene/Transforms/IHyTransformNode.h"
#include "Scene/Transforms/Tweens/HyTweenFloat.h"

IHyTransformNode::IHyTransformNode(HyType eType) :	m_eTYPE(eType),
													m_bEnabled(true),
													m_pParent(NULL),
													m_bDirty(false),
													m_iTag(0)
{
	HyScene::AddTransformNode(this);
}

/*virtual*/ IHyTransformNode::~IHyTransformNode()
{
	HyScene::RemoveTransformNode(this);
}

HyType IHyTransformNode::GetType()
{
	return m_eTYPE;
}

bool IHyTransformNode::IsEnabled()
{
	return m_bEnabled;
}

/*virtual*/ void IHyTransformNode::SetEnabled(bool bEnabled)
{
	m_bEnabled = bEnabled;
}

int64 IHyTransformNode::GetTag()
{
	return m_iTag;
}

void IHyTransformNode::SetTag(int64 iTag)
{
	m_iTag = iTag;
}

void IHyTransformNode::AddChild(IHyTransformNode &childInst)
{
	childInst.Detach();

	childInst.m_pParent = this;
	m_ChildList.push_back(&childInst);
}

void IHyTransformNode::Detach()
{
	if(m_pParent == NULL)
		return;

	for(std::vector<IHyTransformNode *>::iterator iter = m_pParent->m_ChildList.begin(); iter != m_pParent->m_ChildList.end(); ++iter)
	{
		if(*iter == this)
		{
			m_pParent->m_ChildList.erase(iter);
			m_pParent = NULL;
			return;
		}
	}

	HyError("IHyTransformNode::Detach() could not find itself in parent's child list");
}

void IHyTransformNode::Update()
{
	// Update any currently active AnimFloat in the game, and remove any of them that are finished.
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

	OnUpdate();
}

void IHyTransformNode::SetDirty()
{
	m_bDirty = true;

	for(uint32 i = 0; i < m_ChildList.size(); ++i)
		m_ChildList[i]->SetDirty();
}

void IHyTransformNode::InsertActiveAnimFloat(HyTweenFloat *pAnimFloat)
{
	if(pAnimFloat->m_bAddedToOwnerUpdate == false)
	{
		pAnimFloat->m_bAddedToOwnerUpdate = true;
		m_ActiveAnimFloatsList.push_back(pAnimFloat);
	}
}
