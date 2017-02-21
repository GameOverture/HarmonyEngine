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

class IHyNodeData;
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
	class NodeData
	{
		std::map<std::string, uint32>					m_LookupIndexMap;
		std::vector<tData>								m_DataList;

	public:
		void Init(jsonxx::Object &subDirObjRef, HyAssets &assetsRef);
		tData *GetData(const std::string &sPrefix, const std::string &sName);
	};
	NodeData<HyAudioData>								m_Audio;
	NodeData<HySprite2dData>							m_Sprite2d;
	NodeData<HySpine2dData>								m_Spine2d;
	NodeData<HyMesh3dData>								m_Mesh3d;
	NodeData<HyText2dData>								m_Txt2d;
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

	void GetNodeData(IHyDraw2d *pDrawNode, IHyNodeData *&pDataOut);
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
