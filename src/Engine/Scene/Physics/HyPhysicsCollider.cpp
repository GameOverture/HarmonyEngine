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

HyPhysicsCollider::HyPhysicsCollider(b2Body *pBody, const HyShape2d &shapeRef, float fDensity, bool bIsSensor) :
	m_pFixture(nullptr)
{
	b2FixtureDef def;
	def.density = fDensity;
	def.userData = this;
	def.shape = shapeRef.ClonePpmShape(static_cast<HyPhysicsGrid *>(pBody->GetWorld())->GetPpmInverse());
	def.friction = 0.2f;		// The friction coefficient, usually in the range [0,1].
	def.restitution = 0.0f;		// The restitution (elasticity) usually in the range [0,1].
	def.density = 0.0f;			// The density, usually in kg/m^2.
	def.isSensor = bIsSensor;

	m_pFixture = pBody->CreateFixture(&def);

	delete def.shape;
}

HyPhysicsCollider::~HyPhysicsCollider()
{
}

b2Fixture *HyPhysicsCollider::GetFixture()
{
	return m_pFixture;
}
