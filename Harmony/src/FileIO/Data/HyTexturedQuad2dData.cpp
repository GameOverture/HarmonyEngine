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


HyTexturedQuad2dData::HyTexturedQuad2dData(const std::string &sPath) : IHyData2d(HYINST_TexturedQuad2d, sPath),
																		m_uiTextureIndex(0),
																		m_pAtlas(NULL)
{
	m_uiTextureId = atoi(sPath.c_str());
}

HyTexturedQuad2dData::~HyTexturedQuad2dData()
{
}

/*virtual*/ void HyTexturedQuad2dData::DoFileLoad()
{
	m_pAtlas = RequestTexture(m_uiTextureId);
}

HyAtlasGroup *HyTexturedQuad2dData::GetAtlasGroup()
{
	return m_pAtlas;
}

uint32 HyTexturedQuad2dData::GetTextureIndex()
{
	return m_uiTextureIndex;
}
