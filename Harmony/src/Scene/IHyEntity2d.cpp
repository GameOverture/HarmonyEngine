/**************************************************************************
 *	IHyEntity2d.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Scene/IHyEntity2d.h"

IHyEntity2d::IHyEntity2d() :	IHyInst2d(HYTYPE_Entity2d, NULL, NULL)
{
}

IHyEntity2d::IHyEntity2d(const char *szPrefix, const char *szName) :	IHyInst2d(HYTYPE_Entity2d, szPrefix, szName)
{
}

IHyEntity2d::~IHyEntity2d(void)
{
}
