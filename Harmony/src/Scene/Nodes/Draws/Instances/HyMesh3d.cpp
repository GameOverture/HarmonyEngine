/**************************************************************************
 *	HyMesh3d.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Scene/Nodes/Draws/Instances/HyMesh3d.h"

HyMesh3d::HyMesh3d(const char *szPrefix, const char *szName, HyEntity3d *pParent) : IHyDraw3d(HYTYPE_Mesh3d, pParent)
{

}

HyMesh3d::~HyMesh3d(void)
{
}

/*virtual*/ void HyMesh3d::NodeUpdate() /*override*/
{
}

