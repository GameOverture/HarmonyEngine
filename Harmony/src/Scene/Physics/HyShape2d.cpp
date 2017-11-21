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
#include "Scene/Nodes/Draws/Instances/IHyDrawInst2d.h"
#include "Diagnostics/Console/HyConsole.h"

HyShape2d::HyShape2d(IHyDrawInst2d &ownerRef) :	m_OwnerRef(ownerRef),
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
	m_pShape = nullptr;

	switch(rhs.GetType())
	{
		case HYSHAPE_LineSegment: {
			m_eType = HYSHAPE_LineSegment;
			
			b2EdgeShape *pRhsEdgeShape = static_cast<b2EdgeShape *>(rhs.m_pShape);
			m_pShape = HY_NEW b2EdgeShape(*pRhsEdgeShape);
		} break;

		case HYSHAPE_LineLoop: {
			m_eType = HYSHAPE_LineLoop;
			
			b2ChainShape *pRhsChainShape = static_cast<b2ChainShape *>(rhs.m_pShape);
			m_pShape = HY_NEW b2ChainShape(*pRhsChainShape);

			// NOTE: Box2d doesn't have a proper copy constructor for b2ChainShape as it uses its own dynamic memory
			static_cast<b2ChainShape *>(m_pShape)->m_vertices = nullptr;
			static_cast<b2ChainShape *>(m_pShape)->CreateLoop(pRhsChainShape->m_vertices, pRhsChainShape->m_count);
		} break;

		case HYSHAPE_LineChain: {
			m_eType = HYSHAPE_LineChain;
			
			b2ChainShape *pRhsChainShape = static_cast<b2ChainShape *>(rhs.m_pShape);
			m_pShape = HY_NEW b2ChainShape(*pRhsChainShape);

			// NOTE: Box2d doesn't have a proper copy constructor for b2ChainShape as it uses its own dynamic memory
			static_cast<b2ChainShape *>(m_pShape)->m_vertices = nullptr;
			static_cast<b2ChainShape *>(m_pShape)->CreateChain(pRhsChainShape->m_vertices, pRhsChainShape->m_count);
		} break;

		case HYSHAPE_Circle: {
			m_eType = HYSHAPE_Circle;
			
			b2CircleShape *pRhsCircleShape = static_cast<b2CircleShape *>(rhs.m_pShape);
			m_pShape = HY_NEW b2CircleShape(*pRhsCircleShape);
		} break;
		
		case HYSHAPE_Polygon: {
			m_eType = HYSHAPE_Polygon;
			
			b2PolygonShape *pRhsPolygonShape = static_cast<b2PolygonShape *>(rhs.m_pShape);
			m_pShape = HY_NEW b2PolygonShape(*pRhsPolygonShape);
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
	m_pShape = HY_NEW b2EdgeShape();
	static_cast<b2EdgeShape *>(m_pShape)->Set(b2Vec2(pt1.x, pt1.y), b2Vec2(pt2.x, pt2.y));

	m_OwnerRef.OnShapeSet(this);
}

void HyShape2d::SetAsLineLoop(const glm::vec2 *pVertices, uint32 uiNumVerts)
{
	HyAssert(uiNumVerts >= 3, "HyShape2d::SetAsLineLoop - not enough verts. Must be >= 3");
	m_eType = HYSHAPE_LineLoop;

	std::vector<b2Vec2> vertList;
	for(uint32 i = 0; i < uiNumVerts; ++i)
		vertList.push_back(b2Vec2(pVertices[i].x, pVertices[i].y));

	delete m_pShape;
	m_pShape = HY_NEW b2ChainShape();
	static_cast<b2ChainShape *>(m_pShape)->CreateLoop(&vertList[0], uiNumVerts);

	m_OwnerRef.OnShapeSet(this);
}

void HyShape2d::SetAsLineChain(const glm::vec2 *pVertices, uint32 uiNumVerts)
{
	HyAssert(uiNumVerts >= 2, "HyShape2d::SetAsLineChain - not enough verts. Must be >= 2");
	m_eType = HYSHAPE_LineChain;

	std::vector<b2Vec2> vertList;
	for(uint32 i = 0; i < uiNumVerts; ++i)
		vertList.push_back(b2Vec2(pVertices[i].x, pVertices[i].y));

	delete m_pShape;
	m_pShape = HY_NEW b2ChainShape();
	static_cast<b2ChainShape *>(m_pShape)->CreateChain(&vertList[0], uiNumVerts);

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
	m_pShape = HY_NEW b2CircleShape();
	static_cast<b2CircleShape *>(m_pShape)->m_p.Set(ptCenter.x, ptCenter.y);
	static_cast<b2CircleShape *>(m_pShape)->m_radius = fRadius;

	m_OwnerRef.OnShapeSet(this);
}

void HyShape2d::SetAsPolygon(const glm::vec2 *pPointArray, uint32 uiCount)
{
	m_eType = HYSHAPE_Polygon;

	std::vector<b2Vec2> vertList;
	for(uint32 i = 0; i < uiCount; ++i)
		vertList.push_back(b2Vec2(pPointArray[i].x, pPointArray[i].y));

	delete m_pShape;
	m_pShape = HY_NEW b2PolygonShape();
	static_cast<b2PolygonShape *>(m_pShape)->Set(&vertList[0], uiCount);

	m_OwnerRef.OnShapeSet(this);
}

void HyShape2d::SetAsBox(float fHalfWidth, float fHalfHeight)
{
	m_eType = HYSHAPE_Polygon;

	delete m_pShape;
	m_pShape = HY_NEW b2PolygonShape();
	static_cast<b2PolygonShape *>(m_pShape)->SetAsBox(fHalfWidth, fHalfHeight);

	m_OwnerRef.OnShapeSet(this);
}

void HyShape2d::SetAsBox(float fHalfWidth, float fHalfHeight, const glm::vec2 &ptBoxCenter, float fRotDeg)
{
	m_eType = HYSHAPE_Polygon;

	delete m_pShape;
	m_pShape = HY_NEW b2PolygonShape();
	static_cast<b2PolygonShape *>(m_pShape)->SetAsBox(fHalfWidth, fHalfHeight, b2Vec2(ptBoxCenter.x, ptBoxCenter.y), glm::radians(fRotDeg));

	m_OwnerRef.OnShapeSet(this);
}

bool HyShape2d::TestPoint(const glm::vec2 &ptWorldPointRef) const
{
	glm::mat4 mtxWorld;
	m_OwnerRef.GetWorldTransform(mtxWorld);
	float fWorldRotationRadians = glm::atan(mtxWorld[0][1], mtxWorld[0][0]);

	return m_pShape && m_pShape->TestPoint(b2Transform(b2Vec2(mtxWorld[3].x, mtxWorld[3].y), b2Rot(fWorldRotationRadians)), b2Vec2(ptWorldPointRef.x, ptWorldPointRef.y));
}
