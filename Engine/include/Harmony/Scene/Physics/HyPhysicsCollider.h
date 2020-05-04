/**************************************************************************
*	HyPhysicsCollider.h
*	
*	Harmony Engine
*	Copyright (c) 2020 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyPhysicsCollider_h__
#define HyPhysicsCollider_h__

#include "Afx/HyStdAfx.h"
#include "Scene/Physics/HyShape2d.h"

class HyPhysicsCollider
{
	friend class HyEntity2d;

	b2Fixture *		m_pFixture;

public:
	HyPhysicsCollider(b2Body *pBody, const b2Shape *pb2PpmShape, float fDensity, float fFriction, float fRestitution, bool bIsSensor, b2Filter collideFilter);
	~HyPhysicsCollider();

private:
	b2Fixture *GetFixture();
};

#endif /* HyPhysicsCollider_h__ */
