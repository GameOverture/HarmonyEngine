/**************************************************************************
*	IHyNode.cpp
*
*	Harmony Engine
*	Copyright (c) 2016 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Scene/Nodes/IHyNode.h"
#include "Scene/Nodes/Transforms/Tweens/HyTweenFloat.h"

IHyNode::IHyNode(HyType eType) :	m_eTYPE(eType),
									m_bDirty(false),
									m_bIsDraw2d(false),
									m_bEnabled(true),
									m_pParent(NULL),
									m_iTag(0)
{
	HyScene::AddNode(this);
}

/*virtual*/ IHyNode::~IHyNode()
{
	if(m_ChildList.empty() == false)
	{
		//HyLogWarning("Deleting Scene Node with '" << m_ChildList.size() << "' children attached. These children are now orphaned");
		for(uint32 i = 0; i < m_ChildList.size(); ++i)
			m_ChildList[i]->Detach();
	}

	HyScene::RemoveNode(this);
}

HyType IHyNode::GetType()
{
	return m_eTYPE;
}

bool IHyNode::IsDraw2d()
{
	return m_bIsDraw2d;
}

bool IHyNode::IsEnabled()
{
	return m_bEnabled;
}

void IHyNode::SetEnabled(bool bEnabled)
{
	m_bEnabled = bEnabled;

	for(uint32 i = 0; i < m_ChildList.size(); ++i)
		m_ChildList[i]->SetEnabled(m_bEnabled);
}

int64 IHyNode::GetTag()
{
	return m_iTag;
}

void IHyNode::SetTag(int64 iTag)
{
	m_iTag = iTag;
}

void IHyNode::AddChild(IHyNode &childInst)
{
	childInst.Detach();

	childInst.m_pParent = this;
	m_ChildList.push_back(&childInst);
}

void IHyNode::Detach()
{
	if(m_pParent == NULL)
		return;

	for(std::vector<IHyNode *>::iterator iter = m_pParent->m_ChildList.begin(); iter != m_pParent->m_ChildList.end(); ++iter)
	{
		if(*iter == this)
		{
			m_pParent->m_ChildList.erase(iter);
			m_pParent = NULL;
			return;
		}
	}

	HyError("IHyNode::Detach() could not find itself in parent's child list");
}

void IHyNode::Update()
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

void IHyNode::SetDirty()
{
	m_bDirty = true;

	for(uint32 i = 0; i < m_ChildList.size(); ++i)
		m_ChildList[i]->SetDirty();
}

void IHyNode::InsertActiveAnimFloat(HyTweenFloat *pAnimFloat)
{
	if(pAnimFloat->m_bAddedToOwnerUpdate == false)
	{
		pAnimFloat->m_bAddedToOwnerUpdate = true;
		m_ActiveAnimFloatsList.push_back(pAnimFloat);
	}
}
