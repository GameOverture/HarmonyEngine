/**************************************************************************
 *	HyScene.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Scene/HyScene.h"
#include "HyEngine.h"
#include "Renderer/IHyRenderer.h"
#include "Renderer/Components/HyWindow.h"
#include "Scene/Nodes/Draws/Instances/HySprite2d.h"
#include "Scene/Nodes/Draws/Instances/HySpine2d.h"
#include "Scene/Nodes/Draws/Instances/HyPrimitive2d.h"
#include "Scene/Nodes/Draws/Instances/HyText2d.h"
#include "Scene/Nodes/Draws/Instances/HyTexturedQuad2d.h"
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
			//HyLog("RemoveNode type: " << pNode->GetType());
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

void HyScene::AddNode_Loaded(IHyDrawInst2d *pInst)
{
	m_NodeList_Loaded.push_back(pInst);
	sm_bInst2dOrderingDirty = true;
}

void HyScene::RemoveNode_Loaded(IHyDrawInst2d *pInst)
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

void HyScene::CopyAllLoadedNodes(std::vector<IHyDrawInst2d *> &nodeListOut)
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
	HY_PROFILE_BEGIN(HYPROFILERSECTION_Physics)
		m_b2World.Step(Hy_UpdateStep(), m_iPhysVelocityIterations, m_iPhysPositionIterations);
	HY_PROFILE_END
}

void HyScene::UpdateNodes()
{
	HY_PROFILE_BEGIN(HYPROFILERSECTION_Nodes)
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

// RENDER STATE BUFFER
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Buffer Header (contains uiNum3dRenderStates; uiNum2dRenderStates) || RenderState3D/UniformData-|-RenderState2D/UniformData-|
void HyScene::PrepareRender(IHyRenderer &rendererRef)
{
	// TODO: Determine whether I can multi-thread this buffer prep and HyRenderState instantiations... Make everything take const references!
	// TODO: should I ensure that I start all writes on a 4byte boundary? ARM systems may be an issue

	HY_PROFILE_BEGIN(HYPROFILERSECTION_PrepRender)

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Initialize the buffer - PrepareBuffers may manipulate current IHyDrawInsts or insert new IHyDrawInsts while it updates all the effects
	rendererRef.PrepareBuffers();

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Sort 2d draw instances based on their display order
	if(sm_bInst2dOrderingDirty)
	{
		std::sort(m_NodeList_Loaded.begin(), m_NodeList_Loaded.end(), &Node2dSortPredicate);
		sm_bInst2dOrderingDirty = false;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Append 3d Render States to buffer
	uint32 uiTotalNumInsts = static_cast<uint32>(m_LoadedInst3dList.size());
	for(uint32 i = 0; i < uiTotalNumInsts; ++i)
	{
		if(m_LoadedInst3dList[i]->IsEnabled() == false)
			continue;

		rendererRef.AppendRenderState(i, *m_NodeList_Loaded[i], HY_FULL_CULL_MASK);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Append 2d Render States to buffer

	// TODO: JAY FIX CULLING ISSUE
	uint32 uiCullMask = HY_FULL_CULL_MASK;//0;
	uiTotalNumInsts = static_cast<uint32>(m_NodeList_Loaded.size());
	for(uint32 i = 0; i < uiTotalNumInsts; ++i)
	{
		if(m_NodeList_Loaded[i]->IsEnabled() == false/* || CalculateCullPasses(*m_NodeList_Loaded[i], uiCullMask) == false*/)
			continue;

		rendererRef.AppendRenderState(i, *m_NodeList_Loaded[i], uiCullMask);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// SetCullMaskBit for each camera
	uint32 iBit = 0;
	for(uint32 i = 0; i < static_cast<uint32>(m_WindowListRef.size()); ++i)
	{
		HyWindow::CameraIterator2d iter(m_WindowListRef[i]->GetCamera2dList());
		while(iter.IsEnd() == false)
		{
			iter.Get()->SetCullMaskBit(iBit);
			++iter;
			++iBit;
		}
	}

	HY_PROFILE_END
}

bool HyScene::CalculateCullPasses(/*const*/ IHyDrawInst2d &instanceRef, uint32 &uiCullMaskOut)
{
	uiCullMaskOut = 0;
	if(instanceRef.GetCoordinateSystem() >= 0)
		return true;

	uint32 iBit = 0;
	for(uint32 i = 0; i < static_cast<uint32>(m_WindowListRef.size()); ++i)
	{
		HyWindow::CameraIterator2d iter(m_WindowListRef[i]->GetCamera2dList());
		while(iter.IsEnd() == false)
		{
			if(b2TestOverlap(iter.Get()->GetWorldViewBounds(), instanceRef.GetWorldAABB()))
				uiCullMaskOut |= (1 << iBit);

			iBit++;
			HyAssert(iBit <= HY_MAX_PASSES_PER_BUFFER, "HyScene::CalculateCullPasses exceeded maximum number of passes. There are too many cameras enabled.");

			++iter;
		}
	}

	return uiCullMaskOut != 0;
}

/*static*/ bool HyScene::Node2dSortPredicate(const IHyDrawInst2d *pInst1, const IHyDrawInst2d *pInst2)
{
	if(pInst1->GetDisplayOrder() == pInst2->GetDisplayOrder())
	{
		if(pInst1->GetTextureHandle() == pInst2->GetTextureHandle())
			return reinterpret_cast<const char *>(pInst1) < reinterpret_cast<const char *>(pInst2);

		return pInst1->GetTextureHandle() < pInst2->GetTextureHandle();
	}

	return pInst1->GetDisplayOrder() < pInst2->GetDisplayOrder();
}

