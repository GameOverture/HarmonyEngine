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

HyPhysicsGrid::HyPhysicsGrid(glm::vec2 vGravity /*= glm::vec2(0.0f, -10.0f)*/, float fPixelsPerMeter /*= 80.0f*/, int32 iVelocityIterations /*= 8*/, int32 iPositionIterations /*= 3*/) :
	b2World(b2Vec2(vGravity.x, vGravity.y)),
	m_fPixelsPerMeter(fPixelsPerMeter),
	m_fPpmInverse(1.0f / fPixelsPerMeter),
	m_iPhysVelocityIterations(iVelocityIterations),
	m_iPhysPositionIterations(iPositionIterations)
{
	HyAssert(m_fPixelsPerMeter > 0.0f, "HarmonyInit's 'fPixelsPerMeter' cannot be <= 0.0f");
	SetContactListener(&m_Phys2dContactListener);

	HyScene::AddPhysicsGrid(this);
}

HyPhysicsGrid::~HyPhysicsGrid()
{
	HyScene::RemovePhysicsGrid(this);
}

void HyPhysicsGrid::Update()
{
	m_DrawPhys2d.GetDrawList().clear();
	DrawDebugData();

	Step(Hy_UpdateStep(), m_iPhysVelocityIterations, m_iPhysPositionIterations);
}

std::vector<HyPrimitive2d> &HyPhysicsGrid::GetDebugDrawList()
{
	return m_DrawPhys2d.GetDrawList();
}

float HyPhysicsGrid::GetPixelsPerMeter()
{
	return m_fPixelsPerMeter;
}

float HyPhysicsGrid::GetPpmInverse()
{
	return m_fPpmInverse;
}
