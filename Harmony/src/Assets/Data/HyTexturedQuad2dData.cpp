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


HyTexturedQuad2dData::HyTexturedQuad2dData(const std::string &sPath, int32 iShaderId) : IHy2dData(HYINST_TexturedQuad2d, sPath, iShaderId),
																						m_uiATLASGROUP_ID(atoi(sPath.c_str())),
																						m_pAtlas(NULL)
{
	
}

HyTexturedQuad2dData::~HyTexturedQuad2dData()
{
}

/*virtual*/ void HyTexturedQuad2dData::DoFileLoad()
{
	m_pAtlas = RequestTexture(m_uiATLASGROUP_ID);
}

HyAtlasGroup *HyTexturedQuad2dData::GetAtlasGroup()
{
	return m_pAtlas;
}
