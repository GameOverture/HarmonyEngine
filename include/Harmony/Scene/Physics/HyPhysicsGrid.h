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
#include "Scene/Physics/HyDrawPhys2d.h"
#include "Scene/Physics/HyBox2dRuntime.h"

class HyPhysicsGrid : public b2World
{
	int32			m_iPhysVelocityIterations;
	int32			m_iPhysPositionIterations;

	HyDrawPhys2d	m_DrawPhys2d;
	HyBox2dRuntime	m_Phys2dContactListener;

public:
	HyPhysicsGrid(glm::vec2 vGravity, int32 iVelocityIterations = 8, int32 iPositionIterations = 3);
	virtual ~HyPhysicsGrid();

	void Update();
	std::vector<HyPrimitive2d> &GetDebugDrawList();
};

#endif /* HyPhysicsGrid_h__ */
