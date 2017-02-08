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

HyEntity2d::HyEntity2d() :	IHyDraw2d(HYTYPE_Entity2d, NULL, NULL)
{
	// TODO: Check IsLoaded with multiple hyentity's in a hierarchy
	m_eLoadState = HYLOADSTATE_Loaded;
}

HyEntity2d::HyEntity2d(const char *szPrefix, const char *szName) :	IHyDraw2d(HYTYPE_Entity2d, szPrefix, szName)
{
	// TODO: Check IsLoaded with multiple hyentity's in a hierarchy
	m_eLoadState = HYLOADSTATE_Loaded;
}

HyEntity2d::~HyEntity2d(void)
{
}
