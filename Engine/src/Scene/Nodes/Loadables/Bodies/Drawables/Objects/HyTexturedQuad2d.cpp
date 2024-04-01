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
#include "Assets/Nodes/Objects/HyTexturedQuadData.h"
#include "Assets/Files/HyFileAtlas.h"
#include "Utilities/HyIO.h"
#include "HyEngine.h"

#include "vendor/SOIL2/src/SOIL2/SOIL2.h"

HyTexturedQuad2d::HyTexturedQuad2d(HyEntity2d *pParent /*= nullptr*/) :
	IHyDrawable2d(HYTYPE_TexturedQuad, HyNodePath(), pParent)
{
	m_ShaderUniforms.SetNumTexUnits(1);
}

HyTexturedQuad2d::HyTexturedQuad2d(const HyNodePath &nodePath, HyEntity2d *pParent /*= nullptr*/) :
	IHyDrawable2d(HYTYPE_TexturedQuad, nodePath, pParent)
{
	m_ShaderUniforms.SetNumTexUnits(1);
}

HyTexturedQuad2d::HyTexturedQuad2d(uint32 uiAtlasFrameChecksum, uint32 uiBankId, HyEntity2d *pParent /*= nullptr*/) :
	IHyDrawable2d(HYTYPE_TexturedQuad, HyNodePath(uiAtlasFrameChecksum, uiBankId), pParent)
{
	m_ShaderUniforms.SetNumTexUnits(1);
}

HyTexturedQuad2d::HyTexturedQuad2d(HyTextureQuadHandle hTextureQuadHandle, HyEntity2d *pParent /*= nullptr*/) :
	IHyDrawable2d(HYTYPE_TexturedQuad, HyNodePath(hTextureQuadHandle.first, hTextureQuadHandle.second), pParent)
{
	m_ShaderUniforms.SetNumTexUnits(1);
}

HyTexturedQuad2d::HyTexturedQuad2d(std::string sFilePath, HyTextureInfo textureInfo, HyEntity2d *pParent /*= nullptr*/) :
	IHyDrawable2d(HYTYPE_TexturedQuad, HyNodePath(), pParent)
{
	m_ShaderUniforms.SetNumTexUnits(1);

	HyTextureQuadHandle hTexQuadHandle = HyEngine::CreateTexture(HyIO::CleanPath(sFilePath.c_str()), textureInfo);
	m_NodePath.Set(hTexQuadHandle.first, hTexQuadHandle.second);
}

HyTexturedQuad2d::HyTexturedQuad2d(const HyTexturedQuad2d &copyRef) :
	IHyDrawable2d(copyRef)
{
	m_ShaderUniforms.SetNumTexUnits(1);
}

HyTexturedQuad2d::~HyTexturedQuad2d()
{
}

const HyTexturedQuad2d &HyTexturedQuad2d::operator=(const HyTexturedQuad2d &rhs)
{
	IHyDrawable2d::operator=(rhs);
	m_UvCoords = rhs.m_UvCoords;

	return *this;
}

void HyTexturedQuad2d::Init(uint32 uiAtlasFrameChecksum, uint32 uiBankId, HyEntity2d *pParent)
{
	IHyLoadable2d::Init(HyNodePath(uiAtlasFrameChecksum, uiBankId), pParent);
}

void HyTexturedQuad2d::Init(HyTextureQuadHandle hTextureQuadHandle, HyEntity2d *pParent)
{
	IHyLoadable2d::Init(HyNodePath(hTextureQuadHandle.first, hTextureQuadHandle.second), pParent);
}

void HyTexturedQuad2d::Init(std::string sFilePath, HyTextureInfo textureInfo, HyEntity2d *pParent)
{
	HyTextureQuadHandle hTexQuadHandle = HyEngine::CreateTexture(HyIO::CleanPath(sFilePath.c_str()), textureInfo);
	IHyLoadable2d::Init(HyNodePath(hTexQuadHandle.first, hTexQuadHandle.second), pParent);
}

void HyTexturedQuad2d::Uninit()
{
	Unload();
	IHyLoadable2d::Init("N/A", "", m_pParent);
}

/*virtual*/ void HyTexturedQuad2d::CalcLocalBoundingShape(HyShape2d &shapeOut) /*override*/
{
	if(GetWidth() == 0 || GetHeight() == 0)
	{
		shapeOut.SetAsNothing();
		return;
	}

	shapeOut.SetAsBox(GetWidth(), GetHeight());
}

/*virtual*/ float HyTexturedQuad2d::GetWidth(float fPercent /*= 1.0f*/) /*override*/
{
	return (m_UvCoords.Width() * GetEntireTextureWidth()) * fPercent;
}

/*virtual*/ float HyTexturedQuad2d::GetHeight(float fPercent /*= 1.0f*/) /*override*/
{
	return (m_UvCoords.Height() * GetEntireTextureHeight()) * fPercent;
}

void HyTexturedQuad2d::SetUvCoordinates(int iX, int iY, int iWidth, int iHeight)
{
	int32 iTextureWidth = GetEntireTextureWidth();
	int32 iTextureHeight = GetEntireTextureHeight();
	if(iTextureWidth == 0 || iTextureHeight == 0)
	{
		HyLogWarning("HyTexturedQuad2d::SetUvCoordinates() was called before the texture was loaded");
		return;
	}

	float fX = static_cast<float>(iX);
	float fY = static_cast<float>(iY);
	float fWidth = static_cast<float>(iWidth);
	float fHeight = static_cast<float>(iHeight);

	m_UvCoords.left = fX / iTextureWidth;
	m_UvCoords.top = fY / iTextureHeight;
	m_UvCoords.right = (fX + fWidth) / iTextureWidth;
	m_UvCoords.bottom = (fY + fHeight) / iTextureHeight;
}

int32 HyTexturedQuad2d::GetEntireTextureWidth()
{
	if(AcquireData() == nullptr)
	{
		HyLogWarning("HyTexturedQuad2d::GetEntireTextureWidth() was called with invalid data");
		return 0;
	}

	return static_cast<const HyTexturedQuadData *>(UncheckedGetData())->GetAtlas()->GetWidth();
}

int32 HyTexturedQuad2d::GetEntireTextureHeight()
{
	if(AcquireData() == nullptr)
	{
		HyLogWarning("HyTexturedQuad2d::GetEntireTextureHeight() was called with invalid data");
		return 0;
	}

	return static_cast<const HyTexturedQuadData *>(UncheckedGetData())->GetAtlas()->GetHeight();
}

/*virtual*/ bool HyTexturedQuad2d::IsLoadDataValid() /*override*/
{
	const HyTexturedQuadData *pData = static_cast<const HyTexturedQuadData *>(AcquireData());
	return pData && pData->GetAtlas() != nullptr;
}

/*virtual*/ void HyTexturedQuad2d::OnDataAcquired() /*override*/
{
	// NOTE: Data is only valid when the checksum ctor is used (checksum stored in the name as a string). Otherwise OnDataAcquired() isn't invoked and internal data is set when the texture is 'hotloaded' within Load()
	const HyTexturedQuadData *pData = static_cast<const HyTexturedQuadData *>(UncheckedGetData());
	m_UvCoords = pData->GetUvCoords();
}

/*virtual*/ void HyTexturedQuad2d::OnLoaded() /*override*/
{
	IHyDrawable2d::OnLoaded();
	m_ShaderUniforms.SetTexHandle(0, static_cast<const HyTexturedQuadData *>(UncheckedGetData())->GetAtlas()->GetTextureHandle());

//	if(IsAuxiliary()) // Do blocking load of texture
//	{
//		int iNum8bitClrChannels;
//		uint8 *pPixelData = SOIL_load_image(m_sPrefix.c_str(), &m_iFullTextureWidth, &m_iFullTextureHeight, &iNum8bitClrChannels, 4);
//		uint32 uiPixelDataSize = m_iFullTextureWidth * m_iFullTextureHeight * 4;
//
//		uint32 uiBucketId = static_cast<uint32>(std::stoll(GetName()));
//		m_hTextureHandle = HyEngine::Renderer().AddTexture(
//			HyTextureInfo(uiBucketId),
//			m_iFullTextureWidth,
//			m_iFullTextureHeight,
//			pPixelData,
//			uiPixelDataSize,
//			0);
//		SOIL_free_image_data(pPixelData);
//
//		m_ShaderUniforms.SetTexHandle(0, m_hTextureHandle);
//		m_UvRect.Set(0.0f, 1.0f, 1.0f, 0.0f);
//	}
//	else
//	{
//		m_hTextureHandle = static_cast<const HyTexturedQuadData *>(UncheckedGetData())->GetAtlas()->GetTextureHandle();
//		m_ShaderUniforms.SetTexHandle(0, m_hTextureHandle);
//	}
}

///*virtual*/ void HyTexturedQuad2d::OnUnloaded() /*override*/
//{
//	IHyDrawable2d::OnUnloaded();
//
//	if(IsAuxiliaryFile())
//	{
//		HyEngine::Renderer().DeleteTexture(m_hTextureHandle);
//		m_hTextureHandle = HY_UNUSED_HANDLE;
//	}
//}

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
	glm::vec2 vSize(m_UvCoords.Width() * pData->GetAtlas()->GetWidth(), m_UvCoords.Height() * pData->GetAtlas()->GetHeight());

	vertexBufferRef.AppendData2d(&vSize, sizeof(glm::vec2));

	glm::vec2 vOffset(0.0f, 0.0f);
	vertexBufferRef.AppendData2d(&vOffset, sizeof(glm::vec2));

	vertexBufferRef.AppendData2d(&CalculateTopTint(fExtrapolatePercent), sizeof(glm::vec3));

	float fAlpha = CalculateAlpha(fExtrapolatePercent);
	vertexBufferRef.AppendData2d(&fAlpha, sizeof(float));
	vertexBufferRef.AppendData2d(&CalculateBotTint(fExtrapolatePercent), sizeof(glm::vec3));
	vertexBufferRef.AppendData2d(&fAlpha, sizeof(float));

	glm::vec2 vUV;
	vUV.x = m_UvCoords.right;//1.0f;
	vUV.y = m_UvCoords.top;//1.0f;
	vertexBufferRef.AppendData2d(&vUV, sizeof(glm::vec2));
	
	vUV.x = m_UvCoords.left;//0.0f;
	vUV.y = m_UvCoords.top;//1.0f;
	vertexBufferRef.AppendData2d(&vUV, sizeof(glm::vec2));
	
	vUV.x = m_UvCoords.right;//1.0f;
	vUV.y = m_UvCoords.bottom;//0.0f;
	vertexBufferRef.AppendData2d(&vUV, sizeof(glm::vec2));

	vUV.x = m_UvCoords.left;//0.0f;
	vUV.y = m_UvCoords.bottom;//0.0f;
	vertexBufferRef.AppendData2d(&vUV, sizeof(glm::vec2));

	vertexBufferRef.AppendData2d(&GetSceneTransform(fExtrapolatePercent), sizeof(glm::mat4));

	return true;
}
