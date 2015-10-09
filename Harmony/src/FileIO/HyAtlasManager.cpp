/**************************************************************************
 *	HyAtlasManager.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2015 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "FileIO/HyAtlasManager.h"

#include "FileIO/IHyFileIO.h"

#include "Utilities/stb_image.h"

std::string	HyAtlasManager::sm_sAtlasDirPath;

HyAtlasManager::HyAtlasManager(std::string sAtlasDataDir)
{
	sm_sAtlasDirPath = sAtlasDataDir;

	jsonxx::Object atlasObject;

	std::string sAtlasInfoFilePath(sm_sAtlasDirPath);
	sAtlasInfoFilePath += "atlasInfo.json";
	atlasObject.parse(IHyFileIO::ReadTextFile(sAtlasInfoFilePath.c_str()));

	m_iWidth = static_cast<int32>(atlasObject.get<jsonxx::Number>("width"));
	m_iHeight = static_cast<int32>(atlasObject.get<jsonxx::Number>("height"));
	m_iNum8bitClrChannels = static_cast<int32>(atlasObject.get<jsonxx::Number>("num8BitClrChannels"));
	jsonxx::Array loadGroupArray = atlasObject.get<jsonxx::Array>("loadGroups");

	m_uiNumAtlasGroups = loadGroupArray.size();
	m_pAtlasGroups = reinterpret_cast<HyAtlasGroup *>(new unsigned char[sizeof(HyAtlasGroup) * m_uiNumAtlasGroups]);
	HyAtlasGroup *pAtlasGroupWriteLocation = m_pAtlasGroups;

	for(uint32 i = 0; i < m_uiNumAtlasGroups; ++i, ++pAtlasGroupWriteLocation)
	{
		jsonxx::Object loadGroupObj = loadGroupArray.get<jsonxx::Object>(i);

		int32 iLoadGroupId = static_cast<int32>(loadGroupObj.get<jsonxx::Number>("id"));
		jsonxx::Array texturesArray = loadGroupObj.get<jsonxx::Array>("textures");

		new (pAtlasGroupWriteLocation)HyAtlasGroup(iLoadGroupId, texturesArray);
	}
}

HyAtlasManager::~HyAtlasManager()
{
}

HyAtlasGroup &HyAtlasManager::RequestTexture(IHyData *pData, uint32 uiTextureId)
{
	for(uint32 i = 0; i < m_uiNumAtlasGroups; ++i)
	{
		if(m_pAtlasGroups[i].ContainsTexture(uiTextureId))
		{
			m_pAtlasGroups[i].Request(pData);
			return m_pAtlasGroups[i];
		}
	}

	HyError("HyAtlasManager::RequestTexture() could not find the atlas group containing texture ID: " << uiTextureId);
	return m_pAtlasGroups[0];
}

void HyAtlasManager::RelinquishTexture(IHyData *pData, uint32 uiTextureId)
{
}

/*static*/ std::string HyAtlasManager::GetTexturePath(uint32 uiTextureId)
{
	std::string sTexturePath(sm_sAtlasDirPath);

	char szTmpBuffer[16];
	sprintf(szTmpBuffer, "%05d", uiTextureId);
	
	sTexturePath += szTmpBuffer;
	sTexturePath += ".png";

	return sTexturePath;
}

//////////////////////////////////////////////////////////////////////////
HyAtlasGroup::HyAtlasGroup(int32 iLoadGroupId, jsonxx::Array &texturesArrayRef) :	m_iLOADGROUPID(iLoadGroupId),
																					m_uiGfxApiHandle(0),
																					m_eLoadState(HYLOADSTATE_Inactive)
{
	m_pAtlases = reinterpret_cast<HyAtlas *>(new unsigned char[sizeof(HyAtlas) * texturesArrayRef.size()]);
	HyAtlas *pAtlasWriteLocation = m_pAtlases;

	for(uint32 j = 0; j < texturesArrayRef.size(); ++j)
	{
		jsonxx::Object texObj = texturesArrayRef.get<jsonxx::Object>(j);

		uint32 uiTextureId = static_cast<uint32>(texObj.get<jsonxx::Number>("id"));
		jsonxx::Array srcFramesArray = texObj.get<jsonxx::Array>("srcFrames");

		new (pAtlasWriteLocation)HyAtlas(uiTextureId, srcFramesArray);
	}
}

HyAtlasGroup::~HyAtlasGroup()
{
}

bool HyAtlasGroup::ContainsTexture(uint32 uiTextureId)
{
	for(uint32 i = 0; i < m_uiNumAtlases; ++i)
	{
		if(m_pAtlases[i].GetId() == uiTextureId)
			return true;
	}

	return false;
}

// Returns 'true' if texture was just loaded
void HyAtlasGroup::Request(IHyData *pData)
{
	m_cs.Lock();

	m_vAssociatedData.push_back(pData);

	if(m_eLoadState == HYLOADSTATE_Inactive)
	{
		m_cs.Unlock();

		for(uint32 i = 0; i < m_uiNumAtlases; ++i)
			m_pAtlases[i].Load();

		m_cs.Lock();
		// State is 'queued' to be uploaded to graphics ram
		m_eLoadState = HYLOADSTATE_Queued;
	}

	m_cs.Unlock();
}

bool HyAtlasGroup::IsUploadNeeded()
{
	bool bUploadNeeded = false;
	m_cs.Lock();
	bUploadNeeded = (m_eLoadState == HYLOADSTATE_Queued);
	m_cs.Unlock();

	return bUploadNeeded;
}

//////////////////////////////////////////////////////////////////////////
HyAtlas::HyAtlas(uint32 uiTextureId, jsonxx::Array &srcFramesArrayRef) : m_uiTEXTUREID(uiTextureId)
{
	m_uiNumFrames = srcFramesArrayRef.size();
	m_pFrames = new HyRectangle<int32>[m_uiNumFrames];

	for(uint32 k = 0; k < m_uiNumFrames; ++k)
	{
		jsonxx::Object srcFrameObj = srcFramesArrayRef.get<jsonxx::Object>(k);

		m_pFrames[k].height = static_cast<uint32>(srcFrameObj.get<jsonxx::Number>("height"));
		m_pFrames[k].width = static_cast<uint32>(srcFrameObj.get<jsonxx::Number>("width"));
		m_pFrames[k].x = static_cast<uint32>(srcFrameObj.get<jsonxx::Number>("x"));
		m_pFrames[k].y = static_cast<uint32>(srcFrameObj.get<jsonxx::Number>("y"));
		m_pFrames[k].iTag = srcFrameObj.get<jsonxx::Boolean>("rotated") ? 1 : 0;
	}
}

HyAtlas::~HyAtlas()
{
}

uint32 HyAtlas::GetId()
{
	return m_uiTEXTUREID;
}

void HyAtlas::Load()
{
	int iWidth, iHeight, iNum8bitClrChannels;
	m_pPixelData = stbi_load(HyAtlasManager::GetTexturePath(m_uiTEXTUREID).c_str(), &iWidth, &iHeight, &iNum8bitClrChannels, 0);

	HyAssert(m_pPixelData != NULL, "HyTexture failed to load image data");
}

//HyAtlasGroupData::~HyAtlasGroupData(void)
//{
//	DeletePixelData();
//}
//
//// To be invoked on the render thread
//void HyAtlasGroupData::Upload(IHyRenderer &gfxApi)
//{
//	m_uiId = gfxApi.AddTexture(m_iNum8bitClrChannels, m_iWidth, m_iHeight, m_pPixelData);
//	DeletePixelData();
//}
//
//void HyAtlasGroupData::DeletePixelData()
//{
//	stbi_image_free(m_pPixelData);
//	m_pPixelData = NULL;
//}
