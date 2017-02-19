/**************************************************************************
 *	HyAssets.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2015 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Assets/HyAssets.h"

#include "Scene/Nodes/Draws/IHyDraw2d.h"

#include "Assets/Data/HyAudioData.h"
#include "Assets/Data/HySpine2dData.h"
#include "Assets/Data/HySprite2dData.h"
#include "Assets/Data/HyText2dData.h"
#include "Assets/Data/HyTexturedQuad2dData.h"
#include "Assets/Data/HyPrimitive2dData.h"
#include "Assets/Data/HyMesh3dData.h"

#include "Utilities/HyMath.h"
#include "Utilities/HyStrManip.h"

#include "Diagnostics/HyGuiComms.h"

HyAssets::HyAssets(std::string sDataDirPath, HyGfxComms &gfxCommsRef, HyScene &sceneRef) :	m_sDATADIR(MakeStringProperPath(sDataDirPath.c_str(), "/", true)),
																										m_GfxCommsRef(gfxCommsRef),
																										m_SceneRef(sceneRef),
																										m_AtlasManager(m_sDATADIR + "Atlases/"),
																										m_Sfx(HYTYPE_Sound2d),
																										m_Sprite2d(HYTYPE_Sprite2d),
																										m_Spine2d(HYTYPE_Spine2d),
																										m_Txt2d(HYTYPE_Text2d),
																										m_Mesh3d(HYTYPE_Mesh3d),
																										m_Quad2d(HYTYPE_TexturedQuad2d),
																										m_Primitive2d(HYTYPE_Primitive2d),
																										m_pLastQueuedData(nullptr),
																										m_LoadingCtrl(m_Load_Shared, m_Load_Retrieval)
{
	// Start up Loading thread
	m_pLoadingThread = ThreadManager::Get()->BeginThread(_T("Loading Thread"), THREAD_START_PROCEDURE(LoadingThread), &m_LoadingCtrl);


	std::string sGameDataFilePath(m_sDATADIR);
	sGameDataFilePath += "Data.json";

	std::string sGameDataFileContents;
	HyReadTextFile(sGameDataFilePath.c_str(), sGameDataFileContents);
	
	jsonxx::Object gameDataObj;
	bool bGameDataParsed = gameDataObj.parse(sGameDataFileContents);
	HyAssert(bGameDataParsed, "Could not parse game data");

	m_Sfx.Init(gameDataObj.get<jsonxx::Object>("Audio"));
	m_Txt2d.Init(gameDataObj.get<jsonxx::Object>("Fonts"));
	m_Sprite2d.Init(gameDataObj.get<jsonxx::Object>("Sprites"));
	//jsonxx::Object &entitiesDataObjRef = gameDataObj.get<jsonxx::Object>("Entities");
	//jsonxx::Object &particlesDataObjRef = gameDataObj.get<jsonxx::Object>("Particles");
	//jsonxx::Object &shadersDataObjRef = gameDataObj.get<jsonxx::Object>("Shaders");
	//jsonxx::Object &spineDataObjRef = gameDataObj.get<jsonxx::Object>("Spine");

	IHyDraw2d::sm_pHyAssets = this;
}

HyAssets::~HyAssets()
{
	HyAssert(IsShutdown(), "Tried to destruct the HyAssets while data still exists");
}

void HyAssets::GetNodeData(IHyDraw2d *pDrawNode, IHyData *pData)
{
	switch(pDrawNode->GetType())
	{
	case HYTYPE_Sprite2d:
		pData = m_Sprite2d.GetData(pDrawNode->GetPrefix(), pDrawNode->GetName());
		break;
	case HYTYPE_Spine2d:
		pData = m_Spine2d.GetData(pDrawNode->GetPrefix(), pDrawNode->GetName());
		break;
	case HYTYPE_Text2d:
		pData = m_Txt2d.GetData(pDrawNode->GetPrefix(), pDrawNode->GetName());
		break;
	case HYTYPE_TexturedQuad2d:
		pData = m_Quad2d.GetData(pDrawNode->GetPrefix(), pDrawNode->GetName());
		break;
	case HYTYPE_Primitive2d:
		pData = m_Primitive2d.GetData(pDrawNode->GetPrefix(), pDrawNode->GetName());
		break;

	default:
		return;
	}
}

void HyAssets::LoadGfxData(HyGfxData &drawDataRef)
{
	HyAssert(drawDataRef.m_eLoadState == HYLOADSTATE_Inactive, "HyAssets::LoadGfxData was called on a gfxData that wasn't HYLOADSTATE_Inactive");

	bool bFullyLoaded = true;

	// Check whether all the required atlases are loaded
	for(auto iter = drawDataRef.m_RequiredAtlasIds.begin(); iter != drawDataRef.m_RequiredAtlasIds.end(); ++iter)
	{
		HyAtlasGroup *pAtlasGrp = m_AtlasManager.GetAtlasGroup(*iter);
		if(QueueData(pAtlasGrp) == false)
			bFullyLoaded = false;
	}

	// Check whether we need to load custom shaders
	for(auto iter = drawDataRef.m_RequiredCustomShaders.begin(); iter != drawDataRef.m_RequiredCustomShaders.end(); ++iter)
	{
		IHyShader *pShader = IHyRenderer::FindShader(*iter);
		if(QueueData(pShader) == false)
			bFullyLoaded = false;
	}

	// Set the instance
	if(bFullyLoaded == false)
	{
		drawDataRef.m_eLoadState = HYLOADSTATE_Queued;
		m_QueuedInst2dList.push_back(&drawDataRef);
	}
	else
	{
		m_SceneRef.AddInstance(
		drawDataRef.m_eLoadState = HYLOADSTATE_Loaded;
	}
}

void HyAssets::RemoveGfxData(HyGfxData &drawDataRef)
{
	HyAssert(drawDataRef.m_eLoadState != HYLOADSTATE_Inactive, "HyAssets::RemoveGfxData was called on a gfxData that was HYLOADSTATE_Inactive");

	for(auto iter = drawDataRef.m_RequiredAtlasIds.begin(); iter != drawDataRef.m_RequiredAtlasIds.end(); ++iter)
	{
		HyAtlasGroup *pAtlasGrp = m_AtlasManager.GetAtlasGroup(*iter);
		DequeData(pAtlasGrp);
	}

	for(auto iter = drawDataRef.m_RequiredCustomShaders.begin(); iter != drawDataRef.m_RequiredCustomShaders.end(); ++iter)
	{
		IHyShader *pShader = IHyRenderer::FindShader(*iter);
		DequeData(pShader);
	}

	if(drawDataRef.m_eLoadState == HYLOADSTATE_Queued)
	{
		for(auto it = m_QueuedInst2dList.begin(); it != m_QueuedInst2dList.end(); ++it)
		{
			if((*it) == &drawDataRef)
			{
				m_QueuedInst2dList.erase(it);
				break;
			}
		}
	}

	drawDataRef.m_eLoadState = HYLOADSTATE_Inactive;
}

// Unload everything
void HyAssets::Shutdown()
{
	std::vector<IHyDraw2d *> vReloadInsts;
	m_SceneRef.CopyAllInsts(vReloadInsts);

	for(uint32 i = 0; i < m_QueuedInst2dList.size(); ++i)
		vReloadInsts.push_back(m_QueuedInst2dList[i]);

	for(uint32 i = 0; i < vReloadInsts.size(); ++i)
		vReloadInsts[i]->Unload();

	m_LoadingCtrl.m_eState = LoadThreadCtrl::STATE_ShouldExit;
	m_LoadingCtrl.m_WaitEvent_HasNewData.Set();
}

bool HyAssets::IsShutdown()
{
	bool bTest = m_pLoadingThread->IsAlive();

	return m_LoadingCtrl.m_eState == LoadThreadCtrl::STATE_HasExited &&
		   m_pLoadingThread->IsAlive() == false &&
		   m_Sfx.IsEmpty() &&
		   m_Sprite2d.IsEmpty() &&
		   m_Spine2d.IsEmpty() &&
		   m_Mesh3d.IsEmpty() &&
		   m_Txt2d.IsEmpty() &&
		   m_Quad2d.IsEmpty();
}

void HyAssets::Update()
{
	// Check to see if we have any pending loads to make
	if(m_Load_Prepare.empty() == false)
	{
		// Copy load queue data into shared data
		m_LoadingCtrl.m_csSharedQueue.Lock();
		{
			while(m_Load_Prepare.empty() == false)
			{
				m_Load_Shared.push(m_Load_Prepare.front());
				m_Load_Prepare.pop();
			}
		}
		m_LoadingCtrl.m_csSharedQueue.Unlock();

		m_LoadingCtrl.m_WaitEvent_HasNewData.Set();
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Check to see if any loaded data (from the load thread) is ready to go to the render thread
	m_LoadingCtrl.m_csRetrievalQueue.Lock();
	{
		while(m_Load_Retrieval.empty() == false)
		{
			IHyLoadableData *pData = m_Load_Retrieval.front();
			m_Load_Retrieval.pop();

			m_GfxCommsRef.TxData(pData);
		}
	}
	m_LoadingCtrl.m_csRetrievalQueue.Unlock();

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Grab and process any returning data from the Render thread
	m_pGfxQueue_Retrieval = m_GfxCommsRef.RxData();
	while(!m_pGfxQueue_Retrieval->empty())
	{
		IHyLoadableData *pData = m_pGfxQueue_Retrieval->front();
		m_pGfxQueue_Retrieval->pop();

		FinalizeData(pData);
	}
}

bool HyAssets::QueueData(IHyLoadableData *pData)
{
	bool bAlreadyLoaded = true;
	if(pData->m_uiRefCount == 0)
	{
		if(pData->m_eLoadState == HYLOADSTATE_Inactive)
		{
			pData->m_eLoadState = HYLOADSTATE_Queued;
			m_Load_Prepare.push(pData);

			m_pLastQueuedData = pData;
		}

		bAlreadyLoaded = false;
	}

	pData->m_uiRefCount++;

	return bAlreadyLoaded;
}

void HyAssets::DequeData(IHyLoadableData *pData)
{
	HyAssert(pData->m_eLoadState != HYLOADSTATE_Inactive, "Trying to DequeData that is HYLOADSTATE_Inactive");
	HyAssert(pData->m_uiRefCount > 0, "Tried to decrement a '0' reference");

	pData->m_uiRefCount--;
	if(pData->m_uiRefCount == 0)
	{
		if(pData->m_eLoadState == HYLOADSTATE_Loaded)
		{
			pData->m_eLoadState = HYLOADSTATE_Discarded;
			m_Load_Prepare.push(pData);
		}
	}
}

void HyAssets::FinalizeData(IHyLoadableData *pData)
{
	HyAssert(pData->m_eLoadState != HYLOADSTATE_Inactive, "HyAssets::FinalizeData was passed data that was HYLOADSTATE_Inactive");
	HyAssert(pData->m_eLoadState != HYLOADSTATE_Loaded, "HyAssets::FinalizeData was passed data that was HYLOADSTATE_Loaded");

	if(pData->m_eLoadState == HYLOADSTATE_Queued)
	{
		if(pData->m_uiRefCount == 0)
		{
			pData->m_eLoadState = HYLOADSTATE_Discarded;
			m_Load_Prepare.push(pData);
		}
		else
		{
			pData->m_eLoadState = HYLOADSTATE_Loaded;

			if(pData == m_pLastQueuedData)
			{
				for(auto iter = m_QueuedInst2dList.begin(); iter != m_QueuedInst2dList.end(); ++iter)
				{
					m_SceneRef.AddInstance
					(*iter)->m_eLoadState = HYLOADSTATE_Loaded;
				}

				m_QueuedInst2dList.clear();
				m_pLastQueuedData = nullptr;
			}
		}
	}
	else
	{
		pData->m_eLoadState = HYLOADSTATE_Inactive;
		HyLog("Deleted loadable data");
	}
}

/*static*/ void HyAssets::LoadingThread(void *pParam)
{
	LoadThreadCtrl *pLoadingCtrl = reinterpret_cast<LoadThreadCtrl *>(pParam);
	std::vector<IHyLoadableData *>	dataList;

	while(pLoadingCtrl->m_eState == LoadThreadCtrl::STATE_Run)
	{
		// Wait idle indefinitely until there is new data to be grabbed
		pLoadingCtrl->m_WaitEvent_HasNewData.Wait();

		// Reset the event so we wait the next time we loop
		pLoadingCtrl->m_WaitEvent_HasNewData.Reset();

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Copy all the ptrs into their vectors to be processed, while emptying the shared queue
		pLoadingCtrl->m_csSharedQueue.Lock();
		{
			while(pLoadingCtrl->m_Load_SharedRef.empty() == false)
			{
				dataList.push_back(pLoadingCtrl->m_Load_SharedRef.front());
				pLoadingCtrl->m_Load_SharedRef.pop();
			}
		}
		pLoadingCtrl->m_csSharedQueue.Unlock();

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Load everything that is enqueued (outside of any critical section)
		for(uint32 i = 0; i < dataList.size(); ++i)
			dataList[i]->OnLoadThread();

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Copy all the (loaded) IData ptrs to the retrieval vector
		pLoadingCtrl->m_csRetrievalQueue.Lock();
		{
			for(uint32 i = 0; i < dataList.size(); ++i)
				pLoadingCtrl->m_Load_RetrievalRef.push(dataList[i]);
		}
		pLoadingCtrl->m_csRetrievalQueue.Unlock();

		dataList.clear();
	}

	pLoadingCtrl->m_eState = LoadThreadCtrl::STATE_HasExited;
}
