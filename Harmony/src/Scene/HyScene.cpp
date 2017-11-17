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
#include "Renderer/IHyRenderer.h"
#include "Renderer/Components/HyWindow.h"
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
	char *pVertexBuffer = rendererRef.GetVertexBuffer();

	IHyRenderer::RenderStateBufferHeader *pHeader = reinterpret_cast<IHyRenderer::RenderStateBufferHeader *>(pRsBufferWritePos);
	memset(pHeader, 0, sizeof(IHyRenderer::RenderStateBufferHeader));

	pRsBufferWritePos += sizeof(IHyRenderer::RenderStateBufferHeader);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// WRITE 3d Render States
	uint32 uiTotalNumInsts = static_cast<uint32>(m_LoadedInst3dList.size());
	for(uint32 i = 0; i < uiTotalNumInsts; ++i)
	{
		if(m_LoadedInst3dList[i]->IsEnabled())
		{
			// TODO: 
			//new (m_pCurWritePos) HyDrawText2d(reinterpret_cast<HyText2d *>(m_NodeList_Loaded[i]), uiVertexDataOffset, pCurVertexWritePos);
			//pRsBufferWritePos += sizeof(HyRenderState);
			pHeader->uiNum3dRenderStates++;
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// WRITE 2d Render States
	uiTotalNumInsts = static_cast<uint32>(m_NodeList_Loaded.size());

	size_t	uiVertexDataOffset = 0;
	HyRenderState *pCurRenderState2d = nullptr;
	for(uint32 i = 0; i < uiTotalNumInsts; ++i)
	{
		if(m_NodeList_Loaded[i]->IsEnabled() == false)
			continue;

		m_NodeList_Loaded[i]

		// If previously written instance has equal render state by "operator ==" then it's to be assumed the instance data can be batched and doesn't need to write another render state
		//if(pCurRenderState2d == nullptr ||
		//   m_NodeList_Loaded[i]->GetRenderState() != *pCurRenderState2d ||
		//   m_NodeList_Loaded[i]->GetRenderState().IsEnabled(HyRenderState::DRAWINSTANCED) == false)
		{
			// Start a new draw. Write render state to buffer to be sent to render thread
			memcpy(m_pCurWritePos, &m_NodeList_Loaded[i]->GetRenderState(), sizeof(HyRenderState));
			pCurRenderState2d = reinterpret_cast<HyRenderState *>(m_pCurWritePos);
			pCurRenderState2d->SetDataOffset(uiVertexDataOffset);

			// This function is responsible for incrementing the draw pointer to after what's written
			m_NodeList_Loaded[i]->WriteShaderUniformBuffer(pCurVertexWritePos);

			m_pCurWritePos += sizeof(HyRenderState);
			iCount++;
		}
		//else
		//{
		//	// This instance will be batched with the current render state
		//	pCurRenderState2d->AppendInstances(m_NodeList_Loaded[i]->GetRenderState().GetNumInstances());
		//}

		// OnWriteDrawBufferData() is responsible for incrementing the draw pointer to after what's written
		m_NodeList_Loaded[i]->OnWriteDrawBufferData(pCurVertexWritePos);
		uiVertexDataOffset = pCurVertexWritePos - pStartVertexWritePos;
	}

	*(reinterpret_cast<uint32 *>(pWriteNum2dRenderStatesHere)) = iCount;
	pDrawHeader->uiVertexBufferSize2d = pCurVertexWritePos - pStartVertexWritePos;

	// Do final check to see if we wrote passed our bounds
	HyAssert(pDrawHeader->uiVertexBufferSize2d < HY_VERTEX_BUFFER_SIZE, "HyScene::WriteUpdateBuffer() has written passed its bounds! Embiggen 'HY_VERTEX_BUFFER_SIZE'");

	HY_PROFILE_END
}

/*static*/ bool HyScene::Node2dSortPredicate(const IHyLeafDraw2d *pInst1, const IHyLeafDraw2d *pInst2)
{
	if(pInst1->GetDisplayOrder() == pInst2->GetDisplayOrder())
	{
		if(pInst1->GetRenderState() == pInst2->GetRenderState())
			return reinterpret_cast<const char *>(pInst1) < reinterpret_cast<const char *>(pInst2);

		return pInst1->GetRenderState() < pInst2->GetRenderState();
	}

	return pInst1->GetDisplayOrder() < pInst2->GetDisplayOrder();
}

