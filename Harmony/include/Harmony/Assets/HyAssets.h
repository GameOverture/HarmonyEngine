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
#include "Utilities/HyMath.h"

#define HYASSETS_DataFile "data.json"
#define HYASSETS_AtlasFile "atlas.json"
#define HYASSETS_AudioFile "audio.json"

#define HYASSETS_AtlasDir "Atlases/"
#define HYASSETS_PrefabDir "Prefabs/"
#define HYASSETS_AudioDir "Audio/"

#if defined(HY_PLATFORM_WINDOWS)
	#define HYASSETS_AudioDirPlatform "Desktop/"
#else
	#define HYASSETS_AudioDirPlatform ""
#endif

class HyScene;
class IHyRenderer;
class IHyLoadable;
class IHyFileData;
class IHyNodeData;
class HyEntityData;
class HyAudioData;
class HySprite2dData;
class HySpine2dData;
class HyText2dData;
class HyTexturedQuad2dData;
class HyPrefabData;
class HyAtlas;
class HyAtlasIndices;
class HyGLTF;
class HyAudioBank;

class HyAssets : public IHyThreadClass
{
	HyScene &													m_SceneRef;
	const std::string											m_sDATADIR;
	std::atomic<bool>											m_bInitialized;

	HyAtlas *													m_pAtlases;
	uint32														m_uiNumAtlases;
	HyAtlasIndices *											m_pLoadedAtlasIndices;

	std::map<std::string, HyAudioBank *>						m_AudioBankMap;
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
	Factory<HyText2dData>										m_FontFactory;
	std::map<std::pair<uint32, uint32>, HyTexturedQuad2dData *>	m_Quad2d;

	std::vector<IHyLoadable *>									m_QueuedInstList;
	std::vector<IHyFileData *>									m_ReloadDataList;

	// Queues responsible for passing and retrieving factory data between the loading thread
	std::queue<IHyFileData *>									m_Load_Prepare;
	std::queue<IHyFileData *>									m_Load_Shared;
	std::queue<IHyFileData *>									m_Load_Retrieval;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Thread control
	std::mutex													m_Mutex;
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:
	HyAssets(HyScene &sceneRef, std::string sDataDirPath);
	virtual ~HyAssets();

	const std::string &GetDataDir();
	bool IsInitialized();

	HyAtlas *GetAtlas(uint32 uiMasterIndex);
	HyAtlas *GetAtlas(uint32 uiChecksum, HyRectangle<float> &UVRectOut);
	HyAtlas *GetAtlasUsingGroupId(uint32 uiAtlasGrpId, uint32 uiIndexInGroup);
	uint32 GetNumAtlases();
	HyAtlasIndices *GetLoadedAtlases();

	HyGLTF *GetGltf(const std::string &sIdentifier);

	HyAudioBank *GetAudioBank(const std::string &sBankName);

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
	void QueueData(IHyFileData *pData);
	void DequeData(IHyFileData *pData);
	void FinalizeData(IHyFileData *pData);

	void SetAsLoaded(IHyLoadable *pLoadable);
	void SetAsUnloaded(IHyLoadable *pLoadable);
};

#endif /* HyAssets_h__ */
