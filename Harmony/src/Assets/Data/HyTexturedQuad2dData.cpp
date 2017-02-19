/**************************************************************************
*	HyTexturedQuad2dData.h
*
*	Harmony Engine
*	Copyright (c) 2013 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Assets/Data/HyTexturedQuad2dData.h"

#include "Renderer/IHyRenderer.h"


HyTexturedQuad2dData::HyTexturedQuad2dData(const std::string &sPath, const jsonxx::Value &dataValueRef, HyAtlasContainer &atlasContainerRef) :	IHyData(HYTYPE_TexturedQuad2d, sPath),
																																				m_uiATLASGROUP_ID(sPath == "raw" ? 0xFFFFFFFF : atoi(sPath.c_str())),
																																				m_pAtlas(nullptr)
{
	//if(m_uiATLASGROUP_ID == 0xFFFFFFFF)
	//	SetLoadState(HYLOADSTATE_Loaded);
	
	if(m_uiATLASGROUP_ID != 0xFFFFFFFF)
		m_pAtlas = atlasContainerRef.GetAtlasGroup(m_uiATLASGROUP_ID);
}

HyTexturedQuad2dData::~HyTexturedQuad2dData()
{
}

HyAtlasGroup *HyTexturedQuad2dData::GetAtlasGroup()
{
	return m_pAtlas;
}

/*virtual*/ void HyTexturedQuad2dData::AppendRequiredAtlasIds(std::set<uint32> &requiredAtlasIdsOut)
{
	HyError("Not implemented");
}
