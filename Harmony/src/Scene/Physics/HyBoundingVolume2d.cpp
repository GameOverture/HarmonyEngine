/**************************************************************************
*	HyBoundingVolume2d.cpp
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Scene/Physics/HyBoundingVolume2d.h"

#include "Scene/Nodes/Transforms/IHyTransform2d.h"

HyBoundingVolume2d::HyBoundingVolume2d(IHyTransform2d &ownerRef) :	m_OwnerRef(ownerRef),
																	m_eType(TYPE_None)
{
}

HyBoundingVolume2d::~HyBoundingVolume2d()
{
}

bool HyBoundingVolume2d::IsEnabled()
{
	return m_eType != TYPE_None;
}

void HyBoundingVolume2d::SetLocalAABB(glm::vec2 &ptLowerBound, glm::vec2 &ptUpperBound)
{
	m_LocalAABB.lowerBound.Set(ptLowerBound.x, ptLowerBound.y);
	m_LocalAABB.upperBound.Set(ptUpperBound.x, ptUpperBound.y);

	m_eType = TYPE_AABB;
}

bool HyBoundingVolume2d::IsWorldPointCollide(glm::vec2 &pt)
{
	UpdateWorldAABB();

	if(pt.x > m_WorldAABB.lowerBound.x && pt.x < m_WorldAABB.upperBound.x &&
	   pt.y > m_WorldAABB.lowerBound.y && pt.y < m_WorldAABB.upperBound.y)
	{
		return true;
	}
	
	return false;
}

HyRectangle<float> HyBoundingVolume2d::GetWorldAABB()
{
	UpdateWorldAABB();
	return HyRectangle<float>(m_WorldAABB.lowerBound.x, m_WorldAABB.upperBound.y, m_WorldAABB.upperBound.x, m_WorldAABB.lowerBound.y);
}

void HyBoundingVolume2d::UpdateWorldAABB()
{
	glm::mat4 mtx;
	m_OwnerRef.GetWorldTransform(mtx);

	m_WorldAABB = m_LocalAABB;

	// Scale
	m_WorldAABB.lowerBound.x *= mtx[0].x;
	m_WorldAABB.lowerBound.y *= mtx[1].y;
	m_WorldAABB.upperBound.x *= mtx[0].x;
	m_WorldAABB.upperBound.y *= mtx[1].y;

	// Translate
	m_WorldAABB.lowerBound.x += mtx[3].x;
	m_WorldAABB.lowerBound.y += mtx[3].y;
	m_WorldAABB.upperBound.x += mtx[3].x;
	m_WorldAABB.upperBound.y += mtx[3].y;
}
