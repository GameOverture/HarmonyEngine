/**************************************************************************
*	HyPrimitive2dData.h
*
*	Harmony Engine
*	Copyright (c) 2013 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Assets/Nodes/HyPrimitive2dData.h"

#include "Renderer/IHyRenderer.h"


HyPrimitive2dData::HyPrimitive2dData(const std::string &sPath, const jsonxx::Value &dataValueRef, HyAtlasContainer &atlasContainerRef) : IHyNodeData(HYTYPE_Primitive2d, sPath)
{
}

HyPrimitive2dData::~HyPrimitive2dData()
{
}

/*virtual*/ void HyPrimitive2dData::AppendRequiredAtlasIds(std::set<uint32> &requiredAtlasIdsOut)
{
}
