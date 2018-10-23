/**************************************************************************
 *	HyBox2dRuntime.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Scene/Physics/HyBox2dRuntime.h"
#include "Scene/Physics/HyPhysEntity2d.h"

HyBox2dRuntime::HyBox2dRuntime(void)
{
}


HyBox2dRuntime::~HyBox2dRuntime(void)
{
}

/// Called when two fixtures begin to touch.
/*virtual*/ void HyBox2dRuntime::BeginContact(b2Contact* contact)
{
	if(contact->GetFixtureA()->GetBody()->GetType() == b2_dynamicBody)
	{
		if(contact->GetFixtureB()->GetBody()->GetType() == b2_kinematicBody || contact->GetFixtureB()->GetBody()->GetType() == b2_staticBody)
			reinterpret_cast<HyPhysEntity2d::tFixture *>(contact->GetFixtureA()->GetUserData())->SetGrounded();
	}
	else if(contact->GetFixtureB()->GetBody()->GetType() == b2_dynamicBody)
	{
		if(contact->GetFixtureA()->GetBody()->GetType() == b2_kinematicBody || contact->GetFixtureA()->GetBody()->GetType() == b2_staticBody)
			reinterpret_cast<HyPhysEntity2d::tFixture *>(contact->GetFixtureB()->GetUserData())->SetGrounded();
	}
}

/// Called when two fixtures cease to touch.
/*virtual*/ void HyBox2dRuntime::EndContact(b2Contact* contact)
{
	if(contact->GetFixtureA()->GetBody()->GetType() == b2_dynamicBody)
	{
		if(contact->GetFixtureB()->GetBody()->GetType() == b2_kinematicBody || contact->GetFixtureB()->GetBody()->GetType() == b2_staticBody)
			reinterpret_cast<HyPhysEntity2d::tFixture *>(contact->GetFixtureA()->GetUserData())->UnsetGrounded();
	}
	else if(contact->GetFixtureB()->GetBody()->GetType() == b2_dynamicBody)
	{
		if(contact->GetFixtureA()->GetBody()->GetType() == b2_kinematicBody || contact->GetFixtureA()->GetBody()->GetType() == b2_staticBody)
			reinterpret_cast<HyPhysEntity2d::tFixture *>(contact->GetFixtureB()->GetUserData())->UnsetGrounded();
	}
}

/// This is called after a contact is updated. This allows you to inspect a
/// contact before it goes to the solver. If you are careful, you can modify the
/// contact manifold (e.g. disable contact).
/// A copy of the old manifold is provided so that you can detect changes.
/// Note: this is called only for awake bodies.
/// Note: this is called even when the number of contact points is zero.
/// Note: this is not called for sensors.
/// Note: if you set the number of contact points to zero, you will not
/// get an EndContact callback. However, you may get a BeginContact callback
/// the next step.
/*virtual*/ void HyBox2dRuntime::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{
}

/// This lets you inspect a contact after the solver is finished. This is useful
/// for inspecting impulses.
/// Note: the contact manifold does not include time of impact impulses, which can be
/// arbitrarily large if the sub-step is small. Hence the impulse is provided explicitly
/// in a separate data structure.
/// Note: this is only called for contacts that are touching, solid, and awake.
/*virtual*/ void HyBox2dRuntime::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
{
}
