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

#include "Renderer/IHyRenderer.h"
#include "Renderer/HyGfxComms.h"
#include "Renderer/Viewport/HyViewport.h"

#include "Creator/Instances/IHyInst2d.h"
#include "Creator/Instances/HySound.h"
#include "Creator/Instances/HySprite2d.h"
#include "Creator/Instances/HySpine2d.h"
#include "Creator/Instances/HyPrimitive2d.h"
#include "Creator/Instances/HyText2d.h"
#include "Creator/Instances/HyTexturedQuad2d.h"
#include "Creator/HyEntity2d.h"
#include "Creator/HyPhysEntity2d.h"

#include "Time/IHyTime.h"

HyCoordinateType	HyCreator::sm_eDefaultCoordType = HYCOORD_Default;
float				HyCreator::sm_fPixelsPerMeter = 0.0f;

HyCreator::HyCreator(HyGfxComms &gfxCommsRef, HyViewport &gameViewport, HyCoordinateType eDefaultCoordType, float fPixelsPerMeter) :	m_b2World(b2Vec2(0.0f, -10.0f)),
																																		m_iPhysVelocityIterations(8),
																																		m_iPhysPositionIterations(3),
																																		m_GfxCommsRef(gfxCommsRef),
																																		m_ViewportRef(gameViewport),
																																		m_bInst2dOrderingDirty(false)
{
	sm_eDefaultCoordType = eDefaultCoordType;
	sm_fPixelsPerMeter = fPixelsPerMeter;

	m_b2World.SetDebugDraw(&m_DrawPhys2d);
	m_b2World.SetContactListener(&m_Phys2dContactListener);
	
	m_DrawPhys2d.SetFlags(b2Draw::e_shapeBit);// | b2Draw::e_jointBit | b2Draw::e_aabbBit | b2Draw::e_pairBit | b2Draw::e_centerOfMassBit);

	// Link HyCreator to all classes that access it
	HyPhysEntity2d::sm_b2WorldRef = &m_b2World;

	HyAnimFloat::sm_pCtor = this;
}

HyCreator::~HyCreator(void)
{
}

void HyCreator::InsertActiveAnimFloat(HyAnimFloat *pAnimFloat)
{
	m_vActiveAnimFloats.push_back(pAnimFloat);
}



//PRIVATE//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HyCreator::PreUpdate()
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

void HyCreator::PostUpdate()
{
	if(m_bInst2dOrderingDirty)
	{
		std::sort(m_vLoadedInst2d.begin(), m_vLoadedInst2d.end(), &Inst2dSortPredicate);
		m_bInst2dOrderingDirty = false;
	}

	WriteDrawBuffers();
}



void HyCreator::WriteDrawBuffers()
{
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
	
	int32 iCount = 0;
	size_t uiTotalNumInsts = m_ViewportRef.m_vCams3d.size();
	for(size_t i = 0; i < uiTotalNumInsts; ++i)
	{
		if(m_ViewportRef.m_vCams3d[i]->IsEnabled())
		{
			*(reinterpret_cast<HyRectangle *>(m_pCurWritePos)) = m_ViewportRef.m_vCams3d[i]->GetRenderRect();
			m_pCurWritePos += sizeof(HyRectangle);
			
			HyError("GetLocalTransform_SRT should be 3d");
			m_ViewportRef.m_vCams3d[i]->GetLocalTransform_SRT(mtxView);
			*(reinterpret_cast<mat4 *>(m_pCurWritePos)) = mtxView;
			m_pCurWritePos += sizeof(mat4);

			iCount++;
		}
	}
	*(reinterpret_cast<int32 *>(pWriteNum3dCamsHere)) = iCount;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// WRITE 2d CAMERA(S) BUFFER
	pDrawHeader->uiOffsetToCameras2d = m_pCurWritePos - m_GfxCommsRef.GetWriteBufferPtr();
	char *pWriteNum2dCamsHere = m_pCurWritePos;
	m_pCurWritePos += sizeof(int32);

	iCount = 0;
	uiTotalNumInsts = m_ViewportRef.m_vCams2d.size();
	for(uint32 i = 0; i < uiTotalNumInsts; ++i)
	{
		if(m_ViewportRef.m_vCams2d[i]->IsEnabled())
		{
			*(reinterpret_cast<HyRectangle *>(m_pCurWritePos)) = m_ViewportRef.m_vCams2d[i]->GetRenderRect();
			m_pCurWritePos += sizeof(HyRectangle);

			m_ViewportRef.m_vCams2d[i]->GetLocalTransform_SRT(mtxView);
			*(reinterpret_cast<mat4 *>(m_pCurWritePos)) = mtxView;
			m_pCurWritePos += sizeof(mat4);

			iCount++;
		}
	}
	*(reinterpret_cast<int32 *>(pWriteNum2dCamsHere)) = iCount;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// WRITE 3d DRAW BUFFER
	pDrawHeader->uiOffsetToInst3d = m_pCurWritePos - m_GfxCommsRef.GetWriteBufferPtr();
	char *pWriteNum3dInstsHere = m_pCurWritePos;
	m_pCurWritePos += sizeof(int32);

	iCount = 0;
	uiTotalNumInsts = m_vInst3d.size();
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

/*static*/ bool HyCreator::Inst2dSortPredicate(const IHyInst2d *pInst1, const IHyInst2d *pInst2)
{
	// TODO: Below is commented out because std::sort expects less-than operator to supply a transitive relationship, 
	//		 i.e. when the sort sees A < B is true and B < C is true, it implies that A < C is true as well.
	//
	//		...for some reason this fails that?

	//if(pInst1->GetDisplayOrder() == pInst2->GetDisplayOrder())
	//	return pInst1->GetRenderState() == pInst2->GetRenderState();

	return pInst1->GetDisplayOrder() < pInst2->GetDisplayOrder();
}

