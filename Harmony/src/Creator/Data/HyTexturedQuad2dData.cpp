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

#include "Renderer/Interop/IGfxApi.h"

HyTexturedQuad2dData::HyTexturedQuad2dData(const std::string &sPath) : IData(HYINST_TexturedQuad2d, sPath)
{
}

HyTexturedQuad2dData::~HyTexturedQuad2dData()
{
}

/*virtual*/ void HyTexturedQuad2dData::DoFileLoad()
{

}

/*virtual*/ void HyTexturedQuad2dData::OnGfxLoad(IGfxApi &gfxApi)
{
	m_pTexture->Upload(gfxApi);
}

/*virtual*/ void HyTexturedQuad2dData::OnGfxRemove(IGfxApi &gfxApi)
{
	gfxApi.DeleteTexture(*m_pTexture);
}
