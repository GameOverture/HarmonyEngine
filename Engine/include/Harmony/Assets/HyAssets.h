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

#define HYASSETS_DataFile "data.json"
#define HYASSETS_AtlasFile "atlas.json"
#define HYASSETS_AudioFile "audio.json"

#define HYASSETS_AtlasDir "Atlases/"
#define HYASSETS_PrefabDir "Prefabs/"
#define HYASSETS_AudioDir "Audio/"

class HyAudioManager;
class HyScene;
class IHyRenderer;
class IHyLoadable;
class IHyFile;
class IHyNodeData;
class HyEntityData;
class HyAudioData;
class HySprite2dData;
class HySpine2dData;
class HyText2dData;
class HyTexturedQuad2dData;
class HyPrefabData;
class HyFileAtlas;
class HyGLTF;
class HyFileAudio;

class HyAssets : public IHyThreadClass
{
	HyAudioManager &											m_AudioRef;
	HyScene &													m_SceneRef;
	const std::string											m_sDATADIR;
	std::atomic<bool>											m_bInitialized;

	HyFileAtlas *													m_pAtlases;
	uint32														m_uiNumAtlases;
	HyFilesManifest *											m_pLoadedAtlasIndices;

	HyFileAudio *												m_pAudioFiles;
	uint32														m_uiNumAudioFiles;
	HyFilesManifest *											m_pLoadedAudioManifest;

	std::map<std::string, HyGLTF *>								m_GltfMap;

	template<typename tData>
	class Factory
	{
		std::map<std::string, uint32>							m_LookupIndexMap;
		std::vector<tData>										m_DataList;

	public:
		void Init(const jsonxx::Object &subDirObjRef, HyAssets &assetsRef);
		const tData *GetData(const std::string &sPrefix, const std::string &sName) const;
	};
	Factory<HyAudioData>										m_AudioFactory;
	Factory<HyEntityData>										m_EntityFactory;
	Factory<HySprite2dData>										m_SpriteFactory;
	Factory<HyPrefabData>										m_PrefabFactory;
	Factory<HyText2dData>										m_TextFactory;
	std::map<std::pair<uint32, uint32>, HyTexturedQuad2dData *>	m_Quad2d;

	std::vector<IHyLoadable *>									m_QueuedInstList;
	std::vector<IHyFile *>									m_ReloadDataList;

	// Queues responsible for passing and retrieving factory data between the loading thread
	std::queue<IHyFile *>									m_Load_Prepare;
	std::queue<IHyFile *>									m_Load_Shared;
	std::queue<IHyFile *>									m_Load_Retrieval;

public:
	HyAssets(HyAudioManager &audioRef, HyScene &sceneRef, std::string sDataDirPath);
	virtual ~HyAssets();

	const std::string &GetDataDir();
	bool IsInitialized();

	HyAudioManager &GetAudioRef();

	HyFileAtlas *GetAtlas(uint32 uiMasterIndex);
	HyFileAtlas *GetAtlas(uint32 uiChecksum, HyRectangle<float> &UVRectOut);
	HyFileAtlas *GetAtlasUsingGroupId(uint32 uiAtlasGrpId, uint32 uiIndexInGroup);
	uint32 GetNumAtlases();
	HyFilesManifest *GetLoadedAtlases();

	HyFileAudio *GetAudioFile(uint32 uiManifestIndex);

	HyGLTF *GetGltf(const std::string &sIdentifier);

	void AcquireNodeData(IHyLoadable *pLoadable, const IHyNodeData *&pDataOut);
	void LoadNodeData(IHyLoadable *pLoadable);
	void RemoveNodeData(IHyLoadable *pLoadable);
	void SetEntityLoaded(IHyLoadable *pLoadable);
	bool IsInstLoaded(IHyLoadable *pLoadable);

	void Shutdown();
	bool IsShutdown();

	void Update(IHyRenderer &rendererRef);

protected:
	virtual void OnThreadInit() override;
	virtual void OnThreadUpdate() override;
	virtual void OnThreadShutdown() override;

private:
	void QueueData(IHyFile *pData);
	void DequeData(IHyFile *pData);
	void FinalizeData(IHyFile *pData);

	void SetAsLoaded(IHyLoadable *pLoadable);
	void SetAsUnloaded(IHyLoadable *pLoadable);
};

#endif /* HyAssets_h__ */
