/**************************************************************************
*	HyTexturedQuad2d.h
*
*	Harmony Engine
*	Copyright (c) 2015 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Scene/Nodes/Draws/HyTexturedQuad2d.h"

#include "Assets/Nodes/HyTexturedQuad2dData.h"

HyTexturedQuad2d::HyTexturedQuad2d(uint32 uiAtlasGroupId) :	IHyDraw2d(HYTYPE_TexturedQuad2d, NULL, std::to_string(uiAtlasGroupId).c_str()),
															m_bIS_RAW(false),
															m_uiRAW_TEXTURE_WIDTH(0),
															m_uiRAW_TEXTURE_HEIGHT(0),
															m_uiTextureIndex(0),
															m_SrcRect(0.0f, 0.0f, 1.0f, 1.0f)
{
	m_RenderState.Enable(HyRenderState::DRAWMODE_TRIANGLESTRIP | HyRenderState::DRAWINSTANCED);
	m_RenderState.SetShaderId(HYSHADERPROG_QuadBatch);
	m_RenderState.SetNumInstances(1);
	m_RenderState.SetNumVerticesPerInstance(4);
}

HyTexturedQuad2d::HyTexturedQuad2d(uint32 uiGfxApiHandle, uint32 uiTextureWidth, uint32 uiTextureHeight) :	IHyDraw2d(HYTYPE_TexturedQuad2d, NULL, "raw"),
																											m_bIS_RAW(true),
																											m_uiRAW_TEXTURE_WIDTH(uiTextureWidth),
																											m_uiRAW_TEXTURE_HEIGHT(uiTextureHeight),
																											m_uiTextureIndex(0),
																											m_SrcRect(0.0f, 0.0f, 1.0f, 1.0f)
{
	m_RenderState.Enable(HyRenderState::DRAWMODE_TRIANGLESTRIP | HyRenderState::DRAWINSTANCED);
	m_RenderState.SetShaderId(HYSHADERPROG_QuadBatch);
	m_RenderState.SetNumInstances(1);
	m_RenderState.SetNumVerticesPerInstance(4);
	m_RenderState.SetTextureHandle(uiGfxApiHandle);
}

HyTexturedQuad2d::~HyTexturedQuad2d()
{
}

uint32 HyTexturedQuad2d::GetAtlasGroupId()
{
	if(m_bIS_RAW)
		return 0xFFFFFFFF;

	return static_cast<HyTexturedQuad2dData *>(AcquireData())->GetAtlasGroup()->GetId();
}

uint32 HyTexturedQuad2d::GetGraphicsApiHandle() const
{
	return m_RenderState.GetTextureHandle();
}

void HyTexturedQuad2d::SetTextureSource(uint32 uiTextureIndex)
{
	m_uiTextureIndex = uiTextureIndex;
}

void HyTexturedQuad2d::SetTextureSource(uint32 uiTextureIndex, int iX, int iY, int iWidth, int iHeight)
{
	float fX = static_cast<float>(iX);
	float fY = static_cast<float>(iY);
	float fWidth = static_cast<float>(iWidth);
	float fHeight = static_cast<float>(iHeight);
	float fTexWidth = m_bIS_RAW ? m_uiRAW_TEXTURE_WIDTH : static_cast<float>(static_cast<HyTexturedQuad2dData *>(AcquireData())->GetAtlasGroup()->GetWidth());
	float fTexHeight = m_bIS_RAW ? m_uiRAW_TEXTURE_HEIGHT : static_cast<float>(static_cast<HyTexturedQuad2dData *>(AcquireData())->GetAtlasGroup()->GetHeight());

	m_SrcRect.left = fX / fTexWidth;
	m_SrcRect.top = fY / fTexHeight;
	m_SrcRect.right = (fX + fWidth) / fTexWidth;
	m_SrcRect.bottom = (fY + fHeight) / fTexHeight;

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
	if(m_bIS_RAW)
		return m_uiRAW_TEXTURE_WIDTH;

	return static_cast<HyTexturedQuad2dData *>(AcquireData())->GetAtlasGroup()->GetWidth();
}

uint32 HyTexturedQuad2d::GetEntireTextureHeight()
{
	if(m_bIS_RAW)
		return m_uiRAW_TEXTURE_HEIGHT;

	return static_cast<HyTexturedQuad2dData *>(AcquireData())->GetAtlasGroup()->GetHeight();
}

uint32 HyTexturedQuad2d::GetNumTextures()
{
	if(m_bIS_RAW)
		return 1;

	return static_cast<HyTexturedQuad2dData *>(AcquireData())->GetAtlasGroup()->GetNumTextures();
}

/*virtual*/ void HyTexturedQuad2d::OnUpdate()
{
	if(IsSelfLoaded() == false || m_bIS_RAW)
		return;

	m_RenderState.SetTextureHandle(static_cast<HyTexturedQuad2dData *>(UncheckedGetData())->GetAtlasGroup()->GetGfxApiHandle(m_uiTextureIndex));
}

/*virtual*/ void HyTexturedQuad2d::OnUpdateUniforms()
{
	//m_ShaderUniforms.Set(...);
}

/*virtual*/ void HyTexturedQuad2d::OnWriteDrawBufferData(char *&pRefDataWritePos)
{
	HyTexturedQuad2dData *pData = static_cast<HyTexturedQuad2dData *>(UncheckedGetData());

	glm::vec2 vSize(m_SrcRect.Width() * (m_bIS_RAW ? m_uiRAW_TEXTURE_WIDTH : pData->GetAtlasGroup()->GetWidth()),
					m_SrcRect.Height() * (m_bIS_RAW ? m_uiRAW_TEXTURE_HEIGHT : pData->GetAtlasGroup()->GetHeight()));
	*reinterpret_cast<glm::vec2 *>(pRefDataWritePos) = vSize;
	pRefDataWritePos += sizeof(glm::vec2);

	glm::vec2 vOffset(0.0f, 0.0f);
	*reinterpret_cast<glm::vec2 *>(pRefDataWritePos) = vOffset;
	pRefDataWritePos += sizeof(glm::vec2);

	*reinterpret_cast<glm::vec3 *>(pRefDataWritePos) = topColor.Get();
	pRefDataWritePos += sizeof(glm::vec3);
	*reinterpret_cast<float *>(pRefDataWritePos) = alpha.Get();
	pRefDataWritePos += sizeof(float);

	*reinterpret_cast<glm::vec3 *>(pRefDataWritePos) = botColor.Get();
	pRefDataWritePos += sizeof(glm::vec3);
	*reinterpret_cast<float *>(pRefDataWritePos) = alpha.Get();
	pRefDataWritePos += sizeof(float);

	*reinterpret_cast<float *>(pRefDataWritePos) = static_cast<float>(m_bIS_RAW ? m_uiTextureIndex : pData->GetAtlasGroup()->GetActualGfxApiTextureIndex(m_uiTextureIndex));
	pRefDataWritePos += sizeof(float);

	glm::vec2 vUV;

	vUV.x = m_SrcRect.right;//1.0f;
	vUV.y = m_SrcRect.top;//1.0f;
	*reinterpret_cast<glm::vec2 *>(pRefDataWritePos) = vUV;
	pRefDataWritePos += sizeof(glm::vec2);

	vUV.x = m_SrcRect.left;//0.0f;
	vUV.y = m_SrcRect.top;//1.0f;
	*reinterpret_cast<glm::vec2 *>(pRefDataWritePos) = vUV;
	pRefDataWritePos += sizeof(glm::vec2);

	vUV.x = m_SrcRect.right;//1.0f;
	vUV.y = m_SrcRect.bottom;//0.0f;
	*reinterpret_cast<glm::vec2 *>(pRefDataWritePos) = vUV;
	pRefDataWritePos += sizeof(glm::vec2);

	vUV.x = m_SrcRect.left;//0.0f;
	vUV.y = m_SrcRect.bottom;//0.0f;
	*reinterpret_cast<glm::vec2 *>(pRefDataWritePos) = vUV;
	pRefDataWritePos += sizeof(glm::vec2);

	GetWorldTransform(*reinterpret_cast<glm::mat4 *>(pRefDataWritePos));
	pRefDataWritePos += sizeof(glm::mat4);
}
