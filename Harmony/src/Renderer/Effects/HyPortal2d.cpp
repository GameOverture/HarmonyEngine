/**************************************************************************
*	HyPortal2d.cpp
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Renderer/Effects/HyPortal2d.h"
#include "Renderer/IHyRenderer.h"
#include "Scene/Nodes/Draws/Entities/HyEntity2d.h"
#include "HyEngine.h"

HyPortal2dHandle HyPortal2d::sm_hHandleCount = 0;

HyPortal2d::HyPortal2d(const HyPortalGate2d &gate1Ref, const HyPortalGate2d &gate2Ref) :	m_hHANDLE(++sm_hHandleCount),
																							m_Gate1(gate1Ref),
																							m_Gate2(gate2Ref)
{
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
		pInstance->GetBoundingVolume().GetCentroid(ptCentroid);
		ptCentroid += pInstance->pos.Get();

		// First test whether the instance's centroid is INSIDE either gate, which will cause it to warp
		if(m_Gate1.GetBV()->TestPoint(m_Gate1.GetTransform(), b2Vec2(ptCentroid.x, ptCentroid.y)))
		{
			// Inside gate 1, transform actual instance relative to gate 2
			glm::vec2 v = m_Gate1.Midpoint() - ptCentroid;
			pInstance->pos.Set(m_Gate2.Midpoint() + v);

			// Recalculate centroid
			pInstance->GetBoundingVolume().GetCentroid(ptCentroid);
			ptCentroid += pInstance->pos.Get();
		}
		else if(m_Gate2.GetBV()->TestPoint(m_Gate2.GetTransform(), b2Vec2(ptCentroid.x, ptCentroid.y)))
		{
			// Inside gate 2, transform actual instance relative to gate 1
			glm::vec2 v = m_Gate2.Midpoint() - ptCentroid;
			pInstance->pos.Set(m_Gate1.Midpoint() + v);

			// Recalculate centroid
			pInstance->GetBoundingVolume().GetCentroid(ptCentroid);
			ptCentroid += pInstance->pos.Get();
		}

		glm::mat4 mtxWorld;
		pInstance->GetWorldTransform(mtxWorld);
		float fWorldRotationRadians = glm::atan(mtxWorld[0][1], mtxWorld[0][0]);
		b2Transform instTransform(b2Vec2(mtxWorld[3].x, mtxWorld[3].y), b2Rot(fWorldRotationRadians));

		// Then test if instance is overlapping into either gate, which will render a copy of the instance at the other gate
		if(b2TestOverlap(m_Gate1.GetBV(), 0, pInstance->GetBoundingVolume().GetB2Shape(), 0, m_Gate1.GetTransform(), instTransform))
		{
			glm::vec2 v = m_Gate1.Midpoint() - ptCentroid;

			IHyDrawInst2d *pNewInst = pInstance->Clone();
			pNewInst->pos.Set(m_Gate2.Midpoint() + v);

			m_CloneInstList[i] = pNewInst;
		}
		else if(b2TestOverlap(m_Gate2.GetBV(), 0, pInstance->GetBoundingVolume().GetB2Shape(), 0, m_Gate2.GetTransform(), instTransform))
		{
			glm::vec2 v = m_Gate2.Midpoint() - ptCentroid;

			IHyDrawInst2d *pNewInst = pInstance->Clone();
			pNewInst->pos.Set(m_Gate1.Midpoint() + v);

			m_CloneInstList[i] = pNewInst;
		}
	}
}
