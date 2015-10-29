/**************************************************************************
 *	HyScene.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Scene/HyScene.h"

#include "Renderer/IHyRenderer.h"
#include "Renderer/HyGfxComms.h"
#include "Renderer/Viewport/HyWindow.h"

#include "Scene/Instances/IHyInst2d.h"
#include "Scene/Instances/HySound.h"
#include "Scene/Instances/HySprite2d.h"
#include "Scene/Instances/HySpine2d.h"
#include "Scene/Instances/HyPrimitive2d.h"
#include "Scene/Instances/HyText2d.h"
#include "Scene/Instances/HyTexturedQuad2d.h"
#include "Scene/HyEntity2d.h"
#include "Scene/HyPhysEntity2d.h"

#include "Time/IHyTime.h"

HyCoordinateType	HyScene::sm_eDefaultCoordType = HYCOORD_Default;
float				HyScene::sm_fPixelsPerMeter = 0.0f;
bool				HyScene::sm_bInst2dOrderingDirty = false;

HyScene::HyScene(HyGfxComms &gfxCommsRef, vector<HyWindow> &vWindowRef, HyCoordinateType eDefaultCoordType, float fPixelsPerMeter) :	m_b2World(b2Vec2(0.0f, -10.0f)),
																																		m_iPhysVelocityIterations(8),
																																		m_iPhysPositionIterations(3),
																																		m_GfxCommsRef(gfxCommsRef),
																																		m_vWindowRef(vWindowRef)
{
	sm_eDefaultCoordType = eDefaultCoordType;
	sm_fPixelsPerMeter = fPixelsPerMeter;

	m_b2World.SetDebugDraw(&m_DrawPhys2d);
	m_b2World.SetContactListener(&m_Phys2dContactListener);
	
	m_DrawPhys2d.SetFlags(b2Draw::e_shapeBit);// | b2Draw::e_jointBit | b2Draw::e_aabbBit | b2Draw::e_pairBit | b2Draw::e_centerOfMassBit);

	// Link HyScene to all classes that access it
	HyPhysEntity2d::sm_b2WorldRef = &m_b2World;
	HyAnimFloat::sm_pScene = this;
}

HyScene::~HyScene(void)
{
}

void HyScene::AddInstance(IHyInst2d *pInst)
{
	m_vLoadedInst2d.push_back(pInst);
	sm_bInst2dOrderingDirty = true;
}

void HyScene::RemoveInst(IHyInst2d *pInst)
{
	for(vector<IHyInst2d *>::iterator it = m_vLoadedInst2d.begin(); it != m_vLoadedInst2d.end(); ++it)
	{
		if((*it) == pInst)
		{
			// TODO: Log about erasing instance
			m_vLoadedInst2d.erase(it);
			break;
		}
	}
}

void HyScene::InsertActiveAnimFloat(HyAnimFloat *pAnimFloat)
{
	m_vActiveAnimFloats.push_back(pAnimFloat);
}



//PRIVATE//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HyScene::PreUpdate()
{
	m_b2World.Step(IHyTime::GetUpdateStepSeconds(), m_iPhysVelocityIterations, m_iPhysPositionIterations);

	// Update any currently active AnimFloat in the game, and remove any of them that are finished.
	for (vector<HyAnimFloat *>::iterator iter = m_vActiveAnimFloats.begin(); iter != m_vActiveAnimFloats.end(); )
	{
		if(!(*iter)->Update())
			iter = m_vActiveAnimFloats.erase(iter);
		else
			++iter;
	}


}

void HyScene::PostUpdate()
{
	if(sm_bInst2dOrderingDirty)
	{
		std::sort(m_vLoadedInst2d.begin(), m_vLoadedInst2d.end(), &Inst2dSortPredicate);
		sm_bInst2dOrderingDirty = false;
	}

	WriteDrawBuffers();
}



void HyScene::WriteDrawBuffers()
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// BUFFER HEADER (contains offsets from here)-| Num 3d Cams (4bytes)-|-Cam3d-|-Cam3d-|...|-Num 2d Cams (4bytes)-|-Cam2d-|-Cam2d-|...|-Num 3d Insts (4bytes)-|-Inst3d-|-Inst3d-|-Inst3d...-|-Num 2d Insts (4bytes)-|-Inst2d-|-Inst2d-|-Inst2d...-|-<possible blank/empty data since we skip non-enabled instances>-|-Vertex Data-
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// GET BUFFER HEADER (and write to its members as data offsets become known below)
	m_pCurWritePos = m_GfxCommsRef.GetWriteBufferPtr();
	
	HyGfxComms::tDrawHeader *pDrawHeader = new (m_pCurWritePos) HyGfxComms::tDrawHeader;
	pDrawHeader->uiReturnFlags = 0;
	m_pCurWritePos += sizeof(HyGfxComms::tDrawHeader);

	mat4 mtxView;
	uint32 uiNumWindows = static_cast<uint32>(m_vWindowRef.size());
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// WRITE 3d CAMERA(S) BUFFER
	pDrawHeader->uiOffsetToCameras3d = m_pCurWritePos - m_GfxCommsRef.GetWriteBufferPtr();
	char *pWriteNum3dCamsHere = m_pCurWritePos;
	m_pCurWritePos += sizeof(int32);
	
	int32 iCount = 0;
	for(uint32 i = 0; i < uiNumWindows; ++i)
	{
		uint32 uiNumCameras3d = static_cast<uint32>(m_vWindowRef[i].m_vCams3d.size());
		for(uint32 j = 0; j < uiNumCameras3d; ++j)
		{
			if(m_vWindowRef[i].m_vCams3d[j]->IsEnabled())
			{
				*(reinterpret_cast<uint32 *>(m_pCurWritePos)) = i;
				m_pCurWritePos += sizeof(uint32);

				*(reinterpret_cast<HyRectangle<float> *>(m_pCurWritePos)) = m_vWindowRef[i].m_vCams3d[j]->GetViewport();
				m_pCurWritePos += sizeof(HyRectangle<float>);
			
				HyError("GetLocalTransform_SRT should be 3d");
				m_vWindowRef[i].m_vCams3d[j]->GetLocalTransform_SRT(mtxView);
				*(reinterpret_cast<mat4 *>(m_pCurWritePos)) = mtxView;
				m_pCurWritePos += sizeof(mat4);

				iCount++;
			}
		}
	}
	*(reinterpret_cast<int32 *>(pWriteNum3dCamsHere)) = iCount;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// WRITE 2d CAMERA(S) BUFFER
	pDrawHeader->uiOffsetToCameras2d = m_pCurWritePos - m_GfxCommsRef.GetWriteBufferPtr();
	char *pWriteNum2dCamsHere = m_pCurWritePos;
	m_pCurWritePos += sizeof(int32);

	iCount = 0;
	for(uint32 i = 0; i < uiNumWindows; ++i)
	{
		uint32 uiNumCameras2d = m_vWindowRef[i].m_vCams2d.size();
		for(uint32 j = 0; j < uiNumCameras2d; ++j)
		{
			if(m_vWindowRef[i].m_vCams2d[j]->IsEnabled())
			{
				*(reinterpret_cast<uint32 *>(m_pCurWritePos)) = i;
				m_pCurWritePos += sizeof(uint32);

				*(reinterpret_cast<HyRectangle<float> *>(m_pCurWritePos)) = m_vWindowRef[i].m_vCams2d[j]->GetViewport();
				m_pCurWritePos += sizeof(HyRectangle<float>);

				m_vWindowRef[i].m_vCams2d[j]->GetLocalTransform_SRT(mtxView);
				*(reinterpret_cast<mat4 *>(m_pCurWritePos)) = mtxView;
				m_pCurWritePos += sizeof(mat4);

				iCount++;
			}
		}
	}
	*(reinterpret_cast<int32 *>(pWriteNum2dCamsHere)) = iCount;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// WRITE 3d DRAW BUFFER
	pDrawHeader->uiOffsetToInst3d = m_pCurWritePos - m_GfxCommsRef.GetWriteBufferPtr();
	char *pWriteNum3dInstsHere = m_pCurWritePos;
	m_pCurWritePos += sizeof(int32);

	iCount = 0;
	uint32 uiTotalNumInsts = static_cast<uint32>(m_vInst3d.size());
	for(uint32 i = 0; i < uiTotalNumInsts; ++i)
	{
		if(m_vInst3d[i]->IsEnabled())
		{
			// TODO: 
			//new (m_pCurWritePos) HyDrawText2d(reinterpret_cast<HyText2d *>(m_vLoadedInst2d[i]), uiVertexDataOffset, pCurVertexWritePos);
			//m_pCurWritePos += sizeof(HyDrawText2d);
			iCount++;
		}
	}
	*(reinterpret_cast<int32 *>(pWriteNum3dInstsHere)) = iCount;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// WRITE 2d DRAW BUFFER
	pDrawHeader->uiOffsetToInst2d = m_pCurWritePos - m_GfxCommsRef.GetWriteBufferPtr();
	char *pWriteNum2dInstsHere = m_pCurWritePos;
	m_pCurWritePos += sizeof(int32);

	iCount = 0;
	uiTotalNumInsts = m_vLoadedInst2d.size();

	char *pStartVertexWritePos = m_pCurWritePos + (uiTotalNumInsts * sizeof(HyRenderState));
	pDrawHeader->uiOffsetToVertexData2d = pStartVertexWritePos - m_GfxCommsRef.GetWriteBufferPtr();
	char *pCurVertexWritePos = pStartVertexWritePos;

	size_t	uiVertexDataOffset = 0;
	HyRenderState *pCurRenderState2d = NULL;

	for(size_t i = 0; i < uiTotalNumInsts; ++i)
	{
		if(m_vLoadedInst2d[i]->IsEnabled() == false)
			continue;

		m_vLoadedInst2d[i]->Update();

		// If previously written instance has equal render state by "operator ==" then it's to be assumed the instance data can be batched and doesn't need to write another render state
		if(pCurRenderState2d == NULL || false == (m_vLoadedInst2d[i]->GetRenderState() == *pCurRenderState2d))
		{
			// Start a new draw. Write render state to buffer to be sent to render thread
			memcpy(m_pCurWritePos, &m_vLoadedInst2d[i]->GetRenderState(), sizeof(HyRenderState));
			reinterpret_cast<HyRenderState *>(m_pCurWritePos)->SetDataOffset(uiVertexDataOffset);
			pCurRenderState2d = reinterpret_cast<HyRenderState *>(m_pCurWritePos);
			m_pCurWritePos += sizeof(HyRenderState);

			iCount++;
		}
		else
		{
			// This instance will be batched with the current render state
			pCurRenderState2d->AppendInstances(m_vLoadedInst2d[i]->GetRenderState().GetNumInstances());
		}
		
		// WriteDrawBufferData() is responsible for incrementing the draw pointer to after what's written
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
	//		iCount++;
	//	}
	//}

	*(reinterpret_cast<int32 *>(pWriteNum2dInstsHere)) = iCount;
	pDrawHeader->uiVertexBufferSize2d = pCurVertexWritePos - pStartVertexWritePos;

	// Do final check to see if we wrote passed our bounds
	HyAssert((m_pCurWritePos-m_GfxCommsRef.GetWriteBufferPtr()) < RENDER_BUFFER_SIZE, "HyGfxComms::WriteUpdateBuffer() has written passed its bounds! Embiggen 'RENDER_BUFFER_SIZE'");

	m_GfxCommsRef.Update_SetSharedPtrs();
}

/*static*/ bool HyScene::Inst2dSortPredicate(const IHyInst2d *pInst1, const IHyInst2d *pInst2)
{
	// TODO: Below is commented out because std::sort expects less-than operator to supply a transitive relationship, 
	//		 i.e. when the sort sees A < B is true and B < C is true, it implies that A < C is true as well.
	//
	//		...for some reason this fails that? 
	//	(testing something new, if no problem delete TODO)

	
	if(pInst1->GetDisplayOrder() == pInst2->GetDisplayOrder())
	{
		if(pInst1->GetRenderState() == pInst2->GetRenderState())
			return reinterpret_cast<const char *>(pInst1) > reinterpret_cast<const char *>(pInst2);

		return pInst1->GetRenderState() > pInst2->GetRenderState();
	}

	return pInst1->GetDisplayOrder() > pInst2->GetDisplayOrder();
}

