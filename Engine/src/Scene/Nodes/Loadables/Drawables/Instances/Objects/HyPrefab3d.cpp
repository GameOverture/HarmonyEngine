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
#include "Scene/Nodes/Loadables/Drawables/Instances/Objects/HyPrefab3d.h"

HyPrefab3d::HyPrefab3d() :
	IHyInstance3d(HYTYPE_Prefab)
{
}

HyPrefab3d::HyPrefab3d(const HyPrefab3d &copyRef) :
	IHyInstance3d(copyRef)
{
}

/*virtual*/ HyPrefab3d::~HyPrefab3d()
{
}
