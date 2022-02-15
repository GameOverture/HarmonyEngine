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
#include "Scene/Nodes/Loadables/Bodies/Objects/HyPhysicsGrid2d.h"
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
}

/*virtual*/ HyPhysicsGrid2d::~HyPhysicsGrid2d()
{
}

/*virtual*/ void HyPhysicsGrid2d::SetNewChildAttributes(IHyNode2d &childRef) /*override*/
{
	HyEntity2d::SetNewChildAttributes(childRef);

	if(0 != (childRef.GetInternalFlags() & NODETYPE_IsBody))
	{
		IHyBody2d &bodyRef = static_cast<IHyBody2d &>(childRef);
		InitChildPhysics(bodyRef); // Does nothing if 'bodyRef.physics.m_pInit' is null
	}
}

/*virtual*/ bool HyPhysicsGrid2d::ChildRemove(IHyNode2d *pChild) /*override*/
{
	bool bValidRemoval = HyEntity2d::ChildRemove(pChild);
	if(bValidRemoval)
	{
		if(0 != (pChild->GetInternalFlags() & NODETYPE_IsBody))
		{
			IHyBody2d *pNode = static_cast<IHyBody2d *>(pChild);
			auto iter = m_PhysChildMap.find(pNode);
			if(iter != m_PhysChildMap.end())
			{
				m_b2World.DestroyBody(iter->second.m_pBody);
				
				pNode->physics.m_pData = nullptr;
				m_PhysChildMap.erase(iter);
			}
		}
	}

	return bValidRemoval;
}

void HyPhysicsGrid2d::InitChildPhysics(IHyBody2d &bodyRef)
{
	if(bodyRef.physics.m_pInit == nullptr)
		return;


	
	HyPhysicsComponent2d &physicsCompRef = m_PhysChildMap.emplace(std::pair<IHyBody2d *, HyPhysicsComponent2d>(&bodyRef, HyPhysicsComponent2d())).first->second;
	physicsCompRef.m_pNode = &bodyRef;

	//bodyRef.physics.m_pData = ;



	//PhysRelease();

	//b2Body *pBody, const b2Shape *pb2PpmShape, float fDensity, float fFriction, float fRestitution, bool bIsSensor, b2Filter collideFilter
	//	b2FixtureDef def;
	//def.shape = pb2PpmShape;
	////def.userData = this;
	//def.friction = 0.2f;		// The friction coefficient, usually in the range [0,1].
	//def.restitution = 0.0f;		// The restitution (elasticity) usually in the range [0,1].
	//def.density = fDensity;		// The density, usually in kg/m^2.
	//def.isSensor = bIsSensor;
	//def.filter = collideFilter;

	//m_pFixture = pBody->CreateFixture(&def);

	//b2Body *pPhysicsBody = m_b2World.CreateBody(&bodyDef);

	//bodyRef.physics.m_pData->m_pBody
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

std::vector<HyPrimitive2d> &HyPhysicsGrid2d::GetDebugDrawList()
{
	return m_DebugDraw.GetDrawList();
}

/*virtual*/ void HyPhysicsGrid2d::OnUpdate() /*override*/
{
	for(auto iter = m_PhysChildMap.begin(); iter != m_PhysChildMap.end(); ++iter)
		iter->second.m_pNode->physics.Update();

	if(m_DebugDraw.GetFlags() != 0)
	{
		m_DebugDraw.GetDrawList().clear();
		m_b2World.DebugDraw();
	}

	m_b2World.Step(HyEngine::DeltaTime(), m_iPhysVelocityIterations, m_iPhysPositionIterations);
}
