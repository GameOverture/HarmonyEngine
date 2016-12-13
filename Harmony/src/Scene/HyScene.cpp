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
#include "Scene/Instances/HySprite2d.h"
#include "Scene/Instances/HySpine2d.h"
#include "Scene/Instances/HyPrimitive2d.h"
#include "Scene/Instances/HyText2d.h"
#include "Scene/Instances/HyTexturedQuad2d.h"
#include "Scene/HyEntity2d.h"
#include "Scene/HyPhysEntity2d.h"

#include "Time/IHyTime.h"

bool HyScene::sm_bInst2dOrderingDirty = false;
std::vector<IHyTransformNode *> HyScene::sm_MasterList;

HyScene::HyScene(HyGfxComms &gfxCommsRef, std::vector<HyWindow *> &WindowListRef) :	m_b2World(b2Vec2(0.0f, -10.0f)),
																					m_iPhysVelocityIterations(8),
																					m_iPhysPositionIterations(3),
																					m_GfxCommsRef(gfxCommsRef),
																					m_WindowListRef(WindowListRef)
{
	m_b2World.SetDebugDraw(&m_DrawPhys2d);
	m_b2World.SetContactListener(&m_Phys2dContactListener);
	
	m_DrawPhys2d.SetFlags(b2Draw::e_shapeBit);// | b2Draw::e_jointBit | b2Draw::e_aabbBit | b2Draw::e_pairBit | b2Draw::e_centerOfMassBit);

	// Link HyScene to all classes that access it
	HyPhysEntity2d::sm_b2WorldRef = &m_b2World;
}

HyScene::~HyScene(void)
{
	IHyInst2d::sm_pAssetManager = NULL;
}

/*static*/ void HyScene::AddTransformNode(IHyTransformNode *pNode)
{
	sm_MasterList.push_back(pNode);
}

/*static*/ void HyScene::RemoveTransformNode(IHyTransformNode *pNode)
{
	for(std::vector<IHyTransformNode *>::iterator it = sm_MasterList.begin(); it != sm_MasterList.end(); ++it)
	{
		if((*it) == pNode)
		{
			// TODO: Log about erasing Node
			sm_MasterList.erase(it);
			break;
		}
	}
}

void HyScene::AddInstance(IHyInst2d *pInst)
{
	pInst->m_RenderState.PrimeShaderUniforms();

	m_LoadedInst2dList.push_back(pInst);
	sm_bInst2dOrderingDirty = true;
}

void HyScene::RemoveInst(IHyInst2d *pInst)
{
	for(std::vector<IHyInst2d *>::iterator it = m_LoadedInst2dList.begin(); it != m_LoadedInst2dList.end(); ++it)
	{
		if((*it) == pInst)
		{
			// TODO: Log about erasing instance
			m_LoadedInst2dList.erase(it);
			break;
		}
	}
}

void HyScene::CopyAllInsts(std::vector<IHyInst2d *> &vInstsToCopy)
{
	vInstsToCopy = m_LoadedInst2dList;
}

//PRIVATE//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HyScene::PreUpdate()
{
	m_b2World.Step(IHyTime::GetUpdateStepSeconds(), m_iPhysVelocityIterations, m_iPhysPositionIterations);
}

void HyScene::PostUpdate()
{
	if(sm_bInst2dOrderingDirty)
	{
		std::sort(m_LoadedInst2dList.begin(), m_LoadedInst2dList.end(), &Inst2dSortPredicate);
		sm_bInst2dOrderingDirty = false;
	}

	for(uint32 i = 0; i < m_WindowListRef.size(); ++i)
		m_WindowListRef[i]->Update();

	for(uint32 i = 0; i < sm_MasterList.size(); ++i)
		sm_MasterList[i]->Update();

	WriteDrawBuffer();
}

void HyScene::WriteDrawBuffer()
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// BUFFER HEADER (contains offsets from here)-| Num 3d Cams (4bytes)-|-Cam3d-|-Cam3d-|...|-Num 2d Cams (4bytes)-|-Cam2d-|-Cam2d-|...|-Num 3d Insts (4bytes)-|-Inst3d-|-Inst3d-|-Inst3d...-|-Num 2d Insts (4bytes)-|-Inst2d-|-Inst2d-|-Inst2d...-|-<possible blank/empty data since we skip non-enabled instances>-|-RenderState info (Texture Binds, Uniform Data)-|-Vertex Data-|-RenderState info (Texture Binds, Uniform Data)-|-Vertex Data...
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// TODO: should I ensure that I start all writes on a 4byte boundary? ARM systems may be an issue

	// GET BUFFER HEADER (and write to its members as data offsets become known below)
	m_pCurWritePos = m_GfxCommsRef.GetDrawBuffer();
	
	HyGfxComms::tDrawHeader *pDrawHeader = new (m_pCurWritePos) HyGfxComms::tDrawHeader;
	pDrawHeader->uiReturnFlags = 0;
	m_pCurWritePos += sizeof(HyGfxComms::tDrawHeader);

	glm::mat4 mtxView;
	uint32 uiNumWindows = static_cast<uint32>(m_WindowListRef.size());
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// WRITE 3d CAMERA(S) BUFFER
	pDrawHeader->uiOffsetToCameras3d = m_pCurWritePos - m_GfxCommsRef.GetDrawBuffer();
	char *pWriteNum3dCamsHere = m_pCurWritePos;
	m_pCurWritePos += sizeof(int32);
	
	int32 iCount = 0;
	for(uint32 i = 0; i < uiNumWindows; ++i)
	{
		uint32 uiNumCameras3d = static_cast<uint32>(m_WindowListRef[i]->m_Cams3dList.size());
		for(uint32 j = 0; j < uiNumCameras3d; ++j)
		{
			if(m_WindowListRef[i]->m_Cams3dList[j]->IsEnabled())
			{
				*(reinterpret_cast<uint32 *>(m_pCurWritePos)) = i;
				m_pCurWritePos += sizeof(uint32);

				*(reinterpret_cast<HyRectangle<float> *>(m_pCurWritePos)) = m_WindowListRef[i]->m_Cams3dList[j]->GetViewport();
				m_pCurWritePos += sizeof(HyRectangle<float>);
			
				HyError("GetLocalTransform_SRT should be 3d");
				m_WindowListRef[i]->m_Cams3dList[j]->GetLocalTransform_SRT(mtxView);
				*(reinterpret_cast<glm::mat4 *>(m_pCurWritePos)) = mtxView;
				m_pCurWritePos += sizeof(glm::mat4);

				iCount++;
			}
		}
	}
	*(reinterpret_cast<int32 *>(pWriteNum3dCamsHere)) = iCount;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// WRITE 2d CAMERA(S) BUFFER
	pDrawHeader->uiOffsetToCameras2d = m_pCurWritePos - m_GfxCommsRef.GetDrawBuffer();
	char *pWriteNum2dCamsHere = m_pCurWritePos;
	m_pCurWritePos += sizeof(int32);

	iCount = 0;
	for(uint32 i = 0; i < uiNumWindows; ++i)
	{
		uint32 uiNumCameras2d = static_cast<uint32>(m_WindowListRef[i]->m_Cams2dList.size());
		for(uint32 j = 0; j < uiNumCameras2d; ++j)
		{
			if(m_WindowListRef[i]->m_Cams2dList[j]->IsEnabled())
			{
				*(reinterpret_cast<uint32 *>(m_pCurWritePos)) = i;
				m_pCurWritePos += sizeof(uint32);

				*(reinterpret_cast<HyRectangle<float> *>(m_pCurWritePos)) = m_WindowListRef[i]->m_Cams2dList[j]->GetViewport();
				m_pCurWritePos += sizeof(HyRectangle<float>);

				m_WindowListRef[i]->m_Cams2dList[j]->GetLocalTransform_SRT(mtxView);

				// Reversing X and Y because it's more intuitive (or I'm not multiplying the matrices correctly above or in the shader)
				mtxView[3].x *= -1;
				mtxView[3].y *= -1;

				*(reinterpret_cast<glm::mat4 *>(m_pCurWritePos)) = mtxView;
				m_pCurWritePos += sizeof(glm::mat4);

				iCount++;
			}
		}
	}
	*(reinterpret_cast<int32 *>(pWriteNum2dCamsHere)) = iCount;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// WRITE 3d DRAW BUFFER
	pDrawHeader->uiOffsetToInst3d = m_pCurWritePos - m_GfxCommsRef.GetDrawBuffer();
	char *pWriteNum3dInstsHere = m_pCurWritePos;
	m_pCurWritePos += sizeof(int32);

	iCount = 0;
	uint32 uiTotalNumInsts = static_cast<uint32>(m_LoadedInst3dList.size());
	for(uint32 i = 0; i < uiTotalNumInsts; ++i)
	{
		if(m_LoadedInst3dList[i]->IsEnabled())
		{
			// TODO: 
			//new (m_pCurWritePos) HyDrawText2d(reinterpret_cast<HyText2d *>(m_LoadedInst2dList[i]), uiVertexDataOffset, pCurVertexWritePos);
			//m_pCurWritePos += sizeof(HyDrawText2d);
			iCount++;
		}
	}
	*(reinterpret_cast<int32 *>(pWriteNum3dInstsHere)) = iCount;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// WRITE 2d DRAW BUFFER
	pDrawHeader->uiOffsetToInst2d = m_pCurWritePos - m_GfxCommsRef.GetDrawBuffer();
	char *pWriteNum2dInstsHere = m_pCurWritePos;
	m_pCurWritePos += sizeof(int32);

	iCount = 0;
	uiTotalNumInsts = static_cast<uint32>(m_LoadedInst2dList.size());

	char *pStartVertexWritePos = m_pCurWritePos + (uiTotalNumInsts * sizeof(HyRenderState));
	pDrawHeader->uiOffsetToVertexData2d = pStartVertexWritePos - m_GfxCommsRef.GetDrawBuffer();
	char *pCurVertexWritePos = pStartVertexWritePos;

	size_t	uiVertexDataOffset = 0;
	HyRenderState *pCurRenderState2d = NULL;

	for(size_t i = 0; i < uiTotalNumInsts; ++i)
	{
		if(m_LoadedInst2dList[i]->IsEnabled() == false)
			continue;

		// If previously written instance has equal render state by "operator ==" then it's to be assumed the instance data can be batched and doesn't need to write another render state
		if(pCurRenderState2d == NULL || m_LoadedInst2dList[i]->GetRenderState() != *pCurRenderState2d)
		{
			// Start a new draw. Write render state to buffer to be sent to render thread
			memcpy(m_pCurWritePos, &m_LoadedInst2dList[i]->GetRenderState(), sizeof(HyRenderState));
			pCurRenderState2d = reinterpret_cast<HyRenderState *>(m_pCurWritePos);
			pCurRenderState2d->SetDataOffset(uiVertexDataOffset);

			// This function is responsible for incrementing the draw pointer to after what's written
			pCurRenderState2d->WriteRenderStateInfoBufferData(pCurVertexWritePos);	// Also clears Uniforms' dirty flag

			m_pCurWritePos += sizeof(HyRenderState);
			iCount++;
		}
		else
		{
			// This instance will be batched with the current render state
			pCurRenderState2d->AppendInstances(m_LoadedInst2dList[i]->GetRenderState().GetNumInstances());
		}
		
		// OnWriteDrawBufferData() is responsible for incrementing the draw pointer to after what's written
		m_LoadedInst2dList[i]->OnWriteDrawBufferData(pCurVertexWritePos);
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
	HyAssert(pDrawHeader->uiVertexBufferSize2d < HY_GFX_BUFFER_SIZE, "HyGfxComms::WriteUpdateBuffer() has written passed its bounds! Embiggen 'HY_GFX_BUFFER_SIZE'");

	m_GfxCommsRef.SetSharedPointers();
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

		return pInst1->GetRenderState() < pInst2->GetRenderState();
	}

	return pInst1->GetDisplayOrder() < pInst2->GetDisplayOrder();
}

