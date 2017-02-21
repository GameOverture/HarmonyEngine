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

#include "Assets/Nodes/HyAudioData.h"
#include "Assets/Nodes/HySpine2dData.h"
#include "Assets/Nodes/HySprite2dData.h"
#include "Assets/Nodes/HyText2dData.h"
#include "Assets/Nodes/HyTexturedQuad2dData.h"
#include "Assets/Nodes/HyPrimitive2dData.h"
#include "Assets/Nodes/HyMesh3dData.h"

#include "Utilities/HyMath.h"
#include "Utilities/HyStrManip.h"

#include "Diagnostics/HyGuiComms.h"

#define HYASSETS_AtlasDir "Atlases/"

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
	HyAssert(iter != m_LookupIndexMap.end(), "Could not find data: " << sPath.c_str());

	return &m_DataList[iter->second];
}

HyAssets::HyAssets(std::string sDataDirPath, HyGfxComms &gfxCommsRef, HyScene &sceneRef) :	m_sDATADIR(MakeStringProperPath(sDataDirPath.c_str(), "/", true)),
																							m_GfxCommsRef(gfxCommsRef),
																							m_SceneRef(sceneRef),
																							m_uiNumAtlasGroups(0),
																							m_pAtlasGroups(nullptr),
																							m_pLastQueuedData(nullptr),
																							m_pLastDiscardedData(nullptr),
																							m_LoadingCtrl(m_Load_Shared, m_Load_Retrieval)
{
	IHyDraw2d::sm_pHyAssets = this;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	std::string sAtlasInfoFilePath(m_sDATADIR + HYASSETS_AtlasDir);
	sAtlasInfoFilePath += "atlasInfo.json";
	std::string sAtlasInfoFileContents;
	HyReadTextFile(sAtlasInfoFilePath.c_str(), sAtlasInfoFileContents);

	jsonxx::Array atlasGroupArray;
	if(atlasGroupArray.parse(sAtlasInfoFileContents) == false || atlasGroupArray.size() == 0)
	{
		m_uiNumAtlasGroups = 0;
		m_pAtlasGroups = NULL;
		return;
	}

	m_uiNumAtlasGroups = static_cast<uint32>(atlasGroupArray.size());
	m_pAtlasGroups = reinterpret_cast<HyAtlasGroup *>(HY_NEW unsigned char[sizeof(HyAtlasGroup) * m_uiNumAtlasGroups]);
	HyAtlasGroup *pAtlasGroupWriteLocation = m_pAtlasGroups;

	for(uint32 i = 0; i < m_uiNumAtlasGroups; ++i, ++pAtlasGroupWriteLocation)
	{
		jsonxx::Object atlasGroupObj = atlasGroupArray.get<jsonxx::Object>(i);

		new (pAtlasGroupWriteLocation)HyAtlasGroup(*this,
												   static_cast<uint32>(atlasGroupObj.get<jsonxx::Number>("id")),
												   static_cast<int32>(atlasGroupObj.get<jsonxx::Number>("width")),
												   static_cast<int32>(atlasGroupObj.get<jsonxx::Number>("height")),
												   static_cast<int32>(atlasGroupObj.get<jsonxx::Number>("num8BitClrChannels")),
												   atlasGroupObj.get<jsonxx::Array>("textures"));
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

	
	// Start up Loading thread
	m_pLoadingThread = ThreadManager::Get()->BeginThread(_T("Loading Thread"), THREAD_START_PROCEDURE(LoadingThread), &m_LoadingCtrl);
}

HyAssets::~HyAssets()
{
	if(m_pAtlasGroups == NULL || m_uiNumAtlasGroups == 0)
		return;

	for(uint32 i = 0; i < m_uiNumAtlasGroups; ++i)
		m_pAtlasGroups[i].~HyAtlasGroup();

	unsigned char *pAtlasGrps = reinterpret_cast<unsigned char *>(m_pAtlasGroups);
	delete[] pAtlasGrps;
	m_pAtlasGroups = NULL;

	for(auto iter = m_Quad2d.begin(); iter != m_Quad2d.end(); ++iter)
		delete iter->second;

	HyAssert(IsShutdown(), "Tried to destruct the HyAssets while data still exists");
}

HyAtlasGroup *HyAssets::GetAtlasGroup(uint32 uiAtlasGroupId)
{
	for(uint32 i = 0; i < m_uiNumAtlasGroups; ++i)
	{
		if(m_pAtlasGroups[i].GetId() == uiAtlasGroupId)
			return &m_pAtlasGroups[i];
	}

	HyError("HyAtlasContainer::GetAtlasGroup() could not find the atlas group ID: " << uiAtlasGroupId);
	return &m_pAtlasGroups[0];
}

std::string HyAssets::GetTexturePath(uint32 uiAtlasGroupId, uint32 uiTextureIndex)
{
	std::string sTexturePath(m_sDATADIR + HYASSETS_AtlasDir);

	char szTmpBuffer[16];
	std::sprintf(szTmpBuffer, "%05d/", uiAtlasGroupId);
	sTexturePath += szTmpBuffer;

	std::sprintf(szTmpBuffer, "%05d", uiTextureIndex);
	sTexturePath += szTmpBuffer;

	sTexturePath += ".png";

	return sTexturePath;
}

void HyAssets::GetNodeData(IHyDraw2d *pDrawNode, IHyNodeData *&pDataOut)
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
		if(m_Quad2d.find(std::stoi(pDrawNode->GetName())) == m_Quad2d.end())
		{
			HyTexturedQuad2dData *pNewQuadData = HY_NEW HyTexturedQuad2dData(pDrawNode->GetName(), *this);
			m_Quad2d[std::stoi(pDrawNode->GetName())] = pNewQuadData;
		}
		pDataOut = m_Quad2d[std::stoi(pDrawNode->GetName())];
		break;
	}
}

void HyAssets::LoadGfxData(IHyDraw2d *pDrawNode2d)
{
	if(pDrawNode2d->m_eLoadState != HYLOADSTATE_Inactive)
		return;

	bool bFullyLoaded = true;

	// Check whether all the required atlases are loaded
	for(auto iter = pDrawNode2d->m_RequiredAtlasIds.begin(); iter != pDrawNode2d->m_RequiredAtlasIds.end(); ++iter)
	{
		HyAtlasGroup *pAtlasGrp = GetAtlasGroup(*iter);
		QueueData(pAtlasGrp);

		if(pAtlasGrp->GetLoadState() != HYLOADSTATE_Loaded)
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

	// Set the instance
	if(bFullyLoaded == false)
	{
		pDrawNode2d->m_eLoadState = HYLOADSTATE_Queued;
		m_QueuedInst2dList.push_back(pDrawNode2d);
	}
	else
	{
		m_SceneRef.AddInstance(pDrawNode2d);
		pDrawNode2d->m_eLoadState = HYLOADSTATE_Loaded;
	}
}

void HyAssets::RemoveGfxData(IHyDraw2d *pDrawNode2d)
{
	if(pDrawNode2d->m_eLoadState == HYLOADSTATE_Inactive)
		return;

	for(auto iter = pDrawNode2d->m_RequiredAtlasIds.begin(); iter != pDrawNode2d->m_RequiredAtlasIds.end(); ++iter)
	{
		HyAtlasGroup *pAtlasGrp = GetAtlasGroup(*iter);
		DequeData(pAtlasGrp);
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

	m_SceneRef.RemoveInst(pDrawNode2d);
	pDrawNode2d->m_eLoadState = HYLOADSTATE_Inactive;
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

	//m_LoadingCtrl.m_eState = LoadThreadCtrl::STATE_ShouldExit;
	//m_LoadingCtrl.m_WaitEvent_HasNewData.Set();
}

bool HyAssets::IsShutdown()
{
	return m_pLastDiscardedData == nullptr;// && m_LoadingCtrl.m_eState != LoadThreadCtrl::STATE_Run;
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
				{
					m_SceneRef.AddInstance(*iter);
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

		if(pData == m_pLastDiscardedData)
			m_pLastDiscardedData = nullptr;
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
