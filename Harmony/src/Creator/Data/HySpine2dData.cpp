/**************************************************************************
 *	HySpine2dData.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Creator/Data/HySpine2dData.h"
#include "Renderer/Interop/IGfxApi.h"

HySpine2dData::HySpine2dData(const std::string &sPath) :	IData(HYINST_Spine2d, sPath)
{
	
}

HySpine2dData::~HySpine2dData()
{
	spSkeletonData_dispose(m_SpineSkeletonData);
	spAtlas_dispose(m_SpineAtlasData);
}

/*virtual*/ void HySpine2dData::DoFileLoad()
{
	std::string sAtlasPath(m_ksPath);
	sAtlasPath += ".atlas";

	m_SpineAtlasData = spAtlas_createFromFile(sAtlasPath.c_str(), this);
	HyAssert(m_SpineAtlasData, "Could not read atlas file at \"" << sAtlasPath.c_str() << "\".");

	printf("First region name: %s, x: %d, y: %d\n", m_SpineAtlasData->regions->name, m_SpineAtlasData->regions->x, m_SpineAtlasData->regions->y);
	printf("First page name: %s, size: %d, %d\n", m_SpineAtlasData->pages->name, m_SpineAtlasData->pages->width, m_SpineAtlasData->pages->height);

	spSkeletonJson * pSpineJsonData = spSkeletonJson_create(m_SpineAtlasData);

	std::string sJsonPath(m_ksPath);
	sJsonPath += ".json";

	m_SpineSkeletonData = spSkeletonJson_readSkeletonDataFile(pSpineJsonData, sJsonPath.c_str());
	HyAssert(m_SpineSkeletonData, pSpineJsonData->error);
		//printf("Error: %s\n", );
	spSkeletonJson_dispose(pSpineJsonData);

	printf("Default skin name: %s\n", m_SpineSkeletonData->defaultSkin->name);
}

/*virtual*/ void HySpine2dData::OnGfxLoad(IGfxApi &gfxApi)
{
	// TODO: possibly support multiple textures (aka rendererObject's)
	HyTexture *pTexture = reinterpret_cast<HyTexture *>(m_SpineAtlasData->pages->rendererObject);
	pTexture->Upload(gfxApi);
}

/*virtual*/ void HySpine2dData::OnGfxRemove(IGfxApi &gfxApi)
{
	HyTexture *pTexture = reinterpret_cast<HyTexture *>(m_SpineAtlasData->pages->rendererObject);
	gfxApi.DeleteTexture(pTexture->GetId());
}
