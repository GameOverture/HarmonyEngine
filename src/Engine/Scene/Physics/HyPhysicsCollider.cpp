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
#include "Scene/Physics/HyPhysicsCollider.h"

HyPhysicsCollider::HyPhysicsCollider(b2Fixture *pFixture) :
	m_pFixture(pFixture)
{
}

HyPhysicsCollider::~HyPhysicsCollider()
{
}

b2Fixture *HyPhysicsCollider::GetFixture()
{
	return m_pFixture;
}
