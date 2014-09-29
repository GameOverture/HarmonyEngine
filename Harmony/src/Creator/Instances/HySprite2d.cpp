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

HySprite2d::HySprite2d(HyTexture *pTexture, HyRectangle rSrcRect) :	IObjInst2d(HYINST_Sprite2d, NULL, NULL),
																	m_pTexture(pTexture),
																	m_rSrcRect(rSrcRect)
{
	m_uiRenderStates |= (RS_SHADER_SPINE | RS_DRAWMODE_TRIANGLESTRIP);
}

HySprite2d::HySprite2d(const char *szPrefix, const char *szName) :	IObjInst2d(HYINST_Sprite2d, szPrefix, szName),
																	m_pTexture(NULL)
{
	m_uiRenderStates |= (RS_SHADER_SPINE | RS_DRAWMODE_TRIANGLESTRIP);
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
