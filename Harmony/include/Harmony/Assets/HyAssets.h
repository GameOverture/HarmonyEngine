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

#include "Assets/Data/HyGfxData.h"
#include "Assets/Containers/HyNodeDataContainer.h"
#include "Assets/Containers/HyAtlasContainer.h"

#include "Renderer/Components/HyGfxComms.h"
#include "Scene/HyScene.h"

#include "Threading/Threading.h"

#include <queue>

class IHyDraw2d;

class IHyData;
class HyGfxData;
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
	std::map<int32, HyTexturedQuad2dData *>				m_Quad2d;

	IHyLoadableData *									m_pLastQueuedData;
	IHyLoadableData *									m_pLastDiscardedData;
	std::vector<IHyDraw2d *>							m_QueuedInst2dList;

	// Queues responsible for passing and retrieving factory data between the loading thread
	std::queue<IHyLoadableData *>						m_Load_Prepare;
	std::queue<IHyLoadableData *>						m_Load_Shared;
	std::queue<IHyLoadableData *>						m_Load_Retrieval;

	std::queue<IHyLoadableData *> *						m_pGfxQueue_Retrieval;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Thread control structure to help sync loading of factory data
	struct LoadThreadCtrl
	{
		std::queue<IHyLoadableData *> &		m_Load_SharedRef;
		std::queue<IHyLoadableData *> &		m_Load_RetrievalRef;

		WaitEvent							m_WaitEvent_HasNewData;
		BasicSection						m_csSharedQueue;
		BasicSection						m_csRetrievalQueue;

		enum eState
		{
			STATE_Run = 0,
			STATE_ShouldExit,
			STATE_HasExited
		};
		eState				m_eState;

		LoadThreadCtrl(std::queue<IHyLoadableData *> &load_SharedRef,
					   std::queue<IHyLoadableData *> &Load_RetrievalRef) :	m_Load_SharedRef(load_SharedRef),
																			m_Load_RetrievalRef(Load_RetrievalRef),
																			m_WaitEvent_HasNewData(L"Thread Idler", true),
																			m_eState(STATE_Run)
		{ }
	};
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	LoadThreadCtrl										m_LoadingCtrl;
	ThreadInfoPtr										m_pLoadingThread;	// Loading thread info pointer

public:
	HyAssets(std::string sDataDirPath, HyGfxComms &gfxCommsRef, HyScene &sceneRef);
	virtual ~HyAssets();

	void GetNodeData(IHyDraw2d *pDrawNode, IHyData *&pDataOut);
	void LoadGfxData(IHyDraw2d *pDraw2d);
	void RemoveGfxData(IHyDraw2d *pDraw2d);

	void Shutdown();
	bool IsShutdown();

	void Update();

private:
	bool QueueData(IHyLoadableData *pData);
	void DequeData(IHyLoadableData *pData);
	void FinalizeData(IHyLoadableData *pData);

	static void LoadingThread(void *pParam);
};

#endif /* __IHyAssets_h__ */
