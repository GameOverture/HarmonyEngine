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
	m_RenderState.Enable(HyRenderState::DRAWMODE_TRIANGLESTRIP | HyRenderState::SHADER_QUADBATCH);
	m_RenderState.SetNumInstances(1);
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

		m_SrcRect.x = fX / fTexWidth;
		m_SrcRect.y = fY / fTexHeight;
		m_SrcRect.width = fX + (fWidth / fTexWidth);
		m_SrcRect.height = fY + (fHeight / fTexHeight);
	}
	else
	{
		m_SrcRect.x = static_cast<float>(iX);
		m_SrcRect.y = static_cast<float>(iY);
		m_SrcRect.width = static_cast<float>(iWidth);
		m_SrcRect.height = static_cast<float>(iHeight);
	}

	m_uiTextureIndex = uiTextureIndex;
}

uint32 HyTexturedQuad2d::GetTextureIndex()
{
	return m_uiTextureIndex;
}

uint32 HyTexturedQuad2d::GetTextureWidth()
{
	return static_cast<HyTexturedQuad2dData *>(m_pData)->GetAtlasGroup()->GetWidth();
}

uint32 HyTexturedQuad2d::GetTextureHeight()
{
	return static_cast<HyTexturedQuad2dData *>(m_pData)->GetAtlasGroup()->GetHeight();
}

uint32 HyTexturedQuad2d::GetNumTextures()
{
	return static_cast<HyTexturedQuad2dData *>(m_pData)->GetAtlasGroup()->GetNumTextures();
}

/*virtual*/ void HyTexturedQuad2d::OnDataLoaded()
{
	HyTexturedQuad2dData *pData = static_cast<HyTexturedQuad2dData *>(m_pData);
	m_RenderState.SetTextureHandle(pData->GetAtlasGroup()->GetGfxApiHandle());

	// Correct 'm_SrcRect' if using pixel coords
	if(m_SrcRect.x > 1.0f || m_SrcRect.y > 1.0f || m_SrcRect.width > 1.0f || m_SrcRect.height > 1.0f)
	{
		float fTexWidth = static_cast<float>(static_cast<HyTexturedQuad2dData *>(m_pData)->GetAtlasGroup()->GetWidth());
		float fTexHeight = static_cast<float>(static_cast<HyTexturedQuad2dData *>(m_pData)->GetAtlasGroup()->GetHeight());

		m_SrcRect.x = m_SrcRect.x / fTexWidth;
		m_SrcRect.y = m_SrcRect.y / fTexHeight;
		m_SrcRect.width = m_SrcRect.x + (m_SrcRect.width / fTexWidth);
		m_SrcRect.height = m_SrcRect.y + (m_SrcRect.height / fTexHeight);
	}
}

/*virtual*/ void HyTexturedQuad2d::Update()
{
}

/*virtual*/ void HyTexturedQuad2d::WriteDrawBufferData(char *&pRefDataWritePos)
{
	HyTexturedQuad2dData *pData = static_cast<HyTexturedQuad2dData *>(m_pData);

	vec2 vSize((m_SrcRect.width - m_SrcRect.x) * pData->GetAtlasGroup()->GetWidth(), (m_SrcRect.height - m_SrcRect.y) * pData->GetAtlasGroup()->GetHeight());
	vec2 vOffset(pos.X(), pos.Y());

	*reinterpret_cast<vec2 *>(pRefDataWritePos) = vSize;
	pRefDataWritePos += sizeof(vec2);
	*reinterpret_cast<vec2 *>(pRefDataWritePos) = vOffset;
	pRefDataWritePos += sizeof(vec2);

	*reinterpret_cast<vec4 *>(pRefDataWritePos) = color.Get();
	pRefDataWritePos += sizeof(vec4);

	*reinterpret_cast<uint32 *>(pRefDataWritePos) = m_uiTextureIndex;
	pRefDataWritePos += sizeof(uint32);

	vec2 vUV;

	vUV.x = m_SrcRect.width;//1.0f;
	vUV.y = m_SrcRect.y;//0.0f;
	*reinterpret_cast<vec2 *>(pRefDataWritePos) = vUV;
	pRefDataWritePos += sizeof(vec2);

	vUV.x = m_SrcRect.x;//0.0f;
	vUV.y = m_SrcRect.y;//0.0f;
	*reinterpret_cast<vec2 *>(pRefDataWritePos) = vUV;
	pRefDataWritePos += sizeof(vec2);

	vUV.x = m_SrcRect.width;//1.0f;
	vUV.y = m_SrcRect.height;//1.0f;
	*reinterpret_cast<vec2 *>(pRefDataWritePos) = vUV;
	pRefDataWritePos += sizeof(vec2);

	vUV.x = m_SrcRect.x;//0.0f;
	vUV.y = m_SrcRect.height;//1.0f;
	*reinterpret_cast<vec2 *>(pRefDataWritePos) = vUV;
	pRefDataWritePos += sizeof(vec2);

	GetWorldTransform(*reinterpret_cast<mat4 *>(pRefDataWritePos));
	pRefDataWritePos += sizeof(mat4);
}
