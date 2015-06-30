/**************************************************************************
 *	HySprite2dData.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Creator/Data/HySprite2dData.h"

#include "Renderer/Interop/IGfxApi.h"

HySprite2dData::HySprite2dData(const std::string &sPath) :	IData(HYINST_Spine2d, sPath)
{
}

HySprite2dData::~HySprite2dData(void)
{
}

/*virtual*/ void HySprite2dData::DoFileLoad()
{
	// Empty path is an indicator to construct a simple sprite
	if(m_ksPath.empty())
	{
		m_bIsSimpleSprite = true;

		m_iNumStates = 1;
		m_pAnimStates = new AnimState[1];
		m_pAnimStates->sName = "SimpleSprite";
		m_pAnimStates->bLoop = false;
		m_pAnimStates->bReverse = false;
		m_pAnimStates->bBounce = false;

		m_pAnimStates->m_iNumFrames = 1;
		m_pAnimStates->m_pFrames = new AnimState::Frame[1];
		m_pAnimStates->m_pFrames[0].m_fDur = 0.0f;
		m_pAnimStates->m_pFrames[0].m_fRot = 0.0f;
		m_pAnimStates->m_pFrames[0].m_iRectIndex = 0;
		m_pAnimStates->m_pFrames[0].m_iTextureIndex = 0;
		m_pAnimStates->m_pFrames[0].m_vOffset.x = m_pAnimStates->m_pFrames[0].m_vOffset.y = 0.0f;
		m_pAnimStates->m_pFrames[0].m_vScale.x = m_pAnimStates->m_pFrames[0].m_vScale.y = 1.0f;
	}
	else
	{
		m_bIsSimpleSprite = false;
	}

	//std::string sAtlasPath(sFilePath);
	//sAtlasPath += ".atlas";

	//HyFileIO::ParseJsonFile(path, length);

	//m_SpineAtlasData = spAtlas_createFromFile(sAtlasPath.c_str(), this);
	//HyAssert(m_SpineAtlasData, "Could not read atlas file at \"" << sAtlasPath.c_str() << "\".");

	//printf("First region name: %s, x: %d, y: %d\n", m_SpineAtlasData->regions->name, m_SpineAtlasData->regions->x, m_SpineAtlasData->regions->y);
	//printf("First page name: %s, size: %d, %d\n", m_SpineAtlasData->pages->name, m_SpineAtlasData->pages->width, m_SpineAtlasData->pages->height);

	//spSkeletonJson * pSpineJsonData = spSkeletonJson_create(m_SpineAtlasData);

	//std::string sJsonPath(sFilePath);
	//sJsonPath += ".json";

	//m_SpineSkeletonData = spSkeletonJson_readSkeletonDataFile(pSpineJsonData, sJsonPath.c_str());
	//HyAssert(m_SpineSkeletonData, pSpineJsonData->error);
	////printf("Error: %s\n", );
	//spSkeletonJson_dispose(pSpineJsonData);

	//printf("Default skin name: %s\n", m_SpineSkeletonData->defaultSkin->name);
}

/*virtual*/ void HySprite2dData::OnGfxLoad(IGfxApi &gfxApi)
{
}

/*virtual*/ void HySprite2dData::OnGfxRemove(IGfxApi &gfxApi)
{
}
