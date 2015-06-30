/**************************************************************************
*	HyTexture.cpp
*
*	Harmony Engine
*	Copyright (c) 2013 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "FileIO/Atlas/HyAtlas.h"
#include "FileIO/HyFileIO.h"

HyAtlas::HyAtlas() :	m_ppTextures(NULL),
						m_uiNumTextures(0)
{
}


HyAtlas::~HyAtlas()
{
}

void HyAtlas::Initialize(std::string sAtlasInfoFilePath)
{
	// TODO: parse JSON file and get texture information
	uint32 iNumTextures = 1;

	m_ppTextures = new HyTexture *[iNumTextures];
	m_uiNumTextures = iNumTextures;

	for(uint32 i = 0; i < m_uiNumTextures; ++i)
		m_ppTextures[i] = NULL;
}

/*static*/ HyTexture *HyAtlas::GetTexture(uint32 uiTextureIndex, std::string sFilePath)
{
	HyAssert(uiTextureIndex >= 0 && uiTextureIndex < m_uiNumTextures, "HyAtlas::GetTexture() was passed an invalid 'uiTextureIndex'");

	if(m_ppTextures[uiTextureIndex] == NULL)
		m_ppTextures[uiTextureIndex] = new HyTexture(sFilePath);

	return m_ppTextures[uiTextureIndex];
}
