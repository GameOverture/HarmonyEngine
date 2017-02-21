/**************************************************************************
 *	HyAssets.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2015 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __IHyAssets_h__
#define __IHyAssets_h__

#include "Afx/HyStdAfx.h"

#include "Assets/Loadables/HyAtlasGroup.h"

#include "Renderer/Components/HyGfxComms.h"
#include "Scene/HyScene.h"

#include "Threading/Threading.h"

#include <queue>

class IHyDraw2d;

class IHyData;
class HyAudioData;
class HySprite2dData;
class HySpine2dData;
class HyText2dData;
class HyTexturedQuad2dData;
class HyPrimitive2dData;
class HyMesh3dData;

class HyAssets
{
	const std::string									m_sDATADIR;

	HyGfxComms &										m_GfxCommsRef;
	HyScene &											m_SceneRef;

	HyAtlasGroup *										m_pAtlasGroups;
	uint32												m_uiNumAtlasGroups;

	template<typename tData>
	class HyNodeDataContainer
	{
		std::map<std::string, uint32>	m_LookupIndexMap;
		std::vector<tData>				m_DataList;

	public:
		HyNodeDataContainer()
		{ }

		~HyNodeDataContainer()
		{ }

		void Init(jsonxx::Object &subDirObjRef, HyAssets &assetsRef)
		{
			m_DataList.reserve(subDirObjRef.size());

			uint32 i = 0;
			for(auto iter = subDirObjRef.kv_map().begin(); iter != subDirObjRef.kv_map().end(); ++iter, ++i)
			{
				std::string sPath = MakeStringProperPath(iter->first.c_str(), nullptr, true);
				m_LookupIndexMap.insert(std::make_pair(sPath, i));

				m_DataList.emplace_back(iter->first, subDirObjRef.get<jsonxx::Value>(iter->first), assetsRef);
			}
		}

		tData *GetData(const std::string &sPrefix, const std::string &sName)
		{
			std::string sPath;

			if(sPrefix.empty() == false)
				sPath += MakeStringProperPath(sPrefix.c_str(), "/", true);

			sPath += sName;
			sPath = MakeStringProperPath(sPath.c_str(), nullptr, true);

			auto iter = m_LookupIndexMap.find(sPath);
			HyAssert(iter != m_LookupIndexMap.end(), "Could not find data: " << sPath.c_str());

			return &m_DataList[iter->second];
		}
	};
	HyNodeDataContainer<HyAudioData>					m_Audio;
	HyNodeDataContainer<HySprite2dData>					m_Sprite2d;
	HyNodeDataContainer<HySpine2dData>					m_Spine2d;
	HyNodeDataContainer<HyMesh3dData>					m_Mesh3d;
	HyNodeDataContainer<HyText2dData>					m_Txt2d;
	std::map<int32, HyTexturedQuad2dData *>				m_Quad2d;

	IHyLoadableData *									m_pLastQueuedData;
	IHyLoadableData *									m_pLastDiscardedData;
	std::vector<IHyDraw2d *>							m_QueuedInst2dList;

	// Queues responsible for passing and retrieving factory data between the loading thread
	std::queue<IHyLoadableData *>						m_Load_Prepare;
	std::queue<IHyLoadableData *>						m_Load_Shared;
	std::queue<IHyLoadableData *>						m_Load_Retrieval;

	std::queue<IHyLoadableData *> *						m_pGfxQueue_Retrieval;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Thread control structure to help sync loading of factory data
	struct LoadThreadCtrl
	{
		std::queue<IHyLoadableData *> &		m_Load_SharedRef;
		std::queue<IHyLoadableData *> &		m_Load_RetrievalRef;

		WaitEvent							m_WaitEvent_HasNewData;
		BasicSection						m_csSharedQueue;
		BasicSection						m_csRetrievalQueue;

		enum eState
		{
			STATE_Run = 0,
			STATE_ShouldExit,
			STATE_HasExited
		};
		eState				m_eState;

		LoadThreadCtrl(std::queue<IHyLoadableData *> &load_SharedRef,
					   std::queue<IHyLoadableData *> &Load_RetrievalRef) :	m_Load_SharedRef(load_SharedRef),
																			m_Load_RetrievalRef(Load_RetrievalRef),
																			m_WaitEvent_HasNewData(L"Thread Idler", true),
																			m_eState(STATE_Run)
		{ }
	};
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	LoadThreadCtrl										m_LoadingCtrl;
	ThreadInfoPtr										m_pLoadingThread;	// Loading thread info pointer

public:
	HyAssets(std::string sDataDirPath, HyGfxComms &gfxCommsRef, HyScene &sceneRef);
	virtual ~HyAssets();

	HyAtlasGroup *GetAtlasGroup(uint32 uiAtlasGroupId);
	std::string GetTexturePath(uint32 uiAtlasGroupId, uint32 uiTextureIndex);

	void GetNodeData(IHyDraw2d *pDrawNode, IHyData *&pDataOut);
	void LoadGfxData(IHyDraw2d *pDraw2d);
	void RemoveGfxData(IHyDraw2d *pDraw2d);

	void Shutdown();
	bool IsShutdown();

	void Update();

private:
	void QueueData(IHyLoadableData *pData);
	void DequeData(IHyLoadableData *pData);
	void FinalizeData(IHyLoadableData *pData);

	static void LoadingThread(void *pParam);
};

#endif /* __IHyAssets_h__ */
