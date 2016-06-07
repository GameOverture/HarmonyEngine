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
#include "Assets/HyTextures.h"

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

class HyAssetManager
{
	const std::string									m_sDATADIR;

	enum eSubDirs
	{
		SUBDIR_Atlases = 0,
		SUBDIR_Audio,
		SUBDIR_Particles,
		SUBDIR_Fonts,
		SUBDIR_Spine,
		SUBDIR_Sprites,
		SUBDIR_Shaders,
		SUBDIR_Entities,
		SUBDIR_Meshes,

		NUM_SUBDIRS
	};
	static const std::string							sm_sSUBDIRNAMES[NUM_SUBDIRS];

	HyGfxComms &										m_GfxCommsRef;
	HyScene &											m_SceneRef;

	// Thread control structure to help sync loading of factory data
	struct LoadThreadCtrl
	{
		queue<IHyData *> &	m_LoadQueueRef_Shared;
		queue<IHyData *> &	m_LoadQueueRef_Retrieval;

		WaitEvent			m_WaitEvent_HasNewData;
		BasicSection		m_csSharedQueue;
		BasicSection		m_csRetrievalQueue;

		enum eState
		{
			STATE_Run = 0,
			STATE_ShouldExit,
			STATE_HasExited
		};
		eState				m_eState;

		LoadThreadCtrl(queue<IHyData *> &LoadQueueRef_Shared, queue<IHyData *> &LoadQueueRef_Retrieval) :	m_LoadQueueRef_Shared(LoadQueueRef_Shared),
																											m_LoadQueueRef_Retrieval(LoadQueueRef_Retrieval),
																											m_WaitEvent_HasNewData(L"Thread Idler", true),
																											m_eState(STATE_Run)
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

	HyTextures											m_AtlasManager;

	// Queues responsible for passing and retrieving factory data between the loading thread
	queue<IHyData *>									m_LoadQueue_Prepare;
	queue<IHyData *>									m_LoadQueue_Shared;
	queue<IHyData *>									m_LoadQueue_Retrieval;

	queue<IHyData2d *> *								m_pGfxQueue_Retrieval;

	// Loading thread info pointer
	ThreadInfoPtr										m_pLoadingThread;

public:
	HyAssetManager(std::string sDataDirPath, HyGfxComms &gfxCommsRef, HyScene &sceneRef);
	virtual ~HyAssetManager();

	void Update();

	void LoadInst2d(IHyInst2d *pInst);
	void RemoveInst(IHyInst2d *pInst);

	void Shutdown();
	bool IsShutdown();

private:
	void FinalizeData(IHyData *pData);
	void DiscardData(IHyData *pData);

	static void LoadingThread(void *pParam);
};

#endif /* __IHyAssetManager_h__ */
