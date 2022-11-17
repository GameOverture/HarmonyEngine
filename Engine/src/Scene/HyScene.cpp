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
#include "Scene/Nodes/Loadables/IHyLoadable.h"
#include "Scene/Nodes/Loadables/Bodies/Drawables/IHyDrawable2d.h"
#include "Scene/Nodes/Loadables/Bodies/Drawables/IHyDrawable3d.h"
#include "Scene/Nodes/Loadables/Bodies/Drawables/Objects/HySprite2d.h"
#include "Scene/Nodes/Loadables/Bodies/Drawables/Objects/HySpine2d.h"
#include "Scene/Nodes/Loadables/Bodies/Drawables/Objects/HyPrimitive2d.h"
#include "Scene/Nodes/Loadables/Bodies/Drawables/Objects/HyText2d.h"
#include "Scene/Nodes/Loadables/Bodies/Drawables/Objects/HyTexturedQuad2d.h"

std::vector<IHyNode *> HyScene::sm_NodeList_All;
std::vector<IHyNode *> HyScene::sm_NodeList_PauseUpdate;
bool HyScene::sm_bInst2dOrderingDirty = false;

HyScene::HyScene(glm::vec2 vGravity2d, float fPixelsPerMeter, HyAudioCore &audioCoreRef, std::vector<HyWindow *> &WindowListRef) :
	m_AudioCoreRef(audioCoreRef),
	m_WindowListRef(WindowListRef),
	m_bPauseGame(false),
	m_fPixelsPerMeter(fPixelsPerMeter),
	m_fPpmInverse(1.0f / fPixelsPerMeter),
	m_b2World(b2Vec2(vGravity2d.x, vGravity2d.y))
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

void HyScene::AddNode_Loaded(IHyDrawable2d *pDrawable)
{
	m_NodeList_LoadedDrawable2d.push_back(pDrawable);
	sm_bInst2dOrderingDirty = true;
}

void HyScene::AddNode_Loaded(IHyDrawable3d *pDrawable)
{
	m_NodeList_LoadedDrawable3d.push_back(pDrawable);
}

void HyScene::RemoveNode_Loaded(const IHyDrawable2d *pDrawable)
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

void HyScene::RemoveNode_Loaded(const IHyDrawable3d *pDrawable)
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

void HyScene::CopyAllLoadedNodes(std::vector<IHyLoadable *> &nodeListOut)
{
	nodeListOut.clear();

	for(auto it = sm_NodeList_All.begin(); it != sm_NodeList_All.end(); ++it)
	{
		if(((*it)->GetInternalFlags() & IHyNode::NODETYPE_IsLoadable) != 0)
		{
			if((*it)->Is2D())
			{
				if(static_cast<IHyLoadable2d *>(*it)->IsLoaded())
					nodeListOut.push_back(static_cast<IHyLoadable2d *>(*it));
			}
			else if(static_cast<IHyLoadable3d *>(*it)->IsLoaded())
				nodeListOut.push_back(static_cast<IHyLoadable3d *>(*it));
		}
	}
}

void HyScene::AddNode_Collidable(IHyBody2d *pBody)
{
	//pBody-> = m_CollisionTree.CreateProxy(pBody->GetSceneAABB(), pBody);
}

void HyScene::RemoveNode_Collidable(IHyBody2d *pBody)
{
}

void HyScene::ProcessAudioCue(IHyNode *pNode, HySoundCue eCueType)
{
	m_AudioCoreRef.ProcessCue(pNode, eCueType);
}

void HyScene::SetPause(bool bPause)
{
	m_bPauseGame = bPause;
}

//PRIVATE//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HyScene::UpdateNodes()
{
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

	m_AudioCoreRef.Update();
}

// RENDER STATE BUFFER
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Buffer Header (contains uiNum3dRenderStates; uiNum2dRenderStates) || RenderState3D/UniformData-|-RenderState2D/UniformData-|
void HyScene::PrepareRender(IHyRenderer &rendererRef, float fExtrapolatePercent)
{
	// TODO: Determine whether I can multi-thread this buffer prep and HyRenderBuffer::State instantiations... Make everything take const references!
	// TODO: should I ensure that I start all writes on a 4byte boundary? ARM systems may be an issue

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Initialize the buffer - PrepareBuffers writes internal render states first, used by things like HyStencil
	rendererRef.PrepareBuffers(fExtrapolatePercent);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Append 3d Render States to buffer
	uint32 uiTotalNumInsts = static_cast<uint32>(m_NodeList_LoadedDrawable3d.size());
	for(uint32 i = 0; i < uiTotalNumInsts; ++i)
	{
		if(m_NodeList_LoadedDrawable3d[i]->IsValidToRender() == false)
			continue;

		rendererRef.AppendDrawable3d(i, *m_NodeList_LoadedDrawable3d[i], HY_FULL_CAMERA_MASK, fExtrapolatePercent);
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
		if(m_NodeList_LoadedDrawable2d[i]->IsValidToRender() == false || CalculateCameraMask(*m_NodeList_LoadedDrawable2d[i], uiCameraMask) == false)
			continue;

		rendererRef.AppendDrawable2d(i, *m_NodeList_LoadedDrawable2d[i], uiCameraMask, fExtrapolatePercent);
	}
	
	//// Debug physics draws
	//for(auto physGrid : sm_PhysicsGridList)
	//{
	//	std::vector<HyPrimitive2d> &physDrawListRef = physGrid->GetDebugDrawList();
	//	for(uint32 i = 0; i < static_cast<uint32>(physDrawListRef.size()); ++i)
	//	{
	//		if(CalculateCameraMask(physDrawListRef[i], uiCameraMask) == false)
	//			continue;

	//		rendererRef.AppendDrawable2d(i, physDrawListRef[i], uiCameraMask);
	//	}
	//}

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
}

bool HyScene::CalculateCameraMask(/*const*/ IHyDrawable2d &instanceRef, uint32 &uiCameraMaskOut) const
{
	uiCameraMaskOut = 0;
	if(instanceRef.GetCoordinateSystem() >= 0) // Only test if using camera (world) coordinates
		return true;

	uint32 iBit = 0;
	for(uint32 i = 0; i < static_cast<uint32>(m_WindowListRef.size()); ++i)
	{
		HyWindow::CameraIterator2d iter(m_WindowListRef[i]->GetCamera2dList());
		while(iter.IsEnd() == false)
		{
			if(b2TestOverlap(iter.Get()->GetWorldViewBounds(), instanceRef.GetSceneAABB()))
				uiCameraMaskOut |= (1 << iBit);

			iBit++;
			HyAssert(iBit <= HY_MAX_CAMERA_MASK_BITS, "HyScene::CalculateCameraMask exceeded maximum number of passes. There are too many cameras enabled.");

			++iter;
		}
	}

	return uiCameraMaskOut != 0;
}

/*static*/ bool HyScene::Node2dSortPredicate(IHyDrawable2d *pInst1, IHyDrawable2d *pInst2)
{
	if(pInst1->GetDisplayOrder() == pInst2->GetDisplayOrder())
	{
		if(pInst1->GetShaderUniforms().GetCrc64() == pInst2->GetShaderUniforms().GetCrc64())
			return reinterpret_cast<const char *>(pInst1) < reinterpret_cast<const char *>(pInst2);

		return pInst1->GetShaderUniforms().GetCrc64() < pInst2->GetShaderUniforms().GetCrc64();
	}

	return pInst1->GetDisplayOrder() < pInst2->GetDisplayOrder();
}

