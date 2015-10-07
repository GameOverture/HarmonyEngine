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

#include "Utilities/jsonxx.h"
#include "Utilities/stb_image.h"

HyAtlasManager::HyAtlasManager(const char *szDataDirPath)
{
	jsonxx::Object atlasObject;

	std::string sAtlasFilePath(szDataDirPath);
	sAtlasFilePath += "Atlas/atlasInfo.json";
	atlasObject.parse(IHyFileIO::ReadTextFile(sAtlasFilePath.c_str()));

	m_iWidth = static_cast<int32>(atlasObject.get<jsonxx::Number>("width"));
	m_iHeight = static_cast<int32>(atlasObject.get<jsonxx::Number>("height"));
	m_iNum8bitClrChannels = static_cast<int32>(atlasObject.get<jsonxx::Number>("num8BitClrChannels"));
	jsonxx::Array loadGroupArray = atlasObject.get<jsonxx::Array>("loadGroups");

	m_pAtlasGroups = reinterpret_cast<HyAtlasGroup *>(new unsigned char[sizeof(HyAtlasGroup) * loadGroupArray.size()]);
	HyAtlasGroup *pAtlasGroupWriteLocation = m_pAtlasGroups;

	for(uint32 i = 0; i < loadGroupArray.size(); ++i, ++pAtlasGroupWriteLocation)
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

//////////////////////////////////////////////////////////////////////////
HyAtlasGroup::HyAtlasGroup(int32 iLoadGroupId, jsonxx::Array &texturesArrayRef) :	m_iLOADGROUPID(iLoadGroupId),
																					m_uiGraphicsApiId(0)
{
	m_pAtlases = reinterpret_cast<HyAtlas *>(new unsigned char[sizeof(HyAtlas) * texturesArrayRef.size()]);
	HyAtlas *pAtlasWriteLocation = m_pAtlases;

	for(uint32 j = 0; j < texturesArrayRef.size(); ++j)
	{
		jsonxx::Object texObj = texturesArrayRef.get<jsonxx::Object>(j);

		uint32 uiTextureId = texObj.get<jsonxx::Number>("id");
		jsonxx::Array srcFramesArray = texObj.get<jsonxx::Array>("srcFrames");

		new (pAtlasWriteLocation)HyAtlas(uiTextureId, srcFramesArray);
	}
}

HyAtlasGroup::~HyAtlasGroup()
{
}

//////////////////////////////////////////////////////////////////////////
HyAtlas::HyAtlas(uint32 uiTextureId, jsonxx::Array &srcFramesArrayRef) : uiTEXTUREID(uiTextureId)
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

void HyAtlas::Load()
{
	m_pPixelData = stbi_load(m_ksPath.c_str(), &m_iWidth, &m_iHeight, &m_iNum8bitClrChannels, 0);
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
