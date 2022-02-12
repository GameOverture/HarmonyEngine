/**************************************************************************
*	HyPhysicsGrid2d.cpp
*	
*	Harmony Engine
*	Copyright (c) 2022 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Scene/Physics/HyPhysicsGrid2d.h"
#include "HyEngine.h"

HyPhysicsGrid2d::HyPhysicsGrid2d(glm::vec2 vGravity /*= glm::vec2(0.0f, -10.0f)*/, float fPixelsPerMeter /*= 80.0f*/, int32 iVelocityIterations /*= 8*/, int32 iPositionIterations /*= 3*/, HyEntity2d *pParent /*= nullptr*/) :
	HyEntity2d(pParent),
	m_b2World(b2Vec2(vGravity.x, vGravity.y)),
	m_fPixelsPerMeter(fPixelsPerMeter),
	m_fPpmInverse(1.0f / fPixelsPerMeter),
	m_iPhysVelocityIterations(iVelocityIterations),
	m_iPhysPositionIterations(iPositionIterations),
	m_DebugDraw(*this)
{
	HyAssert(m_fPixelsPerMeter > 0.0f, "HarmonyInit's 'fPixelsPerMeter' cannot be <= 0.0f");
	m_b2World.SetContactListener(&m_ContactListener);

	m_DebugDraw.SetFlags(0xff);
	m_b2World.SetDebugDraw(&m_DebugDraw);

	HyScene::AddPhysicsGrid(this);
}

HyPhysicsGrid2d::~HyPhysicsGrid2d()
{
	const b2Body *pIter = m_b2World.GetBodyList();
	while(pIter)
	{
		reinterpret_cast<HyEntity2d *>(pIter->GetUserData().pointer)->PhysRelease();
		pIter = m_b2World.GetBodyList();
	}

	HyScene::RemovePhysicsGrid(this);
}

void HyPhysicsGrid2d::Update()
{
	if(m_DebugDraw.GetFlags() != 0)
	{
		m_DebugDraw.GetDrawList().clear();
		m_b2World.DebugDraw();
	}

	m_b2World.Step(HyEngine::DeltaTime(), m_iPhysVelocityIterations, m_iPhysPositionIterations);
}

std::vector<HyPrimitive2d> &HyPhysicsGrid2d::GetDebugDrawList()
{
	return m_DebugDraw.GetDrawList();
}

float HyPhysicsGrid2d::GetPixelsPerMeter()
{
	return m_fPixelsPerMeter;
}

float HyPhysicsGrid2d::GetPpmInverse()
{
	return m_fPpmInverse;
}

bool HyPhysicsGrid2d::IsDebugDraw()
{
	return m_DebugDraw.GetFlags() != 0;
}

void HyPhysicsGrid2d::EnableDebugDraw(bool bEnableDebugDraw)
{
	if(bEnableDebugDraw)
		m_DebugDraw.SetFlags(b2Draw::e_shapeBit/* | b2Draw::e_jointBit | b2Draw::e_aabbBit | b2Draw::e_pairBit | b2Draw::e_centerOfMassBit*/);
	else
		m_DebugDraw.ClearFlags(b2Draw::e_shapeBit/* | b2Draw::e_jointBit | b2Draw::e_aabbBit | b2Draw::e_pairBit | b2Draw::e_centerOfMassBit*/);
}
