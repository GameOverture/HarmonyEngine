/**************************************************************************
*	HyPrefab3d.h
*
*	Harmony Engine
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Scene/Nodes/Loadables/Drawables/HyPrefab3d.h"

HyPrefab3d::HyPrefab3d(const char *szPrefix, const char *szName, HyEntity3d *pParent) : IHyLoadable3d(HYTYPE_Prefab3d, szPrefix, szName, pParent)
{
}

HyPrefab3d::HyPrefab3d(const HyPrefab3d &copyRef) :	IHyLoadable3d(copyRef)
{
}

/*virtual*/ HyPrefab3d::~HyPrefab3d()
{
}

/*virtual*/ void HyPrefab3d::NodeUpdate() /*override*/
{
}
