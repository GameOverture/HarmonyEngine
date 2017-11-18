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
#include "HyEngine.h"
#include "Renderer/IHyRenderer.h"
#include "Renderer/Components/HyWindow.h"
#include "Renderer/Components/HyRenderState.h"
#include "Scene/Nodes/Leafs/Draws/HySprite2d.h"
#include "Scene/Nodes/Leafs/Draws/HySpine2d.h"
#include "Scene/Nodes/Leafs/Draws/HyPrimitive2d.h"
#include "Scene/Nodes/Leafs/Draws/HyText2d.h"
#include "Scene/Nodes/Leafs/Draws/HyTexturedQuad2d.h"
#include "Scene/Physics/HyPhysEntity2d.h"

bool HyScene::sm_bInst2dOrderingDirty = false;
std::vector<IHyNode *> HyScene::sm_MasterNodeList;
std::vector<IHyNode *> HyScene::sm_NodeList_PauseUpdate;

HyScene::HyScene(std::vector<HyWindow *> &WindowListRef) :	m_b2World(b2Vec2(0.0f, -10.0f)),
															m_iPhysVelocityIterations(8),
															m_iPhysPositionIterations(3),
															m_WindowListRef(WindowListRef),
															m_bPauseGame(false)
{
	m_b2World.SetDebugDraw(&m_DrawPhys2d);
	m_b2World.SetContactListener(&m_Phys2dContactListener);
	
	m_DrawPhys2d.SetFlags(b2Draw::e_shapeBit);// | b2Draw::e_jointBit | b2Draw::e_aabbBit | b2Draw::e_pairBit | b2Draw::e_centerOfMassBit);

	// Link HyScene to all classes that access it
	HyPhysEntity2d::sm_b2WorldRef = &m_b2World;
}

HyScene::~HyScene(void)
{
	IHyLeafDraw2d::sm_pHyAssets = nullptr;
}

/*static*/ void HyScene::AddNode(IHyNode *pNode)
{
	sm_MasterNodeList.push_back(pNode);
}

/*static*/ void HyScene::RemoveNode(IHyNode *pNode)
{
	for(auto it = sm_MasterNodeList.begin(); it != sm_MasterNodeList.end(); ++it)
	{
		if((*it) == pNode)
		{
			// TODO: Log about erasing Node
			sm_MasterNodeList.erase(it);
			break;
		}
	}
}

/*static*/ void HyScene::AddNode_PauseUpdate(IHyNode *pNode)
{
	sm_NodeList_PauseUpdate.push_back(pNode);
}

/*static*/ void HyScene::RemoveNode_PauseUpdate(IHyNode *pNode)
{
	for(auto it = sm_NodeList_PauseUpdate.begin(); it != sm_NodeList_PauseUpdate.end(); ++it)
	{
		if((*it) == pNode)
		{
			// TODO: Log about erasing Node
			sm_NodeList_PauseUpdate.erase(it);
			break;
		}
	}
}

void HyScene::AddNode_Loaded(IHyLeafDraw2d *pInst)
{
	m_NodeList_Loaded.push_back(pInst);
	sm_bInst2dOrderingDirty = true;
}

void HyScene::RemoveNode_Loaded(IHyLeafDraw2d *pInst)
{
	for(auto it = m_NodeList_Loaded.begin(); it != m_NodeList_Loaded.end(); ++it)
	{
		if((*it) == pInst)
		{
			// TODO: Log about erasing instance
			m_NodeList_Loaded.erase(it);
			break;
		}
	}
}

void HyScene::CopyAllLoadedNodes(std::vector<IHyLeafDraw2d *> &nodeListOut)
{
	nodeListOut = m_NodeList_Loaded;
}

b2World &HyScene::GetPhysics2d()
{
	return m_b2World;
}

void HyScene::SetPause(bool bPause)
{
	m_bPauseGame = bPause;
}

//PRIVATE//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HyScene::UpdatePhysics()
{
	HY_PROFILE_BEGIN("Physics")
		m_b2World.Step(Hy_UpdateStep(), m_iPhysVelocityIterations, m_iPhysPositionIterations);
	HY_PROFILE_END
}

void HyScene::UpdateNodes()
{
	HY_PROFILE_BEGIN("Nodes")
	if(m_bPauseGame == false)
	{
		for(uint32 i = 0; i < sm_MasterNodeList.size(); ++i)
			sm_MasterNodeList[i]->Update();
	}
	else
	{
		for(uint32 i = 0; i < sm_NodeList_PauseUpdate.size(); ++i)
			sm_NodeList_PauseUpdate[i]->Update();
	}
	HY_PROFILE_END
}

void HyScene::PrepareRender(IHyRenderer &rendererRef)
{
	HY_PROFILE_BEGIN("PrepareRender")
	if(sm_bInst2dOrderingDirty)
	{
		std::sort(m_NodeList_Loaded.begin(), m_NodeList_Loaded.end(), &Node2dSortPredicate);
		sm_bInst2dOrderingDirty = false;
	}
	
	// RENDER STATE BUFFER
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Buffer Header (contains uiNum3dRenderStates; uiNum2dRenderStates; uiOffsetTo2d) || RenderState3D/UniformData-|-RenderState2D/UniformData-|
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// TODO: should I ensure that I start all writes on a 4byte boundary? ARM systems may be an issue

	char *pRsBufferWritePos = rendererRef.GetRenderStateBuffer();
	char *pVertBufferWritePos = rendererRef.GetVertexBuffer();

	IHyRenderer::RenderStateBufferHeader *pHeader = reinterpret_cast<IHyRenderer::RenderStateBufferHeader *>(pRsBufferWritePos);
	memset(pHeader, 0, sizeof(IHyRenderer::RenderStateBufferHeader));

	pRsBufferWritePos += sizeof(IHyRenderer::RenderStateBufferHeader);

	// TODO: Determine whether I can multi-thread these HyRenderState instantiations

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// WRITE 3d Render States
	uint32 uiTotalNumInsts = static_cast<uint32>(m_LoadedInst3dList.size());
	for(uint32 i = 0; i < uiTotalNumInsts; ++i)
	{
		if(m_LoadedInst3dList[i]->IsEnabled() == false)
			continue;

		// TODO: 
		//new (m_pCurWritePos) HyDrawText2d(reinterpret_cast<HyText2d *>(m_NodeList_Loaded[i]), uiVertexDataOffset, pCurVertexWritePos);
		pRsBufferWritePos += sizeof(HyRenderState);
		pHeader->uiNum3dRenderStates++;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// WRITE 2d Render States
	uint32 uiCullMask = 0;
	uiTotalNumInsts = static_cast<uint32>(m_NodeList_Loaded.size());
	for(uint32 i = 0; i < uiTotalNumInsts; ++i)
	{
		if(m_NodeList_Loaded[i]->IsEnabled() == false || CalculateCullPasses(*m_NodeList_Loaded[i], uiCullMask) == false)
			continue;

		HyRenderState *pRenderState = new (pRsBufferWritePos) HyRenderState(*m_NodeList_Loaded[i],
																			uiCullMask,
																			reinterpret_cast<size_t>(pVertBufferWritePos) - reinterpret_cast<size_t>(rendererRef.GetVertexBuffer()));

		pRsBufferWritePos += sizeof(HyRenderState);

		char *pStartOfExData = pRsBufferWritePos;
		m_NodeList_Loaded[i]->WriteShaderUniformBuffer(pRsBufferWritePos);	// This function is responsible for incrementing the draw pointer to after what's written
		pRenderState->SetExSize(reinterpret_cast<size_t>(pRsBufferWritePos) - reinterpret_cast<size_t>(pStartOfExData));

		// OnWriteDrawBufferData() is responsible for incrementing the draw pointer to after what's written
		m_NodeList_Loaded[i]->OnWriteDrawBufferData(pVertBufferWritePos);

		pHeader->uiNum2dRenderStates++;
	}

	rendererRef.SetVertexBufferUsed(reinterpret_cast<size_t>(pVertBufferWritePos) - reinterpret_cast<size_t>(rendererRef.GetVertexBuffer()));

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// SetCullMaskStartBit for each window
	uint32 iBit = 0;
	for(uint32 i = 0; i < static_cast<uint32>(m_WindowListRef.size()); ++i)
	{
		m_WindowListRef[i]->SetCullMaskStartBit(iBit);

		HyWindow::CameraIterator2d iter = m_WindowListRef[i]->GetCamera2dIterator();
		while(iter.IsEnd() == false)
			++iBit;
	}

	HY_PROFILE_END
}

bool HyScene::CalculateCullPasses(/*const*/ IHyLeafDraw2d &instanceRef, uint32 &uiCullMaskOut)
{
	uiCullMaskOut = 0;
	if(instanceRef.GetCoordinateSystem() >= 0)
		return true;

	uint32 iBit = 0;
	for(uint32 i = 0; i < static_cast<uint32>(m_WindowListRef.size()); ++i)
	{
		HyWindow::CameraIterator2d iter = m_WindowListRef[i]->GetCamera2dIterator();
		while(iter.IsEnd() == false)
		{
			if(iter.Get()->GetWorldViewBounds().Contains(instanceRef.GetWorldAABB()))
				uiCullMaskOut |= (1 << iBit);

			iBit++;
			HyAssert(iBit > HY_MAX_PASSES_PER_BUFFER, "HyScene::CalculateCullPasses exceeded maximum number of passes. There are too many cameras enabled.");

			++iter;
		}
	}

	return uiCullMaskOut != 0;
}

/*static*/ bool HyScene::Node2dSortPredicate(const IHyLeafDraw2d *pInst1, const IHyLeafDraw2d *pInst2)
{
	if(pInst1->GetDisplayOrder() == pInst2->GetDisplayOrder())
	{
		if(pInst1->GetTextureHandle() == pInst2->GetTextureHandle())
			return reinterpret_cast<const char *>(pInst1) < reinterpret_cast<const char *>(pInst2);

		return pInst1->GetTextureHandle() < pInst2->GetTextureHandle();
	}

	return pInst1->GetDisplayOrder() < pInst2->GetDisplayOrder();
}

