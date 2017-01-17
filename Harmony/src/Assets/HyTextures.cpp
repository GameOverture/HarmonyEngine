/**************************************************************************
 *	HyTextures.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2015 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Assets/HyTextures.h"

#include "Renderer/IHyRenderer.h"

#include "Utilities/stb_image.h"

HyTextures::HyTextures(std::string sAtlasDataDir) : m_sATLAS_DIR_PATH(sAtlasDataDir),
													m_uiNumAtlasGroups(0),
													m_pAtlasGroups(NULL)
{
	jsonxx::Array atlasGroupArray;

	std::string sAtlasInfoFilePath(m_sATLAS_DIR_PATH);
	sAtlasInfoFilePath += "atlasInfo.json";
	std::string sAtlasInfoFileContents;
	HyReadTextFile(sAtlasInfoFilePath.c_str(), sAtlasInfoFileContents);

	if(atlasGroupArray.parse(sAtlasInfoFileContents) == false || atlasGroupArray.size() == 0)
	{
		m_uiNumAtlasGroups = 0;
		m_pAtlasGroups = NULL;
		return;
	}

	m_uiNumAtlasGroups = static_cast<uint32>(atlasGroupArray.size());
	m_pAtlasGroups = reinterpret_cast<HyAtlasGroup *>(HY_NEW unsigned char[sizeof(HyAtlasGroup) * m_uiNumAtlasGroups]);
	HyAtlasGroup *pAtlasGroupWriteLocation = m_pAtlasGroups;

	for(uint32 i = 0; i < m_uiNumAtlasGroups; ++i, ++pAtlasGroupWriteLocation)
	{
		jsonxx::Object atlasGroupObj = atlasGroupArray.get<jsonxx::Object>(i);

		new (pAtlasGroupWriteLocation)HyAtlasGroup(*this,
			static_cast<uint32>(atlasGroupObj.get<jsonxx::Number>("id")),
			static_cast<int32>(atlasGroupObj.get<jsonxx::Number>("width")),
			static_cast<int32>(atlasGroupObj.get<jsonxx::Number>("height")),
			static_cast<int32>(atlasGroupObj.get<jsonxx::Number>("num8BitClrChannels")),
			atlasGroupObj.get<jsonxx::Array>("textures"));
	}
}

HyTextures::~HyTextures()
{
	if(m_pAtlasGroups == NULL || m_uiNumAtlasGroups == 0)
		return;

	for(uint32 i = 0; i < m_uiNumAtlasGroups; ++i)
		m_pAtlasGroups[i].~HyAtlasGroup();

	unsigned char *pAtlasGrps = reinterpret_cast<unsigned char *>(m_pAtlasGroups);
	delete[] pAtlasGrps;
	m_pAtlasGroups = NULL;
}

HyAtlasGroup *HyTextures::RequestTexture(uint32 uiAtlasGroupId)
{
	for(uint32 i = 0; i < m_uiNumAtlasGroups; ++i)
	{
		if(m_pAtlasGroups[i].GetId() == uiAtlasGroupId)
		{
			m_pAtlasGroups[i].Load();
			return &m_pAtlasGroups[i];
		}
	}
	
	HyError("HyTextures::RequestTexture() could not find the atlas group ID: " << uiAtlasGroupId);
	return &m_pAtlasGroups[0];
}

std::string HyTextures::GetTexturePath(uint32 uiAtlasGroupId, uint32 uiTextureIndex)
{
	std::string sTexturePath(m_sATLAS_DIR_PATH);

	char szTmpBuffer[16];
	sprintf(szTmpBuffer, "%05d/", uiAtlasGroupId);
	sTexturePath += szTmpBuffer;

	sprintf(szTmpBuffer, "%05d", uiTextureIndex);
	sTexturePath += szTmpBuffer;

	sTexturePath += ".png";

	return sTexturePath;
}

//////////////////////////////////////////////////////////////////////////
HyAtlasGroup::HyAtlasGroup(HyTextures &managerRef, uint32 uiLoadGroupId, uint32 uiWidth, uint32 uiHeight, uint32 uiNumClrChannels, jsonxx::Array &texturesArrayRef) :	m_ManagerRef(managerRef),
																																										m_uiLOADGROUPID(uiLoadGroupId),
																																										m_uiWIDTH(uiWidth),
																																										m_uiHEIGHT(uiHeight),
																																										m_uiNUM_8BIT_CHANNELS(uiNumClrChannels),
																																										m_uiNUM_ATLASES(static_cast<uint32>(texturesArrayRef.size())),
																																										m_uiRefCount(0)
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

void HyAtlasGroup::Load()
{
	m_csTextures.Lock();

	if(m_uiRefCount == 0)
	{
		for(uint32 i = 0; i < m_uiNUM_ATLASES; ++i)
			m_pAtlases[i].Load(m_ManagerRef.GetTexturePath(m_uiLOADGROUPID, i).c_str());
	}

	m_csTextures.Unlock();
}

void HyAtlasGroup::OnRenderThread(IHyRenderer &rendererRef, IHy2dData *pData)
{
	bool bUpload = m_uiRefCount == 0;

	if(pData->IsIncrementRenderRefs())
		m_uiRefCount++;
	else
	{
		HyAssert(m_uiRefCount == 0, "HyAtlasGroup::OnRenderThread Tried to decrement an empty ref");
		m_uiRefCount--;
	}

	m_csTextures.Lock();
	if(bUpload)
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
	else if(m_uiRefCount == 0)
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
HyAtlas::HyAtlas(jsonxx::Array &srcFramesArrayRef) :	m_uiGfxApiHandle(0),
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

HyAtlas::~HyAtlas()
{
	delete [] m_pFrames;
	DeletePixelData();
}

uint32 HyAtlas::GetGfxApiHandle() const
{
	return m_uiGfxApiHandle;
}

uint32 HyAtlas::GetGfxApiTextureIndex() const
{
	return m_uiGfxApiTextureIndex;
}

void HyAtlas::SetGfxApiHandle(uint32 uiGfxApiHandle, uint32 uiGfxApiTextureIndex)
{
	m_uiGfxApiHandle = uiGfxApiHandle;
	m_uiGfxApiTextureIndex = uiGfxApiTextureIndex;
}

const HyRectangle<int32> *HyAtlas::GetSrcRect(uint32 uiChecksum) const
{
	std::map<uint32, HyRectangle<int32> *>::const_iterator iter = m_ChecksumMap.find(uiChecksum);
	if(iter == m_ChecksumMap.end())
		return NULL;
	else
		return iter->second;
}

void HyAtlas::Load(const char *szFilePath)
{
	if(m_pPixelData)
		return;

	int iWidth, iHeight, iNum8bitClrChannels;
	m_pPixelData = stbi_load(szFilePath, &iWidth, &iHeight, &iNum8bitClrChannels, 0);

	HyAssert(m_pPixelData != NULL, "HyAtlas failed to load image data");
}

unsigned char *HyAtlas::GetPixelData()
{
	return m_pPixelData;
}

void HyAtlas::DeletePixelData()
{
	stbi_image_free(m_pPixelData);
	m_pPixelData = NULL;
}
