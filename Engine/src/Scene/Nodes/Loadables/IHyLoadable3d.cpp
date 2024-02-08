/**************************************************************************
*	IHyLoadable3d.cpp
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Loadables/IHyLoadable3d.h"
#include "Scene/Nodes/Loadables/Bodies/Objects/HyEntity3d.h"

IHyLoadable3d::IHyLoadable3d(HyType eNodeType, const HyNodePath &nodePath, HyEntity3d *pParent) :
	IHyNode3d(eNodeType, pParent),
	IHyLoadable(nodePath)
{
	m_uiFlags |= NODETYPE_IsLoadable;

	if(m_pParent)
		m_pParent->m_eLoadState = HYLOADSTATE_Inactive;
}

IHyLoadable3d::IHyLoadable3d(const IHyLoadable3d &copyRef) :
	IHyNode3d(copyRef),
	IHyLoadable(copyRef)
{
}

IHyLoadable3d::IHyLoadable3d(IHyLoadable3d &&donor) noexcept :
	IHyNode3d(std::move(donor)),
	IHyLoadable(std::move(donor))
{
}

IHyLoadable3d::~IHyLoadable3d()
{
	Unload();
}

IHyLoadable3d &IHyLoadable3d::operator=(const IHyLoadable3d &rhs)
{
	IHyNode3d::operator=(rhs);
	IHyLoadable::operator=(rhs);

	return *this;
}

IHyLoadable3d &IHyLoadable3d::operator=(IHyLoadable3d &&donor)
{
	IHyNode3d::operator=(std::move(donor));
	IHyLoadable::operator=(std::move(donor));

	return *this;
}

void IHyLoadable3d::Init(const HyNodePath &nodePath, HyEntity3d *pParent)
{
	IHyLoadable::_Reinitialize(nodePath);

	if(m_pParent != pParent)
	{
		if(pParent != nullptr)
			pParent->ChildAppend(*this);
		else
			ParentDetach();
	}
}

void IHyLoadable3d::Init(const char *szPrefix, const char *szName, HyEntity3d *pParent)
{
	Init(HyNodePath(szPrefix, szName), pParent);
}

/*virtual*/ void IHyLoadable3d::Update() /*override*/
{
	IHyNode3d::Update();

	if(IsLoaded())
		OnLoadedUpdate();
}

/*virtual*/ HyType IHyLoadable3d::_LoadableGetType() /*override final*/
{
	return GetType();
}

/*virtual*/ IHyLoadable *IHyLoadable3d::_LoadableGetParentPtr() /*override final*/
{
	return m_pParent;
}
