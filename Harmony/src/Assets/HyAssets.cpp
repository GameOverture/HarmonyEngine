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
#include "Diagnostics/Console/HyConsole.h"

#define HYASSETS_AtlasDir "Atlases/"

void HyAssetInit(HyAssets *pThis)
{
	HyLog("Assets are initializing...");
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
																							m_pAtlases(nullptr),
																							m_uiNumAtlases(0),
																							m_pLoadedAtlasIndices(nullptr),
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

	jsonxx::Array atlasGrpArray;
	if(atlasGrpArray.parse(sAtlasInfoFileContents))
	{
		// Iterate through each atlas group and determine how many textures total there are between all groups
		m_uiNumAtlases = 0;
		for(uint32 i = 0; i < static_cast<uint32>(atlasGrpArray.size()); ++i)
		{
			jsonxx::Object atlasGrpObj = atlasGrpArray.get<jsonxx::Object>(i);
			jsonxx::Array texturesArray = atlasGrpObj.get<jsonxx::Array>("textures");
			m_uiNumAtlases += static_cast<uint32>(texturesArray.size());
		}
		m_pAtlases = reinterpret_cast<HyAtlas *>(HY_NEW unsigned char[sizeof(HyAtlas) * m_uiNumAtlases]);
		HyAtlas *pAtlasWriteLocation = m_pAtlases;

		// Then iterate back over each atlas group and instantiate a HyAtlas for each texture
		uint32 uiMasterIndex = 0;
		char szTmpBuffer[16];
		for(uint32 i = 0; i < static_cast<uint32>(atlasGrpArray.size()); ++i)
		{
			jsonxx::Object atlasGrpObj = atlasGrpArray.get<jsonxx::Object>(i);
			uint32 uiAtlasGroupId = static_cast<uint32>(atlasGrpObj.get<jsonxx::Number>("atlasGrpId"));

			std::string sRootAtlasFilePath = m_sDATADIR + HYASSETS_AtlasDir;
			std::sprintf(szTmpBuffer, "%05d", uiAtlasGroupId);
			sRootAtlasFilePath += szTmpBuffer;
			sRootAtlasFilePath += "/";

			jsonxx::Array texturesArray = atlasGrpObj.get<jsonxx::Array>("textures");
			for(uint32 j = 0; j < static_cast<uint32>(texturesArray.size()); ++j)
			{
				HyAssert(uiMasterIndex < m_uiNumAtlases, "HyAssets::ParseInitInfo instantiated too many atlases");

				std::string sAtlasFilePath = sRootAtlasFilePath;
				std::sprintf(szTmpBuffer, "%05d", j);
				sAtlasFilePath += szTmpBuffer;

				uint32 uiTextureFormat = static_cast<uint32>(atlasGrpObj.get<jsonxx::Number>("textureType"));

				if(uiTextureFormat == HYTEXTURE_R8G8B8A8 || uiTextureFormat == HYTEXTURE_R8G8B8)
					sAtlasFilePath += ".png";
				else
					sAtlasFilePath += ".dds";

				new (pAtlasWriteLocation)HyAtlas(sAtlasFilePath,
												 uiAtlasGroupId,
												 j,
												 uiMasterIndex,
												 static_cast<int32>(atlasGrpObj.get<jsonxx::Number>("width")),
												 static_cast<int32>(atlasGrpObj.get<jsonxx::Number>("height")),
												 static_cast<HyTextureFormat>(uiTextureFormat),
												 texturesArray.get<jsonxx::Array>(j));

				++pAtlasWriteLocation;
				++uiMasterIndex;
			}
		}
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Set HyAtlasIndices::sm_iIndexFlagsArraySize now that the total number of atlases is known
	HyAtlasIndices::sm_iIndexFlagsArraySize = (m_uiNumAtlases / 32);
	if(m_uiNumAtlases % 32 != 0)
		HyAtlasIndices::sm_iIndexFlagsArraySize++;

	m_pLoadedAtlasIndices = HY_NEW HyAtlasIndices();

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

HyAtlas *HyAssets::GetAtlasUsingGroupId(uint32 uiAtlasGrpId, uint32 uiIndexInGroup)
{
	for(uint32 i = 0; i < m_uiNumAtlases; ++i)
	{
		if(m_pAtlases[i].GetAtlasGroupId() == uiAtlasGrpId && m_pAtlases[i].GetIndexInGroup() == uiIndexInGroup)
			return &m_pAtlases[i];
	}

	return nullptr;
}

uint32 HyAssets::GetNumAtlases()
{
	return m_uiNumAtlases;
}

HyAtlasIndices *HyAssets::GetLoadedAtlases()
{
	return m_pLoadedAtlasIndices;
}

void HyAssets::GetNodeData(IHyLeafDraw2d *pDrawNode2d, IHyNodeData *&pDataOut)
{
	switch(pDrawNode2d->GetType())
	{
	case HYTYPE_Sprite2d:
		pDataOut = m_Sprite2d.GetData(pDrawNode2d->GetPrefix(), pDrawNode2d->GetName());
		break;
	case HYTYPE_Spine2d:
		pDataOut = m_Spine2d.GetData(pDrawNode2d->GetPrefix(), pDrawNode2d->GetName());
		break;
	case HYTYPE_Text2d:
		pDataOut = m_Txt2d.GetData(pDrawNode2d->GetPrefix(), pDrawNode2d->GetName());
		break;
	case HYTYPE_TexturedQuad2d:
		if(pDrawNode2d->GetName() != "raw")
		{
			std::pair<uint32, uint32> key(std::stoi(pDrawNode2d->GetPrefix()), std::stoi(pDrawNode2d->GetName()));
			if(m_Quad2d.find(key) == m_Quad2d.end())
			{
				HyTexturedQuad2dData *pNewQuadData = HY_NEW HyTexturedQuad2dData(key.first, key.second, *this);
				m_Quad2d[key] = pNewQuadData;
			}
			pDataOut = m_Quad2d[key];
		}
		break;
	}
}

void HyAssets::LoadNodeData(IHyLeafDraw2d *pDrawNode2d)
{
	if(pDrawNode2d->m_eLoadState != HYLOADSTATE_Inactive)
		return;

	bool bFullyLoaded = true;

	// Check whether all the required atlases are loaded
	if(pDrawNode2d->AcquireData() != nullptr)
	{
		// TODO: Perhaps make this more efficient by skipping entire 32 bits when those equal '0'
		const HyAtlasIndices &requiredAtlases = pDrawNode2d->UncheckedGetData()->GetRequiredAtlasIndices();
		for(uint32 i = 0; i < m_uiNumAtlases; ++i)
		{
			if(requiredAtlases.IsSet(i))
			{
				HyAtlas *pAtlas = GetAtlas(i);
				QueueData(pAtlas);

				if(pAtlas->GetLoadState() != HYLOADSTATE_Loaded)
					bFullyLoaded = false;
			}
		}
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
	if(bFullyLoaded == false) // Could also use IsNodeLoaded() here
	{
		pDrawNode2d->m_eLoadState = HYLOADSTATE_Queued;
		m_QueuedInst2dList.push_back(pDrawNode2d);
	}
	else
	{
		SetNodeAsLoaded(pDrawNode2d);
	}
}

void HyAssets::RemoveNodeData(IHyLeafDraw2d *pDrawNode2d)
{
	if(pDrawNode2d->m_eLoadState == HYLOADSTATE_Inactive)
		return;

	if(pDrawNode2d->AcquireData() != nullptr)
	{
		// TODO: Perhaps make this more efficient by skipping entire 32 bits when those equal '0'
		const HyAtlasIndices &requiredAtlases = pDrawNode2d->UncheckedGetData()->GetRequiredAtlasIndices();
		for(uint32 i = 0; i < m_uiNumAtlases; ++i)
		{
			if(requiredAtlases.IsSet(i))
			{
				HyAtlas *pAtlas = GetAtlas(i);
				DequeData(pAtlas);
			}
		}
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

bool HyAssets::IsNodeLoaded(IHyLeafDraw2d *pDrawNode2d)
{
	// Atlases check
	if(pDrawNode2d->AcquireData() != nullptr)
	{
		const HyAtlasIndices &requiredAtlases = pDrawNode2d->UncheckedGetData()->GetRequiredAtlasIndices();
		if(m_pLoadedAtlasIndices->IsSet(requiredAtlases) == false)
			return false;
	}

	// Custom shaders check
	for(auto iter = pDrawNode2d->m_RequiredCustomShaders.begin(); iter != pDrawNode2d->m_RequiredCustomShaders.end(); ++iter)
	{
		IHyShader *pShader = IHyRenderer::FindShader(*iter);
		if(pShader->GetLoadState() != HYLOADSTATE_Loaded)
			return false;
	}

	return true;
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
	return m_pLoadedAtlasIndices->IsEmpty();
}

void HyAssets::Update()
{
	// Check to see if we have any pending loads to make
	if(m_Load_Prepare.empty() == false)
	{
		// Copy load queue data into shared data
		if(m_LoadingCtrl.m_csSharedQueue.Lock(1))
		{
			while(m_Load_Prepare.empty() == false)
			{
				m_Load_Shared.push(m_Load_Prepare.front());
				m_Load_Prepare.pop();
			}
		
			m_LoadingCtrl.m_csSharedQueue.Unlock();
		}

		m_LoadingCtrl.m_WaitEvent_HasNewData.Set();
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Check to see if any loaded data (from the load thread) is ready to go to the render thread
	if(m_LoadingCtrl.m_csRetrievalQueue.Lock(1))
	{
		while(m_Load_Retrieval.empty() == false)
		{
			IHyLoadableData *pData = m_Load_Retrieval.front();
			m_Load_Retrieval.pop();

			m_GfxCommsRef.TxData(pData);
		}
	
		m_LoadingCtrl.m_csRetrievalQueue.Unlock();
	}

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

			if(pData->GetType() == HYGFXTYPE_AtlasGroup)
				m_pLoadedAtlasIndices->Clear(static_cast<HyAtlas *>(pData)->GetMasterIndex());

			m_Load_Prepare.push(pData);
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
		}
		else
		{
			pData->m_eLoadState = HYLOADSTATE_Loaded;

			if(pData->GetType() == HYGFXTYPE_AtlasGroup)
			{
				HyLogInfo("Atlas [" << static_cast<HyAtlas *>(pData)->GetMasterIndex() << "] loaded");
				m_pLoadedAtlasIndices->Set(static_cast<HyAtlas *>(pData)->GetMasterIndex());
			}
			else {
				HyLogInfo("Custom Shader Loaded");
			}

			// TODO: Check if there's lots of overhead here checking queued list upon every loaded piece of data that comes through
			for(auto iter = m_QueuedInst2dList.begin(); iter != m_QueuedInst2dList.end(); /*++iter*/) // Increment is handled within loop
			{
				if(IsNodeLoaded(*iter))
				{
					SetNodeAsLoaded(*iter);
					iter = m_QueuedInst2dList.erase(iter);
				}
				else
					++iter;
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

				if(pData->GetType() == HYGFXTYPE_AtlasGroup) {
					HyLogInfo("Atlas [" << static_cast<HyAtlas *>(pData)->GetMasterIndex() << "] reloading");
				}
				else {
					HyLogInfo("Custom Shader reloading");
				}

				m_Load_Prepare.push(pData);

				bFoundInReloadList = true;
				m_ReloadDataList.erase(iter);
				break;
			}
		}

		if(bFoundInReloadList == false)
		{
			pData->m_eLoadState = HYLOADSTATE_Inactive;

			if(pData->GetType() == HYGFXTYPE_AtlasGroup) {
				HyLogInfo("Atlas [" << static_cast<HyAtlas *>(pData)->GetMasterIndex() << "] deleted");
			}
			else {
				HyLogInfo("Custom Shader deleted");
			}
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
