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
	m_RenderState.Enable(HyRenderState::DRAWMODE_TRIANGLESTRIP | HyRenderState::DRAWINSTANCED);
	m_RenderState.SetShaderIndex(IHyShader::SHADER_QuadBatch);
	m_RenderState.SetNumVertices(4);
}


HySprite2d::~HySprite2d(void)
{
}

/*virtual*/ void HySprite2d::OnDataLoaded()
{
	HySprite2dData *pData = static_cast<HySprite2dData *>(m_pData);
	//m_RenderState.SetTextureHandle(pData->GetAtlasGroup()->GetGfxApiHandle());

}

/*virtual*/ void HySprite2d::OnUpdate()
{
}

/*virtual*/ void HySprite2d::OnUpdateUniforms(HyShaderUniforms *pShaderUniformsRef)
{
}

/*virtual*/ void HySprite2d::DefaultWriteDrawBufferData(char *&pRefDataWritePos)
{

}
