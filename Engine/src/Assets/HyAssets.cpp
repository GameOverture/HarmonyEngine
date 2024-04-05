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
#include "Assets/Nodes/Objects/HyAudioData.h"
#include "Assets/Nodes/Objects/HySpineData.h"
#include "Assets/Nodes/Objects/HySpriteData.h"
#include "Assets/Nodes/Objects/HyTextData.h"
#include "Assets/Nodes/Objects/HyTexturedQuadData.h"
#include "Assets/Nodes/Objects/HyPrefabData.h"
#include "Scene/HyScene.h"
#include "Scene/Nodes/Loadables/IHyLoadable.h"
#include "Scene/Nodes/Loadables/Bodies/Objects/HyEntity2d.h"
#include "Scene/Nodes/Loadables/Bodies/Objects/HyEntity3d.h"
#include "Renderer/IHyRenderer.h"
#include "Utilities/Crc32.h"
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
		HyNodePath nodePath(v.name.GetString());
	
		m_LookupIndexMap.insert(std::make_pair(nodePath.GetHash1(), i));

		HyJsonObj obj = v.value.GetObject();
		m_DataList.emplace_back(nodePath, obj, assetsRef);

		++i;
	}
}

template<typename tData>
const tData *HyAssets::Factory<tData>::GetData(const HyNodePath &nodePath) const
{
	auto iter = m_LookupIndexMap.find(nodePath.GetHash1());
	if(iter == m_LookupIndexMap.end())
	{
		if(nodePath.GetName().empty() == false)
			HyLogError("Cannot find data for: " << nodePath.GetPath());

		return nullptr;
	}

	return &m_DataList[iter->second];
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

HyAssets::HyAssets(HyAudioCore &audioCoreRef, HyScene &sceneRef, std::string sDataDirPath) :
	IHyThreadClass(HYTHREAD_Lowest),
	m_AudioCoreRef(audioCoreRef),
	m_SceneRef(sceneRef),
	m_sDATADIR(HyIO::CleanPath(sDataDirPath.c_str(), "/")),
	m_bInitialized(false),
	m_uiLoadingCountTotal(0)
{
	IHyLoadable::sm_pHyAssets = this;
	ThreadStart();
	ThreadWait();

	m_AudioCoreRef.SetHyAssetsPtr(this);
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

	for(auto iter = m_AuxiliaryTextureQuadMap.begin(); iter != m_AuxiliaryTextureQuadMap.end(); ++iter)
		delete iter->second;

	for(auto iter = m_AuxiliaryAudioMap.begin(); iter != m_AuxiliaryAudioMap.end(); ++iter)
		delete iter->second;

	for(auto iter = m_AuxiliaryFileMap.begin(); iter != m_AuxiliaryFileMap.end(); ++iter)
		delete iter->second;
}

HyAudioCore &HyAssets::GetAudioCore() const
{
	return m_AudioCoreRef;
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

IHyFile *HyAssets::GetFileWithAsset(HyFileType eFileType, uint32 uiAssetChecksum, uint32 uiBankId)
{
	switch(eFileType)
	{
	case HYFILE_Atlas: {
		HyMargins<float> tmp;
		uint64 uiCropMask = 0;
		for(uint32 i = 0; i < m_FilesMap[HYFILE_Atlas].m_uiNumFiles; ++i)
		{
			if(static_cast<HyFileAtlas *>(m_FilesMap[HYFILE_Atlas].m_pFiles)[i].GetBankId() == uiBankId && static_cast<HyFileAtlas *>(m_FilesMap[HYFILE_Atlas].m_pFiles)[i].GetUvRect(uiAssetChecksum, tmp, uiCropMask))
				return &static_cast<HyFileAtlas *>(m_FilesMap[HYFILE_Atlas].m_pFiles)[i];
		}
		break; }

	case HYFILE_AudioBank:
		for(uint32 i = 0; i < m_FilesMap[HYFILE_AudioBank].m_uiNumFiles; ++i)
		{
			if(static_cast<HyFileAudio *>(m_FilesMap[HYFILE_AudioBank].m_pFiles)[i].GetBankId() == uiBankId && static_cast<HyFileAudio *>(m_FilesMap[HYFILE_AudioBank].m_pFiles)[i].ContainsAsset(uiAssetChecksum))
				return &static_cast<HyFileAudio *>(m_FilesMap[HYFILE_AudioBank].m_pFiles)[i];
		}
		break;

	default:
		break;
	}

	return nullptr;
}

IHyFile *HyAssets::GetAuxiliaryFile(HyAuxiliaryFileHandle hHandle)
{
	HyAssert(m_AuxiliaryFileMap.find(hHandle) != m_AuxiliaryFileMap.end(), "HyAssets::GetAuxiliaryFile was given an invalid handle: " << hHandle);
	return m_AuxiliaryFileMap[hHandle];
}

void HyAssets::SetAuxiliaryFile(HyAuxiliaryFileHandle hHandle, IHyFile *pFile)
{
	HyAssert(m_AuxiliaryFileMap.find(hHandle) == m_AuxiliaryFileMap.end(), "HyAssets::SetAuxiliaryFile was given a handle that already exists: " << hHandle);
	m_AuxiliaryFileMap.insert({ hHandle, pFile });
}

HyFileAtlas *HyAssets::GetAtlas(uint32 uiChecksum, uint32 uiBankId, HyMargins<float> &UVRectOut, uint64 &uiCropMaskOut)
{
	for(uint32 i = 0; i < m_FilesMap[HYFILE_Atlas].m_uiNumFiles; ++i)
	{
		if(static_cast<HyFileAtlas *>(m_FilesMap[HYFILE_Atlas].m_pFiles)[i].GetBankId() == uiBankId && static_cast<HyFileAtlas *>(m_FilesMap[HYFILE_Atlas].m_pFiles)[i].GetUvRect(uiChecksum, UVRectOut, uiCropMaskOut))
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
	case HYTYPE_Sprite:
		pDataOut = m_SpriteFactory.GetData(pLoadable->GetPath());
		break;
	case HYTYPE_Text:
		pDataOut = m_TextFactory.GetData(pLoadable->GetPath());
		break;
	case HYTYPE_Spine:
		pDataOut = m_SpineFactory.GetData(pLoadable->GetPath());
		break;
	case HYTYPE_Prefab:
		pDataOut = m_PrefabFactory.GetData(pLoadable->GetPath());
		break;
	
	// Handle types that could be auxiliary together here
	case HYTYPE_Audio:
	case HYTYPE_TexturedQuad:
		if(pLoadable->GetPath().IsAuxiliary())
		{
			// Convert Prefix and Name back into an auxiliary handle
			std::pair<uint32, uint32> auxiliaryHandle = std::make_pair(pLoadable->GetPath().GetHash1(),
																	   pLoadable->GetPath().GetHash2());

			// If FIRST is non-zero then it's holding a checksum, and SECOND is bankId
			if(auxiliaryHandle.first != 0)
			{
				uint32 uiChecksum = auxiliaryHandle.first;
				uint32 uiBankId = auxiliaryHandle.second;

				if(pLoadable->_LoadableGetType() == HYTYPE_TexturedQuad)
				{
					if(m_AuxiliaryTextureQuadMap.find(auxiliaryHandle) == m_AuxiliaryTextureQuadMap.end())
					{
						HyTexturedQuadData *pNewTexQuadData = HY_NEW HyTexturedQuadData(uiChecksum, uiBankId, *this);
						m_AuxiliaryTextureQuadMap.insert({ auxiliaryHandle, pNewTexQuadData });
						pDataOut = pNewTexQuadData;
					}
					else
						pDataOut = m_AuxiliaryTextureQuadMap[auxiliaryHandle];
				}
				else if(pLoadable->_LoadableGetType() == HYTYPE_Audio)
				{
					if(m_AuxiliaryAudioMap.find(auxiliaryHandle) == m_AuxiliaryAudioMap.end())
					{
						HyAudioData *pNewAudioData = HY_NEW HyAudioData(uiChecksum, uiBankId, *this);
						m_AuxiliaryAudioMap.insert({ auxiliaryHandle, pNewAudioData });
						pDataOut = pNewAudioData;
					}
					else
						pDataOut = m_AuxiliaryAudioMap[auxiliaryHandle];
				}
			}
			else // If FIRST is zero, then SECOND is holding a HyAuxiliaryFileHandle
			{
				if(pLoadable->_LoadableGetType() == HYTYPE_TexturedQuad)
				{
					HyAssert(m_AuxiliaryTextureQuadMap.find(auxiliaryHandle) != m_AuxiliaryTextureQuadMap.end(), "HyAssets::AcquireNodeData was given an invalid auxiliary file handle: " << auxiliaryHandle.first << ", " << auxiliaryHandle.second);
					pDataOut = m_AuxiliaryTextureQuadMap[auxiliaryHandle];
				}
				else if(pLoadable->_LoadableGetType() == HYTYPE_Audio)
				{
					HyAssert(m_AuxiliaryAudioMap.find(auxiliaryHandle) != m_AuxiliaryAudioMap.end(), "HyAssets::AcquireNodeData was given an invalid auxiliary file handle: " << auxiliaryHandle.first << ", " << auxiliaryHandle.second);
					pDataOut = m_AuxiliaryAudioMap[auxiliaryHandle];
				}
			}
		}
		else
		{
			// TODO: Possibly add a new item type that is just a 'sampled' auxiliary audio, so HyAudio can be just a regular project item handled above
			if(pLoadable->_LoadableGetType() == HYTYPE_Audio)
				pDataOut = m_AudioFactory.GetData(pLoadable->GetPath());
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
			const HyFilesManifest *pRequiredManifest = pLoadable->UncheckedGetData()->GetManifestFiles(static_cast<HyFileType>(iFileType));
			if(pRequiredManifest != nullptr)
			{
				// A valid manifest means the files are project specified
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
			else
			{
				// Non valid manifest means the files are auxiliary
				IHyFile *pFile = pLoadable->UncheckedGetData()->GetAuxiliaryFile();
				QueueData(pFile);

				if(pFile->GetLoadableState() != HYLOADSTATE_Loaded)
					bFullyLoaded = false;

				break; // Only one auxiliary file per loadable
			}
		}
	}

	// Set the node's 'm_eLoadState' appropriately below to prevent additional Loads
	if(bFullyLoaded)
		SetAsLoaded(pLoadable);
	else
	{
		if(m_QueuedInstList.size() == 0)
			m_uiLoadingCountTotal = 0; // Restart the % loaded

		pLoadable->m_eLoadState = HYLOADSTATE_Queued;
		m_QueuedInstList.push_back(pLoadable);
		m_uiLoadingCountTotal++;
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
			const HyFilesManifest *pRequiredManifest = pLoadable->UncheckedGetData()->GetManifestFiles(static_cast<HyFileType>(iFileType));
			if(pRequiredManifest != nullptr)
			{
				for(uint32 i = 0; i < m_FilesMap[iFileType].m_uiNumFiles; ++i)
				{
					if(pRequiredManifest->IsSet(i))
					{
						IHyFile *pFile = GetFile(static_cast<HyFileType>(iFileType), i);
						DequeData(pFile);
					}
				}
			}
			else // Non valid manifest means the files are auxiliary
			{
				IHyFile *pFile = pLoadable->UncheckedGetData()->GetAuxiliaryFile();
				DequeData(pFile);

				break; // Only one auxiliary file per loadable
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
			const HyFilesManifest *pRequiredManifest = pLoadable->UncheckedGetData()->GetManifestFiles(static_cast<HyFileType>(iFileType));
			if(pRequiredManifest != nullptr)
			{
				if(m_FilesMap[iFileType].m_pLoadedManifest->IsSet(*pRequiredManifest) == false)
					return false;
			}
			else // Non valid manifest means the files are auxiliary
			{
				return pLoadable->UncheckedGetData()->GetAuxiliaryFile()->GetLoadableState() == HYLOADSTATE_Loaded;
			}
		}
	}

	return true;
}

void HyAssets::GetNodeLoadingStatus(uint32 &uiNumQueuedOut, uint32 &uiTotalOut) const
{
	uiNumQueuedOut = static_cast<uint32>(m_QueuedInstList.size());
	uiTotalOut = m_uiLoadingCountTotal;
}

HyTextureQuadHandle HyAssets::CreateAuxiliaryTextureQuad(const std::string &sFilePath, HyTextureInfo textureInfo)
{
	std::vector<char> handleData(sFilePath.begin(), sFilePath.end());
	handleData.push_back(textureInfo.m_uiFiltering);
	handleData.push_back(textureInfo.m_uiFormat);
	handleData.push_back(textureInfo.m_uiFormatParam1);
	handleData.push_back(textureInfo.m_uiFormatParam2);
	
	HyAuxiliaryFileHandle hFileHandle = crc32_fast(handleData.data(), handleData.size());
	HyTextureQuadHandle hTexQuadHandle(0, hFileHandle);

	if(m_AuxiliaryTextureQuadMap.find(hTexQuadHandle) == m_AuxiliaryTextureQuadMap.end())
		m_AuxiliaryTextureQuadMap.insert({ hTexQuadHandle, HY_NEW HyTexturedQuadData(hFileHandle, sFilePath, textureInfo, *this) });

	return hTexQuadHandle;
}

HyAudioHandle HyAssets::CreateAuxiliaryAudio(const std::string &sFilePath, bool bIsStreamed, int32 iInstanceLimit, int32 iCategoryId)
{
	std::vector<char> handleData(sFilePath.begin(), sFilePath.end());
	handleData.push_back(bIsStreamed ? '1' : '0');
	handleData.push_back(static_cast<char>(iInstanceLimit & 0x000000FF));
	handleData.push_back(static_cast<char>((iInstanceLimit & 0x0000FF00) >> 8));
	handleData.push_back(static_cast<char>((iInstanceLimit & 0x00FF0000) >> 16));
	handleData.push_back(static_cast<char>((iInstanceLimit & 0xFF000000) >> 24));
	handleData.push_back(static_cast<char>(iCategoryId & 0x000000FF));
	handleData.push_back(static_cast<char>((iCategoryId & 0x0000FF00) >> 8));
	handleData.push_back(static_cast<char>((iCategoryId & 0x00FF0000) >> 16));
	handleData.push_back(static_cast<char>((iCategoryId & 0xFF000000) >> 24));

	HyAuxiliaryFileHandle hFileHandle = crc32_fast(handleData.data(), handleData.size());
	HyAudioHandle hAudioHandle(0, hFileHandle);

	if(m_AuxiliaryAudioMap.find(hAudioHandle) == m_AuxiliaryAudioMap.end())
		m_AuxiliaryAudioMap.insert({ hAudioHandle, HY_NEW HyAudioData(hFileHandle, sFilePath, bIsStreamed, iInstanceLimit, iCategoryId, *this) });

	return hAudioHandle;
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
		// should be doing the initial bootup 
		if (m_Load_Retrieval.size() >= 10)
		{
			while(m_Load_Retrieval.empty() == false)
			{
				IHyFile* pData = m_Load_Retrieval.front();
				m_Load_Retrieval.pop();

				rendererRef.TxData(pData);
			}
		}
		else
		{
			if (m_Load_Retrieval.empty() == false)
			{
				IHyFile* pData = m_Load_Retrieval.front();
				m_Load_Retrieval.pop();

				rendererRef.TxData(pData);
			}
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

	for(int i = 0; i < HYNUM_FILETYPES; ++i)
	{
		if(false == ParseManifestFile(static_cast<HyFileType>(i)))
		{
			HyFilesManifest::sm_iIndexFlagsArraySize[i] = 0;
			m_FilesMap[i].m_pLoadedManifest = HY_NEW HyFilesManifest(static_cast<HyFileType>(i));
		}
	}

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
	if(itemsDoc.HasMember("Sprites"))
		m_SpriteFactory.Init(itemsDoc["Sprites"].GetObject(), *this);
	if(itemsDoc.HasMember("Texts"))
		m_TextFactory.Init(itemsDoc["Texts"].GetObject(), *this);
	if(itemsDoc.HasMember("Spine"))
		m_SpineFactory.Init(itemsDoc["Spine"].GetObject(), *this);
	if(itemsDoc.HasMember("Prefabs"))
		m_PrefabFactory.Init(itemsDoc["Prefabs"].GetObject(), *this);
#endif

	// Atomic boolean indicated to main thread that we're initialized
	m_bInitialized = true;
}

/*virtual*/ void HyAssets::OnThreadUpdate() /*override*/
{
	bool bAllFinished = false;
	while(bAllFinished == false)
	{
		IHyFile *pFileToLoad = nullptr;

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Copy all the ptrs into their vectors to be processed, while emptying the shared queue
		m_Mutex.lock();
		
		if(m_Load_Shared.empty() == false)
		{
			pFileToLoad = m_Load_Shared.front();
			m_Load_Shared.pop();
		}

		if(m_Load_Shared.empty())
			bAllFinished = true;
		
		m_Mutex.unlock();

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Load everything that is enqueued (outside of any critical section)
		if(pFileToLoad == nullptr)
			continue;

		pFileToLoad->OnLoadThread();

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Copy all the (loaded) IData ptrs to the retrieval vector
		m_Mutex.lock();
		m_Load_Retrieval.push(pFileToLoad);
		m_Mutex.unlock();
	}
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
				HyTextureInfo texInfo(texObj["textureInfo"].GetUint());
				sAtlasFilePath += texInfo.GetFileExt();

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

			std::string sBankFilePath = GetDataDir() + HYASSETS_AudioDir;
			sprintf(szTmpBuffer, "%05d", uiBankId);
			sBankFilePath += szTmpBuffer;

			new (pPlacementLocation)HyFileAudio(sBankFilePath, uiBankId, i, bankObj, m_AudioCoreRef);
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
	if(pData->m_uiRefCount <= 0)
	{
		HyLogError("HyAssets::DequeData Tried to decrement a '0' reference");
		return;
	}

	pData->m_uiRefCount--;
	if(pData->m_uiRefCount == 0)
	{
		if(pData->m_eLoadState == HYLOADSTATE_Loaded)
		{
			pData->m_eLoadState = HYLOADSTATE_Discarded;

			if(pData->IsAuxiliary() == false)//m_FilesMap[pData->GetLoadableType()].m_pLoadedManifest)
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

			if(pData->IsAuxiliary() == false)// && m_FilesMap[pData->GetLoadableType()].m_pLoadedManifest)
			{
				m_FilesMap[pData->GetLoadableType()].m_pLoadedManifest->Set(static_cast<HyFileAtlas *>(pData)->GetManifestIndex());
				HyLogInfo(pData->AssetTypeName() << " loaded " << pData->GetAssetInfo());
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
				HyLogInfo(pData->AssetTypeName() << " reloading " << pData->GetAssetInfo());

				m_Load_Prepare.push(pData);

				bFoundInReloadList = true;
				m_ReloadDataList.erase(iter);
				break;
			}
		}

		if(bFoundInReloadList == false)
		{
			pData->m_eLoadState = HYLOADSTATE_Inactive;
			HyLogInfo(pData->AssetTypeName() << " deleted " << pData->GetAssetInfo());
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
				// Canceling the loading
				m_QueuedInstList.erase(it);
				m_uiLoadingCountTotal--;
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
	list.push_back(HYTEXTURE_Uncompressed);
	list.push_back(HYTEXTURE_DXT);
	list.push_back(HYTEXTURE_ASTC);
	
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
	case HYTEXTURE_Uncompressed:
		return "Uncompressed";
	case HYTEXTURE_DXT:
		return "DXT";
	case HYTEXTURE_ASTC:
		return "ASTC";
	
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
