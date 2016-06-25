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

#include "Assets/Data/HyTexturedQuad2dData.h"

HyTexturedQuad2d::HyTexturedQuad2d(uint32 uiAtlasGroupId) :	IHyInst2d(HYINST_TexturedQuad2d, NULL, std::to_string(uiAtlasGroupId).c_str()),
															m_uiTextureIndex(0),
															m_SrcRect(0.0f, 0.0f, 1.0f, 1.0f)
{
	m_RenderState.Enable(HyRenderState::DRAWMODE_TRIANGLESTRIP | HyRenderState::DRAWINSTANCED);
	m_RenderState.SetShaderIndex(IHyShader::SHADER_QuadBatch);
	m_RenderState.SetNumInstances(1);
	m_RenderState.SetNumVertices(4);
}

HyTexturedQuad2d::~HyTexturedQuad2d()
{
}

uint32 HyTexturedQuad2d::GetAtlasGroupId() const
{
	return static_cast<HyTexturedQuad2dData *>(m_pData)->GetAtlasGroup()->GetId();
}

void HyTexturedQuad2d::SetTextureSource(uint32 uiTextureIndex)
{
	m_uiTextureIndex = uiTextureIndex;
}

void HyTexturedQuad2d::SetTextureSource(uint32 uiTextureIndex, int iX, int iY, int iWidth, int iHeight)
{
	if(IsLoaded())
	{
		float fX = static_cast<float>(iX);
		float fY = static_cast<float>(iY);
		float fWidth = static_cast<float>(iWidth);
		float fHeight = static_cast<float>(iHeight);
		float fTexWidth = static_cast<float>(static_cast<HyTexturedQuad2dData *>(m_pData)->GetAtlasGroup()->GetWidth());
		float fTexHeight = static_cast<float>(static_cast<HyTexturedQuad2dData *>(m_pData)->GetAtlasGroup()->GetHeight());

		m_SrcRect.left = fX / fTexWidth;
		m_SrcRect.top = fY / fTexHeight;
		m_SrcRect.right = (fX + fWidth) / fTexWidth;
		m_SrcRect.bottom = (fY + fHeight) / fTexHeight;
	}
	else
	{
		m_SrcRect.left = static_cast<float>(iX);
		m_SrcRect.top = static_cast<float>(iY);
		m_SrcRect.right = m_SrcRect.left + static_cast<float>(iWidth);
		m_SrcRect.bottom = m_SrcRect.top + static_cast<float>(iHeight);
	}

	m_uiTextureIndex = uiTextureIndex;
}

uint32 HyTexturedQuad2d::GetTextureIndex()
{
	return m_uiTextureIndex;
}

uint32 HyTexturedQuad2d::GetWidth()
{
	return static_cast<uint32>(m_SrcRect.Width() * GetEntireTextureWidth());
}

uint32 HyTexturedQuad2d::GetHeight()
{
	return static_cast<uint32>(m_SrcRect.Height() * GetEntireTextureHeight());
}

uint32 HyTexturedQuad2d::GetEntireTextureWidth()
{
	if(IsLoaded())
		return static_cast<HyTexturedQuad2dData *>(m_pData)->GetAtlasGroup()->GetWidth();
	else
		return 0;
}

uint32 HyTexturedQuad2d::GetEntireTextureHeight()
{
	if(IsLoaded())
		return static_cast<HyTexturedQuad2dData *>(m_pData)->GetAtlasGroup()->GetHeight();
	else
		return 0;
}

uint32 HyTexturedQuad2d::GetNumTextures()
{
	if(IsLoaded())
		return static_cast<HyTexturedQuad2dData *>(m_pData)->GetAtlasGroup()->GetNumTextures();
	else
		return 0;
}

/*virtual*/ void HyTexturedQuad2d::OnDataLoaded()
{
	HyTexturedQuad2dData *pData = static_cast<HyTexturedQuad2dData *>(m_pData);
	m_RenderState.SetTextureHandle(pData->GetAtlasGroup()->GetGfxApiHandle());

	// Correct 'm_SrcRect' if using pixel coords
	if(m_SrcRect.left > 1.0f || m_SrcRect.top > 1.0f || m_SrcRect.right > 1.0f || m_SrcRect.bottom > 1.0f)
	{
		float fTexWidth = static_cast<float>(static_cast<HyTexturedQuad2dData *>(m_pData)->GetAtlasGroup()->GetWidth());
		float fTexHeight = static_cast<float>(static_cast<HyTexturedQuad2dData *>(m_pData)->GetAtlasGroup()->GetHeight());

		m_SrcRect.left = m_SrcRect.left / fTexWidth;
		m_SrcRect.top = m_SrcRect.top / fTexHeight;
		m_SrcRect.right = m_SrcRect.right / fTexWidth;
		m_SrcRect.bottom = m_SrcRect.bottom / fTexHeight;
	}
}

/*virtual*/ void HyTexturedQuad2d::OnUpdate()
{
}

/*virtual*/ void HyTexturedQuad2d::OnUpdateUniforms(HyShaderUniforms *pShaderUniformsRef)
{
}

/*virtual*/ void HyTexturedQuad2d::DefaultWriteDrawBufferData(char *&pRefDataWritePos)
{
	HyTexturedQuad2dData *pData = static_cast<HyTexturedQuad2dData *>(m_pData);

	glm::vec2 vSize(m_SrcRect.Width() * pData->GetAtlasGroup()->GetWidth(), m_SrcRect.Height() * pData->GetAtlasGroup()->GetHeight());
	*reinterpret_cast<glm::vec2 *>(pRefDataWritePos) = vSize;
	pRefDataWritePos += sizeof(glm::vec2);

	glm::vec2 vOffset(0.0f, 0.0f);
	*reinterpret_cast<glm::vec2 *>(pRefDataWritePos) = vOffset;
	pRefDataWritePos += sizeof(glm::vec2);

	*reinterpret_cast<glm::vec4 *>(pRefDataWritePos) = color.Get();
	pRefDataWritePos += sizeof(glm::vec4);

	*reinterpret_cast<float *>(pRefDataWritePos) = static_cast<float>(m_uiTextureIndex);
	pRefDataWritePos += sizeof(float);

	glm::vec2 vUV;

	vUV.x = m_SrcRect.right;//1.0f;
	vUV.y = m_SrcRect.top;//0.0f;
	*reinterpret_cast<glm::vec2 *>(pRefDataWritePos) = vUV;
	pRefDataWritePos += sizeof(glm::vec2);

	vUV.x = m_SrcRect.left;//0.0f;
	vUV.y = m_SrcRect.top;//0.0f;
	*reinterpret_cast<glm::vec2 *>(pRefDataWritePos) = vUV;
	pRefDataWritePos += sizeof(glm::vec2);

	vUV.x = m_SrcRect.right;//1.0f;
	vUV.y = m_SrcRect.bottom;//1.0f;
	*reinterpret_cast<glm::vec2 *>(pRefDataWritePos) = vUV;
	pRefDataWritePos += sizeof(glm::vec2);

	vUV.x = m_SrcRect.left;//0.0f;
	vUV.y = m_SrcRect.bottom;//1.0f;
	*reinterpret_cast<glm::vec2 *>(pRefDataWritePos) = vUV;
	pRefDataWritePos += sizeof(glm::vec2);

	GetWorldTransform(*reinterpret_cast<glm::mat4 *>(pRefDataWritePos));
	pRefDataWritePos += sizeof(glm::mat4);
}
