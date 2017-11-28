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

HyPortal2dHandle HyPortal2d::sm_hHandleCount = 0;

HyPortal2d::HyPortal2d(const HyPortalGate2d &gate1Ref, const HyPortalGate2d &gate2Ref) :	m_hHANDLE(++sm_hHandleCount),
																							m_Gate1(gate1Ref),
																							m_Gate2(gate2Ref)
{
	IHyRenderer::AddPortal2d(this);
}

HyPortal2d::~HyPortal2d()
{
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

void HyPortal2d::TestEntity(HyEntity2d *pEnt)
{
	for(uint32 i = 0; i < pEnt->ChildCount(); ++i)
	{
		switch(pEnt->ChildGet(i)->GetType())
		{
		}
	}
}
