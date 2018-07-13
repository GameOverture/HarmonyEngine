/**************************************************************************
*	HyPrefab3d.h
*
*	Harmony Engine
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Scene/Nodes/Draws/Instances/HyPrefab3d.h"

HyPrefab3d::HyPrefab3d(const char *szPrefix, const char *szName, HyEntity3d *pParent) : IHyDraw3d(HYTYPE_Prefab3d, pParent)
{
}

HyPrefab3d::HyPrefab3d(const HyPrefab3d &copyRef) :	IHyDraw3d(copyRef)
{
}

/*virtual*/ HyPrefab3d::~HyPrefab3d()
{
}

/*virtual*/ void HyPrefab3d::NodeUpdate() /*override*/
{
}
