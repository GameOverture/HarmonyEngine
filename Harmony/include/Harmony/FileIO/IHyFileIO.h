/**************************************************************************
 *	IHyFileIO.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2015 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __IHyFileIO_h__
#define __IHyFileIO_h__

#include "Afx/HyStdAfx.h"
#include "FileIO/Atlas/HyTexture.h"

#include "Data/HyFactory.h"
#include "Threading/Threading.h"

#include "Renderer/HyGfxComms.h"

#include <queue>
using std::queue;

class IHyInst2d;

class IHyData;
class HySfxData;
class HySprite2dData;
class HySpine2dData;
class HyText2dData;
class HyTexturedQuad2dData;
class HyMesh3dData;

class IHyFileIO
{
	HyGfxComms &										m_GfxCommsRef;
	HyScene &											m_SceneRef;

	// Thread control structure to help sync loading of factory data
	struct LoadThreadCtrl
	{
		WaitEvent		m_WaitEvent_HasNewData;

		queue<IHyData *> *m_pLoadQueue_Shared;
		queue<IHyData *> *m_pLoadQueue_Retrieval;

		BasicSection	m_csSharedQueue;
		BasicSection	m_csRetrievalQueue;

		LoadThreadCtrl() : m_WaitEvent_HasNewData(L"Thread Idler", true)
		{}
	};
	LoadThreadCtrl										m_LoadingCtrl;

	vector<IHyInst2d *>									m_vQueuedInst2d;

	HyFactory<HySfxData>								m_Sfx;
	HyFactory<HySprite2dData>							m_Sprite2d;
	HyFactory<HySpine2dData>							m_Spine2d;
	HyFactory<HyMesh3dData>								m_Mesh3d;
	HyFactory<HyText2dData>								m_Txt2d;
	HyFactory<HyTexturedQuad2dData>						m_Quad2d;

	// Queues responsible for passing and retrieving factory data between the loading thread
	queue<IHyData *>									m_LoadQueue_Prepare;
	queue<IHyData *>									m_LoadQueue_Shared;
	queue<IHyData *>									m_LoadQueue_Retrieval;

	queue<IHyData *> *									m_pGfxQueue_Retrieval;

	// Loading thread info pointer
	ThreadInfoPtr										m_pLoadingThread;

	HyTexture **	m_ppTextures;
	uint32			m_uiNumTextures;

public:
	IHyFileIO(const char *szDataDirPath, HyGfxComms &gfxCommsRef, HyScene &sceneRef);
	virtual ~IHyFileIO();

	void Update();

	void LoadInst2d(IHyInst2d *pInst);
	void RemoveInst(IHyInst2d *pInst);

private:
	void OnDataLoaded(IHyData *pData);
	void DiscardData(IHyData *pData);
	void DeleteData(IHyData *pData);

	static void LoadingThread(void *pParam);
};

#endif __IHyFileIO_h__
