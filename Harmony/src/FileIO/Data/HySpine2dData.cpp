/**************************************************************************
 *	HySpine2dData.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "FileIO/Data/HySpine2dData.h"

#include "Renderer/IHyRenderer.h"

HySpine2dData::HySpine2dData(const std::string &sPath) :	IHyData(HYINST_Spine2d, sPath)
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

/*virtual*/ void HySpine2dData::OnGfxLoad(IHyRenderer &gfxApi)
{
	// TODO: possibly support multiple textures (aka rendererObject's)
	HyAtlas *pTexture = reinterpret_cast<HyAtlas *>(m_SpineAtlasData->pages->rendererObject);
	pTexture->Upload(gfxApi);
}

/*virtual*/ void HySpine2dData::OnGfxRemove(IHyRenderer &gfxApi)
{
	HyAtlas *pTexture = reinterpret_cast<HyAtlas *>(m_SpineAtlasData->pages->rendererObject);
	gfxApi.DeleteTexture(*pTexture);
}

// Below functions are invoked within the Spine API and expect to be overloaded
void _spAtlasPage_createTexture(spAtlasPage* self, const char* path)
{
	// THIS IS INVOKED FROM THE LOAD THREAD from any IData::DoLoad()

	// TODO: Convert 'path' to Atlas texture index
	uint32 uiTextureIndex = 0;

	HyAtlas *pNewTexture = HyFileIO::GetAtlasTexture(uiTextureIndex);
	self->rendererObject = pNewTexture;

	self->width = pNewTexture->GetWidth();
	self->height = pNewTexture->GetHeight();
}

void _spAtlasPage_disposeTexture(spAtlasPage* self)
{
}

char* _spUtil_readFile(const char* path, int* length)
{
	// The returned data is freed within the spine API
	return IHyFileIO::ReadTextFile(path, length);
}
