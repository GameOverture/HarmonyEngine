/**************************************************************************
 *	HyScene.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Scene/HyScene.h"
#include "HyEngine.h"
#include "Renderer/IHyRenderer.h"
#include "Window/HyWindow.h"
#include "Scene/Nodes/Loadables/Drawables/Instances/IHyInstance2d.h"
#include "Scene/Nodes/Loadables/Drawables/Instances/IHyInstance3d.h"
#include "Scene/Nodes/Loadables/Drawables/Instances/Objects/HySprite2d.h"
#include "Scene/Nodes/Loadables/Drawables/Instances/Objects/HySpine2d.h"
#include "Scene/Nodes/Loadables/Drawables/Instances/Objects/HyPrimitive2d.h"
#include "Scene/Nodes/Loadables/Drawables/Instances/Objects/HyText2d.h"
#include "Scene/Nodes/Loadables/Drawables/Instances/Objects/HyTexturedQuad2d.h"
#include "Scene/Physics/HyPhysEntity2d.h"

std::vector<IHyNode *> HyScene::sm_NodeList_All;
std::vector<IHyNode *> HyScene::sm_NodeList_PauseUpdate;
std::vector<HyPhysicsGrid2d *> HyScene::sm_PhysicsGridList;
bool HyScene::sm_bInst2dOrderingDirty = false;

HyScene::HyScene(HyAudioHarness &audioRef, std::vector<HyWindow *> &WindowListRef) :
	m_AudioRef(audioRef),
	m_WindowListRef(WindowListRef),
	m_bPauseGame(false)
{
	IHyNode::sm_pScene = this;
}

HyScene::~HyScene(void)
{
	IHyNode::sm_pScene = nullptr;
}

/*static*/ void HyScene::SetInstOrderingDirty()
{
	sm_bInst2dOrderingDirty = true;
}

/*static*/ void HyScene::AddNode(IHyNode *pNode)
{
	sm_NodeList_All.push_back(pNode);
}

/*static*/ void HyScene::RemoveNode(IHyNode *pNode)
{
	for(auto it = sm_NodeList_All.begin(); it != sm_NodeList_All.end(); ++it)
	{
		if((*it) == pNode)
		{
			//HyLog("RemoveNode type: " << pNode->GetType());
			sm_NodeList_All.erase(it);
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

/*static*/ void HyScene::AddPhysicsGrid(HyPhysicsGrid2d *pPhysGrid)
{
	sm_PhysicsGridList.push_back(pPhysGrid);
}

/*static*/ void HyScene::RemovePhysicsGrid(HyPhysicsGrid2d *pPhysGrid)
{
	for(auto it = sm_PhysicsGridList.begin(); it != sm_PhysicsGridList.end(); ++it)
	{
		if((*it) == pPhysGrid)
		{
			//HyLog("Remove Physics Grid: " << pNode->GetType());
			sm_PhysicsGridList.erase(it);
			break;
		}
	}
}

void HyScene::AddNode_Loaded(IHyInstance2d *pDrawable)
{
	m_NodeList_LoadedDrawable2d.push_back(pDrawable);
	sm_bInst2dOrderingDirty = true;
}

void HyScene::AddNode_Loaded(IHyInstance3d *pDrawable)
{
	m_NodeList_LoadedDrawable3d.push_back(pDrawable);
}

void HyScene::RemoveNode_Loaded(const IHyInstance2d *pDrawable)
{
	for(auto it = m_NodeList_LoadedDrawable2d.begin(); it != m_NodeList_LoadedDrawable2d.end(); ++it)
	{
		if((*it) == pDrawable)
		{
			// TODO: Log about erasing instance
			m_NodeList_LoadedDrawable2d.erase(it);
			break;
		}
	}
}

void HyScene::RemoveNode_Loaded(const IHyInstance3d *pDrawable)
{
	for(auto it = m_NodeList_LoadedDrawable3d.begin(); it != m_NodeList_LoadedDrawable3d.end(); ++it)
	{
		if((*it) == pDrawable)
		{
			// TODO: Log about erasing instance
			m_NodeList_LoadedDrawable3d.erase(it);
			break;
		}
	}
}

void HyScene::CopyAllLoadedNodes(std::vector<IHyInstance2d *> &nodeListOut)
{
	nodeListOut = m_NodeList_LoadedDrawable2d;
}

void HyScene::AppendAudioCue(IHyNode *pNode, IHyAudioCore::CueType eCueType)
{
	m_AudioRef.AppendCue(pNode, eCueType);
}

void HyScene::SetPause(bool bPause)
{
	m_bPauseGame = bPause;
}

//PRIVATE//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HyScene::UpdateNodes()
{
	HY_PROFILE_BEGIN(HYPROFILERSECTION_Nodes)
	if(m_bPauseGame == false)
	{
		for(uint32 i = 0; i < sm_NodeList_All.size(); ++i)
			sm_NodeList_All[i]->Update();
	}
	else
	{
		for(uint32 i = 0; i < sm_NodeList_PauseUpdate.size(); ++i)
			sm_NodeList_PauseUpdate[i]->Update();
	}

	HY_PROFILE_END
}

void HyScene::UpdatePhysics()
{
	HY_PROFILE_BEGIN(HYPROFILERSECTION_Physics)
		for(auto physGrid : sm_PhysicsGridList)
			physGrid->Update();
	HY_PROFILE_END
}

// RENDER STATE BUFFER
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Buffer Header (contains uiNum3dRenderStates; uiNum2dRenderStates) || RenderState3D/UniformData-|-RenderState2D/UniformData-|
void HyScene::PrepareRender(IHyRenderer &rendererRef)
{
	// TODO: Determine whether I can multi-thread this buffer prep and HyRenderBuffer::State instantiations... Make everything take const references!
	// TODO: should I ensure that I start all writes on a 4byte boundary? ARM systems may be an issue

	HY_PROFILE_BEGIN(HYPROFILERSECTION_PrepRender)

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Initialize the buffer - PrepareBuffers may manipulate current IHyDrawInsts or insert new IHyDrawInsts while it updates all the effects
	rendererRef.PrepareBuffers();

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Append 3d Render States to buffer
	uint32 uiTotalNumInsts = static_cast<uint32>(m_NodeList_LoadedDrawable3d.size());
	for(uint32 i = 0; i < uiTotalNumInsts; ++i)
	{
		if(m_NodeList_LoadedDrawable3d[i]->IsValid() == false)
			continue;

		rendererRef.AppendDrawable3d(i, *m_NodeList_LoadedDrawable3d[i], HY_FULL_CAMERA_MASK);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Append 2d Render States to buffer

	// Sort 2d draw instances based on their display order
	if(sm_bInst2dOrderingDirty)
	{
		std::sort(m_NodeList_LoadedDrawable2d.begin(), m_NodeList_LoadedDrawable2d.end(), &Node2dSortPredicate);
		sm_bInst2dOrderingDirty = false;
	}

	// Only draw instances that are within the enabled cameras' frustums
	uint32 uiCameraMask = 0; // HY_FULL_CAMERA_MASK
	uiTotalNumInsts = static_cast<uint32>(m_NodeList_LoadedDrawable2d.size());
	for(uint32 i = 0; i < uiTotalNumInsts; ++i)
	{
		if(m_NodeList_LoadedDrawable2d[i]->IsValid() == false || CalculateCameraMask(*m_NodeList_LoadedDrawable2d[i], uiCameraMask) == false)
			continue;

		rendererRef.AppendDrawable2d(i, *m_NodeList_LoadedDrawable2d[i], uiCameraMask);
	}
	
	// Debug physics draws
	for(auto physGrid : sm_PhysicsGridList)
	{
		std::vector<HyPrimitive2d> &physDrawListRef = physGrid->GetDebugDrawList();
		for(uint32 i = 0; i < static_cast<uint32>(physDrawListRef.size()); ++i)
		{
			if(CalculateCameraMask(physDrawListRef[i], uiCameraMask) == false)
				continue;

			physDrawListRef[i].Load();
			rendererRef.AppendDrawable2d(i, physDrawListRef[i], uiCameraMask);
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// SetCullMaskBit for each enabled camera
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

bool HyScene::CalculateCameraMask(/*const*/ IHyInstance2d &instanceRef, uint32 &uiCameraMaskOut) const
{
	uiCameraMaskOut = 0;
	if(instanceRef.GetCoordinateSystem() >= 0)
		return true;

	uint32 iBit = 0;
	for(uint32 i = 0; i < static_cast<uint32>(m_WindowListRef.size()); ++i)
	{
		HyWindow::CameraIterator2d iter(m_WindowListRef[i]->GetCamera2dList());
		while(iter.IsEnd() == false)
		{
			if(b2TestOverlap(iter.Get()->GetWorldViewBounds(), instanceRef.GetWorldAABB()))
				uiCameraMaskOut |= (1 << iBit);

			iBit++;
			HyAssert(iBit <= HY_MAX_CAMERA_MASK_BITS, "HyScene::CalculateCameraMask exceeded maximum number of passes. There are too many cameras enabled.");

			++iter;
		}
	}

	return uiCameraMaskOut != 0;
}

/*static*/ bool HyScene::Node2dSortPredicate(const IHyInstance2d *pInst1, const IHyInstance2d *pInst2)
{
	if(pInst1->GetDisplayOrder() == pInst2->GetDisplayOrder())
	{
		if(pInst1->GetTextureHandle() == pInst2->GetTextureHandle())
			return reinterpret_cast<const char *>(pInst1) < reinterpret_cast<const char *>(pInst2);

		return pInst1->GetTextureHandle() < pInst2->GetTextureHandle();
	}

	return pInst1->GetDisplayOrder() < pInst2->GetDisplayOrder();
}

