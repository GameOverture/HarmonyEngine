/**************************************************************************
*	HyTexturedQuad2d.cpp
*
*	Harmony Engine
*	Copyright (c) 2015 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Loadables/Bodies/Drawables/Objects/HyTexturedQuad2d.h"
#include "Scene/Nodes/Loadables/Bodies/Objects/HyEntity2d.h"
#include "Assets/Nodes/HyTexturedQuadData.h"
#include "Utilities/HyIO.h"
#include "HyEngine.h"

#include "vendor/SOIL2/src/SOIL2/SOIL2.h"

HyTexturedQuad2d::HyTexturedQuad2d(HyEntity2d *pParent /*= nullptr*/) :
	IHyDrawable2d(HYTYPE_TexturedQuad, "N/A", "", pParent),
	m_iFullTextureWidth(0),
	m_iFullTextureHeight(0),
	m_hTextureHandle(HY_UNUSED_HANDLE)
{
	m_ShaderUniforms.SetNumTexUnits(1);
}

HyTexturedQuad2d::HyTexturedQuad2d(uint32 uiAtlasFrameChecksum, uint32 uiBankId, HyEntity2d *pParent /*= nullptr*/) :
	IHyDrawable2d(HYTYPE_TexturedQuad, "", std::to_string(uiAtlasFrameChecksum) + ":" + std::to_string(uiBankId), pParent),
	m_iFullTextureWidth(0),
	m_iFullTextureHeight(0),
	m_hTextureHandle(HY_UNUSED_HANDLE)
{
	m_ShaderUniforms.SetNumTexUnits(1);
}

HyTexturedQuad2d::HyTexturedQuad2d(std::string sFilePath, HyTextureInfo useTextureInfo, HyEntity2d *pParent /*= nullptr*/) :
	IHyDrawable2d(HYTYPE_TexturedQuad, HyIO::CleanPath(sFilePath.c_str(), nullptr, true), std::to_string(useTextureInfo.GetBucketId()), pParent),
	m_iFullTextureWidth(0),
	m_iFullTextureHeight(0),
	m_hTextureHandle(HY_UNUSED_HANDLE)
{
	m_ShaderUniforms.SetNumTexUnits(1);
}

HyTexturedQuad2d::HyTexturedQuad2d(const HyTexturedQuad2d &copyRef) :
	IHyDrawable2d(copyRef),
	m_iFullTextureWidth(copyRef.m_iFullTextureWidth),
	m_iFullTextureHeight(copyRef.m_iFullTextureHeight),
	m_hTextureHandle(copyRef.m_hTextureHandle)
{
	m_ShaderUniforms.SetNumTexUnits(1);
}

HyTexturedQuad2d::~HyTexturedQuad2d()
{
}

const HyTexturedQuad2d &HyTexturedQuad2d::operator=(const HyTexturedQuad2d &rhs)
{
	IHyDrawable2d::operator=(rhs);

	m_iFullTextureWidth = rhs.m_iFullTextureWidth;
	m_iFullTextureHeight = rhs.m_iFullTextureHeight;
	m_UvRect = rhs.m_UvRect;
	m_hTextureHandle = rhs.m_hTextureHandle;

	return *this;
}

void HyTexturedQuad2d::Init(uint32 uiAtlasFrameChecksum, uint32 uiBankId, HyEntity2d *pParent)
{
	IHyLoadable2d::Init("", std::to_string(uiAtlasFrameChecksum) + ":" + std::to_string(uiBankId), pParent);
}

void HyTexturedQuad2d::Init(std::string sFilePath, HyTextureInfo useTextureInfo, HyEntity2d *pParent)
{
	IHyLoadable2d::Init(HyIO::CleanPath(sFilePath.c_str(), nullptr, true), std::to_string(useTextureInfo.GetBucketId()), pParent);
}

void HyTexturedQuad2d::Uninit()
{
	Unload();
	IHyLoadable2d::Init("N/A", "", m_pParent);
}

bool HyTexturedQuad2d::IsHotloading() const
{
	return m_sPrefix.empty() == false;
}

/*virtual*/ void HyTexturedQuad2d::CalcLocalBoundingShape(HyShape2d &shapeOut) /*override*/
{
	if(GetWidth() == 0 || GetHeight() == 0)
	{
		shapeOut.SetAsNothing();
		return;
	}

	shapeOut.SetAsBox(static_cast<float>(GetWidth()), static_cast<float>(GetHeight()));
}

//void HyTexturedQuad2d::SetUvCoordinates(int iX, int iY, int iWidth, int iHeight)
//{
//	if(m_uiFullTextureWidth == 0 || m_uiFullTextureHeight == 0)
//	{
//		HyLogWarning("HyTexturedQuad2d::SetUvCoordinates() was called before the texture was loaded");
//		return;
//	}
//
//	float fX = static_cast<float>(iX);
//	float fY = static_cast<float>(iY);
//	float fWidth = static_cast<float>(iWidth);
//	float fHeight = static_cast<float>(iHeight);
//
//	m_UvRect.left = fX / m_uiFullTextureWidth;
//	m_UvRect.top = fY / m_uiFullTextureHeight;
//	m_UvRect.right = (fX + fWidth) / m_uiFullTextureWidth;
//	m_UvRect.bottom = (fY + fHeight) / m_uiFullTextureHeight;
//}

uint32 HyTexturedQuad2d::GetWidth() const
{
	return static_cast<uint32>(m_UvRect.Width() * GetEntireTextureWidth());
}

uint32 HyTexturedQuad2d::GetHeight() const
{
	return static_cast<uint32>(m_UvRect.Height() * GetEntireTextureHeight());
}

int32 HyTexturedQuad2d::GetEntireTextureWidth() const
{
	return m_iFullTextureWidth;
}

int32 HyTexturedQuad2d::GetEntireTextureHeight() const
{
	return m_iFullTextureHeight;
}

/*virtual*/ void HyTexturedQuad2d::OnDataAcquired() /*override*/
{
	// NOTE: Data is only valid when the checksum ctor is used (checksum stored in the name as a string). Otherwise OnDataAcquired() isn't invoked and internal data is set when the texture is 'hotloaded' within Load()
	const HyTexturedQuadData *pData = static_cast<const HyTexturedQuadData *>(UncheckedGetData());

	m_iFullTextureWidth = pData->GetAtlas()->GetWidth();
	m_iFullTextureHeight = pData->GetAtlas()->GetHeight();

	uint32 uiChecksum = static_cast<uint32>(std::stoll(GetName()));
	pData->GetAtlas()->GetUvRect(uiChecksum, m_UvRect);
}

/*virtual*/ void HyTexturedQuad2d::OnLoaded() /*override*/
{
	IHyDrawable2d::OnLoaded();

	if(IsHotloading()) // Do blocking load of texture
	{
		int iNum8bitClrChannels;
		uint8 *pPixelData = SOIL_load_image(m_sPrefix.c_str(), &m_iFullTextureWidth, &m_iFullTextureHeight, &iNum8bitClrChannels, 4);
		uint32 uiPixelDataSize = m_iFullTextureWidth * m_iFullTextureHeight * 4;

		uint32 uiBucketId = static_cast<uint32>(std::stoll(GetName()));
		m_hTextureHandle = HyEngine::Renderer().AddTexture(
			HyTextureInfo(uiBucketId),
			m_iFullTextureWidth,
			m_iFullTextureHeight,
			pPixelData,
			uiPixelDataSize,
			0);
		SOIL_free_image_data(pPixelData);

		m_ShaderUniforms.SetTexHandle(0, m_hTextureHandle);
	}
	else
	{
		m_hTextureHandle = static_cast<const HyTexturedQuadData *>(UncheckedGetData())->GetAtlas()->GetTextureHandle();
		m_ShaderUniforms.SetTexHandle(0, m_hTextureHandle);
	}
}

/*virtual*/ void HyTexturedQuad2d::OnUnloaded() /*override*/
{
	IHyDrawable2d::OnUnloaded();

	if(IsHotloading())
	{
		HyEngine::Renderer().DeleteTexture(m_hTextureHandle);
		m_hTextureHandle = HY_UNUSED_HANDLE;
	}
}

/*virtual*/ bool HyTexturedQuad2d::OnIsValidToRender() /*override*/
{
	return true;
}

/*virtual*/ void HyTexturedQuad2d::PrepRenderStage(uint32 uiStageIndex, HyRenderMode &eRenderModeOut, uint32 &uiNumInstancesOut, uint32 &uiNumVerticesPerInstOut, bool &bIsBatchable) /*override*/
{
	eRenderModeOut = HYRENDERMODE_TriangleStrip;
	uiNumInstancesOut = 1;
	uiNumVerticesPerInstOut = 4;
	bIsBatchable = true;
}

/*virtual*/ bool HyTexturedQuad2d::WriteVertexData(uint32 uiNumInstances, HyVertexBuffer &vertexBufferRef, float fExtrapolatePercent)
{
	const HyTexturedQuadData *pData = static_cast<const HyTexturedQuadData *>(UncheckedGetData());

	glm::vec2 vSize(m_UvRect.Width() * m_iFullTextureWidth, m_UvRect.Height() * m_iFullTextureHeight);

	vertexBufferRef.AppendData2d(&vSize, sizeof(glm::vec2));

	glm::vec2 vOffset(0.0f, 0.0f);
	vertexBufferRef.AppendData2d(&vOffset, sizeof(glm::vec2));

	vertexBufferRef.AppendData2d(&CalculateTopTint(fExtrapolatePercent), sizeof(glm::vec3));

	float fAlpha = CalculateAlpha(fExtrapolatePercent);
	vertexBufferRef.AppendData2d(&fAlpha, sizeof(float));
	vertexBufferRef.AppendData2d(&CalculateBotTint(fExtrapolatePercent), sizeof(glm::vec3));
	vertexBufferRef.AppendData2d(&fAlpha, sizeof(float));

	glm::vec2 vUV;
	vUV.x = m_UvRect.right;//1.0f;
	vUV.y = m_UvRect.top;//1.0f;
	vertexBufferRef.AppendData2d(&vUV, sizeof(glm::vec2));
	
	vUV.x = m_UvRect.left;//0.0f;
	vUV.y = m_UvRect.top;//1.0f;
	vertexBufferRef.AppendData2d(&vUV, sizeof(glm::vec2));
	
	vUV.x = m_UvRect.right;//1.0f;
	vUV.y = m_UvRect.bottom;//0.0f;
	vertexBufferRef.AppendData2d(&vUV, sizeof(glm::vec2));

	vUV.x = m_UvRect.left;//0.0f;
	vUV.y = m_UvRect.bottom;//0.0f;
	vertexBufferRef.AppendData2d(&vUV, sizeof(glm::vec2));

	vertexBufferRef.AppendData2d(&GetSceneTransform(fExtrapolatePercent), sizeof(glm::mat4));

	return true;
}
