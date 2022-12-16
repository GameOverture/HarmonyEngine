///**************************************************************************
//*	HyPhysicsGrid2d.cpp
//*	
//*	Harmony Engine
//*	Copyright (c) 2022 Jason Knobler
//*
//*	Harmony License:
//*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
//*************************************************************************/
//#include "Afx/HyStdAfx.h"
//#include "Scene/Nodes/Loadables/Bodies/Objects/HyPhysicsGrid2d.h"
//#include "HyEngine.h"
//
//HyPhysicsGrid2d::HyPhysicsGrid2d(glm::vec2 vGravity /*= glm::vec2(0.0f, -10.0f)*/, float fPixelsPerMeter /*= 80.0f*/, int32 iVelocityIterations /*= 8*/, int32 iPositionIterations /*= 3*/, HyEntity2d *pParent /*= nullptr*/) :
//	HyEntity2d(pParent),
//	m_b2World(b2Vec2(vGravity.x, vGravity.y)),
//	m_fPixelsPerMeter(fPixelsPerMeter),
//	m_fPpmInverse(1.0f / fPixelsPerMeter),
//	m_fTimeScalar(1.0f),
//	m_iPhysVelocityIterations(iVelocityIterations),
//	m_iPhysPositionIterations(iPositionIterations),
//	m_DebugDraw(*this)
//{
//	m_uiFlags |= NODETYPE_IsPhysicsGrid;
//
//	HyAssert(m_fPixelsPerMeter > 0.0f, "HarmonyInit's 'fPixelsPerMeter' cannot be <= 0.0f");
//	//m_b2World.SetContactListener(&m_ContactListener);
//
//	//m_DebugDraw.SetFlags(0xff);
//	//m_b2World.SetDebugDraw(&m_DebugDraw);
//}
//
///*virtual*/ HyPhysicsGrid2d::~HyPhysicsGrid2d()
//{
//	while(m_ChildList.empty() == false)
//		m_ChildList[m_ChildList.size() - 1]->ParentDetach();
//}
//
///*virtual*/ void HyPhysicsGrid2d::SetNewChildAttributes(IHyNode2d &childRef) /*override*/
//{
//	HyEntity2d::SetNewChildAttributes(childRef);
//
//	if(0 != (childRef.GetInternalFlags() & NODETYPE_IsBody))
//	{
//		IHyBody2d &bodyRef = static_cast<IHyBody2d &>(childRef);
//		TryInitChildPhysics(bodyRef);
//	}
//}
//
///*virtual*/ bool HyPhysicsGrid2d::ChildRemove(IHyNode2d *pChild) /*override*/
//{
//	bool bValidRemoval = HyEntity2d::ChildRemove(pChild);
//	if(bValidRemoval)
//	{
//		if(0 != (pChild->GetInternalFlags() & NODETYPE_IsBody))
//		{
//			IHyBody2d *pNode = static_cast<IHyBody2d *>(pChild);
//			auto iter = m_PhysChildMap.find(pNode);
//			if(iter != m_PhysChildMap.end())
//			{
//				m_b2World.DestroyBody(iter->second.m_pBody);
//				
//				pNode->physics.m_pSimData = nullptr;
//				m_PhysChildMap.erase(iter);
//			}
//		}
//	}
//
//	return bValidRemoval;
//}
//
//void HyPhysicsGrid2d::TryInitChildPhysics(IHyBody2d &bodyRef)
//{
//
//}
//
//void HyPhysicsGrid2d::UninitChildPhysics(IHyBody2d &bodyRef)
//{
//	auto iter = m_PhysChildMap.find(&bodyRef);
//	if(iter == m_PhysChildMap.end())
//		return;
//
//	m_b2World.DestroyBody(iter->second.m_pBody);
//	iter->first->physics.m_pSimData = nullptr;
//	
//	m_PhysChildMap.erase(&bodyRef);
//}
//
//void  HyPhysicsGrid2d::SetTimeScalar(float fTimeScalar)
//{
//	m_fTimeScalar = fTimeScalar;
//}
//
//float HyPhysicsGrid2d::GetPixelsPerMeter()
//{
//	return m_fPixelsPerMeter;
//}
//
//float HyPhysicsGrid2d::GetPpmInverse()
//{
//	return m_fPpmInverse;
//}
//
//bool HyPhysicsGrid2d::IsDebugDraw()
//{
//	return m_DebugDraw.GetFlags() != 0;
//}
//
//void HyPhysicsGrid2d::EnableDebugDraw(bool bEnableDebugDraw)
//{
//	if(bEnableDebugDraw)
//		m_DebugDraw.SetFlags(b2Draw::e_shapeBit/* | b2Draw::e_jointBit | b2Draw::e_aabbBit | b2Draw::e_pairBit | b2Draw::e_centerOfMassBit*/);
//	else
//		m_DebugDraw.ClearFlags(b2Draw::e_shapeBit/* | b2Draw::e_jointBit | b2Draw::e_aabbBit | b2Draw::e_pairBit | b2Draw::e_centerOfMassBit*/);
//}
//
//std::vector<HyPrimitive2d> &HyPhysicsGrid2d::GetDebugDrawList()
//{
//	return m_DebugDraw.GetDrawList();
//}
//
///*virtual*/ void HyPhysicsGrid2d::Update() /*override*/
//{
//	for(auto iter = m_PhysChildMap.begin(); iter != m_PhysChildMap.end(); ++iter)
//		iter->first->physics.Update();
//
//	if(m_DebugDraw.GetFlags() != 0)
//	{
//		m_DebugDraw.GetDrawList().clear();
//		m_b2World.DebugDraw();
//	}
//
//	m_b2World.Step(HyEngine::DeltaTime() * m_fTimeScalar, m_iPhysVelocityIterations, m_iPhysPositionIterations);
//
//	HyEntity2d::Update();
//}
