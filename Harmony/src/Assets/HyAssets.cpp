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
																										m_LoadingCtrl(m_LoadQueue_Shared, m_LoadQueue_Retrieval)
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

	HyDataDraw::sm_pTextures = &m_AtlasManager;
	IHyDraw2d::sm_pHyAssets = this;
}

HyAssets::~HyAssets()
{
	HyAssert(IsShutdown(), "Tried to destruct the HyAssets while data still exists");
}

void HyAssets::Update()
{
	// Check to see if we have any pending loads to make
	if(m_LoadQueue_Prepare.empty() == false)
	{
		// Copy load queue data into shared data
		m_LoadingCtrl.m_csSharedQueue.Lock();
		{
			while(m_LoadQueue_Prepare.empty() == false)
			{
				m_LoadQueue_Shared.push(m_LoadQueue_Prepare.front());
				m_LoadQueue_Prepare.pop();
			}
		}
		m_LoadingCtrl.m_csSharedQueue.Unlock();

		m_LoadingCtrl.m_WaitEvent_HasNewData.Set();
	}

	// Check to see if any loaded data (from the load thread) is ready to go
	m_LoadingCtrl.m_csRetrievalQueue.Lock();
	{
		while(m_LoadQueue_Retrieval.empty() == false)
		{
			IHyData *pData = m_LoadQueue_Retrieval.front();
			m_LoadQueue_Retrieval.pop();

			if(pData->GetDataType() == HYDATA_2d)
			{
				pData->SetLoadState(HYLOADSTATE_Queued);
				m_GfxCommsRef.TxData(static_cast<HyDataDraw *>(pData));
			}
			else
				FinalizeData(pData);
		}
	}
	m_LoadingCtrl.m_csRetrievalQueue.Unlock();

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	// Grab and process any returning HyDataDraw's from the Render thread
	m_pGfxQueue_Retrieval = m_GfxCommsRef.RxData();
	while(!m_pGfxQueue_Retrieval->empty())
	{
		HyDataDraw *pData = m_pGfxQueue_Retrieval->front();
		m_pGfxQueue_Retrieval->pop();

		if(pData->GetLoadState() == HYLOADSTATE_ReloadGfx)
		{
			pData->SetLoadState(HYLOADSTATE_Queued);
			m_GfxCommsRef.TxData(pData);
		}
		else
			FinalizeData(pData);
	}
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

void HyAssets::LoadInst2d(IHyDraw2d *pInst)
{
	IHyData *pLoadData = NULL;
	switch(pInst->GetType())
	{
	case HYTYPE_Sprite2d:
		pLoadData = m_Sprite2d.GetOrCreateData2d(pInst->GetPrefix(), pInst->GetName(), pInst->GetShaderId());
		break;
	case HYTYPE_Spine2d:
		pLoadData = m_Spine2d.GetOrCreateData2d(pInst->GetPrefix(), pInst->GetName(), pInst->GetShaderId());
		break;
	case HYTYPE_Text2d:
		pLoadData = m_Txt2d.GetOrCreateData2d(pInst->GetPrefix(), pInst->GetName(), pInst->GetShaderId());
		break;
	case HYTYPE_TexturedQuad2d:
		pLoadData = m_Quad2d.GetOrCreateData2d(pInst->GetPrefix(), pInst->GetName(), pInst->GetShaderId());
		break;
	case HYTYPE_Primitive2d:
		pLoadData = m_Primitive2d.GetOrCreateData2d(pInst->GetPrefix(), pInst->GetName(), pInst->GetShaderId());
		break;

	default:
		return;
	}

	pInst->SetData(pLoadData);
	if(pLoadData)
		pLoadData->IncRef();

	if(pLoadData == NULL || pLoadData->GetLoadState() == HYLOADSTATE_Loaded)
	{
		pInst->SetLoaded();
		m_SceneRef.AddInstance(pInst);
	}
	else
	{
		m_QueuedInst2dList.push_back(pInst);

		if(pLoadData->GetLoadState() == HYLOADSTATE_Inactive)
		{
			pLoadData->SetLoadState(HYLOADSTATE_Queued);
			m_LoadQueue_Prepare.push(pLoadData);
		}
		else if(pLoadData->GetLoadState() == HYLOADSTATE_Discarded)
		{
			pLoadData->SetLoadState(HYLOADSTATE_ReloadGfx);
		}
	}
}

void HyAssets::RemoveInst(IHyDraw2d *pInst)
{
	IHyData *pInstData = NULL;

	switch(pInst->GetLoadState())
	{
	case HYLOADSTATE_Inactive:
		break;

	case HYLOADSTATE_Loaded:
		m_SceneRef.RemoveInst(pInst);

		pInstData = pInst->GetData();
		if(pInstData && pInstData->DecRef())
			DiscardData(pInstData);
		break;

	case HYLOADSTATE_Queued:
	case HYLOADSTATE_ReloadGfx:
		for(std::vector<IHyDraw2d *>::iterator it = m_QueuedInst2dList.begin(); it != m_QueuedInst2dList.end(); ++it)
		{
			if((*it) == pInst)
			{
				m_QueuedInst2dList.erase(it);
				break;
			}
		}
		break;

	default:
		HyError("HyAssets::RemoveInst() passed an invalid HyLoadState");
	}
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

void HyAssets::FinalizeData(IHyData *pData)
{
	if(pData->GetLoadState() == HYLOADSTATE_Queued)
	{
		bool bDataIsUsed = false;
		for(std::vector<IHyDraw2d *>::iterator iter = m_QueuedInst2dList.begin(); iter != m_QueuedInst2dList.end();)
		{
			if((*iter)->GetData() == pData)
			{
				(*iter)->SetLoaded();
				m_SceneRef.AddInstance(*iter);

				bDataIsUsed = true;

				iter = m_QueuedInst2dList.erase(iter);
			}
			else
				++iter;
		}

		if(bDataIsUsed)
			pData->SetLoadState(HYLOADSTATE_Loaded);
		else
			DiscardData(pData);
	}
	else if(pData->GetLoadState() == HYLOADSTATE_Discarded)
	{
		HyAssert(pData->GetRefCount() <= 0, "HyAssets::Update() tried to delete an IData with active references. Num Refs: " << pData->GetRefCount());

		switch(pData->GetInstType())
		{
		case HYTYPE_Sound2d:		m_Sfx.DeleteData(static_cast<HySfxData *>(pData));					break;
		case HYTYPE_Sprite2d:		m_Sprite2d.DeleteData(static_cast<HySprite2dData *>(pData));		break;
		case HYTYPE_Spine2d:		m_Spine2d.DeleteData(static_cast<HySpine2dData *>(pData));			break;
		case HYTYPE_Text2d:			m_Txt2d.DeleteData(static_cast<HyText2dData *>(pData));				break;
		case HYTYPE_TexturedQuad2d:	m_Quad2d.DeleteData(static_cast<HyTexturedQuad2dData *>(pData));	break;
		default:
			HyError("HyAssets::Update() got a returned IHyData from gfx comms with an invalid type: " << pData->GetInstType());
		}
	}
	else
	{
		HyError("HyAssets::Update() got a returned IHyData from gfx comms with an invalid state: " << pData->GetLoadState());
	}
}

void HyAssets::DiscardData(IHyData *pData)
{
	HyAssert(pData->GetRefCount() <= 0, "HyAssets::DiscardData() tried to remove an IData with active references");


	// TODO: Wot in tarnation
	if(pData->GetInstType() == HYTYPE_Primitive2d && static_cast<HyDataDraw *>(pData)->GetShaderId() < HYSHADERPROG_CustomStartIndex)
		return;


	//HyLog("Deleting data: " << pData->GetPath());
	pData->SetLoadState(HYLOADSTATE_Discarded);

	if(pData->GetDataType() == HYDATA_2d)
		m_GfxCommsRef.TxData(static_cast<HyDataDraw *>(pData));
	else
		FinalizeData(pData);
}

/*static*/ void HyAssets::LoadingThread(void *pParam)
{
	LoadThreadCtrl *pLoadingCtrl = reinterpret_cast<LoadThreadCtrl *>(pParam);
	std::vector<IHyData *>	vCurLoadData;

	while(pLoadingCtrl->m_eState == LoadThreadCtrl::STATE_Run)
	{
		// Wait idle indefinitely until there is new data to be grabbed
		pLoadingCtrl->m_WaitEvent_HasNewData.Wait();

		// Reset the event so we wait the next time we loop
		pLoadingCtrl->m_WaitEvent_HasNewData.Reset();

		// Copy all the IData ptrs into the 'vCurLoadData' to be processed, while emptying the shared queue
		pLoadingCtrl->m_csSharedQueue.Lock();
		{
			while(pLoadingCtrl->m_LoadQueueRef_Shared.empty() == false)
			{
				vCurLoadData.push_back(pLoadingCtrl->m_LoadQueueRef_Shared.front());
				pLoadingCtrl->m_LoadQueueRef_Shared.pop();
			}
		}
		pLoadingCtrl->m_csSharedQueue.Unlock();

		// Load everything that is enqueued (outside of any critical section)
		for(uint32 i = 0; i < vCurLoadData.size(); ++i)
			vCurLoadData[i]->OnLoadThread();

		// Copy all the (loaded) IData ptrs to the retrieval vector
		pLoadingCtrl->m_csRetrievalQueue.Lock();
		{
			for(uint32 i = 0; i < vCurLoadData.size(); ++i)
				pLoadingCtrl->m_LoadQueueRef_Retrieval.push(vCurLoadData[i]);
		}
		pLoadingCtrl->m_csRetrievalQueue.Unlock();

		vCurLoadData.clear();
	}

	pLoadingCtrl->m_eState = LoadThreadCtrl::STATE_HasExited;
}
