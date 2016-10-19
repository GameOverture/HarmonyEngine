/**************************************************************************
 *	HyText2dData.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Assets/Data/HyText2dData.h"

#include "Renderer/IHyRenderer.h"

HyText2dData::HyText2dData(const std::string &sPath, int32 iShaderId) : IHy2dData(HYINST_Text2d, sPath, iShaderId)
{
}

HyText2dData::~HyText2dData(void)
{
}

/*virtual*/ void HyText2dData::DoFileLoad()
{

}
