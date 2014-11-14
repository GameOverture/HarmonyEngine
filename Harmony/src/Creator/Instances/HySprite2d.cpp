/**************************************************************************
 *	HySprite2d.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Creator/Instances/HySprite2d.h"

HySprite2d::HySprite2d(const char *szPrefix, const char *szName) :	IObjInst2d(HYINST_Sprite2d, szPrefix, szName),
																	m_pTexture(NULL)
{
	m_RenderState.SetAttributes(HyRenderState::RS_DRAWMODE_TRIANGLESTRIP | HyRenderState::RS_SHADER_QUADBATCH);
}


HySprite2d::~HySprite2d(void)
{
}

/*virtual*/ void HySprite2d::OnDataLoaded()
{
	if(m_ksPath.empty())
		return;
}

/*virtual*/ void HySprite2d::Update()
{
}
