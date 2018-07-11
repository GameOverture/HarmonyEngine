/**************************************************************************
*	HyPortal2d.cpp
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Renderer/Effects/HyPortal2d.h"
#include "Renderer/IHyRenderer.h"
#include "Scene/Nodes/Draws/Entities/HyEntity2d.h"
#include "HyEngine.h"

HyPortal2dHandle HyPortal2d::sm_hHandleCount = 0;

HyPortal2d::HyPortal2d(const HyPortal2dInit &initRef) :	m_hHANDLE(++sm_hHandleCount),
														m_GateA(initRef.ptGateA1, initRef.ptGateA2, initRef.ptGateA_EntranceHalfSpace, initRef.fGateDepthAmt, initRef.fStencilCullExtents),
														m_GateB(initRef.ptGateB1, initRef.ptGateB2, initRef.ptGateB_EntranceHalfSpace, initRef.fGateDepthAmt, initRef.fStencilCullExtents)
{
	// Error check portal gate initialization values
	HyAssert(false == b2TestOverlap(m_GateA.GetBV(), 0, m_GateB.GetBV(), 0, m_GateA.GetTransform(), m_GateB.GetTransform()), "HyPortal2d::HyPortal2d gates' bounding volumes overlap");

	IHyRenderer::AddPortal2d(this);
}

HyPortal2d::~HyPortal2d()
{
	while(m_DrawInstList.empty() == false)
		m_DrawInstList[0]->ClearPortal(this);
}

void HyPortal2d::Destroy()
{
	IHyRenderer::RemovePortal2d(this);
	delete this;
}

HyPortal2dHandle HyPortal2d::GetHandle() const
{
	return m_hHANDLE;
}

void HyPortal2d::EnableDebugDraw(bool bEnable)
{
	m_GateA.EnableDebugDraw(bEnable);
	m_GateB.EnableDebugDraw(bEnable);
}

void HyPortal2d::AddInstance(IHyDrawInst2d *pInstance)
{
	pInstance->Load();

	m_DrawInstList.push_back(pInstance);
	m_CloneInstList.push_back(nullptr);
}

bool HyPortal2d::RemoveInstance(IHyDrawInst2d *pInstance)
{
	auto iterDraw = m_DrawInstList.begin();
	auto iterClone = m_CloneInstList.begin();
	for(; iterDraw != m_DrawInstList.end(); ++iterDraw, ++iterClone)
	{
		if((*iterDraw) == pInstance)
		{
			delete (*iterClone);
			m_CloneInstList.erase(iterClone);

			m_DrawInstList.erase(iterDraw);
			return true;
		}
	}

	return false;
}

void HyPortal2d::PrepareClones()
{
	for(uint32 i = 0; i < static_cast<uint32>(m_CloneInstList.size()); ++i)
	{
		delete m_CloneInstList[i];
		m_CloneInstList[i] = nullptr;
	}

	for(uint32 i = 0; i < static_cast<uint32>(m_DrawInstList.size()); ++i)
	{
		IHyDrawInst2d *pInstance = m_DrawInstList[i];

		glm::vec2 ptCentroid;
		pInstance->GetLocalBoundingVolume().GetCentroid(ptCentroid);
		ptCentroid += pInstance->pos.Get();

		// First test whether the instance's centroid is INSIDE either gate, which will cause it to warp
		if(m_GateA.GetBV()->TestPoint(m_GateA.GetTransform(), b2Vec2(ptCentroid.x, ptCentroid.y)))
		{
			// Inside gate 1, transform actual instance relative to gate 2
			glm::vec2 v = m_GateA.Midpoint() - ptCentroid;
			pInstance->pos.Set(m_GateB.Midpoint() + v);

			// Recalculate centroid
			pInstance->GetLocalBoundingVolume().GetCentroid(ptCentroid);
			ptCentroid += pInstance->pos.Get();
		}
		else if(m_GateB.GetBV()->TestPoint(m_GateB.GetTransform(), b2Vec2(ptCentroid.x, ptCentroid.y)))
		{
			// Inside gate 2, transform actual instance relative to gate 1
			glm::vec2 v = m_GateB.Midpoint() - ptCentroid;
			pInstance->pos.Set(m_GateA.Midpoint() + v);

			// Recalculate centroid
			pInstance->GetLocalBoundingVolume().GetCentroid(ptCentroid);
			ptCentroid += pInstance->pos.Get();
		}

		glm::mat4 mtxWorld;
		pInstance->GetWorldTransform(mtxWorld);
		float fWorldRotationRadians = glm::atan(mtxWorld[0][1], mtxWorld[0][0]);
		b2Transform instTransform(b2Vec2(mtxWorld[3].x, mtxWorld[3].y), b2Rot(fWorldRotationRadians));

		// Then test if instance is overlapping into either gate, which will render a copy of the instance at the other gate
		if(b2TestOverlap(m_GateA.GetBV(), 0, pInstance->GetLocalBoundingVolume().GetB2Shape(), 0, m_GateA.GetTransform(), instTransform))
		{
			glm::vec2 v = ptCentroid - m_GateA.Midpoint();

			IHyDrawInst2d *pNewInst = pInstance->Clone();
			pNewInst->ClearPortal(this);	// Avoid infinite loop
			pNewInst->Load();
			pNewInst->pos.Set(m_GateB.Midpoint() + v);

			pInstance->SetStencil(m_GateA.GetStencil());
			pNewInst->SetStencil(m_GateB.GetStencil());

			m_CloneInstList[i] = pNewInst;
		}
		else if(b2TestOverlap(m_GateB.GetBV(), 0, pInstance->GetLocalBoundingVolume().GetB2Shape(), 0, m_GateB.GetTransform(), instTransform))
		{
			glm::vec2 v = ptCentroid - m_GateB.Midpoint();

			IHyDrawInst2d *pNewInst = pInstance->Clone();
			pNewInst->ClearPortal(this);	// Avoid infinite loop
			pNewInst->Load();
			pNewInst->pos.Set(m_GateA.Midpoint() + v);

			pInstance->SetStencil(m_GateB.GetStencil());
			pNewInst->SetStencil(m_GateA.GetStencil());

			m_CloneInstList[i] = pNewInst;
		}
	}
}
