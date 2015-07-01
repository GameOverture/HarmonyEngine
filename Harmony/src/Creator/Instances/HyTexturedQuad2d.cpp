/**************************************************************************
*	HyTexturedQuad2d.h
*
*	Harmony Engine
*	Copyright (c) 2015 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Creator/Instances/HyTexturedQuad2d.h"

HyTexturedQuad2d::HyTexturedQuad2d(uint32 uiTextureIndex) : IObjInst2d(HYINST_TexturedQuad2d, uiTextureIndex)
{
}

HyTexturedQuad2d::~HyTexturedQuad2d()
{
}

/*virtual*/ void HyTexturedQuad2d::OnDataLoaded()
{
	HyTexturedQuad2dData *pSpineData = static_cast<HyTexturedQuad2dData *>(m_pDataPtr);
}

/*virtual*/ void HyTexturedQuad2d::Update()
{
}

/*virtual*/ void HyTexturedQuad2d::WriteDrawBufferData(char *&pRefDataWritePos)
{
	// START HERE:
}
