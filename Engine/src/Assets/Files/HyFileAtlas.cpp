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

#include "vendor/SOIL2/src/SOIL2/SOIL2.h"

HyFileAtlas::HyFileAtlas(std::string sFileName, uint32 uiBankId, uint32 uiIndexInBank, uint32 uiManifestIndex, HyJsonObj textureObj) :
	IHyFile(HYFILE_Atlas, sFileName, uiBankId, uiManifestIndex),
	m_uiINDEX_IN_BANK(uiIndexInBank),
	m_iWidth(textureObj["width"].GetInt()),
	m_iHeight(textureObj["height"].GetInt()),
	m_TextureInfo(textureObj["textureInfo"].GetUint()),
	m_hTextureHandle(HY_UNUSED_HANDLE),
	m_uiNUM_FRAMES(textureObj["assets"].GetArray().Size()),
	m_pPixelData(nullptr),
	m_uiPixelDataSize(0)
{
	m_pFrames = HY_NEW HyRectangle<int32>[m_uiNUM_FRAMES];

	HyJsonArray framesArrayRef = textureObj["assets"].GetArray();
	for(uint32 k = 0; k < m_uiNUM_FRAMES; ++k)
	{
		HyJsonObj srcFrameObj = framesArrayRef[k].GetObject();

		m_pFrames[k].bottom = srcFrameObj["bottom"].GetUint();
		m_pFrames[k].right = srcFrameObj["right"].GetUint();
		m_pFrames[k].left = srcFrameObj["left"].GetUint();
		m_pFrames[k].top = srcFrameObj["top"].GetUint();

		m_ChecksumMap[srcFrameObj["checksum"].GetUint()] = &m_pFrames[k];
	}
}

HyFileAtlas::HyFileAtlas(HyAuxiliaryFileHandle hGivenHandle, std::string sFileName, HyTextureInfo textureInfo) :
	IHyFile(HYFILE_Atlas, sFileName, std::numeric_limits<uint32>::max(), std::numeric_limits<uint32>::max()),
	m_uiINDEX_IN_BANK(std::numeric_limits<uint32>::max()),
	m_iWidth(0),
	m_iHeight(0),
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
	return m_iWidth;
}

int32 HyFileAtlas::GetHeight() const
{
	return m_iHeight;
}

HyTextureHandle HyFileAtlas::GetTextureHandle() const
{
	return m_hTextureHandle;
}

bool HyFileAtlas::GetUvRect(uint32 uiChecksum, HyRectangle<float> &UVRectOut) const
{
	float fTexWidth = static_cast<float>(m_iWidth);
	float fTexHeight = static_cast<float>(m_iHeight);
	HyAssert(fTexWidth > 0.0f && fTexHeight > 0.0f, "HyFileAtlas::GetUvRect was called before the texture was loaded");

	const HyRectangle<int32> *pSrcRect = nullptr;
	std::map<uint32, HyRectangle<int32> *>::const_iterator iter = m_ChecksumMap.find(uiChecksum);
	if(iter != m_ChecksumMap.end())
		pSrcRect = iter->second;

	if(pSrcRect)
	{
		UVRectOut.left = static_cast<float>(pSrcRect->left) / fTexWidth;
		UVRectOut.top = static_cast<float>(pSrcRect->top) / fTexHeight;
		UVRectOut.right = static_cast<float>(pSrcRect->right) / fTexWidth;
		UVRectOut.bottom = static_cast<float>(pSrcRect->bottom) / fTexHeight;

		return true;
	}

	return false;
}

void HyFileAtlas::DeletePixelData()
{
	if(m_TextureInfo.GetFormat() == HYTEXTURE_ASTC)
		delete[] m_pPixelData;
	else
		SOIL_free_image_data(m_pPixelData);// stbi_image_free(m_pPixelData);

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
			char szTmpBuffer[16];
			sAtlasFilePath = HyEngine::DataDir() + HYASSETS_AtlasDir;
			sprintf(szTmpBuffer, "%05d", m_uiBANK_ID);
			sAtlasFilePath += szTmpBuffer;
			sAtlasFilePath += "/";
			sAtlasFilePath += m_sFILE_NAME;
		}
		else
			sAtlasFilePath = m_sFILE_NAME; // This is an auxiliary file, don't prepend the data directory

		switch(m_TextureInfo.GetFormat())
		{
		case HYTEXTURE_Uncompressed: {
			// Param1: num channels
			// Param2: disk file type (PNG, ...)
			int iNum8bitClrChannels; // out variables
			m_pPixelData = SOIL_load_image(sAtlasFilePath.c_str(), &m_iWidth, &m_iHeight, &iNum8bitClrChannels, m_TextureInfo.m_uiFormatParam1);

			m_uiPixelDataSize = m_iWidth * m_iHeight * 4;
			break; }

		case HYTEXTURE_DXT:
			m_pPixelData = SOIL_load_DDS(sAtlasFilePath.c_str(), &m_uiPixelDataSize, 0);
			break;

		case HYTEXTURE_ASTC:
			m_pPixelData = LoadAstc(sAtlasFilePath.c_str(), m_uiPixelDataSize);
			break;

		case HYTEXTURE_Unknown:
		default:
			HyError("HyFileAtlas::OnLoadThread() - Unknown texture type");
			break;
		}

		// Use PBO/DMA transfer if available
		if(m_pGfxApiPixelBuffer)
		{
			memcpy(m_pGfxApiPixelBuffer, m_pPixelData, m_uiPixelDataSize);
		}

		HyAssert(m_pPixelData != nullptr, "HyFileAtlas failed to load image data");
	}

	m_Mutex_PixelData.unlock();
}

/*virtual*/ void HyFileAtlas::OnRenderThread(IHyRenderer &rendererRef) /*override*/
{
	m_Mutex_PixelData.lock();
	if(GetLoadableState() == HYLOADSTATE_Queued)
	{
		HyAssert(m_iWidth > 0 && m_iHeight > 0, "HyFileAtlas::OnRenderThread() - Texture dimensions are invalid");
		m_hTextureHandle = rendererRef.AddTexture(m_TextureInfo, m_iWidth, m_iHeight, m_pPixelData, m_uiPixelDataSize, m_hGfxApiPbo);
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
	ss << "Bank " << m_uiBANK_ID << ", Index " << m_uiINDEX_IN_BANK << " (" << HyAssets::GetTextureFormatName(m_TextureInfo.GetFormat()) << ")";
	return ss.str();
}

uint8 *HyFileAtlas::LoadAstc(std::string sAtlasFilePath, uint32 &uiPixelDataSizeOut)
{
	std::vector<uint8> astcData;
	HyIO::ReadBinaryFile(sAtlasFilePath.c_str(), astcData);
	if(astcData.empty())
	{
		HyLogError("HyFileAtlas::LoadAstc() failed to read binary file: " << sAtlasFilePath);
		return nullptr;
	}

	// ASTC header declaration.
	struct AstcHeader
	{
		unsigned char  magic[4];
		unsigned char  blockdim_x;
		unsigned char  blockdim_y;
		unsigned char  blockdim_z;
		unsigned char  xsize[3];   /* x-size = xsize[0] + xsize[1] + xsize[2] */
		unsigned char  ysize[3];   /* x-size, y-size and z-size are given in texels */
		unsigned char  zsize[3];   /* block count is inferred */
	};

	// Traverse the file structure
	AstcHeader *pAstcHeader = reinterpret_cast<AstcHeader *>(astcData.data());

	// Store number of bytes for each dimension
	// Merge x,y,z-sizes from 3 chars into one integer value
	int32 xsize = pAstcHeader->xsize[0] + (pAstcHeader->xsize[1] << 8) + (pAstcHeader->xsize[2] << 16);
	int32 ysize = pAstcHeader->ysize[0] + (pAstcHeader->ysize[1] << 8) + (pAstcHeader->ysize[2] << 16);
	int32 zsize = pAstcHeader->zsize[0] + (pAstcHeader->zsize[1] << 8) + (pAstcHeader->zsize[2] << 16);

	// Number of blocks in the x, y and z direction
	int32 xblocks = (xsize + pAstcHeader->blockdim_x - 1) / pAstcHeader->blockdim_x;
	int32 yblocks = (ysize + pAstcHeader->blockdim_y - 1) / pAstcHeader->blockdim_y;
	int32 zblocks = (zsize + pAstcHeader->blockdim_z - 1) / pAstcHeader->blockdim_z;

	// Each block is encoded on 16 bytes, so calculate total compressed image data size
	uiPixelDataSizeOut = xblocks * yblocks * zblocks << 4;

	uint8 *pPixelBuffer = HY_NEW uint8[uiPixelDataSizeOut];
	memcpy(pPixelBuffer, &pAstcHeader[1], uiPixelDataSizeOut);

	return pPixelBuffer;
}
