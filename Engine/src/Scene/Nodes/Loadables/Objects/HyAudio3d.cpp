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

HyAudio3d::HyAudio3d(HyEntity3d *pParent /*= nullptr*/) :
	IHyAudio<IHyLoadable3d, HyEntity3d>(HyNodePath(), pParent)
{
}

HyAudio3d::HyAudio3d(const HyNodePath &nodePath, HyEntity3d *pParent /*= nullptr*/) :
	IHyAudio<IHyLoadable3d, HyEntity3d>(nodePath, pParent)
{
}

HyAudio3d::HyAudio3d(const char *szPrefix, const char *szName, HyEntity3d *pParent /*= nullptr*/) :
	IHyAudio<IHyLoadable3d, HyEntity3d>(HyNodePath(szPrefix, szName), pParent)
{
}

/*virtual*/ HyAudio3d::~HyAudio3d(void)
{
}
