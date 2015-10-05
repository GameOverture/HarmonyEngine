/**************************************************************************
 *	HySprite2d.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Scene/Instances/HySprite2d.h"

HySprite2d::HySprite2d(const char *szPrefix, const char *szName) :	IHyInst2d(HYINST_Sprite2d, szPrefix, szName)
{
	m_RenderState.Enable(HyRenderState::DRAWMODE_TRIANGLESTRIP | HyRenderState::SHADER_QUADBATCH);
}


HySprite2d::~HySprite2d(void)
{
}

/*virtual*/ void HySprite2d::OnDataLoaded()
{
}

/*virtual*/ void HySprite2d::Update()
{
}

/*virtual*/ void HySprite2d::WriteDrawBufferData(char *&pRefDataWritePos)
{

}
