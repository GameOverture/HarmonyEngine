/**************************************************************************
 *	HyCreator.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Creator/HyCreator.h"

#include "Renderer/HyRenderer.h"
#include "Renderer/HyGfxComms.h"

#include "Creator/Instances/IObjInst2d.h"
#include "Creator/Instances/HySound.h"
#include "Creator/Instances/HySprite2d.h"
#include "Creator/Instances/HySpine2d.h"
#include "Creator/Instances/HyPrimitive2d.h"
#include "Creator/HyPhysEntity2d.h"
#include "Creator/Viewport/HyViewport.h"
#include "Creator/HyEntity2d.h"

#include "Time/ITimeApi.h"

HyCoordinateType	HyCreator::sm_eDefaultCoordType = HYCOORD_Default;
float				HyCreator::sm_fPixelsPerMeter = 0.0f;

HyCreator::HyCreator(HyGfxComms &gfxCommsRef, HyViewport &gameViewport, HyCoordinateType eDefaultCoordType, float fPixelsPerMeter) :	m_Sfx(HYINST_Sound2d),
																																		m_Sprite2d(HYINST_Sprite2d),
																																		m_Spine2d(HYINST_Spine2d),
																																		m_Txt2d(HYISNT_Text2d),
																																		m_Mesh3d(HYINST_Mesh3d),
																																		m_b2World(b2Vec2(0.0f, -10.0f)),
																																		m_iPhysVelocityIterations(8),
																																		m_iPhysPositionIterations(3),
																																		m_GfxCommsRef(gfxCommsRef),
																																		m_ViewportRef(gameViewport),
																																		m_bInst2dOrderingDirty(false)
{
	sm_eDefaultCoordType = eDefaultCoordType;
	sm_fPixelsPerMeter = fPixelsPerMeter;

	int yPosition = -1;

	m_b2World.SetDebugDraw(&m_DrawPhys2d);
	m_b2World.SetContactListener(&m_Phys2dContactListener);
	
	m_DrawPhys2d.SetFlags(b2Draw::e_shapeBit);// | b2Draw::e_jointBit | b2Draw::e_aabbBit | b2Draw::e_pairBit | b2Draw::e_centerOfMassBit);

	// Link HyCreator to all classes that access it
	HyPhysEntity2d::sm_b2WorldRef = &m_b2World;
	IObjInst2d::sm_pCtor = this;
	HyAnimFloat::sm_pCtor = this;

	// Start up Loading thread
	m_LoadingCtrl.m_pLoadQueue_Shared = &m_LoadQueue_Shared;
	m_LoadingCtrl.m_pLoadQueue_Retrieval = &m_LoadQueue_Retrieval;
	m_pLoadingThread = ThreadManager::Get()->BeginThread(_T("Loading Thread"), THREAD_START_PROCEDURE(LoadingThread), &m_LoadingCtrl);
}

HyCreator::~HyCreator(void)
{
}

void HyCreator::InsertActiveAnimFloat(HyAnimFloat *pAnimFloat)
{
	m_vActiveAnimFloats.push_back(pAnimFloat);
}

void HyCreator::LoadInst2d(IObjInst2d *pInst)
{
	IData *pLoadData = NULL;
	switch(pInst->GetInstType())
	{
	case HYINST_Sprite2d:
		pLoadData = m_Sprite2d.GetOrCreateData(pInst->GetPath());
		break;
	case HYINST_Spine2d:
		pLoadData = m_Spine2d.GetOrCreateData(pInst->GetPath());
		break;
	case HYISNT_Text2d:
		pLoadData = m_Txt2d.GetOrCreateData(pInst->GetPath());
		break;
	}

	pInst->SetData(pLoadData);

	if(pLoadData == NULL || pLoadData->GetLoadState() == HYLOADSTATE_Loaded)
	{
		pInst->SetLoaded();
		m_vLoadedInst2d.push_back(pInst);
		m_bInst2dOrderingDirty = true;

		if(pLoadData)
			pLoadData->IncRef();
	}
	else
	{
		m_vQueuedInst2d.push_back(pInst);

		if(pLoadData->GetLoadState() == HYLOADSTATE_Inactive)
		{
			pLoadData->SetLoadState(HYLOADSTATE_Queued);
			m_LoadQueue_Prepare.push(pLoadData);
		}
	}
}

void HyCreator::RemoveInst(IObjInst2d *pInst)
{
	switch(pInst->GetLoadState())
	{
	case HYLOADSTATE_Loaded:
		for(vector<IObjInst2d *>::iterator it = m_vLoadedInst2d.begin(); it != m_vLoadedInst2d.end(); ++it)
		{
			if((*it) == pInst)
			{
				IData *pInstData = pInst->GetData();
				if(pInstData && pInstData->DecRef())
					DiscardData(pInstData);

				// TODO: Log about erasing data
				m_vLoadedInst2d.erase(it);
				break;
			}
		}
		break;

	case HYLOADSTATE_Queued:
		for(vector<IObjInst2d *>::iterator it = m_vQueuedInst2d.begin(); it != m_vQueuedInst2d.end(); ++it)
		{
			if((*it) == pInst)
			{
				m_vQueuedInst2d.erase(it);
				break;
			}
		}
		break;

	default:
		HyError("HyCreator::RemoveInst() passed an invalid HyLoadState");
	}
}

//PRIVATE//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HyCreator::PreUpdate()
{
	m_b2World.Step(ITimeApi::GetUpdateStepSeconds(), m_iPhysVelocityIterations, m_iPhysPositionIterations);

	// Update any currently active AnimFloat in the game, and remove any of them that are finished.
	for (vector<HyAnimFloat *>::iterator iter = m_vActiveAnimFloats.begin(); iter != m_vActiveAnimFloats.end(); )
	{
		if(!(*iter)->Update())
			iter = m_vActiveAnimFloats.erase(iter);
		else
			++iter;
	}
}

void HyCreator::PostUpdate()
{
	UpdateLoading();
	WriteDrawBuffers();
}

void HyCreator::UpdateLoading()
{
	// Check to see if we have any pending loads to make
	if(m_LoadQueue_Prepare.empty() == false)
	{
		// Copy load queue data into shared data
		m_LoadingCtrl.m_csSharedQueue.Lock();
		{
			while(m_LoadQueue_Prepare.empty() == false)
			{
				m_LoadQueue_Shared.push(m_LoadQueue_Prepare.front());
				m_LoadQueue_Prepare.pop();
			}
		}
		m_LoadingCtrl.m_csSharedQueue.Unlock();

		m_LoadingCtrl.m_WaitEvent_HasNewData.Set();
	}

	// Check to see if any loaded data (from the load thread) is ready to go
	m_LoadingCtrl.m_csRetrievalQueue.Lock();
	{
		while(m_LoadQueue_Retrieval.empty() == false)
		{
			IData *pData = m_LoadQueue_Retrieval.front();
			m_LoadQueue_Retrieval.pop();

			if(pData->GetType() != HYINST_Sound2d)
				m_GfxCommsRef.Update_SendData(pData);
			else
				OnDataLoaded(pData);
		}
	}
	m_LoadingCtrl.m_csRetrievalQueue.Unlock();

	// Grab and process any returning IData's from the Render thread
	m_pGfxQueue_Retrieval = m_GfxCommsRef.Update_RetrieveData();
	while(!m_pGfxQueue_Retrieval->empty())
	{
		IData *pData = m_pGfxQueue_Retrieval->front();
		m_pGfxQueue_Retrieval->pop();

		if(pData->GetLoadState() == HYLOADSTATE_Queued)
			OnDataLoaded(pData);
		else
			DeleteData(pData);
	}
}

void HyCreator::OnDataLoaded(IData *pData)
{
	bool bDataIsUsed = false;
	for (vector<IObjInst2d *>::iterator iter = m_vQueuedInst2d.begin(); iter != m_vQueuedInst2d.end(); )
	{
		if((*iter)->GetData() == pData)
		{
			pData->IncRef();
			(*iter)->SetLoaded();
			m_vLoadedInst2d.push_back(*iter);
			
			bDataIsUsed = true;

			iter = m_vQueuedInst2d.erase(iter);
		}
		else
			++iter;
	}
	
	if(bDataIsUsed)
	{
		m_bInst2dOrderingDirty = true;
		pData->SetLoadState(HYLOADSTATE_Loaded);
	}
	else
		DiscardData(pData);
}

void HyCreator::DiscardData(IData *pData)
{
	HyAssert(pData->GetRefCount() <= 0, "HyCreator::DeleteData() tried to remove an IData with active references");

	pData->SetLoadState(HYLOADSTATE_Discarded);

	if(pData->GetType() != HYINST_Sound2d)
		m_GfxCommsRef.Update_SendData(pData);
	else
		DeleteData(pData);
}

void HyCreator::DeleteData(IData *pData)
{
	HyAssert(pData->GetRefCount() <= 0, "HyCreator::DeleteData() tried to delete an IData with active references");

	switch(pData->GetType())
	{
	case HYINST_Sound2d:	m_Sfx.DeleteData(reinterpret_cast<HySfxData *>(pData));			break;
	case HYINST_Sprite2d:	m_Sprite2d.DeleteData(reinterpret_cast<HySprite2dData *>(pData));	break;
	case HYINST_Spine2d:	m_Spine2d.DeleteData(reinterpret_cast<HySpine2dData *>(pData));	break;
	case HYISNT_Text2d:		m_Txt2d.DeleteData(reinterpret_cast<HyText2dData *>(pData));	break;
	}
}

void HyCreator::WriteDrawBuffers()
{
	if(m_bInst2dOrderingDirty)
	{
		std::sort(m_vLoadedInst2d.begin(), m_vLoadedInst2d.end(), &Inst2dSortPredicate);
		m_bInst2dOrderingDirty = false;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// BUFFER HEADER (contains offsets from here)-| Num 3d Cams (4bytes)-|-Cam3d-|-Cam3d-|...|-Num 2d Cams (4bytes)-|-Cam2d-|-Cam2d-|...|-Num 3d Insts (4bytes)-|-Ent3d-|-Ent3d-|-Ent3d...-|-Num 2d Insts (4bytes)-|-Inst2d-|-Inst2d-|-Inst2d...-|-<possible empty data (skipping non-visible Ents)>-|-Vertex Data-
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// GET BUFFER HEADER (and write to its members as data offsets become known below)
	m_pCurWritePos = m_GfxCommsRef.GetWriteBufferPtr();
	
	HyGfxComms::tDrawHeader *pDrawHeader = new (m_pCurWritePos) HyGfxComms::tDrawHeader;
	pDrawHeader->uiReturnFlags = 0;
	m_pCurWritePos += sizeof(HyGfxComms::tDrawHeader);

	mat4 mtxView;
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// WRITE 3d CAMERA(S) BUFFER
	pDrawHeader->uiOffsetToCameras3d = m_pCurWritePos - m_GfxCommsRef.GetWriteBufferPtr();
	char *pWriteNum3dCamsHere = m_pCurWritePos;
	m_pCurWritePos += sizeof(int32);
	
	int32 iNumInsts = 0;
	uint32 iTotalNumInsts = m_ViewportRef.m_vCams3d.size();
	for(uint32 i = 0; i < iTotalNumInsts; ++i)
	{
		if(m_ViewportRef.m_vCams3d[i]->IsEnabled())
		{
			*(reinterpret_cast<HyRectangle *>(m_pCurWritePos)) = m_ViewportRef.m_vCams3d[i]->GetRenderRect();
			m_pCurWritePos += sizeof(HyRectangle);
			
			HyError("GetLocalTransform_SRT should be 3d");
			m_ViewportRef.m_vCams3d[i]->GetLocalTransform_SRT(mtxView);
			*(reinterpret_cast<mat4 *>(m_pCurWritePos)) = mtxView;
			m_pCurWritePos += sizeof(mat4);

			iNumInsts++;
		}
	}
	*(reinterpret_cast<int32 *>(pWriteNum3dCamsHere)) = iNumInsts;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// WRITE 2d CAMERA(S) BUFFER
	pDrawHeader->uiOffsetToCameras2d = m_pCurWritePos - m_GfxCommsRef.GetWriteBufferPtr();
	char *pWriteNum2dCamsHere = m_pCurWritePos;
	m_pCurWritePos += sizeof(int32);

	iNumInsts = 0;
	iTotalNumInsts = m_ViewportRef.m_vCams2d.size();
	for(uint32 i = 0; i < iTotalNumInsts; ++i)
	{
		if(m_ViewportRef.m_vCams2d[i]->IsEnabled())
		{
			*(reinterpret_cast<HyRectangle *>(m_pCurWritePos)) = m_ViewportRef.m_vCams2d[i]->GetRenderRect();
			m_pCurWritePos += sizeof(HyRectangle);

			m_ViewportRef.m_vCams2d[i]->GetLocalTransform_SRT(mtxView);
			*(reinterpret_cast<mat4 *>(m_pCurWritePos)) = mtxView;
			m_pCurWritePos += sizeof(mat4);

			iNumInsts++;
		}
	}
	*(reinterpret_cast<int32 *>(pWriteNum2dCamsHere)) = iNumInsts;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// WRITE 3d DRAW BUFFER
	pDrawHeader->uiOffsetToInst3d = m_pCurWritePos - m_GfxCommsRef.GetWriteBufferPtr();
	char *pWriteNum3dInstsHere = m_pCurWritePos;
	m_pCurWritePos += sizeof(int32);

	iNumInsts = 0;
	iTotalNumInsts = m_vInst3d.size();
	for(uint32 i = 0; i < iTotalNumInsts; ++i)
	{
		if(m_vInst3d[i]->IsEnabled())
		{
			// TODO: 
			//new (m_pCurWritePos) HyDrawText2d(reinterpret_cast<HyText2d *>(m_vLoadedInst2d[i]), uiVertexDataOffset, pCurVertexWritePos);
			//m_pCurWritePos += sizeof(HyDrawText2d);
			iNumInsts++;
		}
	}
	*(reinterpret_cast<int32 *>(pWriteNum3dInstsHere)) = iNumInsts;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// WRITE 2d DRAW BUFFER
	pDrawHeader->uiOffsetToInst2d = m_pCurWritePos - m_GfxCommsRef.GetWriteBufferPtr();
	char *pWriteNum2dInstsHere = m_pCurWritePos;
	m_pCurWritePos += sizeof(int32);

	iNumInsts = 0;
	iTotalNumInsts = m_vLoadedInst2d.size();

	char *pStartVertexWritePos = m_pCurWritePos + (iTotalNumInsts * sizeof(HyRenderState));
	pDrawHeader->uiOffsetToVertexData2d = pStartVertexWritePos - m_GfxCommsRef.GetWriteBufferPtr();
	char *pCurVertexWritePos = pStartVertexWritePos;

	uint32	uiVertexDataOffset = 0;
	HyRenderState *pCurRenderState2d = NULL;

	for(uint32 i = 0; i < iTotalNumInsts; ++i)
	{
		if(m_vLoadedInst2d[i]->IsEnabled() == false)
			continue;

		m_vLoadedInst2d[i]->Update();

		// If previously written instance has equal render state by "operator ==" then it's to be assumed the instance data can be batched and doesn't need to write another render state
		if(pCurRenderState2d == NULL || false == (m_vLoadedInst2d[i]->GetRenderState() == *pCurRenderState2d))
		{
			memcpy(m_pCurWritePos, &m_vLoadedInst2d[i]->GetRenderState(), sizeof(HyRenderState));
			reinterpret_cast<HyRenderState *>(m_pCurWritePos)->SetDataOffset(uiVertexDataOffset);
			m_pCurWritePos += sizeof(HyRenderState);

			iNumInsts++;
		}
		
		m_vLoadedInst2d[i]->WriteDrawBufferData(pCurVertexWritePos);
		uiVertexDataOffset = pCurVertexWritePos - pStartVertexWritePos;
	}

	//if(m_DrawPhys2d.IsDrawEnabled())
	//{
	//	m_DrawPhys2d.Reset();
	//	m_b2World.DrawDebugData();

	//	uint32 uiNumPhysDraws = m_DrawPhys2d.GetNumPhysDraws();
	//	for(uint32 i = 0; i < uiNumPhysDraws; ++i)
	//	{
	//		new (m_pCurWritePos) HyDrawPrimitive2d(*m_DrawPhys2d.GetInstPtr(i)->GetPrimitive(), uiVertexDataOffset, pCurVertexWritePos);
	//		m_pCurWritePos += sizeof(HyDrawPrimitive2d);

	//		uiVertexDataOffset = pCurVertexWritePos - pStartVertexWritePos;
	//		iNumInsts++;
	//	}
	//}

	*(reinterpret_cast<int32 *>(pWriteNum2dInstsHere)) = iNumInsts;
	pDrawHeader->uiVertexBufferSize2d = pCurVertexWritePos - pStartVertexWritePos;

	// Do final check to see if we wrote passed our bounds
	HyAssert((m_pCurWritePos-m_GfxCommsRef.GetWriteBufferPtr()) < RENDER_BUFFER_SIZE, "HyGfxComms::WriteUpdateBuffer() has written passed its bounds! Embiggen 'RENDER_BUFFER_SIZE'");

	m_GfxCommsRef.Update_SetSharedPtrs();
}

/*static*/ void HyCreator::LoadingThread(void *pParam)
{
	LoadThreadCtrl *pLoadingCtrl = reinterpret_cast<LoadThreadCtrl *>(pParam);
	vector<IData *>	vCurLoadData;

	while(true)
	{
		// Wait idle indefinitely until there is new data to be grabbed
		pLoadingCtrl->m_WaitEvent_HasNewData.Wait();

		// Reset the event so we wait the next time we loop
		pLoadingCtrl->m_WaitEvent_HasNewData.Reset();
		
		// Copy all the IData ptrs into the 'vCurLoadData' to be processed, while emptying the shared queue
		pLoadingCtrl->m_csSharedQueue.Lock();
		{
			while(pLoadingCtrl->m_pLoadQueue_Shared->empty() == false)
			{
				vCurLoadData.push_back(pLoadingCtrl->m_pLoadQueue_Shared->front());
				pLoadingCtrl->m_pLoadQueue_Shared->pop();
			}
		}
		pLoadingCtrl->m_csSharedQueue.Unlock();

		// Load everything that is enqueued (outside of any critical section)
		for(uint32 i = 0; i < vCurLoadData.size(); ++i)
			vCurLoadData[i]->DoFileLoad();
		
		// Copy all the (loaded) IData ptrs to the retrieval vector
		pLoadingCtrl->m_csRetrievalQueue.Lock();
		{
			for(uint32 i = 0; i < vCurLoadData.size(); ++i)
				pLoadingCtrl->m_pLoadQueue_Retrieval->push(vCurLoadData[i]);
		}
		pLoadingCtrl->m_csRetrievalQueue.Unlock();

		vCurLoadData.clear();
	}
}

/*static*/ bool HyCreator::Inst2dSortPredicate(const IObjInst2d *pInst1, const IObjInst2d *pInst2)
{
	if(pInst1->GetDisplayOrder() == pInst2->GetDisplayOrder())
		return pInst1->GetRenderState() < pInst2->GetRenderState();

	return pInst1->GetDisplayOrder() < pInst2->GetDisplayOrder();
}

