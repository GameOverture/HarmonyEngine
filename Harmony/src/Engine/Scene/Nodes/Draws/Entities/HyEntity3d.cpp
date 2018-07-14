/**************************************************************************
 *	HyEntity2d.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Scene/Nodes/Draws/Entities/HyEntity3d.h"
#include "Scene/HyScene.h"
#include "HyEngine.h"

HyEntity3d::HyEntity3d(const char *szPrefix, const char *szName, HyEntity3d *pParent) :	IHyDraw3d(HYTYPE_Entity3d, pParent)
{
}

HyEntity3d::~HyEntity3d(void)
{
	//while(m_ChildList.empty() == false)
	//	m_ChildList[m_ChildList.size() - 1]->ParentDetach();
}

/*virtual*/ void HyEntity3d::NodeUpdate() /*override final*/
{
	OnUpdate();
}
