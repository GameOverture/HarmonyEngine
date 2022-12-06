/**************************************************************************
 *	HyBox2dContactListener.cpp
 *
 *	Harmony Engine
 *	Copyright (c) 2022 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Scene/HyScene.h"
#include "Scene/Physics/HyBox2dContactListener.h"
#include "Scene/Nodes/Loadables/Bodies/IHyBody2d.h"

HyBox2dContactListener::HyBox2dContactListener(HyScene &sceneRef) :
	m_SceneRef(sceneRef)
{
}


HyBox2dContactListener::~HyBox2dContactListener(void)
{
}

/// Called when two fixtures begin to touch.
/*virtual*/ void HyBox2dContactListener::BeginContact(b2Contact *contact) /*override*/
{
	//contact->GetWorldManifold(
	//if(contact->GetFixtureA()->GetBody()->GetType() == b2_dynamicBody)
	//{
	//	if(contact->GetFixtureB()->GetBody()->GetType() == b2_kinematicBody || contact->GetFixtureB()->GetBody()->GetType() == b2_staticBody)
	//	{
	//		IHyBody2d *pBodyNode = reinterpret_cast<IHyBody2d *>(contact->GetFixtureA()->GetUserData().pointer);
	//		pBodyNode->shape
	//	}
	//}
	//else if(contact->GetFixtureB()->GetBody()->GetType() == b2_dynamicBody)
	//{
	//	if(contact->GetFixtureA()->GetBody()->GetType() == b2_kinematicBody || contact->GetFixtureA()->GetBody()->GetType() == b2_staticBody)
	//		int i = 0;// reinterpret_cast<HyPhysEntity2d::tFixture *>(contact->GetFixtureB()->GetUserData())->SetGrounded();
	//}
}

/// Called when two fixtures cease to touch.
/*virtual*/ void HyBox2dContactListener::EndContact(b2Contact *contact) /*override*/
{
	//if(contact->GetFixtureA()->GetBody()->GetType() == b2_dynamicBody)
	//{
	//	if(contact->GetFixtureB()->GetBody()->GetType() == b2_kinematicBody || contact->GetFixtureB()->GetBody()->GetType() == b2_staticBody)
	//		reinterpret_cast<HyPhysEntity2d::tFixture *>(contact->GetFixtureA()->GetUserData())->UnsetGrounded();
	//}
	//else if(contact->GetFixtureB()->GetBody()->GetType() == b2_dynamicBody)
	//{
	//	if(contact->GetFixtureA()->GetBody()->GetType() == b2_kinematicBody || contact->GetFixtureA()->GetBody()->GetType() == b2_staticBody)
	//		reinterpret_cast<HyPhysEntity2d::tFixture *>(contact->GetFixtureB()->GetUserData())->UnsetGrounded();
	//}
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
/*virtual*/ void HyBox2dContactListener::PreSolve(b2Contact *contact, const b2Manifold *oldManifold) /*override*/
{
}

/// This lets you inspect a contact after the solver is finished. This is useful
/// for inspecting impulses.
/// Note: the contact manifold does not include time of impact impulses, which can be
/// arbitrarily large if the sub-step is small. Hence the impulse is provided explicitly
/// in a separate data structure.
/// Note: this is only called for contacts that are touching, solid, and awake.
/*virtual*/ void HyBox2dContactListener::PostSolve(b2Contact *contact, const b2ContactImpulse *impulse) /*override*/
{
	if(contact->GetFixtureA()->GetBody()->GetType() == b2_dynamicBody)
	{
	
	}
	
	if(contact->GetFixtureB()->GetBody()->GetType() == b2_dynamicBody)
	{
		//b2Body *pBodyBox2d = contact->GetFixtureB()->GetBody();
		//IHyBody2d *pBodyNode = reinterpret_cast<IHyBody2d *>(pBodyBox2d->GetUserData().pointer);

		//pBodyNode->SetDirty(IHyNode::DIRTY_FromUpdater);
		//HyLog("PHYS POS: " << pBodyBox2d->GetPosition().x * m_SceneRef.GetPixelsPerMeter() << ", " << pBodyBox2d->GetPosition().y * m_SceneRef.GetPixelsPerMeter());

		//const glm::mat4 &mtxSceneRef = pBodyNode->GetSceneTransform(0.0f);
		//glm::vec3 ptTranslation = mtxSceneRef[3];
		//glm::vec3 vRotations = glm::eulerAngles(glm::quat_cast(mtxSceneRef));
		//
		//pBodyNode->pos.Offset(pBodyBox2d->GetPosition().x * m_SceneRef.GetPixelsPerMeter() - ptTranslation.x,
		//					  pBodyBox2d->GetPosition().y * m_SceneRef.GetPixelsPerMeter() - ptTranslation.y);
		//pBodyNode->rot.Set(glm::degrees(pBodyBox2d->GetAngle()));
		//pBodyNode->ClearDirty(IHyNode::DIRTY_FromUpdater);
	}
}
