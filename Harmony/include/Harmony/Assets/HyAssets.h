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
#include "Assets/Loadables/HyAtlas.h"
#include "Assets/Loadables/HyAtlasIndices.h"
#include "Assets/Loadables/HyGLTF.h"
#include "Threading/IHyThreadClass.h"

#define HYASSETS_DataFile "data.json"
#define HYASSETS_AtlasDir "Atlases/"
#define HYASSETS_PrefabDir "Prefabs/"

class IHyRenderer;
class IHyLoadable;
class IHyNodeData;
class HyAudioData;
class HySprite2dData;
class HySpine2dData;
class HyText2dData;
class HyTexturedQuad2dData;
class HyPrefabData;

class HyAssets : public IHyThreadClass
{
	const std::string											m_sDATADIR;
	std::atomic<bool>											m_bInitialized;

	HyAtlas *													m_pAtlases;
	uint32														m_uiNumAtlases;
	HyAtlasIndices *											m_pLoadedAtlasIndices;

	// TODO: make this a map since all node datas store a pointer
	std::vector<HyGLTF>											m_GltfList;

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
	Factory<HySprite2dData>										m_SpriteFactory;
	Factory<HySpine2dData>										m_SpineFactory;
	Factory<HyPrefabData>										m_PrefabFactory;
	Factory<HyText2dData>										m_FontFactory;
	std::map<std::pair<uint32, uint32>, HyTexturedQuad2dData *>	m_Quad2d;

	std::vector<IHyLoadable *>									m_QueuedInst2dList;
	std::vector<IHyLoadable *>									m_FullyLoadedList;
	std::vector<IHyLoadableData *>								m_ReloadDataList;

	// Queues responsible for passing and retrieving factory data between the loading thread
	std::queue<IHyLoadableData *>								m_Load_Prepare;
	std::queue<IHyLoadableData *>								m_Load_Shared;
	std::queue<IHyLoadableData *>								m_Load_Retrieval;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Thread control
	std::condition_variable										m_ConditionVariable;
	std::mutex													m_Mutex_ConditionVariable;
	bool														m_bProcessThread;

	std::mutex													m_Mutex_SharedQueue;
	std::mutex													m_Mutex_RetrievalQueue;
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:
	HyAssets(std::string sDataDirPath);
	virtual ~HyAssets();

	const std::string &GetDataDir();
	bool IsInitialized();

	HyAtlas *GetAtlas(uint32 uiMasterIndex);
	HyAtlas *GetAtlas(uint32 uiChecksum, HyRectangle<float> &UVRectOut);
	HyAtlas *GetAtlasUsingGroupId(uint32 uiAtlasGrpId, uint32 uiIndexInGroup);
	uint32 GetNumAtlases();
	HyAtlasIndices *GetLoadedAtlases();

	HyGLTF *GetGltf(const std::string &sIdentifier);

	void AcquireNodeData(IHyLoadable *pLoadable, const IHyNodeData *&pDataOut);
	void LoadNodeData(IHyLoadable *pLoadable);
	void RemoveNodeData(IHyLoadable *pLoadable);
	bool IsInstLoaded(IHyLoadable *pLoadable);

	void Shutdown();
	bool IsShutdown();

	void Update(IHyRenderer &rendererRef);

protected:
	virtual void OnThreadInit() override;
	virtual void OnThreadUpdate() override;
	virtual void OnThreadShutdown() override;

private:
	void QueueData(IHyLoadableData *pData);
	void DequeData(IHyLoadableData *pData);
	void FinalizeData(IHyLoadableData *pData);

	void SetInstAsLoaded(IHyLoadable *pLoadable);
};

#endif /* HyAssets_h__ */
