/**************************************************************************
 *	HyAssets.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2015 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Afx/HyInteropAfx.h"
#include "Assets/HyAssets.h"
#include "Scene/Nodes/Leafs/IHyLeafDraw2d.h"
#include "Assets/Nodes/HyAudioData.h"
#include "Assets/Nodes/HySpine2dData.h"
#include "Assets/Nodes/HySprite2dData.h"
#include "Assets/Nodes/HyText2dData.h"
#include "Assets/Nodes/HyTexturedQuad2dData.h"
#include "Assets/Nodes/HyPrimitive2dData.h"
#include "Assets/Nodes/HyMesh3dData.h"
#include "Utilities/HyMath.h"
#include "Utilities/HyStrManip.h"
#include "Diagnostics/HyConsole.h"
#include "Profiler/HyProfileManager.h"

#define HYASSETS_AtlasDir "Atlases/"

void HyAssetInit(HyAssets *pThis)
{
	pThis->ParseInitInfo();
}

template<typename tData>
void HyAssets::NodeData<tData>::Init(jsonxx::Object &subDirObjRef, HyAssets &assetsRef)
{
	m_DataList.reserve(subDirObjRef.size());

	uint32 i = 0;
	for(auto iter = subDirObjRef.kv_map().begin(); iter != subDirObjRef.kv_map().end(); ++iter, ++i)
	{
		std::string sPath = MakeStringProperPath(iter->first.c_str(), nullptr, true);
		m_LookupIndexMap.insert(std::make_pair(sPath, i));

		m_DataList.emplace_back(iter->first, subDirObjRef.get<jsonxx::Value>(iter->first), assetsRef);
	}
}

template<typename tData>
tData *HyAssets::NodeData<tData>::GetData(const std::string &sPrefix, const std::string &sName)
{
	std::string sPath;

	if(sPrefix.empty() == false)
		sPath += MakeStringProperPath(sPrefix.c_str(), "/", true);

	sPath += sName;
	sPath = MakeStringProperPath(sPath.c_str(), nullptr, true);

	auto iter = m_LookupIndexMap.find(sPath);
	if(iter == m_LookupIndexMap.end())
		return nullptr;

	return &m_DataList[iter->second];
}

HyAssets::HyAssets(std::string sDataDirPath, HyGfxComms &gfxCommsRef, HyScene &sceneRef) :	m_sDATADIR(MakeStringProperPath(sDataDirPath.c_str(), "/", true)),
																							m_GfxCommsRef(gfxCommsRef),
																							m_SceneRef(sceneRef),
																							m_uiNumAtlases(0),
																							m_pAtlases(nullptr),
																							m_pLastQueuedData(nullptr),
																							m_pLastDiscardedData(nullptr),
																							m_LoadingCtrl(m_Load_Shared, m_Load_Retrieval)
{
	IHyLeafDraw2d::sm_pHyAssets = this;

	m_InitFuture = std::async(std::launch::async, &HyAssetInit, this);
}

HyAssets::~HyAssets()
{
	for(uint32 i = 0; i < m_uiNumAtlases; ++i)
		m_pAtlases[i].~HyAtlas();

	unsigned char *pAtlases = reinterpret_cast<unsigned char *>(m_pAtlases);
	delete[] pAtlases;
	m_pAtlases = nullptr;

	for(auto iter = m_Quad2d.begin(); iter != m_Quad2d.end(); ++iter)
		delete iter->second;

	HyAssert(IsShutdown(), "Tried to destruct the HyAssets while data still exists");
}

bool HyAssets::IsLoaded()
{
	auto status = m_InitFuture.wait_for(std::chrono::milliseconds(0));
	return status == std::future_status::ready;
}

void HyAssets::ParseInitInfo()
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	std::string sAtlasInfoFilePath(m_sDATADIR + HYASSETS_AtlasDir);
	sAtlasInfoFilePath += "atlasInfo.json";
	std::string sAtlasInfoFileContents;
	HyReadTextFile(sAtlasInfoFilePath.c_str(), sAtlasInfoFileContents);

	jsonxx::Object atlasObject;
	if(atlasObject.parse(sAtlasInfoFileContents))
	{
		jsonxx::Array texturesArray = atlasObject.get<jsonxx::Array>("textures");

		m_uiNumAtlases = static_cast<uint32>(texturesArray.size());
		m_pAtlases = reinterpret_cast<HyAtlas *>(HY_NEW unsigned char[sizeof(HyAtlas) * m_uiNumAtlases]);
		HyAtlas *pAtlasWriteLocation = m_pAtlases;

		for(uint32 i = 0; i < m_uiNumAtlases; ++i, ++pAtlasWriteLocation)
		{
			std::string sAtlasFilePath = m_sDATADIR + HYASSETS_AtlasDir;
			char szTmpBuffer[16];
			std::sprintf(szTmpBuffer, "%05d", i);
			sAtlasFilePath += szTmpBuffer;
			sAtlasFilePath += ".png";

			new (pAtlasWriteLocation)HyAtlas(sAtlasFilePath,
											 i,
											 static_cast<int32>(atlasObject.get<jsonxx::Number>("width")),
											 static_cast<int32>(atlasObject.get<jsonxx::Number>("height")),
											 static_cast<int32>(atlasObject.get<jsonxx::Number>("num8BitClrChannels")),
											 texturesArray.get<jsonxx::Array>(i));
		}
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef HY_PLATFORM_GUI
	std::string sGameDataFilePath(m_sDATADIR);
	sGameDataFilePath += "Data.json";

	std::string sGameDataFileContents;
	HyReadTextFile(sGameDataFilePath.c_str(), sGameDataFileContents);
	
	jsonxx::Object gameDataObj;
	bool bGameDataParsed = gameDataObj.parse(sGameDataFileContents);
	HyAssert(bGameDataParsed, "Could not parse game data");

	m_Audio.Init(gameDataObj.get<jsonxx::Object>("Audio"), *this);
	m_Txt2d.Init(gameDataObj.get<jsonxx::Object>("Fonts"), *this);
	m_Sprite2d.Init(gameDataObj.get<jsonxx::Object>("Sprites"), *this);
	//jsonxx::Object &entitiesDataObjRef = gameDataObj.get<jsonxx::Object>("Entities");
	//jsonxx::Object &particlesDataObjRef = gameDataObj.get<jsonxx::Object>("Particles");
	//jsonxx::Object &shadersDataObjRef = gameDataObj.get<jsonxx::Object>("Shaders");
	//jsonxx::Object &spineDataObjRef = gameDataObj.get<jsonxx::Object>("Spine");
#endif

	
	// Start up Loading thread
	m_pLoadingThread = ThreadManager::Get()->BeginThread(_T("Loading Thread"), THREAD_START_PROCEDURE(LoadingThread), &m_LoadingCtrl);
}

HyAtlas *HyAssets::GetAtlas(uint32 uiIndex)
{
	HyAssert(uiIndex < m_uiNumAtlases, "HyAssets::GetAtlas was given an invalid index: " << uiIndex);
	return &m_pAtlases[uiIndex];
}

HyAtlas *HyAssets::GetAtlas(uint32 uiChecksum, HyRectangle<float> &UVRectOut)
{
	for(uint32 i = 0; i < m_uiNumAtlases; ++i)
	{
		if(m_pAtlases[i].GetUvRect(uiChecksum, UVRectOut))
			return &m_pAtlases[i];
	}

	return nullptr;
}

void HyAssets::GetNodeData(IHyLeafDraw2d *pDrawNode, IHyNodeData *&pDataOut)
{
	switch(pDrawNode->GetType())
	{
	case HYTYPE_Sprite2d:
		pDataOut = m_Sprite2d.GetData(pDrawNode->GetPrefix(), pDrawNode->GetName());
		break;
	case HYTYPE_Spine2d:
		pDataOut = m_Spine2d.GetData(pDrawNode->GetPrefix(), pDrawNode->GetName());
		break;
	case HYTYPE_Text2d:
		pDataOut = m_Txt2d.GetData(pDrawNode->GetPrefix(), pDrawNode->GetName());
		break;
	case HYTYPE_TexturedQuad2d:
		if(pDrawNode->GetName() != "raw")
		{
			if(m_Quad2d.find(std::stoi(pDrawNode->GetName())) == m_Quad2d.end())
			{
				HyTexturedQuad2dData *pNewQuadData = HY_NEW HyTexturedQuad2dData(pDrawNode->GetName(), *this);
				m_Quad2d[std::stoi(pDrawNode->GetName())] = pNewQuadData;
			}
			pDataOut = m_Quad2d[std::stoi(pDrawNode->GetName())];
		}
		break;
	}
}

void HyAssets::LoadGfxData(IHyLeafDraw2d *pDrawNode2d)
{
	if(pDrawNode2d->m_eLoadState != HYLOADSTATE_Inactive)
		return;

	bool bFullyLoaded = true;

	// Check whether all the required atlases are loaded
	for(auto iter = pDrawNode2d->m_RequiredAtlasIndices.begin(); iter != pDrawNode2d->m_RequiredAtlasIndices.end(); ++iter)
	{
		HyAtlas *pAtlas = GetAtlas(*iter);
		QueueData(pAtlas);

		if(pAtlas->GetLoadState() != HYLOADSTATE_Loaded)
			bFullyLoaded = false;
	}

	// Check whether we need to load custom shaders
	for(auto iter = pDrawNode2d->m_RequiredCustomShaders.begin(); iter != pDrawNode2d->m_RequiredCustomShaders.end(); ++iter)
	{
		IHyShader *pShader = IHyRenderer::FindShader(*iter);
		QueueData(pShader);

		if(pShader->GetLoadState() != HYLOADSTATE_Loaded)
			bFullyLoaded = false;
	}

	// Set the node's 'm_eLoadState' appropriately below to prevent additional Loads
	if(bFullyLoaded == false)
	{
		pDrawNode2d->m_eLoadState = HYLOADSTATE_Queued;
		m_QueuedInst2dList.push_back(pDrawNode2d);
	}
	else
	{
		SetNodeAsLoaded(pDrawNode2d);
	}
}

void HyAssets::RemoveGfxData(IHyLeafDraw2d *pDrawNode2d)
{
	if(pDrawNode2d->m_eLoadState == HYLOADSTATE_Inactive)
		return;

	for(auto iter = pDrawNode2d->m_RequiredAtlasIndices.begin(); iter != pDrawNode2d->m_RequiredAtlasIndices.end(); ++iter)
	{
		HyAtlas *pAtlas = GetAtlas(*iter);
		DequeData(pAtlas);
	}

	for(auto iter = pDrawNode2d->m_RequiredCustomShaders.begin(); iter != pDrawNode2d->m_RequiredCustomShaders.end(); ++iter)
	{
		IHyShader *pShader = IHyRenderer::FindShader(*iter);
		DequeData(pShader);
	}

	if(pDrawNode2d->m_eLoadState == HYLOADSTATE_Queued)
	{
		for(auto it = m_QueuedInst2dList.begin(); it != m_QueuedInst2dList.end(); ++it)
		{
			if((*it) == pDrawNode2d)
			{
				m_QueuedInst2dList.erase(it);
				break;
			}
		}
	}

	m_SceneRef.RemoveNode_Loaded(pDrawNode2d);
	pDrawNode2d->m_eLoadState = HYLOADSTATE_Inactive;
}

// Unload everything
void HyAssets::Shutdown()
{
	std::vector<IHyLeafDraw2d *> vReloadInsts;
	m_SceneRef.CopyAllLoadedNodes(vReloadInsts);

	for(uint32 i = 0; i < m_QueuedInst2dList.size(); ++i)
		vReloadInsts.push_back(m_QueuedInst2dList[i]);

	for(uint32 i = 0; i < vReloadInsts.size(); ++i)
		vReloadInsts[i]->Unload();

	//m_LoadingCtrl.m_eState = LoadThreadCtrl::STATE_ShouldExit;
	//m_LoadingCtrl.m_WaitEvent_HasNewData.Set();
}

bool HyAssets::IsShutdown()
{
	return m_pLastDiscardedData == nullptr;// && m_LoadingCtrl.m_eState != LoadThreadCtrl::STATE_Run;
}

void HyAssets::Update()
{
//	PROFILE("HyAssets::Update()");

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

void HyAssets::QueueData(IHyLoadableData *pData)
{
	if(pData->m_uiRefCount == 0)
	{
		if(pData->m_eLoadState == HYLOADSTATE_Inactive)
		{
			pData->m_eLoadState = HYLOADSTATE_Queued;
			m_Load_Prepare.push(pData);

			m_pLastQueuedData = pData;
		}
		else if(pData->m_eLoadState == HYLOADSTATE_Discarded)
			m_ReloadDataList.push_back(pData);
	}

	pData->m_uiRefCount++;
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

			m_pLastDiscardedData = pData;
		}

		for(auto iter = m_ReloadDataList.begin(); iter != m_ReloadDataList.end(); ++iter)
		{
			if((*iter) == pData)
			{
				m_ReloadDataList.erase(iter);
				break;
			}
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

			m_pLastDiscardedData = pData;
		}
		else
		{
			pData->m_eLoadState = HYLOADSTATE_Loaded;

			if(pData == m_pLastQueuedData)
			{
				for(auto iter = m_QueuedInst2dList.begin(); iter != m_QueuedInst2dList.end(); ++iter)
					SetNodeAsLoaded(*iter);

				m_QueuedInst2dList.clear();
				m_pLastQueuedData = nullptr;
			}
		}
	}
	else // HYLOADSTATE_Discarded
	{
		bool bFoundInReloadList = false;
		for(auto iter = m_ReloadDataList.begin(); iter != m_ReloadDataList.end(); ++iter)
		{
			if((*iter) == pData)
			{
				pData->m_eLoadState = HYLOADSTATE_Queued;
				m_Load_Prepare.push(pData);
				m_pLastQueuedData = pData;

				bFoundInReloadList = true;
				m_ReloadDataList.erase(iter);
				break;
			}
		}

		if(bFoundInReloadList == false)
		{
			pData->m_eLoadState = HYLOADSTATE_Inactive;
			HyLogInfo("Deleted loadable data");

			if(pData == m_pLastDiscardedData)
				m_pLastDiscardedData = nullptr;
		}
	}
}

void HyAssets::SetNodeAsLoaded(IHyLeafDraw2d *pDrawNode2d)
{
	m_SceneRef.AddNode_Loaded(pDrawNode2d);
	pDrawNode2d->m_eLoadState = HYLOADSTATE_Loaded;
	pDrawNode2d->OnLoaded();
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
		{
			dataList[i]->OnLoadThread();

			////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			// Copy all the (loaded) IData ptrs to the retrieval vector
			pLoadingCtrl->m_csRetrievalQueue.Lock();
			{
				//for(uint32 i = 0; i < dataList.size(); ++i)
					pLoadingCtrl->m_Load_RetrievalRef.push(dataList[i]);
			}
			pLoadingCtrl->m_csRetrievalQueue.Unlock();
		}

		dataList.clear();
	}

	pLoadingCtrl->m_eState = LoadThreadCtrl::STATE_HasExited;
}
