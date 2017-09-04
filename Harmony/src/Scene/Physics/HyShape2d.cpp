/**************************************************************************
*	HyShape2d.cpp
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Scene/Physics/HyShape2d.h"
#include "Scene/Nodes/IHyNode2d.h"

HyShape2d::HyShape2d(IHyNode2d &ownerRef) :	m_OwnerRef(ownerRef),
											m_pShape(nullptr)
{
}

/*virtual*/ HyShape2d::~HyShape2d()
{
	delete m_pShape;
}

bool HyShape2d::IsValid()
{
	return m_pShape != nullptr;
}

void HyShape2d::SetAsLineLoop(const glm::vec2 *pVertices, uint32 uiNumVerts)
{
	m_OwnerRef.OnShapeSet(this);
}

void HyShape2d::SetAsLineChain(const glm::vec2 *pVertices, uint32 uiNumVerts)
{
	m_OwnerRef.OnShapeSet(this);
}

void HyShape2d::SetAsLineSegment(const glm::vec2 &pt1, const glm::vec2 &pt2)
{
	m_OwnerRef.OnShapeSet(this);
}

void HyShape2d::SetAsCircle(float fRadius)
{
	m_OwnerRef.OnShapeSet(this);
}

void HyShape2d::SetAsPolygon(const glm::vec2 *pPointArray, uint32 uiCount)
{
	m_OwnerRef.OnShapeSet(this);
}

void HyShape2d::SetAsBox(float fHalfWidth, float fHalfHeight)
{
	m_OwnerRef.OnShapeSet(this);
}

void HyShape2d::SetAsBox(float fHalfWidth, float fHalfHeight, const glm::vec2 &ptBoxCenter, float fRotDeg)
{
	m_OwnerRef.OnShapeSet(this);
}

bool HyShape2d::TestPoint(glm::vec2 ptWorldPoint) const
{
	return m_pShape->TestPoint(b2Transform(b2Vec2(m_OwnerRef.pos.X(), m_OwnerRef.pos.Y()), b2Rot(glm::radians(m_OwnerRef.rot.Get()))), b2Vec2(ptWorldPoint.x, ptWorldPoint.y));
}
