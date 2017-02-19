/**************************************************************************
*	HyPrimitive2dData.h
*
*	Harmony Engine
*	Copyright (c) 2013 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Assets/Data/HyPrimitive2dData.h"

#include "Renderer/IHyRenderer.h"


HyPrimitive2dData::HyPrimitive2dData(const std::string &sPath) : IHyData(HYTYPE_Primitive2d, sPath)
{
}

HyPrimitive2dData::~HyPrimitive2dData()
{
}

/*virtual*/ void HyPrimitive2dData::SetRequiredAtlasIds(HyGfxData &gfxDataOut)
{
	HyError("Not implemented");
}
