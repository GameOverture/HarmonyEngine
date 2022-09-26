/**************************************************************************
 *	HyAssets.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2015 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyAssets_h__
#define HyAssets_h__

#include "Afx/HyStdAfx.h"
#include "Threading/IHyThreadClass.h"
#include "Assets/Files/HyFilesManifest.h"
#include "Utilities/HyMath.h"
#include "Utilities/HyJson.h"

#define HYASSETS_DataFile "Items.data"
#define HYASSETS_AtlasFile "Atlases.data"
#define HYASSETS_AudioFile "Audio.data"
#define HYASSETS_TranslationsFile "Translations.data"

#define HYASSETS_AtlasDir "Atlases/"
#define HYASSETS_PrefabDir "Prefabs/"
#define HYASSETS_AudioDir "Audio/"
#define HYASSETS_SpineDir "Spine/"

class HyAudioCore;
class HyScene;
class IHyRenderer;
class IHyLoadable;
class IHyFile;
class IHyNodeData;
class HyEntityData;
class HyAudioData;
class HySpriteData;
class HySpineData;
class HyTextData;
class HyTexturedQuadData;
class HyPrefabData;
class HyFileAtlas;
class HyGLTF;
class HyFileAudio;

class HyAssets : public IHyThreadClass
{
	HyAudioCore &												m_AudioCoreRef;
	HyScene &													m_SceneRef;
	const std::string											m_sDATADIR;
	std::atomic<bool>											m_bInitialized;

	struct FilesMap {
		IHyFile *												m_pFiles = nullptr;
		uint32													m_uiNumFiles = 0;
		HyFilesManifest *										m_pLoadedManifest = nullptr;
	};
	FilesMap													m_FilesMap[HYNUM_FILETYPES];

	std::map<std::string, HyGLTF *>								m_GltfMap;

	template<typename tData>
	class Factory
	{
		std::map<std::string, uint32>							m_LookupIndexMap;
		std::vector<tData>										m_DataList;

	public:
		void Init(HyJsonObj subDirObjRef, HyAssets &assetsRef);
		const tData *GetData(const std::string &sPrefix, const std::string &sName) const;
	};
	Factory<HyAudioData>										m_AudioFactory;
	Factory<HySpriteData>										m_SpriteFactory;
	Factory<HyTextData>											m_TextFactory;
	Factory<HySpineData>										m_SpineFactory;
	Factory<HyPrefabData>										m_PrefabFactory;
	std::map<std::pair<uint32, uint32>, HyTexturedQuadData *>	m_Quad2d;

	std::vector<IHyLoadable *>									m_QueuedInstList;
	std::vector<IHyFile *>										m_ReloadDataList;

	// Queues responsible for passing and retrieving factory data between the loading thread
	std::queue<IHyFile *>										m_Load_Prepare;
	std::queue<IHyFile *>										m_Load_Shared;
	std::queue<IHyFile *>										m_Load_Retrieval;
	uint32														m_uiLoadingCountTotal;	// Used to determine best guess at % loaded of all queued assets [0.0 - 1.0]

public:
	HyAssets(HyAudioCore &audioCoreRef, HyScene &sceneRef, std::string sDataDirPath);
	virtual ~HyAssets();

	const std::string &GetDataDir();
	bool IsInitialized();

	IHyFile *GetFile(HyFileType eFileType, uint32 uiManifestIndex);
	IHyFile *GetFileWithAsset(HyFileType eFileType, uint32 uiAssetChecksum);

	HyFileAtlas *GetAtlas(uint32 uiChecksum, HyRectangle<float> &UVRectOut);
	HyFileAtlas *GetAtlasUsingGroupId(uint32 uiAtlasGrpId, uint32 uiIndexInGroup);
	uint32 GetNumAtlases();
	HyFilesManifest *GetLoadedAtlases();

	HyGLTF *GetGltf(const std::string &sIdentifier);

	void AcquireNodeData(IHyLoadable *pLoadable, const IHyNodeData *&pDataOut);
	void LoadNodeData(IHyLoadable *pLoadable);
	void RemoveNodeData(IHyLoadable *pLoadable);
	void SetEntityLoaded(IHyLoadable *pEntity);
	bool IsInstLoaded(IHyLoadable *pLoadable);

	void GetNodeLoadingStatus(uint32 &uiNumQueuedOut, uint32 &uiTotalOut) const;

	void Shutdown();
	bool IsShutdown();

	void Update(IHyRenderer &rendererRef);

protected:
	virtual void OnThreadInit() override;
	virtual void OnThreadUpdate() override;
	virtual void OnThreadShutdown() override;

private:
	bool ParseManifestFile(HyFileType eFileType);

	void QueueData(IHyFile *pData);
	void DequeData(IHyFile *pData);
	void FinalizeData(IHyFile *pData);

	void SetAsLoaded(IHyLoadable *pLoadable);
	void SetAsUnloaded(IHyLoadable *pLoadable);

public:
	static std::vector<HyTextureFormat> GetTextureFormatList();
	static std::vector<std::string> GetTextureFormatNameList();
	static std::string GetTextureFormatName(HyTextureFormat eType);
	static HyTextureFormat GetTextureFormatFromString(std::string sFormat);

	static std::vector<HyTextureFiltering> GetTextureFilteringList();
	static std::vector<std::string> GetTextureFilteringNameList();
	static std::string GetTextureFilteringName(HyTextureFiltering eType);
	static HyTextureFiltering GetTextureFilteringFromString(std::string sFilter);
};

#endif /* HyAssets_h__ */
