/**************************************************************************
 *	HyAssetManager.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2015 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Assets/HyAssetManager.h"

#include "Scene/Instances/IHyInst2d.h"

#include "Assets/Data/HySfxData.h"
#include "Assets/Data/HySpine2dData.h"
#include "Assets/Data/HySprite2dData.h"
#include "Assets/Data/HyText2dData.h"
#include "Assets/Data/HyTexturedQuad2dData.h"
#include "Assets/Data/HyPrimitive2dData.h"
#include "Assets/Data/HyMesh3dData.h"

#include "Utilities/HyMath.h"
#include "Utilities/HyStrManip.h"

const std::string	HyAssetManager::sm_sSUBDIRNAMES[NUM_SUBDIRS] = { "Atlases/", "Audio/", "Particles/", "Fonts/", "Spine/", "Sprites/", "Shaders/", "Entities/", "Meshes/" };

HyAssetManager::HyAssetManager(std::string sDataDirPath, HyGfxComms &gfxCommsRef, HyScene &sceneRef) :	m_sDATADIR(MakeStringProperPath(sDataDirPath.c_str(), "/", true)),
																										m_GfxCommsRef(gfxCommsRef),
																										m_SceneRef(sceneRef),
																										m_AtlasManager(m_sDATADIR + sm_sSUBDIRNAMES[SUBDIR_Atlases]),
																										m_Sfx(HYINST_Sound2d, m_sDATADIR + sm_sSUBDIRNAMES[SUBDIR_Audio]),
																										m_Sprite2d(HYINST_Sprite2d, m_sDATADIR + sm_sSUBDIRNAMES[SUBDIR_Sprites]),
																										m_Spine2d(HYINST_Spine2d, m_sDATADIR + sm_sSUBDIRNAMES[SUBDIR_Spine]),
																										m_Txt2d(HYINST_Text2d, m_sDATADIR + sm_sSUBDIRNAMES[SUBDIR_Fonts]),
																										m_Mesh3d(HYINST_Mesh3d, m_sDATADIR + sm_sSUBDIRNAMES[SUBDIR_Meshes]),
																										m_Quad2d(HYINST_TexturedQuad2d, ""),
																										m_Primitive2d(HYINST_Primitive2d, ""),
																										m_LoadingCtrl(m_LoadQueue_Shared, m_LoadQueue_Retrieval)
{
	// Start up Loading thread
	m_pLoadingThread = ThreadManager::Get()->BeginThread(_T("Loading Thread"), THREAD_START_PROCEDURE(LoadingThread), &m_LoadingCtrl);

	IHy2dData::sm_pTextures = &m_AtlasManager;
	IHyInst2d::sm_pAssetManager = this;
}


HyAssetManager::~HyAssetManager()
{
	HyAssert(IsShutdown(), "Tried to destruct the HyAssetManager while data still exists");
}

void HyAssetManager::Update()
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
				m_GfxCommsRef.TxData(static_cast<IHy2dData *>(pData));
			}
			else
				FinalizeData(pData);
		}
	}
	m_LoadingCtrl.m_csRetrievalQueue.Unlock();

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	// Grab and process any returning IHy2dData's from the Render thread
	m_pGfxQueue_Retrieval = m_GfxCommsRef.RxData();
	while(!m_pGfxQueue_Retrieval->empty())
	{
		IHy2dData *pData = m_pGfxQueue_Retrieval->front();
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

void HyAssetManager::LoadInst2d(IHyInst2d *pInst)
{
	IHyData *pLoadData = NULL;
	switch(pInst->GetInstType())
	{
	case HYINST_Sprite2d:
		pLoadData = m_Sprite2d.GetOrCreateData2d(pInst->GetPrefix(), pInst->GetName(), pInst->GetShaderId());
		break;
	case HYINST_Spine2d:
		pLoadData = m_Spine2d.GetOrCreateData2d(pInst->GetPrefix(), pInst->GetName(), pInst->GetShaderId());
		break;
	case HYINST_Text2d:
		pLoadData = m_Txt2d.GetOrCreateData2d(pInst->GetPrefix(), pInst->GetName(), pInst->GetShaderId());
		break;
	case HYINST_TexturedQuad2d:
		pLoadData = m_Quad2d.GetOrCreateData2d(pInst->GetPrefix(), pInst->GetName(), pInst->GetShaderId());
		break;
	case HYINST_Primitive2d:
		pLoadData = m_Primitive2d.GetOrCreateData2d(pInst->GetPrefix(), pInst->GetName(), pInst->GetShaderId());
		break;
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
		m_vQueuedInst2d.push_back(pInst);

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

void HyAssetManager::RemoveInst(IHyInst2d *pInst)
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
		for(vector<IHyInst2d *>::iterator it = m_vQueuedInst2d.begin(); it != m_vQueuedInst2d.end(); ++it)
		{
			if((*it) == pInst)
			{
				m_vQueuedInst2d.erase(it);
				break;
			}
		}
		break;

	default:
		HyError("HyAssetManager::RemoveInst() passed an invalid HyLoadState");
	}
}

// Unload everything
void HyAssetManager::Shutdown()
{
	vector<IHyInst2d *> vReloadInsts;
	m_SceneRef.CopyAllInsts(vReloadInsts);

	for(uint32 i = 0; i < m_vQueuedInst2d.size(); ++i)
		vReloadInsts.push_back(m_vQueuedInst2d[i]);

	for(uint32 i = 0; i < vReloadInsts.size(); ++i)
		vReloadInsts[i]->Unload();

	m_LoadingCtrl.m_eState = LoadThreadCtrl::STATE_ShouldExit;
	m_LoadingCtrl.m_WaitEvent_HasNewData.Set();
}

bool HyAssetManager::IsShutdown()
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

void HyAssetManager::FinalizeData(IHyData *pData)
{
	if(pData->GetLoadState() == HYLOADSTATE_Queued)
	{
		bool bDataIsUsed = false;
		for(vector<IHyInst2d *>::iterator iter = m_vQueuedInst2d.begin(); iter != m_vQueuedInst2d.end();)
		{
			if((*iter)->GetData() == pData)
			{
				(*iter)->SetLoaded();
				m_SceneRef.AddInstance(*iter);

				bDataIsUsed = true;

				iter = m_vQueuedInst2d.erase(iter);
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
		HyAssert(pData->GetRefCount() <= 0, "HyAssetManager::Update() tried to delete an IData with active references. Num Refs: " << pData->GetRefCount());

		switch(pData->GetInstType())
		{
		case HYINST_Sound2d:		m_Sfx.DeleteData(static_cast<HySfxData *>(pData));					break;
		case HYINST_Sprite2d:		m_Sprite2d.DeleteData(static_cast<HySprite2dData *>(pData));		break;
		case HYINST_Spine2d:		m_Spine2d.DeleteData(static_cast<HySpine2dData *>(pData));			break;
		case HYINST_Text2d:			m_Txt2d.DeleteData(static_cast<HyText2dData *>(pData));				break;
		case HYINST_TexturedQuad2d:	m_Quad2d.DeleteData(static_cast<HyTexturedQuad2dData *>(pData));	break;
		default:
			HyError("HyAssetManager::Update() got a returned IHyData from gfx comms with an invalid type: " << pData->GetInstType());
		}
	}
	else
	{
		HyError("HyAssetManager::Update() got a returned IHyData from gfx comms with an invalid state: " << pData->GetLoadState());
	}
}

void HyAssetManager::DiscardData(IHyData *pData)
{
	HyAssert(pData->GetRefCount() <= 0, "HyAssetManager::DiscardData() tried to remove an IData with active references");

	// TODO: Log about erasing data
	pData->SetLoadState(HYLOADSTATE_Discarded);

	if(pData->GetDataType() == HYDATA_2d)
		m_GfxCommsRef.TxData(static_cast<IHy2dData *>(pData));
	else
		FinalizeData(pData);
}

/*static*/ void HyAssetManager::LoadingThread(void *pParam)
{
	LoadThreadCtrl *pLoadingCtrl = reinterpret_cast<LoadThreadCtrl *>(pParam);
	vector<IHyData *>	vCurLoadData;

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
