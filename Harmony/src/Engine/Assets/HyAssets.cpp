/**************************************************************************
 *	HyAssets.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2015 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Afx/HyInteropAfx.h"
#include "Assets/HyAssets.h"
#include "Assets/Files/HyAtlas.h"
#include "Assets/Files/HyGLTF.h"
#include "Assets/Files/HyAudioBank.h"
#include "Assets/Nodes/HyEntityData.h"
#include "Assets/Nodes/HyAudioData.h"
#include "Assets/Nodes/HySpine2dData.h"
#include "Assets/Nodes/HySprite2dData.h"
#include "Assets/Nodes/HyText2dData.h"
#include "Assets/Nodes/HyTexturedQuad2dData.h"
#include "Assets/Nodes/HyPrefabData.h"
#include "Audio/HyAudio.h"
#include "Scene/HyScene.h"
#include "Scene/Nodes/Loadables/IHyLoadable.h"
#include "Scene/Nodes/Loadables/Visables/Objects/HyEntity2d.h"
#include "Scene/Nodes/Loadables/Visables/Objects/HyEntity3d.h"
#include "Scene/Nodes/Loadables/Visables/Drawables/IHyDrawable2d.h"
#include "Renderer/IHyRenderer.h"
#include "Utilities/HyMath.h"
#include "Utilities/HyStrManip.h"
#include "Diagnostics/Console/HyConsole.h"

#include <fstream>
#include <iostream>
#include <filesystem>
namespace fs = std::experimental::filesystem;


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
	{
		if(sName.empty() == false)
			HyLogError("Cannot find data for: " << sPrefix << "/" << sName);

		return nullptr;
	}

	return &m_DataList[iter->second];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

HyAssets::HyAssets(HyAudio &audioRef, HyScene &sceneRef, std::string sDataDirPath) :
	IHyThreadClass(),
	m_AudioRef(audioRef),
	m_SceneRef(sceneRef),
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

HyAudio &HyAssets::GetAudioRef()
{
	return m_AudioRef;
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
	case HYTYPE_Entity:
		pDataOut = m_EntityFactory.GetData(pLoadable->GetPrefix(), pLoadable->GetName());
		break;
	case HYTYPE_Sprite:
		pDataOut = m_SpriteFactory.GetData(pLoadable->GetPrefix(), pLoadable->GetName());
		break;
	case HYTYPE_Text:
		pDataOut = m_TextFactory.GetData(pLoadable->GetPrefix(), pLoadable->GetName());
		break;
	case HYTYPE_Prefab:
		pDataOut = m_PrefabFactory.GetData(pLoadable->GetPrefix(), pLoadable->GetName());
		break;
	case HYTYPE_Audio:
		pDataOut = m_AudioFactory.GetData(pLoadable->GetPrefix(), pLoadable->GetName());
		break;
	case HYTYPE_TexturedQuad:
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
	HyAssert(pLoadable->_LoadableGetType() != HYTYPE_Entity, "HyAssets::LoadNodeData passed an entity");
	if(pLoadable->m_eLoadState != HYLOADSTATE_Inactive || pLoadable->IsLoadDataValid() == false)
		return;

	// Check whether all the required data/assets are loaded to confirm we're fully loaded
	bool bFullyLoaded = true;
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

		HyGLTF *pGltf = pLoadable->UncheckedGetData()->GetGltf();
		if(pGltf)
		{
			QueueData(pGltf);

			if(pGltf->GetLoadableState() != HYLOADSTATE_Loaded)
				bFullyLoaded = false;
		}

		HyAudioBank *pAudioBank = pLoadable->UncheckedGetData()->GetAudioBank();
		if(pAudioBank)
		{
			QueueData(pAudioBank);

			if(pAudioBank->GetLoadableState() != HYLOADSTATE_Loaded)
				bFullyLoaded = false;
		}
	}

	// Set the node's 'm_eLoadState' appropriately below to prevent additional Loads
	if(bFullyLoaded)
		SetAsLoaded(pLoadable);
	else
	{
		pLoadable->m_eLoadState = HYLOADSTATE_Queued;
		m_QueuedInstList.push_back(pLoadable);
	}
}

void HyAssets::RemoveNodeData(IHyLoadable *pLoadable)
{
	HyAssert(pLoadable->_LoadableGetType() != HYTYPE_Entity, "HyAssets::RemoveNodeData passed an entity");
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

	SetAsUnloaded(pLoadable);
}

void HyAssets::SetEntityLoaded(IHyLoadable *pLoadableEntity)
{
	if(pLoadableEntity == nullptr)
		return;

	HyAssert(pLoadableEntity->_LoadableGetType() == HYTYPE_Entity, "HyAssets::SetEntityLoaded was passed a node that wasn't an entity");

	if(pLoadableEntity->IsLoaded())
	{
		if(pLoadableEntity->IsChildrenLoaded() == false)
			SetAsUnloaded(pLoadableEntity);
	}
	else
	{
		if(pLoadableEntity->IsChildrenLoaded())
			SetAsLoaded(pLoadableEntity);
	}
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
	std::vector<IHyDrawable2d *> vReloadInsts;
	m_SceneRef.CopyAllLoadedNodes(vReloadInsts);

	//m_QueuedEntityList.clear();

	for(uint32 i = 0; i < m_QueuedInstList.size(); ++i)
		vReloadInsts.push_back(reinterpret_cast<IHyDrawable2d *>(m_QueuedInstList[i]));

	for(uint32 i = 0; i < vReloadInsts.size(); ++i)
		vReloadInsts[i]->Unload();
}

bool HyAssets::IsShutdown()
{
	bool bIsFullyUnloaded = false;
	m_Mutex.lock();
	{
		bIsFullyUnloaded = m_pLoadedAtlasIndices->IsEmpty() && m_Load_Prepare.empty() && m_Load_Shared.empty() && m_Load_Retrieval.empty();
	}
	m_Mutex.unlock();

	if(bIsFullyUnloaded)
	{
		ThreadStop();
		
		if(IsThreadFinished()) // TODO: This is failing and getting stuck
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
		if(m_Mutex.try_lock())
		{
			while(m_Load_Prepare.empty() == false)
			{
				m_Load_Shared.push(m_Load_Prepare.front());
				m_Load_Prepare.pop();
			}
		
			m_Mutex.unlock();

			ThreadContinue(true);
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Check to see if any loaded data (from the load thread) is ready to uploaded to graphics card
	if(m_Mutex.try_lock())
	{
		while(m_Load_Retrieval.empty() == false)
		{
			IHyFileData *pData = m_Load_Retrieval.front();
			m_Load_Retrieval.pop();

			rendererRef.TxData(pData);
		}
	
		m_Mutex.unlock();
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Grab and process any returning data from the renderer
	std::queue<IHyFileData *> &rxDataQueueRef = rendererRef.RxData();
	while(rxDataQueueRef.empty() == false)
	{
		IHyFileData *pData = rxDataQueueRef.front();
		rxDataQueueRef.pop();

		FinalizeData(pData);
	}
}

/*virtual*/ void HyAssets::OnThreadInit() /*override*/
{
	HyLog("Assets are initializing...");

	std::string sAtlasInfoFilePath(m_sDATADIR + HYASSETS_AtlasDir + HYASSETS_AtlasFile);
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
				uint32 uiTextureFiltering = atlasGrpObj.has<jsonxx::Number>("textureFiltering") ? static_cast<uint32>(atlasGrpObj.get<jsonxx::Number>("textureFiltering")) : HYTEXFILTER_BILINEAR;

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
												 static_cast<HyTextureFiltering>(uiTextureFiltering),
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
#ifndef HY_PLATFORM_GUI
	std::string sGameDataFilePath(m_sDATADIR);
	sGameDataFilePath += HYASSETS_DataFile;
	std::string sGameDataFileContents;
	HyReadTextFile(sGameDataFilePath.c_str(), sGameDataFileContents);

	jsonxx::Object gameDataObj;
	bool bGameDataParsed = gameDataObj.parse(sGameDataFileContents);
	HyAssert(bGameDataParsed, "Could not parse game data");

	if(gameDataObj.has<jsonxx::Object>("Audio"))
		m_AudioFactory.Init(gameDataObj.get<jsonxx::Object>("Audio"), *this);
	if(gameDataObj.has<jsonxx::Object>("Prefabs"))
	{
		const jsonxx::Object &prefabObj = gameDataObj.get<jsonxx::Object>("Prefabs");

		for(auto iter = prefabObj.kv_map().begin(); iter != prefabObj.kv_map().end(); ++iter)
			m_GltfMap[iter->first] = HY_NEW HyGLTF(iter->first);

		m_PrefabFactory.Init(prefabObj, *this);
	}
	if(gameDataObj.has<jsonxx::Object>("Texts"))
		m_TextFactory.Init(gameDataObj.get<jsonxx::Object>("Texts"), *this);
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
	std::vector<IHyFileData *>	dataList;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Copy all the ptrs into their vectors to be processed, while emptying the shared queue
	m_Mutex.lock();
	{
		while(m_Load_Shared.empty() == false)
		{
			dataList.push_back(m_Load_Shared.front());
			m_Load_Shared.pop();
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Load everything that is enqueued (outside of any critical section)
	for(uint32 i = 0; i < dataList.size(); ++i)
	{
		dataList[i]->OnLoadThread();

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Copy all the (loaded) IData ptrs to the retrieval vector
		m_Load_Retrieval.push(dataList[i]);
	}
	m_Mutex.unlock();
}

/*virtual*/ void HyAssets::OnThreadShutdown() /*override*/
{
}

void HyAssets::QueueData(IHyFileData *pData)
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

void HyAssets::DequeData(IHyFileData *pData)
{
	HyAssert(pData->m_eLoadState != HYLOADSTATE_Inactive, "Trying to DequeData that is HYLOADSTATE_Inactive");
	HyAssert(pData->m_uiRefCount > 0, "Tried to decrement a '0' reference");

	pData->m_uiRefCount--;
	if(pData->m_uiRefCount == 0)
	{
		if(pData->m_eLoadState == HYLOADSTATE_Loaded)
		{
			pData->m_eLoadState = HYLOADSTATE_Discarded;

			if(pData->GetLoadableType() == HYFILE_Atlas)
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

void HyAssets::FinalizeData(IHyFileData *pData)
{
	// TODO: this for now...
	if(pData->GetLoadableType() == HYFILE_Shader)
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

			if(pData->GetLoadableType() == HYFILE_Atlas)
			{
				HyLogInfo("Atlas [" << static_cast<HyAtlas *>(pData)->GetMasterIndex() << "] loaded");
				m_pLoadedAtlasIndices->Set(static_cast<HyAtlas *>(pData)->GetMasterIndex());
			}
			else {
				HyLogInfo("Custom Shader Loaded");
			}

			// Check queued list upon every loaded piece of data that comes through
			for(auto iter = m_QueuedInstList.begin(); iter != m_QueuedInstList.end(); /*++iter*/) // Increment is handled within loop
			{
				if(IsInstLoaded(*iter))
				{
					SetAsLoaded(*iter);
					iter = m_QueuedInstList.erase(iter);
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

				if(pData->GetLoadableType() == HYFILE_Atlas) {
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

			if(pData->GetLoadableType() == HYFILE_Atlas)
				HyLogInfo("Atlas [" << static_cast<HyAtlas *>(pData)->GetMasterIndex() << "] deleted")
			else
				HyLogInfo("Custom Shader deleted");
		}
	}
}

void HyAssets::SetAsLoaded(IHyLoadable *pLoadable)
{
	pLoadable->m_eLoadState = HYLOADSTATE_Loaded;
	pLoadable->OnLoaded();
	pLoadable->OnLoadedUpdate();

	SetEntityLoaded(pLoadable->_LoadableGetParentPtr());
}

void HyAssets::SetAsUnloaded(IHyLoadable *pLoadable)
{
	if(pLoadable->m_eLoadState == HYLOADSTATE_Queued)
	{
		for(auto it = m_QueuedInstList.begin(); it != m_QueuedInstList.end(); ++it)
		{
			if((*it) == pLoadable)
			{
				m_QueuedInstList.erase(it);
				break;
			}
		}
	}

	pLoadable->m_eLoadState = HYLOADSTATE_Inactive;
	pLoadable->OnUnloaded();

	SetEntityLoaded(pLoadable->_LoadableGetParentPtr());
}
