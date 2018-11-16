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
#include "Scene/Nodes/Loadables/Visables/Objects/HyEntity3d.h"

HyEntity3d::HyEntity3d(const char *szPrefix, const char *szName, HyEntity3d *pParent) :	IHyVisable3d(HYTYPE_Entity, szPrefix, szName, pParent)
{
}

HyEntity3d::~HyEntity3d(void)
{
	//while(m_ChildList.empty() == false)
	//	m_ChildList[m_ChildList.size() - 1]->ParentDetach();
}

void HyEntity3d::ChildAppend(IHyNode3d &childRef)
{
	HyAssert(&childRef != this, "HyEntity3d::ChildAppend was passed a child that was itself!");

	childRef.ParentDetach();
	childRef.m_pParent = this;

	m_ChildList.push_back(&childRef);
	SetNewChildAttributes(childRef);
}

/*virtual*/ bool HyEntity3d::ChildInsert(IHyNode3d &insertBefore, IHyNode3d &childRef)
{
	childRef.ParentDetach();

	for(auto iter = m_ChildList.begin(); iter != m_ChildList.end(); ++iter)
	{
		if((*iter) == &insertBefore || 
		   ((*iter)->GetType() == HYTYPE_Entity && static_cast<HyEntity3d *>(*iter)->ChildExists(insertBefore)))
		{
			childRef.m_pParent = this;

			m_ChildList.insert(iter, &childRef);
			SetNewChildAttributes(childRef);

			return true;
		}
	}

	return false;
}

bool HyEntity3d::ChildExists(IHyNode3d &childRef)
{
	for(auto iter = m_ChildList.begin(); iter != m_ChildList.end(); ++iter)
	{
		if((*iter) == &childRef ||
		   ((*iter)->GetType() == HYTYPE_Entity && static_cast<HyEntity3d *>(*iter)->ChildExists(childRef)))
		{
			return true;
		}
	}

	return false;
}

/*virtual*/ bool HyEntity3d::ChildRemove(IHyNode3d *pChild)
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

/*virtual*/ void HyEntity3d::ChildrenTransfer(HyEntity3d &newParent)
{
	while(m_ChildList.empty() == false)
		newParent.ChildAppend(*m_ChildList[0]);
}

/*virtual*/ uint32 HyEntity3d::ChildCount()
{
	return static_cast<uint32>(m_ChildList.size());
}

/*virtual*/ IHyNode3d *HyEntity3d::ChildGet(uint32 uiIndex)
{
	HyAssert(uiIndex < static_cast<uint32>(m_ChildList.size()), "HyEntity3d::ChildGet passed an invalid index");
	return m_ChildList[uiIndex];
}

void HyEntity3d::ForEachChild(std::function<void(IHyNode3d *)> func)
{
	func(this);

	for(uint32 i = 0; i < m_ChildList.size(); ++i)
	{
		if(m_ChildList[i]->GetType() == HYTYPE_Entity)
			static_cast<HyEntity3d *>(m_ChildList[i])->ForEachChild(func);
	}
}

/*virtual*/ void HyEntity3d::Update() /*override final*/
{
	IHyVisable3d::Update();
	OnUpdate();
}

/*virtual*/ bool HyEntity3d::IsChildrenLoaded() const /*override final*/
{
	for(uint32 i = 0; i < m_ChildList.size(); ++i)
	{
		if(0 != (m_ChildList[i]->m_uiExplicitAndTypeFlags & NODETYPE_IsLoadable))
		{
			if(static_cast<IHyLoadable3d *>(m_ChildList[i])->IsLoadDataValid() != false && static_cast<IHyLoadable3d *>(m_ChildList[i])->IsLoaded() == false)
				return false;
		}
	}

	return true;
}

void HyEntity3d::SetNewChildAttributes(IHyNode3d &childRef)
{
	SetDirty(DIRTY_ALL);
	childRef._SetEnabled(m_bEnabled, false);
	childRef._SetPauseUpdate(m_bPauseOverride, false);

	if(childRef.GetExplicitAndTypeFlags() & NODETYPE_IsVisable)
		SetupNewChild(*this, static_cast<IHyVisable3d &>(childRef));
}

/*virtual*/ void HyEntity3d::SetDirty(uint32 uiDirtyFlags)
{
	IHyNode3d::SetDirty(uiDirtyFlags);

	for(uint32 i = 0; i < m_ChildList.size(); ++i)
		m_ChildList[i]->SetDirty(uiDirtyFlags);
}

/*friend*/ void _CtorChildAppend(HyEntity3d &entityRef, IHyNode3d &childRef)
{
	entityRef.m_ChildList.push_back(&childRef);
	entityRef.SetDirty(HyEntity3d::DIRTY_ALL);
}
