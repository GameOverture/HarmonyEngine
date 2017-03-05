/**************************************************************************
*	HyTexturedQuad2dData.h
*
*	Harmony Engine
*	Copyright (c) 2013 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Assets/Nodes/HyTexturedQuad2dData.h"

#include "Renderer/IHyRenderer.h"


HyTexturedQuad2dData::HyTexturedQuad2dData(const std::string &sPath, HyAssets &assetsRef) :	IHyNodeData(HYTYPE_TexturedQuad2d, sPath),
																							m_uiATLAS_INDEX(sPath == "raw" ? 0xFFFFFFFF : std::stoi(sPath)),
																							m_pAtlas(nullptr)
{
	//if(m_uiATLASGROUP_ID == 0xFFFFFFFF)
	//	SetLoadState(HYLOADSTATE_Loaded);
	
	if(m_uiATLAS_INDEX != 0xFFFFFFFF)
		m_pAtlas = assetsRef.GetAtlas(m_uiATLAS_INDEX);
}

HyTexturedQuad2dData::~HyTexturedQuad2dData()
{
}

HyAtlas *HyTexturedQuad2dData::GetAtlas()
{
	return m_pAtlas;
}

/*virtual*/ void HyTexturedQuad2dData::AppendRequiredAtlasIds(std::set<uint32> &requiredAtlasIndicesOut)
{
	requiredAtlasIndicesOut.insert(m_uiATLAS_INDEX);
}
