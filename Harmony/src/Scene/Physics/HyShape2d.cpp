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
#include "Diagnostics/Console/HyConsole.h"

#if 0 // Use this if sizeof(b2Vec2) != sizeof(glm::vec2)
	#define HYSHAPEVECTOR_GLM_TO_B2(x) new b2Vec2[uiNumVerts];
...
#else
	#define HYSHAPEVECTOR_GLM_TO_B2(x) reinterpret_cast<const b2Vec2 *>(x);
#endif

HyShape2d::HyShape2d(IHyNode2d &ownerRef) :	m_OwnerRef(ownerRef),
											m_eType(HYSHAPE_Unknown),
											m_pShape(nullptr)
{
}

/*virtual*/ HyShape2d::~HyShape2d()
{
	delete m_pShape;
}

const HyShape2d &HyShape2d::operator=(const HyShape2d &rhs)
{
	delete m_pShape;

	switch(rhs.GetType())
	{
		case HYSHAPE_LineSegment: {
			m_eType = HYSHAPE_LineSegment;
			b2EdgeShape *pRhsEdgeShape = static_cast<b2EdgeShape *>(rhs.m_pShape);

			delete m_pShape;
			m_pShape = new b2EdgeShape(*pRhsEdgeShape);
		} break;

		case HYSHAPE_LineLoop: {
			m_eType = HYSHAPE_LineLoop;
			b2ChainShape *pRhsChainShape = static_cast<b2ChainShape *>(rhs.m_pShape);

			delete m_pShape;
			m_pShape = new b2ChainShape(*pRhsChainShape);

			// NOTE: Box2d doesn't have a proper copy constructor for b2ChainShape as it uses its own dynamic memory
			static_cast<b2ChainShape *>(m_pShape)->m_vertices = nullptr;
			static_cast<b2ChainShape *>(m_pShape)->CreateLoop(pRhsChainShape->m_vertices, pRhsChainShape->m_count);
		} break;

		case HYSHAPE_LineChain: {
			m_eType = HYSHAPE_LineChain;
			b2ChainShape *pRhsChainShape = static_cast<b2ChainShape *>(rhs.m_pShape);

			delete m_pShape;
			m_pShape = new b2ChainShape(*pRhsChainShape);

			// NOTE: Box2d doesn't have a proper copy constructor for b2ChainShape as it uses its own dynamic memory
			static_cast<b2ChainShape *>(m_pShape)->m_vertices = nullptr;
			static_cast<b2ChainShape *>(m_pShape)->CreateChain(pRhsChainShape->m_vertices, pRhsChainShape->m_count);
		} break;

		case HYSHAPE_Circle: {
			m_eType = HYSHAPE_Circle;
			b2CircleShape *pRhsCircleShape = static_cast<b2CircleShape *>(rhs.m_pShape);

			delete m_pShape;
			m_pShape = new b2CircleShape(*pRhsCircleShape);
		} break;
		
		case HYSHAPE_Polygon: {
			m_eType = HYSHAPE_Polygon;
			b2PolygonShape *pRhsPolygonShape = static_cast<b2PolygonShape *>(rhs.m_pShape);

			delete m_pShape;
			m_pShape = new b2PolygonShape(*pRhsPolygonShape);
		} break;

		default:
			HyLogError("HyShape2d::operator=() - Unknown shape type: " << rhs.GetType());
	}

	return *this;
}

HyShapeType HyShape2d::GetType() const
{
	return m_eType;
}

b2Shape *HyShape2d::GetB2Shape()
{
	return m_pShape;
}

bool HyShape2d::IsValid()
{
	return m_pShape != nullptr && m_eType != HYSHAPE_Unknown;
}

void HyShape2d::SetAsLineSegment(const glm::vec2 &pt1, const glm::vec2 &pt2)
{
	m_eType = HYSHAPE_LineSegment;

	delete m_pShape;
	m_pShape = new b2EdgeShape();
	static_cast<b2EdgeShape *>(m_pShape)->Set(b2Vec2(pt1.x, pt1.y), b2Vec2(pt2.x, pt2.y));

	m_OwnerRef.OnShapeSet(this);
}

void HyShape2d::SetAsLineLoop(const glm::vec2 *pVertices, uint32 uiNumVerts)
{
	m_eType = HYSHAPE_LineLoop;

	delete m_pShape;
	m_pShape = new b2ChainShape();

	const b2Vec2 *pTmp = HYSHAPEVECTOR_GLM_TO_B2(pVertices);
	static_cast<b2ChainShape *>(m_pShape)->CreateLoop(pTmp, uiNumVerts);

	m_OwnerRef.OnShapeSet(this);
}

void HyShape2d::SetAsLineChain(const glm::vec2 *pVertices, uint32 uiNumVerts)
{
	m_eType = HYSHAPE_LineChain;

	delete m_pShape;
	m_pShape = new b2ChainShape();

	const b2Vec2 *pTmp = HYSHAPEVECTOR_GLM_TO_B2(pVertices);
	static_cast<b2ChainShape *>(m_pShape)->CreateChain(pTmp, uiNumVerts);

	m_OwnerRef.OnShapeSet(this);
}

void HyShape2d::SetAsCircle(float fRadius)
{
	SetAsCircle(glm::vec2(0.0f, 0.0f), fRadius);
}

void HyShape2d::SetAsCircle(const glm::vec2 &ptCenter, float fRadius)
{
	m_eType = HYSHAPE_Circle;

	delete m_pShape;
	m_pShape = new b2CircleShape();
	static_cast<b2CircleShape *>(m_pShape)->m_p.Set(ptCenter.x, ptCenter.y);
	static_cast<b2CircleShape *>(m_pShape)->m_radius = fRadius;

	m_OwnerRef.OnShapeSet(this);
}

void HyShape2d::SetAsPolygon(const glm::vec2 *pPointArray, uint32 uiCount)
{
	m_eType = HYSHAPE_Polygon;

	delete m_pShape;
	m_pShape = new b2PolygonShape();

	const b2Vec2 *pTmp = HYSHAPEVECTOR_GLM_TO_B2(pPointArray);
	static_cast<b2PolygonShape *>(m_pShape)->Set(pTmp, uiCount);

	m_OwnerRef.OnShapeSet(this);
}

void HyShape2d::SetAsBox(float fHalfWidth, float fHalfHeight)
{
	m_eType = HYSHAPE_Polygon;

	delete m_pShape;
	m_pShape = new b2PolygonShape();
	static_cast<b2PolygonShape *>(m_pShape)->SetAsBox(fHalfWidth, fHalfHeight);

	m_OwnerRef.OnShapeSet(this);
}

void HyShape2d::SetAsBox(float fHalfWidth, float fHalfHeight, const glm::vec2 &ptBoxCenter, float fRotDeg)
{
	m_eType = HYSHAPE_Polygon;

	delete m_pShape;
	m_pShape = new b2PolygonShape();
	static_cast<b2PolygonShape *>(m_pShape)->SetAsBox(fHalfWidth, fHalfHeight, b2Vec2(ptBoxCenter.x, ptBoxCenter.y), glm::radians(fRotDeg));

	m_OwnerRef.OnShapeSet(this);
}

bool HyShape2d::TestPoint(glm::vec2 ptWorldPoint) const
{
	return m_pShape->TestPoint(b2Transform(b2Vec2(m_OwnerRef.pos.X(), m_OwnerRef.pos.Y()), b2Rot(glm::radians(m_OwnerRef.rot.Get()))), b2Vec2(ptWorldPoint.x, ptWorldPoint.y));
}
