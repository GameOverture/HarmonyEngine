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

#include "Utilities/HyStrManip.h"
#include "Utilities/jsonxx.h"

HyAssetManager::HyAssetManager(const char *szDataDirPath, HyGfxComms &gfxCommsRef, HyScene &sceneRef) : m_sDATADIR(MakeStringProperPath(szDataDirPath, "/")),
																										m_GfxCommsRef(gfxCommsRef),
																										m_SceneRef(sceneRef),
																										m_AtlasManager(m_sDATADIR + "Atlas/"),
																										m_Sfx(HYINST_Sound2d, m_sDATADIR + "Sound/"),
																										m_Sprite2d(HYINST_Sprite2d, m_sDATADIR + "Sprite/"),
																										m_Spine2d(HYINST_Spine2d, m_sDATADIR + "Spine/"),
																										m_Txt2d(HYINST_Text2d, m_sDATADIR + "Font/"),
																										m_Mesh3d(HYINST_Mesh3d, m_sDATADIR + "Mesh/"),
																										m_Quad2d(HYINST_TexturedQuad2d, ""),
																										m_LoadingCtrl(m_LoadQueue_Shared, m_LoadQueue_Retrieval),
																										m_bIsReloading(false),
																										m_sNewDataDirPath("")
{
	// Start up Loading thread
	m_pLoadingThread = ThreadManager::Get()->BeginThread(_T("Loading Thread"), THREAD_START_PROCEDURE(LoadingThread), &m_LoadingCtrl);

	IHyData2d::sm_pTextures = &m_AtlasManager;
	IHyInst2d::sm_pAssetManager = this;
}


HyAssetManager::~HyAssetManager()
{
	HyAssert(DoesAnyDataExist() == false, "Tried to destruct the HyAssetManager while data still exists");
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
				m_GfxCommsRef.SendAtlasGroup(static_cast<IHyData2d *>(pData));
			}
			else
				FinalizeData(pData);
		}
	}
	m_LoadingCtrl.m_csRetrievalQueue.Unlock();

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	// Grab and process any returning IHyData2d's from the Render thread
	m_pGfxQueue_Retrieval = m_GfxCommsRef.RetrieveAtlasGroups();
	while(!m_pGfxQueue_Retrieval->empty())
	{
		IHyData2d *pData = m_pGfxQueue_Retrieval->front();
		m_pGfxQueue_Retrieval->pop();

		if(pData->GetLoadState() == HYLOADSTATE_ReloadGfx)
		{
			pData->SetLoadState(HYLOADSTATE_Queued);
			m_GfxCommsRef.SendAtlasGroup(pData);
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
		pLoadData = m_Sprite2d.GetOrCreateData(pInst->GetPrefix(), pInst->GetName());
		break;
	case HYINST_Spine2d:
		pLoadData = m_Spine2d.GetOrCreateData(pInst->GetPrefix(), pInst->GetName());
		break;
	case HYINST_Text2d:
		pLoadData = m_Txt2d.GetOrCreateData(pInst->GetPrefix(), pInst->GetName());
		break;
	case HYINST_TexturedQuad2d:
		pLoadData = m_Quad2d.GetOrCreateData(pInst->GetPrefix(), pInst->GetName());
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

// Reload every instance
bool HyAssetManager::Reload()
{
	if(m_bIsReloading)
		return false;

	m_vReloadInsts.clear();
	m_SceneRef.CopyAllInsts(m_vReloadInsts);

	for(uint32 i = 0; i < m_vQueuedInst2d.size(); ++i)
		m_vReloadInsts.push_back(m_vQueuedInst2d[i]);

	for(uint32 i = 0; i < m_vReloadInsts.size(); ++i)
		m_vReloadInsts[i]->Unload();

	m_bIsReloading = true;

	return true;
}

// Reload only the specified instances
bool HyAssetManager::Reload(std::vector<std::string> &vPathsRef)
{
	// TODO: Deep copy 'vPathsRef' vector of strings to 'm_vReloadInsts' so we only reload the specified contents. vPathsRef can be deleted after this function
	if(m_bIsReloading)
		return false;

	m_vReloadInsts.clear();
	m_SceneRef.CopyAllInsts(m_vReloadInsts);
	
	for(uint32 i = 0; i < m_vQueuedInst2d.size(); ++i)
		m_vReloadInsts.push_back(m_vQueuedInst2d[i]);

	for(uint32 i = 0; i < m_vReloadInsts.size(); ++i)
		m_vReloadInsts[i]->Unload();

	m_bIsReloading = true;
	
	return true;
}

// Unload everything, and reinitialize to a new data directory. Doesn't load up anything when done.
bool HyAssetManager::Reload(std::string sNewDataDirPath)
{
	if(m_bIsReloading)
		return false;

	m_sNewDataDirPath = sNewDataDirPath;

	m_vReloadInsts.clear();
	m_SceneRef.CopyAllInsts(m_vReloadInsts);

	for(uint32 i = 0; i < m_vQueuedInst2d.size(); ++i)
		m_vReloadInsts.push_back(m_vQueuedInst2d[i]);

	for(uint32 i = 0; i < m_vReloadInsts.size(); ++i)
		m_vReloadInsts[i]->Unload();

	m_bIsReloading = true;

	return true;
}

eHyReloadCode HyAssetManager::IsReloading()
{
	if(m_bIsReloading == false)
		return HYRELOADCODE_Inactive;

	Update();

	// Does data still exist
	if(DoesAnyDataExist())
		return HYRELOADCODE_InProgress;

	m_AtlasManager.Unload();

	if(m_sNewDataDirPath.empty() == false)
	{

		m_vReloadInsts.clear();
		return HYRELOADCODE_ReInit;
	}

	m_AtlasManager.Load();

	for(uint32 i = 0; i < m_vReloadInsts.size(); ++i)
		m_vReloadInsts[i]->Load();

	m_bIsReloading = false;
	return HYRELOADCODE_Finished;
}

std::string HyAssetManager::GetNewDataDirPath()
{
	return m_sNewDataDirPath;
}

bool HyAssetManager::DoesAnyDataExist()
{
	return (m_Sfx.IsEmpty() == false || m_Sprite2d.IsEmpty() == false || m_Spine2d.IsEmpty() == false || m_Mesh3d.IsEmpty() == false || m_Txt2d.IsEmpty() == false || m_Quad2d.IsEmpty() == false);
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
		m_GfxCommsRef.SendAtlasGroup(static_cast<IHyData2d *>(pData));
	else
		FinalizeData(pData);
}

/*static*/ void HyAssetManager::LoadingThread(void *pParam)
{
	LoadThreadCtrl *pLoadingCtrl = reinterpret_cast<LoadThreadCtrl *>(pParam);
	vector<IHyData *>	vCurLoadData;

	while(true)
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
			vCurLoadData[i]->DoFileLoad();

		// Copy all the (loaded) IData ptrs to the retrieval vector
		pLoadingCtrl->m_csRetrievalQueue.Lock();
		{
			for(uint32 i = 0; i < vCurLoadData.size(); ++i)
				pLoadingCtrl->m_LoadQueueRef_Retrieval.push(vCurLoadData[i]);
		}
		pLoadingCtrl->m_csRetrievalQueue.Unlock();

		vCurLoadData.clear();
	}
}
