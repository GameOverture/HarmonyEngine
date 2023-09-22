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
#include "Scene/AnimFloats/HyAnimFloat.h"

HyScene *IHyNode::sm_pScene = nullptr;

IHyNode::IHyNode(HyType eNodeType) :
	m_uiFlags(static_cast<uint32>(eNodeType) | SETTING_IsVisible | EXPLICIT_ParentsVisible)
#ifdef HY_ENABLE_USER_TAGS
	, m_iTag(0)
#endif
{
	SetRegistered(true);
}

IHyNode::IHyNode(const IHyNode &copyRef) :
	m_uiFlags(0),
#ifdef HY_ENABLE_USER_TAGS
	m_iTag(copyRef.m_iTag)
#endif
{
	if(copyRef.IsRegistered())
		SetRegistered(true);

	if(copyRef.IsPauseUpdate())
		SetPauseUpdate(true);

	m_uiFlags = copyRef.m_uiFlags;
}

IHyNode::IHyNode(IHyNode &&donor) noexcept :
	m_uiFlags(0)
#ifdef HY_ENABLE_USER_TAGS
	, m_iTag(std::move(donor.m_iTag))
#endif
{
	if(donor.IsRegistered())
	{
		donor.SetRegistered(false);
		SetRegistered(true);
	}

	if(donor.IsPauseUpdate())
	{
		donor.SetPauseUpdate(false);
		SetPauseUpdate(true);
	}

	m_uiFlags = donor.m_uiFlags;
}

/*virtual*/ IHyNode::~IHyNode()
{
	SetPauseUpdate(false);
	SetRegistered(false);
}

IHyNode &IHyNode::operator=(const IHyNode &rhs)
{
	HyAssert(GetType() == rhs.GetType(), "IHyNode::operator= cannot assign from a different HyType");

	SetRegistered(rhs.IsRegistered());
	SetPauseUpdate(rhs.IsPauseUpdate());

	m_uiFlags = rhs.m_uiFlags;

#ifdef HY_ENABLE_USER_TAGS
	m_iTag = rhs.m_iTag;
#endif

	return *this;
}

IHyNode &IHyNode::operator=(IHyNode &&donor)
{
	if(donor.IsRegistered())
	{
		donor.SetRegistered(false);
		SetRegistered(true);
	}

	if(donor.IsPauseUpdate())
	{
		donor.SetPauseUpdate(false);
		SetPauseUpdate(true);
	}

	m_uiFlags = donor.m_uiFlags;

#ifdef HY_ENABLE_USER_TAGS
	m_iTag = donor.m_iTag;
#endif

	return *this;
}

HyType IHyNode::GetType() const
{
	return static_cast<HyType>(m_uiFlags & NODETYPE_HyType);
}

bool IHyNode::Is2D() const
{
	return 0 != (m_uiFlags & NODETYPE_Is2d);
}

bool IHyNode::IsVisible() const
{
	return 0 != (m_uiFlags & SETTING_IsVisible);
}

/*virtual*/ void IHyNode::SetVisible(bool bEnabled)
{
	if(bEnabled)
		m_uiFlags |= SETTING_IsVisible;
	else
		m_uiFlags &= ~SETTING_IsVisible;
}

bool IHyNode::IsPauseUpdate() const
{
	return 0 != (m_uiFlags & SETTING_IsPauseUpdate);
}

/*virtual*/ void IHyNode::SetPauseUpdate(bool bUpdateWhenPaused)
{
	if(bUpdateWhenPaused)
	{
		if(IsPauseUpdate() == false)
		{
			m_uiFlags |= SETTING_IsPauseUpdate;
			HyScene::AddNode_PauseUpdate(this);
		}
	}
	else
	{
		if(IsPauseUpdate())
		{
			m_uiFlags &= ~SETTING_IsPauseUpdate;
			HyScene::RemoveNode_PauseUpdate(this);
		}
	}

	m_uiFlags |= EXPLICIT_PauseUpdate;
}

uint32 IHyNode::GetInternalFlags() const
{
	return m_uiFlags;
}

#ifdef HY_ENABLE_USER_TAGS
int64_t IHyNode::GetTag() const
{
	return m_iTag;
}

void IHyNode::SetTag(int64_t iTag)
{
	m_iTag = iTag;
}
#endif

bool IHyNode::IsRegistered() const
{
	return 0 != (m_uiFlags & SETTING_IsRegistered);
}

/*virtual*/ void IHyNode::Update()
{
	// Update any currently active AnimFloat associated with this transform, and remove any of them that are finished.
	for(std::vector<HyAnimFloat *>::iterator iter = m_ActiveAnimFloatsList.begin(); iter != m_ActiveAnimFloatsList.end();)
	{
		// iter may be invalidated by the call to UpdateFloat() because the "AnimFinished" callback may add more AnimFloats to 'm_ActiveAnimFloatsList'
		// Store a pointer to the current HyAnimFloat in order to reestablish the iterator after the call to UpdateFloat()
		HyAnimFloat *pAnimFloat = *iter;

		if((*iter)->UpdateFloat())
		{
			// Use 'pAnimFloat' to reestablish the iterator
			for(int i = 0; i < m_ActiveAnimFloatsList.size(); ++i)
			{
				if(m_ActiveAnimFloatsList[i] == pAnimFloat)
				{
					iter = m_ActiveAnimFloatsList.begin() + i;
					break;
				}
			}

			(*iter)->m_bAddedToOwnerUpdate = false;
			iter = m_ActiveAnimFloatsList.erase(iter);
		}
		else
		{
			// Use 'pAnimFloat' to reestablish the iterator
			for(int i = 0; i < m_ActiveAnimFloatsList.size(); ++i)
			{
				if(m_ActiveAnimFloatsList[i] == pAnimFloat)
				{
					iter = m_ActiveAnimFloatsList.begin() + i;
					break;
				}
			}

			++iter;
		}
	}
}

/*virtual*/ void IHyNode::SetParentsVisible(bool bParentsVisible)
{
	if(bParentsVisible)
		m_uiFlags |= EXPLICIT_ParentsVisible;
	else
		m_uiFlags &= ~EXPLICIT_ParentsVisible;
}

/*virtual*/ void IHyNode::_SetPauseUpdate(bool bUpdateWhenPaused, bool bIsOverriding)
{
	if(bIsOverriding)
		m_uiFlags &= ~EXPLICIT_PauseUpdate;

	if(0 == (m_uiFlags & EXPLICIT_PauseUpdate))
	{
		if(bUpdateWhenPaused)
		{
			if(0 == (m_uiFlags & SETTING_IsPauseUpdate))
				HyScene::AddNode_PauseUpdate(this);
		}
		else
		{
			if(0 != (m_uiFlags & SETTING_IsPauseUpdate))
				HyScene::RemoveNode_PauseUpdate(this);
		}

		if(bUpdateWhenPaused)
			m_uiFlags |= SETTING_IsPauseUpdate;
		else
			m_uiFlags &= ~SETTING_IsPauseUpdate;
	}
}

/*virtual*/ void IHyNode::SetDirty(uint32 uiDirtyFlags)
{
	HyAssert((uiDirtyFlags & ~DIRTY_ALL) == 0, "IHyNode::SetDirty was passed flags that are not apart of the DirtyFlag enum");

	// Special cases
	if(uiDirtyFlags & DIRTY_Transform)
		uiDirtyFlags |= (DIRTY_SceneAABB | DIRTY_Collision);

	m_uiFlags |= uiDirtyFlags;
}

bool IHyNode::IsDirty(uint32 uiDirtyFlags) const
{
	HyAssert((uiDirtyFlags & ~DIRTY_ALL) == 0, "IHyNode::IsDirty was passed flags that are not apart of the DirtyFlag enum");
	return ((m_uiFlags & uiDirtyFlags) != 0);
}

void IHyNode::ClearDirty(uint32 uiDirtyFlags)
{
	HyAssert((uiDirtyFlags & ~DIRTY_ALL) == 0, "IHyNode::ClearDirty was passed flags that are not apart of the DirtyFlag enum");
	m_uiFlags &= ~uiDirtyFlags;
}

void IHyNode::SetRegistered(bool bRegister)
{
	if(bRegister)
	{
		if(IsRegistered() == false)
		{
			m_uiFlags |= SETTING_IsRegistered;
			HyScene::AddNode(this);
		}
	}
	else
	{
		if(IsRegistered())
		{
			m_uiFlags &= ~SETTING_IsRegistered;
			HyScene::RemoveNode(this);
		}
	}
}

void IHyNode::InsertActiveAnimFloat(HyAnimFloat *pAnimFloat)
{
	if(pAnimFloat->m_bAddedToOwnerUpdate == false)
	{
		pAnimFloat->m_bAddedToOwnerUpdate = true;
		m_ActiveAnimFloatsList.push_back(pAnimFloat);
	}
}
