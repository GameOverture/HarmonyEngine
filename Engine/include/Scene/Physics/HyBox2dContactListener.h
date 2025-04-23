/**************************************************************************
 *	HyBox2dContactListener.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2022 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyBox2dContactListener_h__
#define HyBox2dContactListener_h__

#include "Afx/HyStdAfx.h"

//class HyBox2dContactListener : public b2ContactListener
//{
//	HyScene &			m_SceneRef;
//
//public:
//	HyBox2dContactListener(HyScene &sceneRef);
//	virtual ~HyBox2dContactListener(void);
//
//	/// Called when two fixtures begin to touch.
//	virtual void BeginContact(b2Contact* contact) override;
//
//	/// Called when two fixtures cease to touch.
//	virtual void EndContact(b2Contact* contact) override;
//
//	/// This is called after a contact is updated. This allows you to inspect a
//	/// contact before it goes to the solver. If you are careful, you can modify the
//	/// contact manifold (e.g. disable contact).
//	/// A copy of the old manifold is provided so that you can detect changes.
//	/// Note: this is called only for awake bodies.
//	/// Note: this is called even when the number of contact points is zero.
//	/// Note: this is not called for sensors.
//	/// Note: if you set the number of contact points to zero, you will not
//	/// get an EndContact callback. However, you may get a BeginContact callback
//	/// the next step.
//	virtual void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) override;
//
//	/// This lets you inspect a contact after the solver is finished. This is useful
//	/// for inspecting impulses.
//	/// Note: the contact manifold does not include time of impact impulses, which can be
//	/// arbitrarily large if the sub-step is small. Hence the impulse is provided explicitly
//	/// in a separate data structure.
//	/// Note: this is only called for contacts that are touching, solid, and awake.
//	virtual void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) override;
//};

#endif /* HyBox2dContactListener_h__ */
