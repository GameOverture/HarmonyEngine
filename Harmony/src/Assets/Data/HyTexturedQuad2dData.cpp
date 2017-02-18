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


HyTexturedQuad2dData::HyTexturedQuad2dData(const std::string &sPath, int32 iShaderId) : HyDataDraw(HYTYPE_TexturedQuad2d, sPath, iShaderId),
																						m_uiATLASGROUP_ID(sPath == "raw" ? 0xFFFFFFFF : atoi(sPath.c_str())),
																						m_pAtlas(NULL)
{
	if(m_uiATLASGROUP_ID == 0xFFFFFFFF)
		SetLoadState(HYLOADSTATE_Loaded);
}

HyTexturedQuad2dData::~HyTexturedQuad2dData()
{
}

/*virtual*/ void HyTexturedQuad2dData::DoFileLoad()
{
	if(m_uiATLASGROUP_ID != 0xFFFFFFFF)
		m_pAtlas = RequestTexture(m_uiATLASGROUP_ID);
}

HyAtlasGroup *HyTexturedQuad2dData::GetAtlasGroup()
{
	return m_pAtlas;
}
