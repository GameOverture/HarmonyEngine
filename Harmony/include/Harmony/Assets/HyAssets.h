/**************************************************************************
 *	HyAssets.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2015 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __IHyAssets_h__
#define __IHyAssets_h__

#include "Afx/HyStdAfx.h"

#include "Assets/Containers/HyNodeDataContainer.h"
#include "Assets/Containers/HyAtlasContainer.h"

#include "Renderer/Components/HyGfxComms.h"
#include "Scene/HyScene.h"

#include "Threading/Threading.h"

#include <queue>
using std::queue;

class IHyDraw2d;

class IHyData;
class HyDataDraw;
class HySfxData;
class HySprite2dData;
class HySpine2dData;
class HyText2dData;
class HyTexturedQuad2dData;
class HyPrimitive2dData;
class HyMesh3dData;

class HyAssets
{
	const std::string									m_sDATADIR;

	HyGfxComms &										m_GfxCommsRef;
	HyScene &											m_SceneRef;

	HyAtlasContainer									m_AtlasManager;

	HyNodeDataContainer<HySfxData>						m_Sfx;
	HyNodeDataContainer<HySprite2dData>					m_Sprite2d;
	HyNodeDataContainer<HySpine2dData>					m_Spine2d;
	HyNodeDataContainer<HyMesh3dData>					m_Mesh3d;
	HyNodeDataContainer<HyText2dData>					m_Txt2d;
	HyNodeDataContainer<HyTexturedQuad2dData>			m_Quad2d;
	HyNodeDataContainer<HyPrimitive2dData>				m_Primitive2d;

	std::vector<IHyDraw2d *>							m_QueuedInst2dList;

	// Queues responsible for passing and retrieving factory data between the loading thread
	queue<IHyData *>									m_LoadQueue_Prepare;
	queue<IHyData *>									m_LoadQueue_Shared;
	queue<IHyData *>									m_LoadQueue_Retrieval;

	queue<HyDataDraw *> *								m_pGfxQueue_Retrieval;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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

		LoadThreadCtrl(queue<IHyData *> &LoadQueueRef_Shared, queue<IHyData *> &LoadQueueRef_Retrieval) : m_LoadQueueRef_Shared(LoadQueueRef_Shared),
			m_LoadQueueRef_Retrieval(LoadQueueRef_Retrieval),
			m_WaitEvent_HasNewData(L"Thread Idler", true),
			m_eState(STATE_Run)
		{}
	};
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	LoadThreadCtrl										m_LoadingCtrl;
	ThreadInfoPtr										m_pLoadingThread;	// Loading thread info pointer

public:
	HyAssets(std::string sDataDirPath, HyGfxComms &gfxCommsRef, HyScene &sceneRef);
	virtual ~HyAssets();

	void Update();

	void GetNodeData(IHyDraw2d *pDrawNode, IHyData *pData);
	void LoadInst2d(IHyDraw2d *pInst);
	void RemoveInst(IHyDraw2d *pInst);

	void Shutdown();
	bool IsShutdown();

private:
	void FinalizeData(IHyData *pData);
	void DiscardData(IHyData *pData);

	static void LoadingThread(void *pParam);
};

#endif /* __IHyAssets_h__ */
