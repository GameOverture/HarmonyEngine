/**************************************************************************
 *	HyAtlasGroup.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2015 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Assets/Loadables/HyAtlasGroup.h"

#include "Assets/HyAssets.h"
#include "Renderer/IHyRenderer.h"

#include "Utilities/stb_image.h"

#include <set>

HyAtlasGroup::HyAtlasGroup(HyAssets &assetsRef, uint32 uiLoadGroupId, uint32 uiWidth, uint32 uiHeight, uint32 uiNumClrChannels, jsonxx::Array &texturesArrayRef) :	IHyLoadableData(HYGFXTYPE_AtlasGroup),
																																									m_AssetsRef(assetsRef),
																																									m_uiLOADGROUPID(uiLoadGroupId),
																																									m_uiWIDTH(uiWidth),
																																									m_uiHEIGHT(uiHeight),
																																									m_uiNUM_8BIT_CHANNELS(uiNumClrChannels),
																																									m_uiNUM_ATLASES(static_cast<uint32>(texturesArrayRef.size()))
{
	m_pAtlases = reinterpret_cast<HyAtlas *>(HY_NEW unsigned char[sizeof(HyAtlas) * m_uiNUM_ATLASES]);
	HyAtlas *pAtlasWriteLocation = m_pAtlases;

	for(uint32 j = 0; j < m_uiNUM_ATLASES; ++j, ++pAtlasWriteLocation)
	{
		jsonxx::Array srcFramesArray = texturesArrayRef.get<jsonxx::Array>(j);

		new (pAtlasWriteLocation)HyAtlas(srcFramesArray);
	}
}

HyAtlasGroup::~HyAtlasGroup()
{
	for(uint32 i = 0; i < m_uiNUM_ATLASES; ++i)
		m_pAtlases[i].~HyAtlas();

	unsigned char *pAtlas = reinterpret_cast<unsigned char *>(m_pAtlases);
	delete[] pAtlas;
	m_pAtlases = NULL;
}

uint32 HyAtlasGroup::GetGfxApiHandle(uint32 uiAtlasGroupTextureIndex)
{
	return m_pAtlases[uiAtlasGroupTextureIndex].GetGfxApiHandle();
}

uint32 HyAtlasGroup::GetActualGfxApiTextureIndex(uint32 uiAtlasGroupTextureIndex)
{
	return m_pAtlases[uiAtlasGroupTextureIndex].GetGfxApiTextureIndex();
}

uint32 HyAtlasGroup::GetId() const
{
	return m_uiLOADGROUPID;
}

uint32 HyAtlasGroup::GetNumColorChannels() const
{
	return m_uiNUM_8BIT_CHANNELS;
}

uint32 HyAtlasGroup::GetWidth() const
{
	return m_uiWIDTH;
}

uint32 HyAtlasGroup::GetHeight() const
{
	return m_uiHEIGHT;
}

uint32 HyAtlasGroup::GetNumTextures() const
{
	return m_uiNUM_ATLASES;
}

void HyAtlasGroup::GetUvRect(uint32 uiChecksum, uint32 &uiTextureIndexOut, HyRectangle<float> &UVRectOut) const
{
	float fTexWidth = static_cast<float>(m_uiWIDTH);
	float fTexHeight = static_cast<float>(m_uiHEIGHT);

	const HyRectangle<int32> *pSrcRect = NULL;
	for(uint32 i = 0; i < m_uiNUM_ATLASES; ++i)
	{
		pSrcRect = m_pAtlases[i].GetSrcRect(uiChecksum);
		if(pSrcRect)
		{
			uiTextureIndexOut = i;

			UVRectOut.left = static_cast<float>(pSrcRect->left) / fTexWidth;
			UVRectOut.top = static_cast<float>(pSrcRect->top) / fTexHeight;
			UVRectOut.right = static_cast<float>(pSrcRect->right) / fTexWidth;
			UVRectOut.bottom = static_cast<float>(pSrcRect->bottom) / fTexHeight;

			break;
		}
	}
}

void HyAtlasGroup::OnLoadThread()
{
	m_csTextures.Lock();

	if(GetLoadState() == HYLOADSTATE_Queued)
	{
		for(uint32 i = 0; i < m_uiNUM_ATLASES; ++i)
			m_pAtlases[i].Load(m_AssetsRef.GetTexturePath(m_uiLOADGROUPID, i).c_str());
	}

	m_csTextures.Unlock();
}

void HyAtlasGroup::OnRenderThread(IHyRenderer &rendererRef)
{
	m_csTextures.Lock();
	if(GetLoadState() == HYLOADSTATE_Queued)
	{
		std::vector<unsigned char *> texturePixelDataList;
		for(uint32 i = 0; i < m_uiNUM_ATLASES; ++i)
			texturePixelDataList.push_back(m_pAtlases[i].GetPixelData());

		uint32 uiNumTexturesUploaded = 0;
		while(uiNumTexturesUploaded != m_uiNUM_ATLASES)
		{
			uint32 uiNumSuccess = 0;
			uint32 uiGfxApiHandle = rendererRef.AddTextureArray(m_uiNUM_8BIT_CHANNELS, m_uiWIDTH, m_uiHEIGHT, texturePixelDataList, uiNumSuccess);

			uint32 uiActualTextureIndex = 0;
			for(uint32 i = uiNumTexturesUploaded; i < (uiNumTexturesUploaded + uiNumSuccess); ++i, ++uiActualTextureIndex)
				m_pAtlases[i].SetGfxApiHandle(uiGfxApiHandle, uiActualTextureIndex);

			std::vector<unsigned char *> split_hi(texturePixelDataList.begin() + uiNumSuccess, texturePixelDataList.end());
			texturePixelDataList = split_hi;

			uiNumTexturesUploaded += uiNumSuccess;
		}

		for(uint32 i = 0; i < m_uiNUM_ATLASES; ++i)
			m_pAtlases[i].DeletePixelData();
	}
	else // GetLoadState() == HYLOADSTATE_Discarded
	{
		std::set<uint32> gfxApiHandleSet;
		for(uint32 i = 0; i < m_uiNUM_ATLASES; ++i)
			gfxApiHandleSet.insert(m_pAtlases[i].GetGfxApiHandle());

		for(std::set<uint32>::iterator iter = gfxApiHandleSet.begin(); iter != gfxApiHandleSet.end(); ++iter)
			rendererRef.DeleteTextureArray(*iter);
	}
	m_csTextures.Unlock();
}

//////////////////////////////////////////////////////////////////////////
HyAtlasGroup::HyAtlas::HyAtlas(jsonxx::Array &srcFramesArrayRef) : m_uiGfxApiHandle(0),
														m_uiGfxApiTextureIndex(0),
														m_uiNUM_FRAMES(static_cast<uint32>(srcFramesArrayRef.size())),
														m_pPixelData(NULL)
{
	m_pFrames = HY_NEW HyRectangle<int32>[m_uiNUM_FRAMES];

	for(uint32 k = 0; k < m_uiNUM_FRAMES; ++k)
	{
		jsonxx::Object srcFrameObj = srcFramesArrayRef.get<jsonxx::Object>(k);

		m_pFrames[k].bottom = static_cast<uint32>(srcFrameObj.get<jsonxx::Number>("bottom"));
		m_pFrames[k].right = static_cast<uint32>(srcFrameObj.get<jsonxx::Number>("right"));
		m_pFrames[k].left = static_cast<uint32>(srcFrameObj.get<jsonxx::Number>("left"));
		m_pFrames[k].top = static_cast<uint32>(srcFrameObj.get<jsonxx::Number>("top"));

		m_ChecksumMap[static_cast<uint32>(srcFrameObj.get<jsonxx::Number>("checksum"))] = &m_pFrames[k];
	}
}

HyAtlasGroup::HyAtlas::~HyAtlas()
{
	delete [] m_pFrames;
	DeletePixelData();
}

uint32 HyAtlasGroup::HyAtlas::GetGfxApiHandle() const
{
	return m_uiGfxApiHandle;
}

uint32 HyAtlasGroup::HyAtlas::GetGfxApiTextureIndex() const
{
	return m_uiGfxApiTextureIndex;
}

void HyAtlasGroup::HyAtlas::SetGfxApiHandle(uint32 uiGfxApiHandle, uint32 uiGfxApiTextureIndex)
{
	m_uiGfxApiHandle = uiGfxApiHandle;
	m_uiGfxApiTextureIndex = uiGfxApiTextureIndex;
}

const HyRectangle<int32> *HyAtlasGroup::HyAtlas::GetSrcRect(uint32 uiChecksum) const
{
	std::map<uint32, HyRectangle<int32> *>::const_iterator iter = m_ChecksumMap.find(uiChecksum);
	if(iter == m_ChecksumMap.end())
		return NULL;
	else
		return iter->second;
}

void HyAtlasGroup::HyAtlas::Load(const char *szFilePath)
{
	if(m_pPixelData)
		return;

	int iWidth, iHeight, iNum8bitClrChannels;
	m_pPixelData = stbi_load(szFilePath, &iWidth, &iHeight, &iNum8bitClrChannels, 0);

	HyAssert(m_pPixelData != NULL, "HyAtlas failed to load image data");
}

unsigned char *HyAtlasGroup::HyAtlas::GetPixelData()
{
	return m_pPixelData;
}

void HyAtlasGroup::HyAtlas::DeletePixelData()
{
	stbi_image_free(m_pPixelData);
	m_pPixelData = NULL;
}
