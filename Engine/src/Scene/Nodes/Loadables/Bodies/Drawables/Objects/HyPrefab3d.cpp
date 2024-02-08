/**************************************************************************
*	HyPrefab3d.h
*
*	Harmony Engine
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Loadables/Bodies/Drawables/Objects/HyPrefab3d.h"

HyPrefab3d::HyPrefab3d(HyEntity3d *pParent /*= nullptr*/) :
	IHyDrawable3d(HYTYPE_Prefab, HyNodePath(), pParent)
{
}

HyPrefab3d::HyPrefab3d(const HyNodePath &nodePath, HyEntity3d *pParent /*= nullptr*/) :
	IHyDrawable3d(HYTYPE_Prefab, nodePath, pParent)
{
}

HyPrefab3d::HyPrefab3d(const char *szPrefix, const char *szName, HyEntity3d *pParent /*= nullptr*/) :
	IHyDrawable3d(HYTYPE_Prefab, HyNodePath(szPrefix, szName), pParent)
{
}

HyPrefab3d::HyPrefab3d(const HyPrefab3d &copyRef) :
	IHyDrawable3d(copyRef)
{
}

/*virtual*/ HyPrefab3d::~HyPrefab3d()
{
}
