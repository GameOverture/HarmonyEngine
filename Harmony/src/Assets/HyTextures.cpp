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

#include "Utilities/HyFileIO.h"
#include "Utilities/stb_image.h"

HyTextures::HyTextures(std::string sAtlasDataDir) : m_sATLAS_DIR_PATH(sAtlasDataDir),
													m_uiNumAtlasGroups(0),
													m_pAtlasGroups(NULL)
{
	jsonxx::Array atlasGroupArray;

	std::string sAtlasInfoFilePath(m_sATLAS_DIR_PATH);
	sAtlasInfoFilePath += "atlasInfo.json";
	if(atlasGroupArray.parse(HyReadTextFile(sAtlasInfoFilePath.c_str())) == false || atlasGroupArray.size() == 0)
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
																																											m_uiGfxApiHandle(0)
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

uint32 HyAtlasGroup::GetId() const
{
	return m_uiLOADGROUPID;
}

uint32 HyAtlasGroup::GetGfxApiHandle() const
{
	return m_uiGfxApiHandle;
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

bool HyAtlasGroup::ContainsTexture(uint32 uiTextureIndex) const
{
	return (uiTextureIndex < m_uiNUM_ATLASES);
}

void HyAtlasGroup::Load()
{
	m_csTextures.Lock();

	if(m_uiGfxApiHandle == 0)
	{
		for(uint32 i = 0; i < m_uiNUM_ATLASES; ++i)
			m_pAtlases[i].Load(m_ManagerRef.GetTexturePath(m_uiLOADGROUPID, i).c_str());
	}

	m_csTextures.Unlock();
}

// Returns 'true' if texture was just loaded
void HyAtlasGroup::Assign(IHyData *pData)
{
	m_csDataRefs.Lock();
	m_AssociatedDataSet.insert(pData);
	m_csDataRefs.Unlock();
}

void HyAtlasGroup::Relinquish(IHyData *pData)
{
	m_csDataRefs.Lock();

	for(set<IHyData *>::iterator iter = m_AssociatedDataSet.begin(); iter != m_AssociatedDataSet.end(); ++iter)
	{
		if((*iter) == pData)
		{
			m_AssociatedDataSet.erase(iter);
			break;
		}
	}

	m_csDataRefs.Unlock();
}

void HyAtlasGroup::OnRenderThread(IHyRenderer &rendererRef)
{
	bool bUpload;
	
	m_csDataRefs.Lock();
	bUpload = m_AssociatedDataSet.empty() == false;
	m_csDataRefs.Unlock();
	

	m_csTextures.Lock();
	if(bUpload)
	{
		vector<unsigned char *> vTextureArrayData;
		for(uint32 i = 0; i < m_uiNUM_ATLASES; ++i)
			vTextureArrayData.push_back(m_pAtlases[i].GetPixelData());

		m_uiGfxApiHandle = rendererRef.AddTextureArray(m_uiNUM_8BIT_CHANNELS, m_uiWIDTH, m_uiHEIGHT, vTextureArrayData);

		for(uint32 i = 0; i < m_uiNUM_ATLASES; ++i)
			m_pAtlases[i].DeletePixelData();
	}
	else
	{
		rendererRef.DeleteTextureArray(m_uiGfxApiHandle);
		m_uiGfxApiHandle = 0;
	}
	m_csTextures.Unlock();
}

//////////////////////////////////////////////////////////////////////////
HyAtlas::HyAtlas(jsonxx::Array &srcFramesArrayRef) :	m_uiNUM_FRAMES(srcFramesArrayRef.size()),
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
		m_pFrames[k].iTag = srcFrameObj.get<jsonxx::Boolean>("rotate") ? 1 : 0;
	}
}

HyAtlas::~HyAtlas()
{
	delete [] m_pFrames;
	DeletePixelData();
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
