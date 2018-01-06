/**************************************************************************
 *	HyAudioData.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Assets/Nodes/HyAudioData.h"


HyAudioData::HyAudioData(const std::string &sPath, const jsonxx::Value &dataValueRef, HyAssets &assetsRef) : IHyNodeData(HYTYPE_Sound2d, sPath)
{
}

HyAudioData::~HyAudioData(void)
{
}
