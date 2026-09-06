/**************************************************************************
 *	HyFileAtlas.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2015 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Assets/Files/HyFileAtlas.h"
#include "Assets/HyAssets.h"
#include "Renderer/IHyRenderer.h"
#include "HyEngine.h"

HyFileAtlas::HyFileAtlas(std::string sFileName, uint32 uiBankId, uint32 uiIndexInBank, uint32 uiManifestIndex, HyJsonObj textureObj) :
	IHyFile(HYFILE_Atlas, sFileName, uiBankId, uiManifestIndex),
	m_uiINDEX_IN_BANK(uiIndexInBank),
	m_ImageInfo(textureObj["imageInfo"].GetUint()),
	m_TextureInfo(textureObj["textureInfo"].GetUint()),
	m_hTextureHandle(HY_UNUSED_HANDLE),
	m_uiNUM_FRAMES(textureObj["assets"].GetArray().Size()),
	m_pPixelData(nullptr),
	m_uiPixelDataSize(0),
	m_pFrames(nullptr)
{
	if(m_uiNUM_FRAMES > 0)
	{
		m_pFrames = HY_NEW Frame[m_uiNUM_FRAMES];

		HyJsonArray framesArrayRef = textureObj["assets"].GetArray();
		for(uint32 k = 0; k < m_uiNUM_FRAMES; ++k)
		{
			HyJsonObj srcFrameObj = framesArrayRef[k].GetObject();

			// NOTE: Masks are serialized in LEFT, TOP, RIGHT, BOTTOM order. Each value is uint16 bits.

			uint32 uiCropMaskHi = srcFrameObj["cropMaskHi"].GetUint();
			uint32 uiCropMaskLo = srcFrameObj["cropMaskLo"].GetUint();
			m_pFrames[k].m_uiCropMask = (static_cast<uint64>(uiCropMaskHi) << 32) | uiCropMaskLo;
		
			//m_pFrames[k].m_Rect.bottom = srcFrameObj["bottom"].GetUint();
			//m_pFrames[k].m_Rect.right = srcFrameObj["right"].GetUint();
			//m_pFrames[k].m_Rect.left = srcFrameObj["left"].GetUint();
			//m_pFrames[k].m_Rect.top = srcFrameObj["top"].GetUint();

			uint32 uiFrameMaskHi = srcFrameObj["frameMaskHi"].GetUint();
			uint32 uiFrameMaskLo = srcFrameObj["frameMaskLo"].GetUint();
			m_pFrames[k].m_uiFrameMask = (static_cast<uint64>(uiFrameMaskHi) << 32) | uiFrameMaskLo;

			m_ChecksumMap[srcFrameObj["checksum"].GetUint()] = &m_pFrames[k];
		}
	}
}

HyFileAtlas::HyFileAtlas(HyAuxiliaryFileHandle hGivenHandle, std::string sFileName, HyImageInfo imageInfo, HyTextureIn textureInfo) :
	IHyFile(HYFILE_Atlas, sFileName, std::numeric_limits<uint32>::max(), std::numeric_limits<uint32>::max()),
	m_uiINDEX_IN_BANK(std::numeric_limits<uint32>::max()),
	m_ImageInfo(imageInfo),
	m_TextureInfo(textureInfo),
	m_hTextureHandle(HY_UNUSED_HANDLE),
	m_uiNUM_FRAMES(1),
	m_pPixelData(nullptr),
	m_uiPixelDataSize(0),
	m_pFrames(nullptr)
{
	m_ChecksumMap[hGivenHandle] = nullptr;
}

HyFileAtlas::~HyFileAtlas()
{
	delete [] m_pFrames;
	DeletePixelData();
}

uint32 HyFileAtlas::GetIndexInBank() const
{
	return m_uiINDEX_IN_BANK;
}

int32 HyFileAtlas::GetWidth() const
{
	return m_ImageInfo.GetWidth();
}

int32 HyFileAtlas::GetHeight() const
{
	return m_ImageInfo.GetHeight();
}

HyTextureHandle HyFileAtlas::GetTextureHandle() const
{
	return m_hTextureHandle;
}

bool HyFileAtlas::GetUvRect(uint32 uiChecksum, HyUvCoord &UVRectOut, uint64 &cropMaskOut) const
{
	float fTexWidth = static_cast<float>(GetWidth());
	float fTexHeight = static_cast<float>(GetHeight());
	HyAssert(fTexWidth > 0.0f && fTexHeight > 0.0f, "HyFileAtlas::GetUvRect was called before the texture was loaded");

	//const HyMargins<int32> *pSrcRect = nullptr;
	std::map<uint32, Frame *>::const_iterator iter = m_ChecksumMap.find(uiChecksum);
	if(iter != m_ChecksumMap.end())
	{
		//pSrcRect = ;
		uint64 uiFrameMask = iter->second->m_uiFrameMask;

		// LEFT, TOP, RIGHT, BOTTOM = 16 bits each. Laid out like: 0xLLLLTTTTRRRRBBBB
		uint16 uiLeft = static_cast<uint16>((uiFrameMask >> 48) & 0xFFFF);
		uint16 uiTop = static_cast<uint16>((uiFrameMask >> 32) & 0xFFFF);
		uint16 uiRight = static_cast<uint16>((uiFrameMask >> 16) & 0xFFFF);
		uint16 uiBottom = static_cast<uint16>(uiFrameMask & 0xFFFF);
		UVRectOut.left = static_cast<float>(uiLeft) / fTexWidth;
		UVRectOut.top = static_cast<float>(uiTop) / fTexHeight;
		UVRectOut.right = static_cast<float>(uiRight) / fTexWidth;
		UVRectOut.bottom = static_cast<float>(uiBottom) / fTexHeight;

		cropMaskOut = iter->second->m_uiCropMask;


		//UVRectOut.left = static_cast<float>(iter->second->m_uiFrameMask ->left) / fTexWidth;
		//UVRectOut.top = static_cast<float>(pSrcRect->top) / fTexHeight;
		//UVRectOut.right = static_cast<float>(pSrcRect->right) / fTexWidth;
		//UVRectOut.bottom = static_cast<float>(pSrcRect->bottom) / fTexHeight;

		return true;
		
	}

	return false;
}

void HyFileAtlas::DeletePixelData()
{
	HyIO::DeleteImage(m_pPixelData);

	//if(m_TextureInfo.GetFileType() == HYTEXTUREFILE_ASTC)
	//	delete[] m_pPixelData;
	//else
	//	SOIL_free_image_data(m_pPixelData);// stbi_image_free(m_pPixelData);

	m_pPixelData = nullptr;
	m_uiPixelDataSize = 0;
}

/*virtual*/ std::string HyFileAtlas::AssetTypeName() /*override*/
{
	return "Atlas";
}

/*virtual*/ void HyFileAtlas::OnLoadThread() /*override*/
{
	m_Mutex_PixelData.lock();

	if(GetLoadableState() == HYLOADSTATE_Queued)
	{
		if(m_pPixelData)
		{
			m_Mutex_PixelData.unlock();
			return;
		}

		std::string sAtlasFilePath;
		if(IsAuxiliary() == false)//m_iWidth != 0 && m_iHeight != 0)
		{
			sAtlasFilePath = HyEngine::DataDir() + HYASSETS_AtlasDir;
			
			char szTmpBuffer[16];
			sprintf(szTmpBuffer, "%05d", m_uiBANK_ID);
			sAtlasFilePath += szTmpBuffer;
			
			sAtlasFilePath += "/";
			sAtlasFilePath += m_sFILE_NAME;
		}
		else
			sAtlasFilePath = m_sFILE_NAME; // This is an auxiliary file, don't prepend the data directory

		m_pPixelData = HyIO::ReadImage(sAtlasFilePath, m_ImageInfo, m_uiPixelDataSize);
		if(m_pPixelData == nullptr)
			HyLogError("HyFileAtlas::OnLoadThread - failed to load image data: " << sAtlasFilePath);
	}

	m_Mutex_PixelData.unlock();
}

/*virtual*/ void HyFileAtlas::OnRenderThread(IHyRenderer &rendererRef) /*override*/
{
	m_Mutex_PixelData.lock();
	if(GetLoadableState() == HYLOADSTATE_Queued)
	{
		if(GetWidth() > 0 && GetHeight() > 0)
			m_hTextureHandle = rendererRef.AddTexture(m_ImageInfo, m_TextureInfo, m_pPixelData, m_uiPixelDataSize);
		else
			HyLogError("HyFileAtlas::OnRenderThread() - Texture was invalid");
		DeletePixelData();
	}
	else // GetLoadableState() == HYLOADSTATE_Discarded
	{
		rendererRef.DeleteTexture(m_hTextureHandle);
	}
	m_Mutex_PixelData.unlock();
}

/*virtual*/ std::string HyFileAtlas::GetAssetInfo() /*override*/
{
	std::stringstream ss;
	if(IsAuxiliary())
		ss << "[AUX] " << HyIO::GetFileNameFromPath(m_sFILE_NAME);
	else
		ss << "[" << std::setw(3) << std::setfill('0') << m_uiMANIFEST_INDEX << "] Bank " << m_uiBANK_ID << ", Index " << m_uiINDEX_IN_BANK << " (" << HyAssets::GetImageTypeName(m_ImageInfo.GetType()) << ")";
	
	return ss.str();
}
