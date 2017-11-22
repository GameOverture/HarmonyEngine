/**************************************************************************
 *	HyAssets.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2015 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyAssets_h__
#define HyAssets_h__

#include "Afx/HyStdAfx.h"
#include "Assets/Loadables/HyAtlas.h"
#include "Assets/Loadables/HyAtlasIndices.h"
#include "Scene/HyScene.h"
#include "Threading/Threading.h"

#include <queue>
#include <future>

class IHyRenderer;
class IHyDrawInst2d;
class IHyNodeData;
class HyAudioData;
class HySprite2dData;
class HySpine2dData;
class HyText2dData;
class HyTexturedQuad2dData;
class HyPrimitive2dData;
class HyMesh3dData;

void HyAssetInit(HyAssets *pThis);

class HyAssets
{
	const std::string											m_sDATADIR;

	std::future<void>											m_InitFuture;

	HyScene &													m_SceneRef;

	HyAtlas *													m_pAtlases;
	uint32														m_uiNumAtlases;
	HyAtlasIndices *											m_pLoadedAtlasIndices;

	template<typename tData>
	class NodeData
	{
		std::map<std::string, uint32>							m_LookupIndexMap;
		std::vector<tData>										m_DataList;

	public:
		void Init(jsonxx::Object &subDirObjRef, HyAssets &assetsRef);
		tData *GetData(const std::string &sPrefix, const std::string &sName);
	};
	NodeData<HyAudioData>										m_Audio;
	NodeData<HySprite2dData>									m_Sprite2d;
	NodeData<HySpine2dData>										m_Spine2d;
	NodeData<HyMesh3dData>										m_Mesh3d;
	NodeData<HyText2dData>										m_Txt2d;
	std::map<std::pair<uint32, uint32>, HyTexturedQuad2dData *>	m_Quad2d;

	std::vector<IHyDrawInst2d *>								m_QueuedInst2dList;
	std::vector<IHyLoadableData *>								m_ReloadDataList;

	// Queues responsible for passing and retrieving factory data between the loading thread
	std::queue<IHyLoadableData *>								m_Load_Prepare;
	std::queue<IHyLoadableData *>								m_Load_Shared;
	std::queue<IHyLoadableData *>								m_Load_Retrieval;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Thread control structure to help sync loading of factory data
	struct LoadThreadCtrl
	{
		std::queue<IHyLoadableData *> &							m_Load_SharedRef;
		std::queue<IHyLoadableData *> &							m_Load_RetrievalRef;

		WaitEvent												m_WaitEvent_HasNewData;
		BasicSection											m_csSharedQueue;
		BasicSection											m_csRetrievalQueue;

		
		HyThreadState											m_eState;

		LoadThreadCtrl(std::queue<IHyLoadableData *> &load_SharedRef,
					   std::queue<IHyLoadableData *> &Load_RetrievalRef) :	m_Load_SharedRef(load_SharedRef),
																			m_Load_RetrievalRef(Load_RetrievalRef),
																			m_WaitEvent_HasNewData(L"Thread Idler", true),
																			m_eState(HYTHREADSTATE_Run)
		{ }
	};
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	LoadThreadCtrl												m_LoadingCtrl;
	ThreadInfoPtr												m_pLoadingThread;	// Loading thread info pointer

public:
	HyAssets(std::string sDataDirPath, HyScene &sceneRef);
	virtual ~HyAssets();

	bool IsLoaded();
	void ParseInitInfo();

	HyAtlas *GetAtlas(uint32 uiMasterIndex);
	HyAtlas *GetAtlas(uint32 uiChecksum, HyRectangle<float> &UVRectOut);
	HyAtlas *GetAtlasUsingGroupId(uint32 uiAtlasGrpId, uint32 uiIndexInGroup);

	uint32 GetNumAtlases();
	HyAtlasIndices *GetLoadedAtlases();

	void GetNodeData(IHyDrawInst2d *pDrawInst2d, IHyNodeData *&pDataOut);
	void LoadNodeData(IHyDrawInst2d *pDrawInst2d);
	void RemoveNodeData(IHyDrawInst2d *pDrawInst2d);
	bool IsInstLoaded(IHyDrawInst2d *pDrawInst2d);

	void Shutdown();
	bool IsShutdown();

	void Update(IHyRenderer &rendererRef);

private:
	void QueueData(IHyLoadableData *pData);
	void DequeData(IHyLoadableData *pData);
	void FinalizeData(IHyLoadableData *pData);

	void SetInstAsLoaded(IHyDrawInst2d *pDrawInst2d);

	static void LoadingThread(void *pParam);
};

#endif /* HyAssets_h__ */
