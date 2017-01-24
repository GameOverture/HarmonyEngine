/**************************************************************************
 *	HySfxData.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Assets/Data/HyAudioData.h"


HySfxData::HySfxData(const std::string &sPath) : IHyData(HYDATA_Regular, HYTYPE_Sound2d, sPath)
{
}

HySfxData::~HySfxData(void)
{
}

/*virtual*/ void HySfxData::DoFileLoad()
{
}
