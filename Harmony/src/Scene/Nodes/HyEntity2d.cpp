/**************************************************************************
 *	HyEntity2d.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Scene/Nodes/HyEntity2d.h"

HyEntity2d::HyEntity2d(IHyNode *pParent /*= nullptr*/) :	IHyDraw2d(HYTYPE_Entity2d, nullptr, nullptr, pParent)
{
}

HyEntity2d::HyEntity2d(const char *szPrefix, const char *szName, IHyNode *pParent /*= nullptr*/) :	IHyDraw2d(HYTYPE_Entity2d, szPrefix, szName, pParent)
{
}

HyEntity2d::~HyEntity2d(void)
{
}
