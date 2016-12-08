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


HyPrimitive2dData::HyPrimitive2dData(const std::string &sPath, int32 iShaderId) :	IHy2dData(HYTYPE_TexturedQuad2d, sPath, iShaderId)
{
	if(GetShaderId() < HYSHADERPROG_CustomStartIndex && GetShaderId() >= 0)
		SetLoadState(HYLOADSTATE_Loaded);
}

HyPrimitive2dData::~HyPrimitive2dData()
{
}

/*virtual*/ void HyPrimitive2dData::DoFileLoad()
{
}
