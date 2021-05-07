/**************************************************************************
 *	HySpine2dData.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Assets/Nodes/HySpine2dData.h"
#include "Assets/HyAssets.h"
#include "Renderer/IHyRenderer.h"
#include "Diagnostics/Console/IHyConsole.h"

HySpine2dData::HySpine2dData(const std::string &sPath, HyJsonObj itemDataObj, HyAssets &assetsRef) :
	IHyNodeData(sPath),
	m_pAtlasData(nullptr),
	m_pSkeletonData(nullptr),
	m_pAnimStateData(nullptr)
{
	//m_pAtlasData = new spine::Atlas("", );

	//std::string sAtlasPath(GetPath());
	//sAtlasPath += ".atlas";

	//m_SpineAtlasData = spAtlas_createFromFile(sAtlasPath.c_str(), this);
	//HyAssert(m_SpineAtlasData, "Could not read atlas file at \"" << sAtlasPath.c_str() << "\".");

	//HyLogInfo("First region name: " << m_SpineAtlasData->regions->name << " x: " << m_SpineAtlasData->regions->x << " y: " << m_SpineAtlasData->regions->y);
	//printf("First page name: %s, size: %d, %d\n", m_SpineAtlasData->pages->name, m_SpineAtlasData->pages->width, m_SpineAtlasData->pages->height);

	//spSkeletonJson * pSpineJsonData = spSkeletonJson_create(m_SpineAtlasData);

	//std::string sJsonPath(GetPath());
	//sJsonPath += ".json";

	//m_SpineSkeletonData = spSkeletonJson_readSkeletonDataFile(pSpineJsonData, sJsonPath.c_str());
	//HyAssert(m_SpineSkeletonData, pSpineJsonData->error);
	//	//printf("Error: %s\n", );
	//spSkeletonJson_dispose(pSpineJsonData);

	//printf("Default skin name: %s\n", m_SpineSkeletonData->defaultSkin->name);
}
//
//HySpine2dData::~HySpine2dData()
//{
//	//spSkeletonData_dispose(m_SpineSkeletonData);
//	//spAtlas_dispose(m_SpineAtlasData);
//}
//
//// Below functions are invoked within the Spine API and expect to be overloaded
//void _spAtlasPage_createTexture(spAtlasPage* self, const char* path)
//{
//	// THIS IS INVOKED FROM THE LOAD THREAD from any IData::DoLoad()
//
//	// TODO: Convert 'path' to Atlas texture index
//	uint32 uiTextureIndex = 0;
//}
//
//void _spAtlasPage_disposeTexture(spAtlasPage* self)
//{
//}
//
//char* _spUtil_readFile(const char* path, int* length)
//{
//	// The returned data is freed within the spine API
//	return HyReadTextFile(path, length);
//}
