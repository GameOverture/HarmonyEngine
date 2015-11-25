/**************************************************************************
 *	HyAssetManager.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2015 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __IHyAssetManager_h__
#define __IHyAssetManager_h__

#include "Afx/HyStdAfx.h"

#include "Assets/HyFactory.h"
#include "Assets/HyAtlasManager.h"

#include "Renderer/HyGfxComms.h"
#include "Scene/HyScene.h"

#include "Threading/Threading.h"

#include <queue>
using std::queue;

class IHyInst2d;

class IHyData;
class IHyData2d;
class HySfxData;
class HySprite2dData;
class HySpine2dData;
class HyText2dData;
class HyTexturedQuad2dData;
class HyMesh3dData;

enum eHyReloadCode
{
	HYRELOADCODE_Inactive = 0,
	HYRELOADCODE_InProgress,
	HYRELOADCODE_Finished,
	HYRELOADCODE_ReInit
};

class HyAssetManager
{
	std::string											m_sDataDir;

	HyGfxComms &										m_GfxCommsRef;
	HyScene &											m_SceneRef;

	// Thread control structure to help sync loading of factory data
	struct LoadThreadCtrl
	{
		queue<IHyData *> &	m_LoadQueueRef_Shared;
		queue<IHyData *> &	m_LoadQueueRef_Retrieval;

		WaitEvent		m_WaitEvent_HasNewData;
		BasicSection	m_csSharedQueue;
		BasicSection	m_csRetrievalQueue;

		LoadThreadCtrl(queue<IHyData *> &LoadQueueRef_Shared, queue<IHyData *> &LoadQueueRef_Retrieval) :	m_LoadQueueRef_Shared(LoadQueueRef_Shared),
																											m_LoadQueueRef_Retrieval(LoadQueueRef_Retrieval),
																											m_WaitEvent_HasNewData(L"Thread Idler", true)
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

	HyAtlasManager										m_AtlasManager;

	// Queues responsible for passing and retrieving factory data between the loading thread
	queue<IHyData *>									m_LoadQueue_Prepare;
	queue<IHyData *>									m_LoadQueue_Shared;
	queue<IHyData *>									m_LoadQueue_Retrieval;

	queue<IHyData2d *> *								m_pGfxQueue_Retrieval;

	// Loading thread info pointer
	ThreadInfoPtr										m_pLoadingThread;

	bool												m_bIsReloading;
	vector<IHyInst2d *>									m_vReloadInsts;
	std::string											m_sNewDataDirPath;

public:
	HyAssetManager(const char *szDataDirPath, HyGfxComms &gfxCommsRef, HyScene &sceneRef);
	virtual ~HyAssetManager();

	void Update();

	void LoadInst2d(IHyInst2d *pInst);
	void RemoveInst(IHyInst2d *pInst);

	bool Reload();											// Reload every instance
	bool Reload(std::vector<std::string> &vPathsRef);		// Reload only the specified instances
	bool Reload(std::string sNewDataDirPath);				// Unload everything, and reinitialize to a new data directory. Doesn't load up anything when done.
	eHyReloadCode IsReloading();

	static char *ReadTextFile(const char *szFilePath, int *iLength);
	static std::string ReadTextFile(const char *szFilePath);
	static bool FileExists(const std::string &sFilePath);

private:
	void FinalizeData(IHyData *pData);
	void DiscardData(IHyData *pData);

	static void LoadingThread(void *pParam);
};

#endif /* __IHyAssetManager_h__ */
