/**************************************************************************
*	HyPhysicsGrid.cpp
*	
*	Harmony Engine
*	Copyright (c) 2020 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Scene/Physics/HyPhysicsGrid.h"
#include "HyEngine.h"

HyPhysicsGrid::HyPhysicsGrid(glm::vec2 vGravity, int32 iVelocityIterations /*= 8*/, int32 iPositionIterations /*= 3*/) :
	b2World(b2Vec2(vGravity.x, vGravity.y)),
	m_iPhysVelocityIterations(iVelocityIterations),
	m_iPhysPositionIterations(iPositionIterations)
{
	SetContactListener(&m_Phys2dContactListener);
}

HyPhysicsGrid::~HyPhysicsGrid()
{
}

void HyPhysicsGrid::Update()
{
	m_DrawPhys2d.GetDrawList().clear();
	Step(Hy_UpdateStep(), m_iPhysVelocityIterations, m_iPhysPositionIterations);
}

std::vector<HyPrimitive2d> &HyPhysicsGrid::GetDebugDrawList()
{
	return m_DrawPhys2d.GetDrawList();
}
