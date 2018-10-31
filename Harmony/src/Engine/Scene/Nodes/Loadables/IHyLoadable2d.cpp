/**************************************************************************
*	IHyLoadable2d.cpp
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Loadables/IHyLoadable2d.h"
#include "Scene/Nodes/Loadables/Visables/Objects/HyEntity2d.h"
#include "Scene/Nodes/Loadables/Visables/Objects/HyEntity3d.h"

IHyLoadable2d::IHyLoadable2d(HyType eNodeType, const char *szPrefix, const char *szName, HyEntity2d *pParent) :	IHyNode2d(eNodeType, pParent),
																												IHyLoadable(szPrefix, szName)
{
	m_uiExplicitAndTypeFlags |= NODETYPE_IsLoadable;
}

IHyLoadable2d::IHyLoadable2d(const IHyLoadable2d &copyRef) :	IHyNode2d(copyRef),
																IHyLoadable(copyRef)
{
}

IHyLoadable2d::~IHyLoadable2d()
{
}

const IHyLoadable2d &IHyLoadable2d::operator=(const IHyLoadable2d &rhs)
{
	IHyNode2d::operator=(rhs);
	IHyLoadable::operator=(rhs);

	return *this;
}

/*virtual*/ HyType IHyLoadable2d::_LoadableGetType() /*override final*/
{
	return m_eTYPE;
}

/*virtual*/ IHyLoadable *IHyLoadable2d::_LoadableGetParentPtr() /*override final*/
{
	return m_pParent;
}
