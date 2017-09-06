/**************************************************************************
*	HyTexturedQuad2d.cpp
*
*	Harmony Engine
*	Copyright (c) 2015 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Scene/Nodes/Leafs/Draws/HyTexturedQuad2d.h"
#include "Scene/Nodes/Entities/HyEntity2d.h"
#include "Assets/Nodes/HyTexturedQuad2dData.h"

HyTexturedQuad2d::HyTexturedQuad2d(uint32 uiAtlasGrpId, uint32 uiIndexInGroup, HyEntity2d *pParent) :	IHyLeafDraw2d(HYTYPE_TexturedQuad2d, std::to_string(uiAtlasGrpId).c_str(), std::to_string(uiIndexInGroup).c_str(), pParent),
																										m_bIS_RAW(false),
																										m_uiATLAS_GROUP_ID(uiAtlasGrpId),
																										m_uiATLAS_INDEX_IN_GROUP(uiIndexInGroup),
																										m_uiRAW_TEXTURE_WIDTH(0),
																										m_uiRAW_TEXTURE_HEIGHT(0),
																										m_SrcRect(0.0f, 0.0f, 1.0f, 1.0f)
{
	m_RenderState.SetRenderMode(HYRENDERMODE_TriangleStrip);
	m_RenderState.Enable(HyRenderState::DRAWINSTANCED);
	m_RenderState.SetShaderId(HYSHADERPROG_QuadBatch);
	m_RenderState.SetNumInstances(1);
	m_RenderState.SetNumVerticesPerInstance(4);
}

HyTexturedQuad2d::HyTexturedQuad2d(uint32 uiGfxApiHandle, uint32 uiTextureWidth, uint32 uiTextureHeight, HyEntity2d *pParent) :	IHyLeafDraw2d(HYTYPE_TexturedQuad2d, nullptr, "raw", pParent),
																																m_bIS_RAW(true),
																																m_uiATLAS_GROUP_ID(0),
																																m_uiATLAS_INDEX_IN_GROUP(0),
																																m_uiRAW_TEXTURE_WIDTH(uiTextureWidth),
																																m_uiRAW_TEXTURE_HEIGHT(uiTextureHeight),
																																m_SrcRect(0.0f, 0.0f, 1.0f, 1.0f)
{
	m_RenderState.SetRenderMode(HYRENDERMODE_TriangleStrip);
	m_RenderState.Enable(HyRenderState::DRAWINSTANCED);
	m_RenderState.SetShaderId(HYSHADERPROG_QuadBatch);
	m_RenderState.SetNumInstances(1);
	m_RenderState.SetNumVerticesPerInstance(4);
	m_RenderState.SetTextureHandle(uiGfxApiHandle);
}

HyTexturedQuad2d::~HyTexturedQuad2d()
{
}

uint32 HyTexturedQuad2d::GetGraphicsApiHandle() const
{
	return m_RenderState.GetTextureHandle();
}

void HyTexturedQuad2d::SetTextureSource(int iX, int iY, int iWidth, int iHeight)
{
	float fX = static_cast<float>(iX);
	float fY = static_cast<float>(iY);
	float fWidth = static_cast<float>(iWidth);
	float fHeight = static_cast<float>(iHeight);
	float fTexWidth = m_bIS_RAW ? m_uiRAW_TEXTURE_WIDTH : static_cast<float>(static_cast<HyTexturedQuad2dData *>(AcquireData())->GetAtlas()->GetWidth());
	float fTexHeight = m_bIS_RAW ? m_uiRAW_TEXTURE_HEIGHT : static_cast<float>(static_cast<HyTexturedQuad2dData *>(AcquireData())->GetAtlas()->GetHeight());

	m_SrcRect.left = fX / fTexWidth;
	m_SrcRect.top = fY / fTexHeight;
	m_SrcRect.right = (fX + fWidth) / fTexWidth;
	m_SrcRect.bottom = (fY + fHeight) / fTexHeight;
}

uint32 HyTexturedQuad2d::GetAtlasIndexInGroup()
{
	return m_uiATLAS_INDEX_IN_GROUP;
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

	return static_cast<HyTexturedQuad2dData *>(AcquireData())->GetAtlas()->GetWidth();
}

uint32 HyTexturedQuad2d::GetEntireTextureHeight()
{
	if(m_bIS_RAW)
		return m_uiRAW_TEXTURE_HEIGHT;

	return static_cast<HyTexturedQuad2dData *>(AcquireData())->GetAtlas()->GetHeight();
}

/*virtual*/ void HyTexturedQuad2d::CalcBoundingVolume() /*override*/
{
	HyError("HyText2d::CalcBoundingVolume() not implemented");
}

/*virtual*/ void HyTexturedQuad2d::AcquireBoundingVolumeIndex(uint32 &uiStateOut, uint32 &uiSubStateOut) /*override*/
{
	uiStateOut = uiSubStateOut = 0;
}

/*virtual*/ void HyTexturedQuad2d::DrawUpdate()
{
	if(IsLoaded() == false || m_bIS_RAW)
		return;

	m_RenderState.SetTextureHandle(static_cast<HyTexturedQuad2dData *>(UncheckedGetData())->GetAtlas()->GetGfxApiHandle());
}

/*virtual*/ void HyTexturedQuad2d::OnUpdateUniforms()
{
	//m_ShaderUniforms.Set(...);
}

/*virtual*/ void HyTexturedQuad2d::OnWriteDrawBufferData(char *&pRefDataWritePos)
{
	HyTexturedQuad2dData *pData = static_cast<HyTexturedQuad2dData *>(UncheckedGetData());

	glm::vec2 vSize(m_SrcRect.Width() * (m_bIS_RAW ? m_uiRAW_TEXTURE_WIDTH : pData->GetAtlas()->GetWidth()),
					m_SrcRect.Height() * (m_bIS_RAW ? m_uiRAW_TEXTURE_HEIGHT : pData->GetAtlas()->GetHeight()));
	*reinterpret_cast<glm::vec2 *>(pRefDataWritePos) = vSize;
	pRefDataWritePos += sizeof(glm::vec2);

	glm::vec2 vOffset(0.0f, 0.0f);
	*reinterpret_cast<glm::vec2 *>(pRefDataWritePos) = vOffset;
	pRefDataWritePos += sizeof(glm::vec2);

	*reinterpret_cast<glm::vec3 *>(pRefDataWritePos) = CalculateTopTint();
	pRefDataWritePos += sizeof(glm::vec3);
	*reinterpret_cast<float *>(pRefDataWritePos) = CalculateAlpha();
	pRefDataWritePos += sizeof(float);

	*reinterpret_cast<glm::vec3 *>(pRefDataWritePos) = CalculateBotTint();
	pRefDataWritePos += sizeof(glm::vec3);
	*reinterpret_cast<float *>(pRefDataWritePos) = CalculateAlpha();
	pRefDataWritePos += sizeof(float);

	//*reinterpret_cast<float *>(pRefDataWritePos) = static_cast<float>(m_bIS_RAW ? m_uiTextureIndex : pData->GetAtlas()->GetActualGfxApiTextureIndex(m_uiTextureIndex));
	//pRefDataWritePos += sizeof(float);

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
