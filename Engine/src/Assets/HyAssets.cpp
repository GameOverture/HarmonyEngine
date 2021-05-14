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
#include "Scene/Nodes/Loadables/Bodies/Objects/HyEntity2d.h"
#include "Scene/Nodes/Loadables/Bodies/Objects/HyEntity3d.h"
#include "Renderer/IHyRenderer.h"
#include "Utilities/HyIO.h"
#include "Utilities/HyMath.h"
#include "Diagnostics/Console/IHyConsole.h"

#include <fstream>
#include <iostream>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Nested class Factory
template<typename tData>
void HyAssets::Factory<tData>::Init(HyJsonObj subDirObjRef, HyAssets &assetsRef)
{
	m_DataList.reserve(subDirObjRef.MemberCount());

	uint32 i = 0;
	for(auto &v : subDirObjRef)
	{
		std::string sPath = HyIO::CleanPath(v.name.GetString(), nullptr, true);
		m_LookupIndexMap.insert(std::make_pair(sPath, i));

		HyJsonObj obj = v.value.GetObject();
		m_DataList.emplace_back(v.name.GetString(), obj, assetsRef);

		++i;
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

HyAssets::HyAssets(IHyAudioCore &audioCoreRef, HyScene &sceneRef, std::string sDataDirPath) :
	IHyThreadClass(HYTHREAD_Lowest),
	m_AudioCoreRef(audioCoreRef),
	m_SceneRef(sceneRef),
	m_sDATADIR(HyIO::CleanPath(sDataDirPath.c_str(), "/", true)),
	m_bInitialized(false)
{
	IHyLoadable::sm_pHyAssets = this;
	ThreadStart();
	//ThreadWait();
}

HyAssets::~HyAssets()
{
	IHyLoadable::sm_pHyAssets = nullptr;

	HyAssert(IsShutdown(), "Tried to destruct the manifests while data still exists");
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

IHyFile *HyAssets::GetFile(HyFileType eFileType, uint32 uiManifestIndex)
{
	HyAssert(uiManifestIndex < m_FilesMap[eFileType].m_uiNumFiles, "HyAssets::GetFile was given an invalid manifest index: " << uiManifestIndex);
	switch(eFileType)
	{
	case HYFILE_Atlas:
		return &static_cast<HyFileAtlas *>(m_FilesMap[eFileType].m_pFiles)[uiManifestIndex];

	case HYFILE_AudioBank:
		return &static_cast<HyFileAudio *>(m_FilesMap[eFileType].m_pFiles)[uiManifestIndex];

	default:
		return nullptr;
	}

	return nullptr;
}

IHyFile *HyAssets::GetFileWithAsset(HyFileType eFileType, uint32 uiAssetChecksum)
{
	switch(eFileType)
	{
	case HYFILE_Atlas: {
		HyRectangle<float> tmp;
		for(uint32 i = 0; i < m_FilesMap[HYFILE_Atlas].m_uiNumFiles; ++i)
		{
			if(static_cast<HyFileAtlas *>(m_FilesMap[HYFILE_Atlas].m_pFiles)[i].GetUvRect(uiAssetChecksum, tmp))
				return &m_FilesMap[HYFILE_Atlas].m_pFiles[i];
		}
		break; }

	case HYFILE_AudioBank:
		for(uint32 i = 0; i < m_FilesMap[HYFILE_AudioBank].m_uiNumFiles; ++i)
		{
			if(static_cast<HyFileAudio *>(m_FilesMap[HYFILE_AudioBank].m_pFiles)[i].ContainsAsset(uiAssetChecksum))
				return &m_FilesMap[HYFILE_AudioBank].m_pFiles[i];
		}
		break;

	default:
		return nullptr;
	}

	return nullptr;
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
	//HyAssert(pLoadable->_LoadableGetType() != HYTYPE_Entity, "HyAssets::RemoveNodeData passed an entity");
	if(pLoadable->m_eLoadState == HYLOADSTATE_Inactive || pLoadable->_LoadableGetType() == HYTYPE_Entity)
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

void HyAssets::SetEntityLoaded(IHyLoadable *pEntity)
{
	if(pEntity == nullptr)
		return;

	HyAssert(pEntity->_LoadableGetType() == HYTYPE_Entity, "HyAssets::SetEntityLoaded was passed a node that wasn't an entity");

	if(pEntity->IsLoaded())
	{
		if(pEntity->IsChildrenLoaded() == false)
			SetAsUnloaded(pEntity);
	}
	else
	{
		if(pEntity->IsChildrenLoaded())
			SetAsLoaded(pEntity);
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
	std::vector<IHyLoadable *> vReloadInsts;
	m_SceneRef.CopyAllLoadedNodes(vReloadInsts);

	//m_QueuedEntityList.clear();

	for(uint32 i = 0; i < m_QueuedInstList.size(); ++i)
		vReloadInsts.push_back(static_cast<IHyLoadable *>(m_QueuedInstList[i]));

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
	// ITEMS
#ifndef HY_PLATFORM_GUI
	std::string sGameDataFilePath(m_sDATADIR);
	sGameDataFilePath += HYASSETS_DataFile;
	std::vector<char> sGameDataFileContents;
	HyIO::ReadTextFile(sGameDataFilePath.c_str(), sGameDataFileContents);

	HyJsonDoc itemsDoc;
	if(itemsDoc.ParseInsitu(sGameDataFileContents.data()).HasParseError())
	{
		HyError("HyAssets::OnThreadInit - Items had JSON parsing error: " << rapidjson::GetParseErrorFunc(itemsDoc.GetParseError()));
		return;
	}
	HyAssert(itemsDoc.IsObject(), "HyAssets::OnThreadInit - Items json file wasn't an object");

	if(itemsDoc.HasMember("Audio"))
		m_AudioFactory.Init(itemsDoc["Audio"].GetObject(), *this);
	if(itemsDoc.HasMember("Prefabs"))
	{
		HyJsonObj prefabObj = itemsDoc["Prefabs"].GetObject();

		//for(auto iter = prefabObj.kv_map().begin(); iter != prefabObj.kv_map().end(); ++iter)
		//	m_GltfMap[iter->first] = HY_NEW HyGLTF(iter->first, 0);

		//m_PrefabFactory.Init(prefabObj, *this);
	}
	if(itemsDoc.HasMember("Texts"))
		m_TextFactory.Init(itemsDoc["Texts"].GetObject(), *this);
	if(itemsDoc.HasMember("Sprites"))
		m_SpriteFactory.Init(itemsDoc["Sprites"].GetObject(), *this);
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
	int32 iMaxLoadPerUpdate = 5;
	while(m_Load_Shared.empty() == false && iMaxLoadPerUpdate > 0)
	{
		dataList.push_back(m_Load_Shared.front());
		m_Load_Shared.pop();
		iMaxLoadPerUpdate--;
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

	default:
		HyLogWarning("Cannot parse unhandled file type: " << eFileType);
		return false;
	}

	if(HyIO::FileExists(sManifestFilePath) == false)
	{
		HyLogWarning("Missing asset manifest file: " << sManifestFilePath);
		return false;
	}

	std::vector<char> sManifestFileContents;
	HyIO::ReadTextFile(sManifestFilePath.c_str(), sManifestFileContents);
	HyJsonDoc fileDoc;

	if(fileDoc.ParseInsitu(sManifestFileContents.data()).HasParseError())
	{
		HyError("HyAssets::ParseManifestFile - Manifest had JSON parsing error: " << rapidjson::GetParseErrorFunc(fileDoc.GetParseError()));
		return false;
	}
	HyAssert(fileDoc.IsObject(), "HyAssets::ParseManifestFile - Manifest json file wasn't an object");

	switch(eFileType)
	{
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	case HYFILE_Atlas: {
		HyJsonArray banksArray = fileDoc["banks"].GetArray();

		// Iterate through each bank and determine how many textures total there are between all banks
		m_FilesMap[eFileType].m_uiNumFiles = 0;
		for(uint32 i = 0; i < banksArray.Size(); ++i)
		{
			HyJsonObj bankObj = banksArray[i].GetObject();
			HyJsonArray texturesArray = bankObj["textures"].GetArray();
			m_FilesMap[eFileType].m_uiNumFiles += texturesArray.Size();
		}

		m_FilesMap[eFileType].m_pFiles = reinterpret_cast<HyFileAtlas *>(HY_NEW unsigned char[sizeof(HyFileAtlas) * m_FilesMap[eFileType].m_uiNumFiles]);
		HyFileAtlas *pAtlasWriteLocation = static_cast<HyFileAtlas *>(m_FilesMap[eFileType].m_pFiles);

		// Then iterate back over each atlas group and instantiate a HyFileAtlas for each texture
		uint32 uiManifestIndex = 0;
		char szTmpBuffer[16];
		for(uint32 i = 0; i < banksArray.Size(); ++i)
		{
			HyJsonObj bankObj = banksArray[i].GetObject();

			uint32 uiBankId = bankObj["bankId"].GetUint();

			HyJsonArray texturesArray = bankObj["textures"].GetArray();
			for(uint32 j = 0; j < texturesArray.Size(); ++j)
			{
				HyAssert(uiManifestIndex < m_FilesMap[eFileType].m_uiNumFiles, "HyAssets::OnThreadInit instantiated too many atlases");

				std::sprintf(szTmpBuffer, "%05d", j);
				std::string sAtlasFilePath = szTmpBuffer;

				HyJsonObj texObj = texturesArray[j].GetObject();
				HyTextureFormat eFormat = GetTextureFormatFromString(texObj["format"].GetString());

				if(eFormat == HYTEXTURE_R8G8B8A8 || eFormat == HYTEXTURE_R8G8B8)
					sAtlasFilePath += ".png";
				else
					sAtlasFilePath += ".dds";

				new (pAtlasWriteLocation)HyFileAtlas(sAtlasFilePath,
					uiBankId,
					j,
					uiManifestIndex,
					texObj);

				++pAtlasWriteLocation;
				++uiManifestIndex;
			}
		}
		break; }

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	case HYFILE_AudioBank: {
		HyJsonArray banksArray = fileDoc["banks"].GetArray();
		m_FilesMap[eFileType].m_uiNumFiles = banksArray.Size();

		m_FilesMap[eFileType].m_pFiles = reinterpret_cast<HyFileAudio *>(HY_NEW unsigned char[sizeof(HyFileAudio) * m_FilesMap[eFileType].m_uiNumFiles]);
		HyFileAudio *pPlacementLocation = static_cast<HyFileAudio *>(m_FilesMap[eFileType].m_pFiles);

		char szTmpBuffer[16];
		for(uint32 i = 0; i < m_FilesMap[eFileType].m_uiNumFiles; ++i)
		{
			HyJsonObj bankObj = banksArray[i].GetObject();

			uint32 uiBankId = bankObj["bankId"].GetUint();

			std::string sBankFilePath = HYASSETS_AudioDir;
			sprintf(szTmpBuffer, "%05d", uiBankId);
			sBankFilePath += szTmpBuffer;

			new (pPlacementLocation)HyFileAudio(sBankFilePath, i, m_AudioCoreRef.AllocateAudioBank(bankObj));
			++pPlacementLocation;
		}
		break; }

	default:
		break;
	}


	HyFilesManifest::sm_iIndexFlagsArraySize[eFileType] = (m_FilesMap[eFileType].m_uiNumFiles / 32);
	if(m_FilesMap[eFileType].m_uiNumFiles % 32 != 0)
		HyFilesManifest::sm_iIndexFlagsArraySize[eFileType]++;

	m_FilesMap[eFileType].m_pLoadedManifest = HY_NEW HyFilesManifest(eFileType);
	
	return true;
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
				HyLogInfo("Atlas [" << static_cast<HyFileAtlas *>(pData)->GetManifestIndex() << "] deleted");
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

/*static*/ std::vector<HyTextureFormat> HyAssets::GetTextureFormatList()
{
	std::vector<HyTextureFormat> list;
	list.push_back(HYTEXTURE_R8G8B8A8);
	list.push_back(HYTEXTURE_R8G8B8);
	list.push_back(HYTEXTURE_RGB_DTX1);
	list.push_back(HYTEXTURE_RGBA_DTX1);
	list.push_back(HYTEXTURE_DTX3);
	list.push_back(HYTEXTURE_DTX5);

	HyAssert(list.size() == HYNUM_TEXTUREFORMATS, "HyGlobal::GetTextureFormatList missing a format!");

	return list;
}

/*static*/ std::vector<std::string> HyAssets::GetTextureFormatNameList()
{
	std::vector<HyTextureFormat> formatList = GetTextureFormatList();

	std::vector<std::string> list;
	for(int32 i = 0; i < static_cast<int32>(formatList.size()); ++i)
		list.push_back(GetTextureFormatName(formatList[i]));

	return list;
}

/*static*/ std::string HyAssets::GetTextureFormatName(HyTextureFormat eType)
{
	// WARNING: Changing any of these strings affects data and meta files and requires a version patcher bump!
	switch(eType)
	{
	case HYTEXTURE_R8G8B8A8:
		return "R8G8B8A8";
	case HYTEXTURE_R8G8B8:
		return "R8G8B8 (unsupported)";
	case HYTEXTURE_RGB_DTX1:
		return "RGB_DTX1";
	case HYTEXTURE_RGBA_DTX1:
		return "RGBA_DTX1 (unsupported)";
	case HYTEXTURE_DTX3:
		return "DTX3 (unsupported)";
	case HYTEXTURE_DTX5:
		return "DTX5";

	case HYTEXTURE_Unknown:
	default:
		return "Unknown";
	}
}

/*static*/ HyTextureFormat HyAssets::GetTextureFormatFromString(std::string sFormat)
{
	std::transform(sFormat.begin(), sFormat.end(), sFormat.begin(), ::tolower);

	std::vector<std::string> sTextureFormatList = GetTextureFormatNameList();
	for(int32 i = 0; i < static_cast<int32>(sTextureFormatList.size()); ++i)
	{
		std::string sCurStr = sTextureFormatList[i];
		std::transform(sCurStr.begin(), sCurStr.end(), sCurStr.begin(), ::tolower);
		
		if(sFormat == sCurStr)
			return GetTextureFormatList()[i];
	}

	return HYTEXTURE_Unknown;
}


/*static*/ std::vector<HyTextureFiltering> HyAssets::GetTextureFilteringList()
{
	std::vector<HyTextureFiltering> list;
	list.push_back(HYTEXFILTER_NEAREST);
	list.push_back(HYTEXFILTER_NEAREST_MIPMAP);
	list.push_back(HYTEXFILTER_LINEAR_MIPMAP);
	list.push_back(HYTEXFILTER_BILINEAR);
	list.push_back(HYTEXFILTER_BILINEAR_MIPMAP);
	list.push_back(HYTEXFILTER_TRILINEAR);

	HyAssert(list.size() == HYNUM_TEXTUREFILTERS, "HyGlobal::GetTextureFilteringList missing a format!");
	return list;
}

/*static*/ std::vector<std::string> HyAssets::GetTextureFilteringNameList()
{
	std::vector<HyTextureFiltering> formatList = GetTextureFilteringList();

	std::vector<std::string> list;
	for(int32 i = 0; i < static_cast<int32>(formatList.size()); ++i)
		list.push_back(GetTextureFilteringName(formatList[i]));

	return list;
}

/*static*/ std::string HyAssets::GetTextureFilteringName(HyTextureFiltering eType)
{
	// WARNING: Changing any of these strings affects data and meta files and requires a version patcher bump!
	switch(eType)
	{
	case HYTEXFILTER_NEAREST:
		return "Nearest";
	case HYTEXFILTER_NEAREST_MIPMAP:
		return "Nearest Mipmap";
	case HYTEXFILTER_LINEAR_MIPMAP:
		return "Linear Mipmap";
	case HYTEXFILTER_BILINEAR:
		return "Bilinear";
	case HYTEXFILTER_BILINEAR_MIPMAP:
		return "Bilinear Mipmap";
	case HYTEXFILTER_TRILINEAR:
		return "Trilinear";

	case HYTEXFILTER_Unknown:
	default:
		return "Unknown Filter";
	}
}

/*static*/ HyTextureFiltering HyAssets::GetTextureFilteringFromString(std::string sFilter)
{
	std::transform(sFilter.begin(), sFilter.end(), sFilter.begin(), ::tolower);

	std::vector<std::string> sTextureFilteringList = GetTextureFilteringNameList();
	for(int32 i = 0; i < static_cast<int32>(sTextureFilteringList.size()); ++i)
	{
		std::string sCurStr = sTextureFilteringList[i];
		std::transform(sCurStr.begin(), sCurStr.end(), sCurStr.begin(), ::tolower);
		
		if(sFilter == sCurStr)
			return GetTextureFilteringList()[i];
	}

	return HYTEXFILTER_Unknown;
}
