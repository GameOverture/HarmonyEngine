/**************************************************************************
*	HyTexturedQuad2dData.h
*
*	Harmony Engine
*	Copyright (c) 2013 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Creator/Data/HyTexturedQuad2dData.h"

#include "Renderer/HyRenderer.h"


HyTexturedQuad2dData::HyTexturedQuad2dData(const std::string &sPath) : IData(HYINST_TexturedQuad2d, sPath)
{
}

HyTexturedQuad2dData::~HyTexturedQuad2dData()
{
}

const HyTexture *HyTexturedQuad2dData::GetTexture() const
{
	return m_pTexture;
}

/*virtual*/ void HyTexturedQuad2dData::DoFileLoad()
{
	int32 iTextureIndex = atoi(m_ksPath.c_str());
	m_pTexture = HyFileIO::GetAtlasTexture(iTextureIndex);
}

/*virtual*/ void HyTexturedQuad2dData::OnGfxLoad(HyRenderer &gfxApi)
{
	m_pTexture->Upload(gfxApi);
}

/*virtual*/ void HyTexturedQuad2dData::OnGfxRemove(HyRenderer &gfxApi)
{
	gfxApi.DeleteTexture(*m_pTexture);
}
