/**************************************************************************
*	HyTexturedQuad2dData.h
*
*	Harmony Engine
*	Copyright (c) 2013 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "FileIO/Data/HyTexturedQuad2dData.h"

#include "Renderer/IHyRenderer.h"


HyTexturedQuad2dData::HyTexturedQuad2dData(const std::string &sPath) :	IHyData2d(HYINST_TexturedQuad2d, sPath),
																		m_uiATLASGROUP_ID(atoi(sPath.c_str())),
																		m_pAtlas(NULL)
{
	
}

HyTexturedQuad2dData::~HyTexturedQuad2dData()
{
}

/*virtual*/ void HyTexturedQuad2dData::DoFileLoad()
{
	m_pAtlas = RequestTexture(m_uiATLASGROUP_ID, 0);
}

HyAtlasGroup *HyTexturedQuad2dData::GetAtlasGroup()
{
	return m_pAtlas;
}
