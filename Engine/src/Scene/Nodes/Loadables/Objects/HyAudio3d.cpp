/**************************************************************************
 *	HyAudio3d.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2020 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Loadables/Objects/HyAudio3d.h"

HyAudio3d::HyAudio3d(std::string sPrefix /*= ""*/, std::string sName /*= ""*/, HyEntity3d *pParent /*= nullptr*/) :
	IHyAudio<IHyLoadable3d, HyEntity3d>(HyNodePath(sPrefix, sName), pParent)
{
}

/*virtual*/ HyAudio3d::~HyAudio3d(void)
{
}
