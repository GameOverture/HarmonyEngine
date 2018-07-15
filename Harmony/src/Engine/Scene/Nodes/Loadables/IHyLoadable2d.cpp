/**************************************************************************
*	IHyLoadable2d.cpp
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Scene/Nodes/Loadables/IHyLoadable2d.h"

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

/*virtual*/ HyType IHyLoadable2d::_LoadableGetType() /*override*/
{
	return m_eTYPE;
}
