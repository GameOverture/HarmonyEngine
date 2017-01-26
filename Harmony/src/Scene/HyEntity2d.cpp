/**************************************************************************
 *	HyEntity2d.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Scene/HyEntity2d.h"

HyEntity2d::HyEntity2d() :	IHyInst2d(HYTYPE_Entity2d, NULL, NULL)
{
}

HyEntity2d::HyEntity2d(const char *szPrefix, const char *szName) :	IHyInst2d(HYTYPE_Entity2d, szPrefix, szName)
{
}

HyEntity2d::~HyEntity2d(void)
{
}
