/**************************************************************************
 *	HyAssets.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2015 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Afx/HyInteropAfx.h"
#include "Assets/HyAssets.h"
#include "Renderer/IHyRenderer.h"
#include "Scene/Nodes/Loadables/IHyLoadable.h"
#include "Assets/Nodes/HyAudioData.h"
#include "Assets/Nodes/HySpine2dData.h"
#include "Assets/Nodes/HySprite2dData.h"
#include "Assets/Nodes/HyText2dData.h"
#include "Assets/Nodes/HyTexturedQuad2dData.h"
#include "Assets/Nodes/HyPrefabData.h"
#include "Utilities/HyMath.h"
#include "Utilities/HyStrManip.h"
#include "Diagnostics/Console/HyConsole.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Nested class Factory
template<typename tData>
void HyAssets::Factory<tData>::Init(const jsonxx::Object &subDirObjRef, HyAssets &assetsRef)
{
	m_DataList.reserve(subDirObjRef.size());

	uint32 i = 0;
	for(auto iter = subDirObjRef.kv_map().begin(); iter != subDirObjRef.kv_map().end(); ++iter, ++i)
	{
		std::string sPath = HyStr::MakeStringProperPath(iter->first.c_str(), nullptr, true);
		m_LookupIndexMap.insert(std::make_pair(sPath, i));

		m_DataList.emplace_back(iter->first, subDirObjRef.get<jsonxx::Value>(iter->first), assetsRef);
	}
}

template<typename tData>
const tData *HyAssets::Factory<tData>::GetData(const std::string &sPrefix, const std::string &sName) const
{
	std::string sPath;

	if(sPrefix.empty() == false)
		sPath += HyStr::MakeStringProperPath(sPrefix.c_str(), "/", true);

	sPath += sName;
	sPath = HyStr::MakeStringProperPath(sPath.c_str(), nullptr, true);

	auto iter = m_LookupIndexMap.find(sPath);
	if(iter == m_LookupIndexMap.end())
		return nullptr;

	return &m_DataList[iter->second];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

HyAssets::HyAssets(std::string sDataDirPath) :	IHyThreadClass(),
												m_sDATADIR(HyStr::MakeStringProperPath(sDataDirPath.c_str(), "/", true)),
												m_bInitialized(false),
												m_pAtlases(nullptr),
												m_uiNumAtlases(0),
												m_pLoadedAtlasIndices(nullptr)
{
	IHyLoadable::sm_pHyAssets = this;
	ThreadStart();
	ThreadWait();
}

HyAssets::~HyAssets()
{
	IHyLoadable::sm_pHyAssets = nullptr;

	HyAssert(IsShutdown(), "Tried to destruct the HyAssets while data still exists");

	for(uint32 i = 0; i < m_uiNumAtlases; ++i)
		m_pAtlases[i].~HyAtlas();
	unsigned char *pAtlases = reinterpret_cast<unsigned char *>(m_pAtlases);
	delete[] pAtlases;
	m_pAtlases = nullptr;

	for(auto iter = m_GltfMap.begin(); iter != m_GltfMap.end(); ++iter)
		delete iter->second;
	m_GltfMap.clear();

	for(auto iter = m_Quad2d.begin(); iter != m_Quad2d.end(); ++iter)
		delete iter->second;

	delete m_pLoadedAtlasIndices;
}

const std::string &HyAssets::GetDataDir()
{
	return m_sDATADIR;
}

bool HyAssets::IsInitialized()
{
	return m_bInitialized;
}

HyAtlas *HyAssets::GetAtlas(uint32 uiMasterIndex)
{
	HyAssert(uiMasterIndex < m_uiNumAtlases, "HyAssets::GetAtlas was given an invalid index: " << uiMasterIndex);
	return &m_pAtlases[uiMasterIndex];
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

HyGLTF *HyAssets::GetGltf(const std::string &sIdentifier)
{
	auto iter = m_GltfMap.find(sIdentifier);
	if(iter != m_GltfMap.end())
		return iter->second;

	return nullptr;
}

void HyAssets::AcquireNodeData(IHyLoadable *pLoadable, const IHyNodeData *&pDataOut)
{
	switch(pLoadable->_LoadableGetType())
	{
	case HYTYPE_Sprite2d:
		pDataOut = m_SpriteFactory.GetData(pLoadable->GetPrefix(), pLoadable->GetName());
		break;
	case HYTYPE_Text2d:
		pDataOut = m_FontFactory.GetData(pLoadable->GetPrefix(), pLoadable->GetName());
		break;
	case HYTYPE_Prefab3d:
		pDataOut = m_PrefabFactory.GetData(pLoadable->GetPrefix(), pLoadable->GetName());
		break;
	case HYTYPE_TexturedQuad2d:
		if(pLoadable->GetName() != "raw")
		{
			std::pair<uint32, uint32> key(std::stoi(pLoadable->GetPrefix()), std::stoi(pLoadable->GetName()));
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

void HyAssets::LoadNodeData(IHyLoadable *pLoadable)
{
	if(pLoadable->m_eLoadState != HYLOADSTATE_Inactive || pLoadable->IsLoadDataValid() == false)
		return;

	bool bFullyLoaded = true;


	// Check whether all the required atlases are loaded
	if(pLoadable->AcquireData() != nullptr)
	{
		const HyAtlasIndices &requiredAtlases = pLoadable->UncheckedGetData()->GetRequiredAtlasIndices();
		if(requiredAtlases.IsEmpty() == false)
		{
			for(uint32 i = 0; i < m_uiNumAtlases; ++i)
			{
				if(requiredAtlases.IsSet(i))
				{
					HyAtlas *pAtlas = GetAtlas(i);
					QueueData(pAtlas);

					if(pAtlas->GetLoadableState() != HYLOADSTATE_Loaded)
						bFullyLoaded = false;
				}
			}
		}

		if(pLoadable->UncheckedGetData()->GetGltf())
		{
			HyGLTF *pGltf = pLoadable->UncheckedGetData()->GetGltf();
			QueueData(pGltf);

			if(pGltf->GetLoadableState() != HYLOADSTATE_Loaded)
				bFullyLoaded = false;
		}
	}

	// Set the node's 'm_eLoadState' appropriately below to prevent additional Loads
	if(bFullyLoaded == false) // Could also use IsInstLoaded() here
	{
		pLoadable->m_eLoadState = HYLOADSTATE_Queued;
		m_QueuedInst2dList.push_back(pLoadable);
	}
	else
	{
		SetInstAsLoaded(pLoadable);
	}
}

void HyAssets::RemoveNodeData(IHyLoadable *pLoadable)
{
	if(pLoadable->m_eLoadState == HYLOADSTATE_Inactive)
		return;

	if(pLoadable->AcquireData() != nullptr)
	{
		const HyAtlasIndices &requiredAtlases = pLoadable->UncheckedGetData()->GetRequiredAtlasIndices();
		for(uint32 i = 0; i < m_uiNumAtlases; ++i)
		{
			if(requiredAtlases.IsSet(i))
			{
				HyAtlas *pAtlas = GetAtlas(i);
				DequeData(pAtlas);
			}
		}
	}

	if(pLoadable->m_eLoadState == HYLOADSTATE_Queued)
	{
		for(auto it = m_QueuedInst2dList.begin(); it != m_QueuedInst2dList.end(); ++it)
		{
			if((*it) == pLoadable)
			{
				m_QueuedInst2dList.erase(it);
				break;
			}
		}
	}

	// Remove from fully loaded list
	for(auto it = m_FullyLoadedList.begin(); it != m_FullyLoadedList.end(); ++it)
	{
		if((*it) == pLoadable)
		{
			// TODO: Log about erasing instance
			m_FullyLoadedList.erase(it);
			break;
		}
	}

	pLoadable->m_eLoadState = HYLOADSTATE_Inactive;
	pLoadable->OnUnloaded();
}

bool HyAssets::IsInstLoaded(IHyLoadable *pLoadable)
{
	if(pLoadable->AcquireData() != nullptr)
	{
		// Atlases check
		const HyAtlasIndices &requiredAtlases = pLoadable->UncheckedGetData()->GetRequiredAtlasIndices();
		if(m_pLoadedAtlasIndices->IsSet(requiredAtlases) == false)
			return false;

		// glTF check
		HyGLTF *pGltf = pLoadable->UncheckedGetData()->GetGltf();
		if(pGltf && pGltf->GetLoadableState() != HYLOADSTATE_Loaded)
			return false;
	}

	return true;
}

// Unload everything
void HyAssets::Shutdown()
{
	std::vector<IHyLoadable *> vReloadInsts;
	vReloadInsts = m_FullyLoadedList;

	for(uint32 i = 0; i < m_QueuedInst2dList.size(); ++i)
		vReloadInsts.push_back(m_QueuedInst2dList[i]);

	for(uint32 i = 0; i < vReloadInsts.size(); ++i)
		vReloadInsts[i]->Unload();
}

bool HyAssets::IsShutdown()
{
	if(m_pLoadedAtlasIndices->IsEmpty())
	{
		ThreadStop();
		
		if(IsThreadFinished())
			return true;
	}

	return false;
}

void HyAssets::Update(IHyRenderer &rendererRef)
{
	// Check to see if we have any pending loads to make
	if(m_Load_Prepare.empty() == false)
	{
		// Copy load queue data into shared data
		if(m_Mutex_SharedQueue.try_lock())
		{
			while(m_Load_Prepare.empty() == false)
			{
				m_Load_Shared.push(m_Load_Prepare.front());
				m_Load_Prepare.pop();
			}
		
			m_Mutex_SharedQueue.unlock();

			ThreadContinue(true);
		}
	}


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Check to see if any loaded data (from the load thread) is ready to uploaded to graphics card
	if(m_Mutex_RetrievalQueue.try_lock())
	{
		while(m_Load_Retrieval.empty() == false)
		{
			IHyLoadableData *pData = m_Load_Retrieval.front();
			m_Load_Retrieval.pop();

			rendererRef.TxData(pData);
		}
	
		m_Mutex_RetrievalQueue.unlock();
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Grab and process any returning data from the renderer
	std::queue<IHyLoadableData *> &rxDataQueueRef = rendererRef.RxData();
	while(rxDataQueueRef.empty() == false)
	{
		IHyLoadableData *pData = rxDataQueueRef.front();
		rxDataQueueRef.pop();

		FinalizeData(pData);
	}
}

/*virtual*/ void HyAssets::OnThreadInit() /*override*/
{
	HyLog("Assets are initializing...");

	std::string sAtlasInfoFilePath(m_sDATADIR + HYASSETS_AtlasDir);
	sAtlasInfoFilePath += "atlas.json";
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
			sprintf_s(szTmpBuffer, sizeof(szTmpBuffer), "%05d", uiAtlasGroupId);
			sRootAtlasFilePath += szTmpBuffer;
			sRootAtlasFilePath += "/";

			jsonxx::Array texturesArray = atlasGrpObj.get<jsonxx::Array>("textures");
			for(uint32 j = 0; j < static_cast<uint32>(texturesArray.size()); ++j)
			{
				HyAssert(uiMasterIndex < m_uiNumAtlases, "HyAssets::OnThreadInit instantiated too many atlases");

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

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	std::string sGameDataFilePath(m_sDATADIR);
	sGameDataFilePath += HYASSETS_DataFile;
	std::string sGameDataFileContents;
	HyReadTextFile(sGameDataFilePath.c_str(), sGameDataFileContents);

	jsonxx::Object gameDataObj;
	bool bGameDataParsed = gameDataObj.parse(sGameDataFileContents);
	HyAssert(bGameDataParsed, "Could not parse game data");

	if(gameDataObj.has<jsonxx::Object>("Prefabs"))
	{
		const jsonxx::Object &prefabObj = gameDataObj.get<jsonxx::Object>("Prefabs");

		for(auto iter = prefabObj.kv_map().begin(); iter != prefabObj.kv_map().end(); ++iter)
			m_GltfMap[iter->first] = HY_NEW HyGLTF(iter->first);

		m_PrefabFactory.Init(prefabObj, *this);
	}

#ifndef HY_PLATFORM_GUI
	if(gameDataObj.has<jsonxx::Object>("Fonts"))
		m_FontFactory.Init(gameDataObj.get<jsonxx::Object>("Fonts"), *this);
	if(gameDataObj.has<jsonxx::Object>("Sprites"))
		m_SpriteFactory.Init(gameDataObj.get<jsonxx::Object>("Sprites"), *this);
	if(gameDataObj.has<jsonxx::Object>("Prefabs"))
		m_PrefabFactory.Init(gameDataObj.get<jsonxx::Object>("Prefabs"), *this);
#endif

	// Atomic boolean indicated to main thread that we're initialized
	m_bInitialized = true;
}

/*virtual*/ void HyAssets::OnThreadUpdate() /*override*/
{
	std::vector<IHyLoadableData *>	dataList;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Copy all the ptrs into their vectors to be processed, while emptying the shared queue
	m_Mutex_SharedQueue.lock();
	{
		while(m_Load_Shared.empty() == false)
		{
			dataList.push_back(m_Load_Shared.front());
			m_Load_Shared.pop();
		}
	}
	m_Mutex_SharedQueue.unlock();

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Load everything that is enqueued (outside of any critical section)
	for(uint32 i = 0; i < dataList.size(); ++i)
	{
		dataList[i]->OnLoadThread();

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Copy all the (loaded) IData ptrs to the retrieval vector
		m_Mutex_RetrievalQueue.lock();
		m_Load_Retrieval.push(dataList[i]);
		m_Mutex_RetrievalQueue.unlock();
	}
}

/*virtual*/ void HyAssets::OnThreadShutdown() /*override*/
{
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

			if(pData->GetLoadableType() == HYLOADABLE_Atlas)
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
	// TODO: this for now...
	if(pData->GetLoadableType() == HYLOADABLE_Shader)
		return;

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

			if(pData->GetLoadableType() == HYLOADABLE_Atlas)
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
				if(IsInstLoaded(*iter))
				{
					SetInstAsLoaded(*iter);
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

				if(pData->GetLoadableType() == HYLOADABLE_Atlas) {
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

			if(pData->GetLoadableType() == HYLOADABLE_Atlas) {
				HyLogInfo("Atlas [" << static_cast<HyAtlas *>(pData)->GetMasterIndex() << "] deleted");
			}
			else {
				HyLogInfo("Custom Shader deleted");
			}
		}
	}
}

void HyAssets::SetInstAsLoaded(IHyLoadable *pLoadable)
{
	pLoadable->m_eLoadState = HYLOADSTATE_Loaded;
	pLoadable->OnLoaded();
	pLoadable->DrawLoadedUpdate();

	m_FullyLoadedList.push_back(pLoadable);
}
