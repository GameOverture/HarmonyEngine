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
#include "Assets/Files/HyFileAtlas.h"
#include "Assets/Files/HyGLTF.h"
#include "Assets/Files/HyFileAudio.h"
#include "Assets/Nodes/HyEntityData.h"
#include "Assets/Nodes/HyAudioData.h"
#include "Assets/Nodes/HySpine2dData.h"
#include "Assets/Nodes/HySprite2dData.h"
#include "Assets/Nodes/HyText2dData.h"
#include "Assets/Nodes/HyTexturedQuad2dData.h"
#include "Assets/Nodes/HyPrefabData.h"
#include "Audio/HyAudioHarness.h"
#include "Scene/HyScene.h"
#include "Scene/Nodes/Loadables/IHyLoadable.h"
#include "Scene/Nodes/Loadables/Drawables/Objects/HyEntity2d.h"
#include "Scene/Nodes/Loadables/Drawables/Objects/HyEntity3d.h"
#include "Scene/Nodes/Loadables/Drawables/Instances/IHyInstance2d.h"
#include "Renderer/IHyRenderer.h"
#include "Utilities/HyIO.h"
#include "Utilities/HyMath.h"
#include "Diagnostics/Console/HyConsole.h"

#include <fstream>
#include <iostream>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Nested class Factory
template<typename tData>
void HyAssets::Factory<tData>::Init(const jsonxx::Object &subDirObjRef, HyAssets &assetsRef)
{
	m_DataList.reserve(subDirObjRef.size());

	uint32 i = 0;
	for(auto iter = subDirObjRef.kv_map().begin(); iter != subDirObjRef.kv_map().end(); ++iter, ++i)
	{
		std::string sPath = HyIO::CleanPath(iter->first.c_str(), nullptr, true);
		m_LookupIndexMap.insert(std::make_pair(sPath, i));

		m_DataList.emplace_back(iter->first, subDirObjRef.get<jsonxx::Object>(iter->first), assetsRef);
	}
}

template<typename tData>
const tData *HyAssets::Factory<tData>::GetData(const std::string &sPrefix, const std::string &sName) const
{
	std::string sPath;

	if(sPrefix.empty() == false)
		sPath += HyIO::CleanPath(sPrefix.c_str(), "/", true);

	sPath += sName;
	sPath = HyIO::CleanPath(sPath.c_str(), nullptr, true);

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

HyAssets::HyAssets(HyAudioHarness &audioRef, HyScene &sceneRef, std::string sDataDirPath) :
	IHyThreadClass(HYTHREAD_Lowest),
	m_AudioRef(audioRef),
	m_SceneRef(sceneRef),
	m_sDATADIR(HyIO::CleanPath(sDataDirPath.c_str(), "/", true)),
	m_bInitialized(false)
{
	IHyLoadable::sm_pHyAssets = this;
	ThreadStart();
	ThreadWait();
}

HyAssets::~HyAssets()
{
	IHyLoadable::sm_pHyAssets = nullptr;

	HyAssert(IsShutdown(), "Tried to destruct the HyAssets while data still exists");

	for(uint32 i = 0; i < HYNUM_FILETYPES; ++i)
	{
		switch(i)
		{
		case HYFILE_Atlas:
			for(uint32 j = 0; j < m_FilesMap[i].m_uiNumFiles; ++j)
				static_cast<HyFileAtlas *>(m_FilesMap[i].m_pFiles)[j].~HyFileAtlas();
			break;
	
		case HYFILE_AudioBank:
			for(uint32 j = 0; j < m_FilesMap[i].m_uiNumFiles; ++j)
				static_cast<HyFileAudio *>(m_FilesMap[i].m_pFiles)[j].~HyFileAudio();
			break;
		}

		unsigned char *pFileBank = reinterpret_cast<unsigned char *>(m_FilesMap[i].m_pFiles);
		delete[] pFileBank;
		delete m_FilesMap[i].m_pLoadedManifest;
	}

	for(auto iter = m_GltfMap.begin(); iter != m_GltfMap.end(); ++iter)
		delete iter->second;
	m_GltfMap.clear();

	for(auto iter = m_Quad2d.begin(); iter != m_Quad2d.end(); ++iter)
		delete iter->second;
}

const std::string &HyAssets::GetDataDir()
{
	return m_sDATADIR;
}

bool HyAssets::IsInitialized()
{
	return m_bInitialized;
}

HyAudioHarness &HyAssets::GetAudioRef()
{
	return m_AudioRef;
}

IHyFile *HyAssets::GetFile(HyFileType eFileType, uint32 uiManifestIndex)
{
	HyAssert(uiManifestIndex < m_FilesMap[eFileType].m_uiNumFiles, "HyAssets::GetFile was given an invalid manifest index: " << uiManifestIndex);
	switch(eFileType)
	{
	case HYFILE_Atlas:
		return &static_cast<HyFileAtlas *>(m_FilesMap[eFileType].m_pFiles)[uiManifestIndex];

	case HYFILE_AudioBank:
		return &static_cast<HyFileAudio *>(m_FilesMap[eFileType].m_pFiles)[uiManifestIndex];
	}
}

HyFileAtlas *HyAssets::GetAtlas(uint32 uiChecksum, HyRectangle<float> &UVRectOut)
{
	for(uint32 i = 0; i < m_FilesMap[HYFILE_Atlas].m_uiNumFiles; ++i)
	{
		if(static_cast<HyFileAtlas *>(m_FilesMap[HYFILE_Atlas].m_pFiles)[i].GetUvRect(uiChecksum, UVRectOut))
			return &static_cast<HyFileAtlas *>(m_FilesMap[HYFILE_Atlas].m_pFiles)[i];
	}

	return nullptr;
}

HyFileAtlas *HyAssets::GetAtlasUsingGroupId(uint32 uiAtlasGrpId, uint32 uiIndexInGroup)
{
	for(uint32 i = 0; i < m_FilesMap[HYFILE_Atlas].m_uiNumFiles; ++i)
	{
		if(static_cast<HyFileAtlas *>(m_FilesMap[HYFILE_Atlas].m_pFiles)[i].GetBankId() == uiAtlasGrpId && static_cast<HyFileAtlas *>(m_FilesMap[HYFILE_Atlas].m_pFiles)[i].GetIndexInGroup() == uiIndexInGroup)
			return &static_cast<HyFileAtlas *>(m_FilesMap[HYFILE_Atlas].m_pFiles)[i];
	}

	return nullptr;
}

uint32 HyAssets::GetNumAtlases()
{
	return m_FilesMap[HYFILE_Atlas].m_uiNumFiles;
}

HyFilesManifest *HyAssets::GetLoadedAtlases()
{
	return m_FilesMap[HYFILE_Atlas].m_pLoadedManifest;
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

	default:
		break;
	}
}

void HyAssets::LoadNodeData(IHyLoadable *pLoadable)
{
	HyAssert(pLoadable->_LoadableGetType() != HYTYPE_Entity, "HyAssets::LoadNodeData passed an entity");
	if(pLoadable->m_eLoadState != HYLOADSTATE_Inactive || pLoadable->IsLoadDataValid() == false)
		return;

	
	bool bFullyLoaded = true;
	if(pLoadable->AcquireData() != nullptr)
	{
		// Check whether all the required data/assets are loaded to confirm we're fully loaded
		for(uint32 iFileType = 0; iFileType < HYNUM_FILETYPES; ++iFileType)
		{
			const HyFilesManifest *pRequiredManifest = pLoadable->UncheckedGetData()->GetRequiredFiles(static_cast<HyFileType>(iFileType));
			if(pRequiredManifest == nullptr)
				continue;

			if(pRequiredManifest->IsEmpty() == false)
			{
				for(uint32 i = 0; i < m_FilesMap[iFileType].m_uiNumFiles; ++i)
				{
					if(pRequiredManifest->IsSet(i))
					{
						IHyFile *pFile = GetFile(static_cast<HyFileType>(iFileType), i);
						QueueData(pFile);

						if(pFile->GetLoadableState() != HYLOADSTATE_Loaded)
							bFullyLoaded = false;
					}
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
		for(uint32 iFileType = 0; iFileType < HYNUM_FILETYPES; ++iFileType)
		{
			const HyFilesManifest *pRequiredManifest = pLoadable->UncheckedGetData()->GetRequiredFiles(static_cast<HyFileType>(iFileType));
			if(pRequiredManifest == nullptr)
				continue;

			for(uint32 i = 0; i < m_FilesMap[iFileType].m_uiNumFiles; ++i)
			{
				if(pRequiredManifest->IsSet(i))
				{
					IHyFile *pFile = GetFile(static_cast<HyFileType>(iFileType), i);
					DequeData(pFile);
				}
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
		for(uint32 iFileType = 0; iFileType < HYNUM_FILETYPES; ++iFileType)
		{
			const HyFilesManifest *pRequiredManifest = pLoadable->UncheckedGetData()->GetRequiredFiles(static_cast<HyFileType>(iFileType));
			if(pRequiredManifest == nullptr)
				continue;

			if(m_FilesMap[iFileType].m_pLoadedManifest->IsSet(*pRequiredManifest) == false)
				return false;
		}
	}

	// glTF check
	HyGLTF *pGltf = pLoadable->UncheckedGetData()->GetGltf();
	if(pGltf && pGltf->GetLoadableState() != HYLOADSTATE_Loaded)
		return false;

	return true;
}

// Unload everything
void HyAssets::Shutdown()
{
	std::vector<IHyInstance2d *> vReloadInsts;
	m_SceneRef.CopyAllLoadedNodes(vReloadInsts);

	//m_QueuedEntityList.clear();

	for(uint32 i = 0; i < m_QueuedInstList.size(); ++i)
		vReloadInsts.push_back(static_cast<IHyInstance2d *>(m_QueuedInstList[i]));

	for(uint32 i = 0; i < vReloadInsts.size(); ++i)
		vReloadInsts[i]->Unload();
}

bool HyAssets::IsShutdown()
{
	bool bIsFullyUnloaded = true;
	m_Mutex.lock();
	{
		for(uint32 iFileType = 0; iFileType < HYNUM_FILETYPES; ++iFileType)
		{
			if(m_FilesMap[iFileType].m_pLoadedManifest && m_FilesMap[iFileType].m_pLoadedManifest->IsEmpty() == false)
			{
				bIsFullyUnloaded = false;
				break;
			}
		}
		if(bIsFullyUnloaded)
			bIsFullyUnloaded = m_Load_Prepare.empty() && m_Load_Shared.empty() && m_Load_Retrieval.empty();
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
				IHyFile *pFileData = m_Load_Prepare.front();
				if(pFileData->GetLoadableType() == HYFILE_Atlas)
				{
					HyFileAtlas *pAtlas = static_cast<HyFileAtlas *>(pFileData);
					uint32 uiBufferSize = pAtlas->GetWidth() * pAtlas->GetHeight() * 4;
					pFileData->m_pGfxApiPixelBuffer = rendererRef.GetPixelBufferPtr(uiBufferSize, pFileData->m_hGfxApiPbo);
				}

				m_Load_Shared.push(pFileData);
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
			IHyFile *pData = m_Load_Retrieval.front();
			m_Load_Retrieval.pop();

			rendererRef.TxData(pData);
		}
	
		m_Mutex.unlock();
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Grab and process any returning data from the renderer
	std::queue<IHyFile *> &rxDataQueueRef = rendererRef.RxData();
	while(rxDataQueueRef.empty() == false)
	{
		IHyFile *pData = rxDataQueueRef.front();
		rxDataQueueRef.pop();

		FinalizeData(pData);
	}
}

/*virtual*/ void HyAssets::OnThreadInit() /*override*/
{
	HyLogTitle("Assets");

#if defined(HY_PLATFORM_WINDOWS) && defined(HY_COMPILER_MSVC) && !defined(HY_CONFIG_SINGLETHREAD)
	SetThreadPriority(m_Thread.native_handle(), THREAD_MODE_BACKGROUND_BEGIN);
#endif

	ParseManifestFile(HYFILE_Atlas);
	ParseManifestFile(HYFILE_AudioBank);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// INSTANCE ITEMS
#ifndef HY_PLATFORM_GUI
	std::string sGameDataFilePath(m_sDATADIR);
	sGameDataFilePath += HYASSETS_DataFile;
	std::string sGameDataFileContents;
	HyIO::ReadTextFile(sGameDataFilePath.c_str(), sGameDataFileContents);

	jsonxx::Object gameDataObj;
	bool bGameDataParsed = gameDataObj.parse(sGameDataFileContents);
	HyAssert(bGameDataParsed, "Could not parse game data");

	if(gameDataObj.has<jsonxx::Object>("Audio"))
		m_AudioFactory.Init(gameDataObj.get<jsonxx::Object>("Audio"), *this);
	if(gameDataObj.has<jsonxx::Object>("Prefabs"))
	{
		const jsonxx::Object &prefabObj = gameDataObj.get<jsonxx::Object>("Prefabs");

		for(auto iter = prefabObj.kv_map().begin(); iter != prefabObj.kv_map().end(); ++iter)
			m_GltfMap[iter->first] = HY_NEW HyGLTF(iter->first, 0);

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
	std::vector<IHyFile *>	dataList;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Copy all the ptrs into their vectors to be processed, while emptying the shared queue
	m_Mutex.lock();
	while(m_Load_Shared.empty() == false)
	{
		dataList.push_back(m_Load_Shared.front());
		m_Load_Shared.pop();
	}
	m_Mutex.unlock();

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Load everything that is enqueued (outside of any critical section)
	for(uint32 i = 0; i < dataList.size(); ++i)
		dataList[i]->OnLoadThread();

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Copy all the (loaded) IData ptrs to the retrieval vector
	m_Mutex.lock();
	for(uint32 i = 0; i < dataList.size(); ++i)
		m_Load_Retrieval.push(dataList[i]);
	m_Mutex.unlock();
}

/*virtual*/ void HyAssets::OnThreadShutdown() /*override*/
{
}

bool HyAssets::ParseManifestFile(HyFileType eFileType)
{
	std::string sManifestFilePath;

	switch(eFileType)
	{
	case HYFILE_Atlas:
		sManifestFilePath = m_sDATADIR + HYASSETS_AtlasDir + HYASSETS_AtlasFile;
		break;

	case HYFILE_AudioBank:
		sManifestFilePath = m_sDATADIR + HYASSETS_AudioDir + HYASSETS_AudioFile;
		break;
	}

	if(HyIO::FileExists(sManifestFilePath) == false)
	{
		HyLogWarning("Missing asset manifest file: " << sManifestFilePath);
		return false;
	}

	std::string sManifestFileContents;
	HyIO::ReadTextFile(sManifestFilePath.c_str(), sManifestFileContents);
	jsonxx::Object fileObj;
	if(fileObj.parse(sManifestFileContents) == false)
	{
		HyLogWarning("Failed to open/read manifest file: " << sManifestFileContents);
		return false;
	}

	jsonxx::Object manifestFileObj;
	if(manifestFileObj.parse(sManifestFileContents))
	{
		switch(eFileType)
		{
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		case HYFILE_Atlas: {
			// TODO: rename to banks
			jsonxx::Array atlasGrpArray = manifestFileObj.get<jsonxx::Array>("atlasGroups");

			// Iterate through each atlas group and determine how many textures total there are between all groups
			m_FilesMap[eFileType].m_uiNumFiles = 0;
			for(uint32 i = 0; i < static_cast<uint32>(atlasGrpArray.size()); ++i)
			{
				jsonxx::Object atlasGrpObj = atlasGrpArray.get<jsonxx::Object>(i);

				// TODO: rename to assets
				jsonxx::Array texturesArray = atlasGrpObj.get<jsonxx::Array>("textures");
				m_FilesMap[eFileType].m_uiNumFiles += static_cast<uint32>(texturesArray.size());
			}

			m_FilesMap[eFileType].m_pFiles = reinterpret_cast<HyFileAtlas *>(HY_NEW unsigned char[sizeof(HyFileAtlas) * m_FilesMap[eFileType].m_uiNumFiles]);
			HyFileAtlas *pAtlasWriteLocation = static_cast<HyFileAtlas *>(m_FilesMap[eFileType].m_pFiles);

			// Then iterate back over each atlas group and instantiate a HyFileAtlas for each texture
			uint32 uiManifestIndex = 0;
			char szTmpBuffer[16];
			for(uint32 i = 0; i < static_cast<uint32>(atlasGrpArray.size()); ++i)
			{
				jsonxx::Object atlasGrpObj = atlasGrpArray.get<jsonxx::Object>(i);

				// TODO: rename to bankId
				uint32 uiAtlasGroupId = static_cast<uint32>(atlasGrpObj.get<jsonxx::Number>("atlasGrpId"));

				// TODO: rename to assets
				jsonxx::Array texturesArray = atlasGrpObj.get<jsonxx::Array>("textures");
				for(uint32 j = 0; j < static_cast<uint32>(texturesArray.size()); ++j)
				{
					HyAssert(uiManifestIndex < m_FilesMap[eFileType].m_uiNumFiles, "HyAssets::OnThreadInit instantiated too many atlases");

					std::sprintf(szTmpBuffer, "%05d", j);
					std::string sAtlasFilePath = szTmpBuffer;

					uint32 uiTextureFormat = static_cast<uint32>(atlasGrpObj.get<jsonxx::Number>("textureType"));
					uint32 uiTextureFiltering = atlasGrpObj.has<jsonxx::Number>("textureFiltering") ? static_cast<uint32>(atlasGrpObj.get<jsonxx::Number>("textureFiltering")) : HYTEXFILTER_BILINEAR;

					if(uiTextureFormat == HYTEXTURE_R8G8B8A8 || uiTextureFormat == HYTEXTURE_R8G8B8)
						sAtlasFilePath += ".png";
					else
						sAtlasFilePath += ".dds";

					new (pAtlasWriteLocation)HyFileAtlas(sAtlasFilePath,
						uiAtlasGroupId,
						j,
						uiManifestIndex,
						static_cast<int32>(atlasGrpObj.get<jsonxx::Number>("width")),
						static_cast<int32>(atlasGrpObj.get<jsonxx::Number>("height")),
						static_cast<HyTextureFormat>(uiTextureFormat),
						static_cast<HyTextureFiltering>(uiTextureFiltering),
						texturesArray.get<jsonxx::Array>(j));

					++pAtlasWriteLocation;
					++uiManifestIndex;
				}
			}
			break; }

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		case HYFILE_AudioBank: {
			// TODO: rename to banks
			jsonxx::Array banksArray = manifestFileObj.get<jsonxx::Array>("atlasGroups");
			m_FilesMap[eFileType].m_uiNumFiles = static_cast<uint32>(banksArray.size());

			m_FilesMap[eFileType].m_pFiles = reinterpret_cast<HyFileAudio *>(HY_NEW unsigned char[sizeof(HyFileAudio) * m_FilesMap[eFileType].m_uiNumFiles]);
			HyFileAudio *pPlacementLocation = static_cast<HyFileAudio *>(m_FilesMap[eFileType].m_pFiles);

			char szTmpBuffer[16];
			for(uint32 i = 0; i < m_FilesMap[eFileType].m_uiNumFiles; ++i)
			{
				jsonxx::Object bankObj = banksArray.get<jsonxx::Object>(i);

				// TODO: rename to bankId
				uint32 uiAtlasGroupId = static_cast<uint32>(bankObj.get<jsonxx::Number>("atlasGrpId"));

				std::string sBankFilePath = HYASSETS_AudioDir;
				sprintf(szTmpBuffer, "%05d", uiAtlasGroupId);
				sBankFilePath += szTmpBuffer;

				new (pPlacementLocation)HyFileAudio(sBankFilePath, i, m_AudioRef.AllocateAudioBank(bankObj));
				++pPlacementLocation;
			}
			break; }
		}
	}
	else
	{
		HyLogWarning("Failed to parse manifest file: " << sManifestFileContents);
		return false;
	}

	HyFilesManifest::sm_iIndexFlagsArraySize[eFileType] = (m_FilesMap[eFileType].m_uiNumFiles / 32);
	if(m_FilesMap[eFileType].m_uiNumFiles % 32 != 0)
		HyFilesManifest::sm_iIndexFlagsArraySize[eFileType]++;

	m_FilesMap[eFileType].m_pLoadedManifest = HY_NEW HyFilesManifest(eFileType);
}

void HyAssets::QueueData(IHyFile *pData)
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

void HyAssets::DequeData(IHyFile *pData)
{
	HyAssert(pData->m_eLoadState != HYLOADSTATE_Inactive, "Trying to DequeData that is HYLOADSTATE_Inactive");
	HyAssert(pData->m_uiRefCount > 0, "Tried to decrement a '0' reference");

	pData->m_uiRefCount--;
	if(pData->m_uiRefCount == 0)
	{
		if(pData->m_eLoadState == HYLOADSTATE_Loaded)
		{
			pData->m_eLoadState = HYLOADSTATE_Discarded;

			if(m_FilesMap[pData->GetLoadableType()].m_pLoadedManifest)
				m_FilesMap[pData->GetLoadableType()].m_pLoadedManifest->Clear(pData->GetManifestIndex());

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

void HyAssets::FinalizeData(IHyFile *pData)
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

			if(m_FilesMap[pData->GetLoadableType()].m_pLoadedManifest)
			{
				m_FilesMap[pData->GetLoadableType()].m_pLoadedManifest->Set(static_cast<HyFileAtlas *>(pData)->GetManifestIndex());

				std::string sAssetType;
				if(pData->GetLoadableType() == HYFILE_Atlas)
					sAssetType = "Atlas";
				else if(pData->GetLoadableType() == HYFILE_AudioBank)
					sAssetType = "Audio";

				HyLogInfo(sAssetType << " [" << pData->GetManifestIndex() << "] loaded");
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
					HyLogInfo("Atlas [" << static_cast<HyFileAtlas *>(pData)->GetManifestIndex() << "] reloading");
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
				HyLogInfo("Atlas [" << static_cast<HyFileAtlas *>(pData)->GetManifestIndex() << "] deleted")
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
