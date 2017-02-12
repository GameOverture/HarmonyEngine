/**************************************************************************
*	HyBoundingVolume2d.h
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef __HyBoundingVolume2d_h__
#define __HyBoundingVolume2d_h__

#include "Afx/HyStdAfx.h"
#include "Box2D/Box2D.h"

#include "Utilities/HyMath.h"

class IHyTransform2d;

class HyBoundingVolume2d
{
	IHyTransform2d &		m_OwnerRef;

	enum eType
	{
		TYPE_None = 0,
		TYPE_AABB
	};
	eType					m_eType;

	b2AABB					m_LocalAABB;
	b2AABB					m_WorldAABB;

public:
	HyBoundingVolume2d(IHyTransform2d &ownerRef);
	~HyBoundingVolume2d();

	bool IsEnabled();

	void SetLocalAABB(glm::vec2 &ptLowerBound, glm::vec2 &ptUpperBound);
	void SetLocalAABB(HyBoundingVolume2d &otherBV);

	bool IsWorldPointCollide(glm::vec2 &pt);

	HyRectangle<float> GetWorldAABB();
	

private:
	void UpdateWorldAABB();
};

#endif /* __HyBoundingVolume2d_h__ */
