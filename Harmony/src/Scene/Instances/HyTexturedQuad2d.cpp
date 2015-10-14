/**************************************************************************
*	HyTexturedQuad2d.h
*
*	Harmony Engine
*	Copyright (c) 2015 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Scene/Instances/HyTexturedQuad2d.h"

#include "FileIO/Data/HyTexturedQuad2dData.h"

HyTexturedQuad2d::HyTexturedQuad2d(uint32 uiTextureIndex) : IHyInst2d(HYINST_TexturedQuad2d, NULL, std::to_string(uiTextureIndex).c_str())
{
	m_RenderState.Enable(HyRenderState::DRAWMODE_TRIANGLESTRIP | HyRenderState::SHADER_QUADBATCH);
	m_RenderState.SetNumInstances(1);
}

HyTexturedQuad2d::~HyTexturedQuad2d()
{
}

/*virtual*/ void HyTexturedQuad2d::OnDataLoaded()
{
	HyTexturedQuad2dData *pData = static_cast<HyTexturedQuad2dData *>(m_pData);
}

/*virtual*/ void HyTexturedQuad2d::Update()
{
}

/*virtual*/ void HyTexturedQuad2d::WriteDrawBufferData(char *&pRefDataWritePos)
{
	//const HyTexture *pTexture = static_cast<HyTexturedQuad2dData *>(m_pDataPtr)->GetTexture();

	HyError("Need texture dimensions");
	vec2 vSize(1.0f/*pTexture->GetWidth(), pTexture->GetHeight()*/);
	vec2 vOffset(m_ptPosition.X(), m_ptPosition.Y());

	*reinterpret_cast<vec2 *>(pRefDataWritePos) = vSize;
	pRefDataWritePos += sizeof(vec2);
	*reinterpret_cast<vec2 *>(pRefDataWritePos) = vOffset;
	pRefDataWritePos += sizeof(vec2);

	*reinterpret_cast<vec4 *>(pRefDataWritePos) = m_vColor.Get();
	pRefDataWritePos += sizeof(vec4);

	vec2 vUV;

	vUV.x = 0.0f;
	vUV.y = 0.0f;
	*reinterpret_cast<vec2 *>(pRefDataWritePos) = vUV;
	pRefDataWritePos += sizeof(vec2);

	vUV.x = 1.0f;
	vUV.y = 0.0f;
	*reinterpret_cast<vec2 *>(pRefDataWritePos) = vUV;
	pRefDataWritePos += sizeof(vec2);

	vUV.x = 0.0f;
	vUV.y = 1.0f;
	*reinterpret_cast<vec2 *>(pRefDataWritePos) = vUV;
	pRefDataWritePos += sizeof(vec2);

	vUV.x = 1.0f;
	vUV.y = 1.0f;
	*reinterpret_cast<vec2 *>(pRefDataWritePos) = vUV;
	pRefDataWritePos += sizeof(vec2);

	GetWorldTransform(*reinterpret_cast<mat4 *>(pRefDataWritePos));
	pRefDataWritePos += sizeof(mat4);
}
