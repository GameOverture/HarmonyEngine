/**************************************************************************
*	HyPhysicsCollider.cpp
*	
*	Harmony Engine
*	Copyright (c) 2020 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Scene/Physics/HyPhysicsGrid.h"
#include "Scene/Physics/HyPhysicsCollider.h"

HyPhysicsCollider::HyPhysicsCollider(b2Body *pBody, const b2Shape *pb2PpmShape, float fDensity, float fFriction, float fRestitution, bool bIsSensor, b2Filter collideFilter) :
	m_pFixture(nullptr)
{
	b2FixtureDef def;
	def.shape = pb2PpmShape;
	def.userData = this;
	def.friction = 0.2f;		// The friction coefficient, usually in the range [0,1].
	def.restitution = 0.0f;		// The restitution (elasticity) usually in the range [0,1].
	def.density = fDensity;		// The density, usually in kg/m^2.
	def.isSensor = bIsSensor;
	def.filter = collideFilter;

	m_pFixture = pBody->CreateFixture(&def);
}

HyPhysicsCollider::~HyPhysicsCollider()
{
}

b2Fixture *HyPhysicsCollider::GetFixture()
{
	return m_pFixture;
}
