/**************************************************************************
*	HyBoundingVolume2d.h
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyBoundingVolume2d_h__
#define HyBoundingVolume2d_h__

#include "Afx/HyStdAfx.h"
#include "Box2D/Box2D.h"
#include "Utilities/HyMath.h"

class IHyNode2d;

class HyBoundingVolume2d
{
	IHyNode2d &		m_OwnerRef;

	b2Shape *		m_pPhysicsShape;

	enum eType
	{
		TYPE_None = 0,
		TYPE_AABB
	};
	eType					m_eType;

	b2AABB					m_LocalAABB;
	b2AABB					m_WorldAABB;

public:
	HyBoundingVolume2d(IHyNode2d &ownerRef);
	~HyBoundingVolume2d();

	bool IsEnabled();

	void SetLocalAABB(const glm::vec2 &ptLowerBound, const glm::vec2 &ptUpperBound);

	bool IsWorldPointCollide(glm::vec2 &pt);

	HyRectangle<float> GetWorldAABB();
	

private:
	void UpdateWorldAABB();
};

#endif /* HyBoundingVolume2d_h__ */
