/**************************************************************************
*	HyPhysicsGrid.h
*	
*	Harmony Engine
*	Copyright (c) 2020 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyPhysicsGrid_h__
#define HyPhysicsGrid_h__

#include "Afx/HyStdAfx.h"
#include "Scene/Physics/HyPhysicsDebug2d.h"
#include "Scene/Physics/HyBox2dRuntime.h"

class HyPhysicsGrid2d : public b2World
{
	float				m_fPixelsPerMeter;
	float				m_fPpmInverse;

	int32				m_iPhysVelocityIterations;
	int32				m_iPhysPositionIterations;

	HyPhysicsDebug2d	m_DrawPhys2d;
	HyBox2dRuntime		m_Phys2dContactListener;

public:
	HyPhysicsGrid2d(glm::vec2 vGravity = glm::vec2(0.0f, -10.0f), float fPixelsPerMeter = 80.0f, int32 iVelocityIterations = 8, int32 iPositionIterations = 3);
	virtual ~HyPhysicsGrid2d();

	void Update();
	std::vector<HyPrimitive2d> &GetDebugDrawList();

	float GetPixelsPerMeter();
	float GetPpmInverse();

	bool IsDebugDraw();
	void DebugDraw(bool bEnableDebugDraw);
};

#endif /* HyPhysicsGrid_h__ */
