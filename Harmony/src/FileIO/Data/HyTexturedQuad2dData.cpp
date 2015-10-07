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


HyTexturedQuad2dData::HyTexturedQuad2dData(const std::string &sPath) : IHyData(HYINST_TexturedQuad2d, sPath)
{
}

HyTexturedQuad2dData::~HyTexturedQuad2dData()
{
}

//const HyTexture *HyTexturedQuad2dData::GetTexture() const
//{
//	return m_pTexture;
//}

/*virtual*/ void HyTexturedQuad2dData::DoFileLoad(HyAtlasManager &atlasManagerRef)
{
	//int32 iTextureIndex = atoi(m_ksPath.c_str());
	//m_pTexture = HyFileIO::GetAtlasTexture(iTextureIndex);
}

/*virtual*/ void HyTexturedQuad2dData::OnGfxLoad(IHyRenderer &gfxApi)
{
	//m_pTexture->Upload(gfxApi);
}

/*virtual*/ void HyTexturedQuad2dData::OnGfxRemove(IHyRenderer &gfxApi)
{
	//gfxApi.DeleteTexture(*m_pTexture);
}
