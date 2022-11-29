/**************************************************************************
*	HyPhysicsGrid.h
*	
*	Harmony Engine
*	Copyright (c) 2022 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyPhysicsGrid_h__
#define HyPhysicsGrid_h__

#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Loadables/Bodies/Objects/HyEntity2d.h"
#include "Scene/Physics/HyBox2dContactListener.h"
#include "Scene/Physics/HyPhysicsDebug2d.h"

struct HyPhysicsComponent2d
{
	b2Body *									m_pBody;
	b2Fixture *									m_pFixture;

	HyPhysicsComponent2d() :
		m_pBody(nullptr),
		m_pFixture(nullptr)
	{ }
};

class HyPhysicsGrid2d : public HyEntity2d
{
	friend class HyPhysicsCtrl2d;
	friend class IHyBody2d;

protected:
	b2World										m_b2World;

	float										m_fPixelsPerMeter;
	float										m_fPpmInverse;
	float										m_fTimeScalar;

	int32										m_iPhysVelocityIterations;
	int32										m_iPhysPositionIterations;

	HyBox2dContactListener						m_ContactListener;
	HyPhysicsDebug2d							m_DebugDraw;

	
	std::map<IHyBody2d *, HyPhysicsComponent2d>	m_PhysChildMap;

public:
	HyPhysicsGrid2d(glm::vec2 vGravity = glm::vec2(0.0f, -10.0f), float fPixelsPerMeter = 80.0f, int32 iVelocityIterations = 8, int32 iPositionIterations = 3, HyEntity2d *pParent = nullptr);
	virtual ~HyPhysicsGrid2d();

	virtual void SetNewChildAttributes(IHyNode2d &childRef) override;
	virtual bool ChildRemove(IHyNode2d *pChild) override;
	
	void  SetTimeScalar(float fTimeScalar);

	float GetPixelsPerMeter();
	float GetPpmInverse();

	b2World& GetWorld() { return m_b2World; }

	bool IsDebugDraw();
	void EnableDebugDraw(bool bEnableDebugDraw);
	std::vector<HyPrimitive2d> &GetDebugDrawList();

protected:
	virtual void Update() override;
	
	void TryInitChildPhysics(IHyBody2d &bodyRef); // Does nothing if 'bodyRef.physics.m_pInit' is null or shape not valid
	void UninitChildPhysics(IHyBody2d &bodyRef);
};

#endif /* HyPhysicsGrid_h__ */
