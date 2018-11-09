/**************************************************************************
*	HyShape2d.cpp
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Scene/Physics/HyShape2d.h"
#include "Scene/Nodes/IHyNode2d.h"
#include "Diagnostics/Console/HyConsole.h"

HyShape2d::HyShape2d(IHyNode2d *pOwnerNode) :	m_pOwnerNode(pOwnerNode),
												m_ChangedCallback(nullptr),
												m_eType(HYSHAPE_Unknown),
												m_pShape(nullptr)
{
}

HyShape2d::HyShape2d(IHyNode2d *pOwnerNode, const HyShape2d &copyRef) :	m_pOwnerNode(pOwnerNode),
																		m_ChangedCallback(nullptr),
																		m_eType(HYSHAPE_Unknown),
																		m_pShape(nullptr)
{
	operator=(copyRef);
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

void HyShape2d::GetCentroid(glm::vec2 &ptCentroidOut) const
{
	switch(m_eType)
	{
	case HYSHAPE_Circle:
		ptCentroidOut.x = static_cast<b2CircleShape *>(m_pShape)->m_p.x;
		ptCentroidOut.y = static_cast<b2CircleShape *>(m_pShape)->m_p.y;
		break;

	case HYSHAPE_Polygon:
		ptCentroidOut.x = static_cast<b2PolygonShape *>(m_pShape)->m_centroid.x;
		ptCentroidOut.y = static_cast<b2PolygonShape *>(m_pShape)->m_centroid.y;
		break;

	default:
		HyError("HyShape2d::GetCentoid() - Unsupported shape type: " << m_eType);
	}
}

const b2Shape *HyShape2d::GetB2Shape() const
{
	return m_pShape;
}

b2Shape *HyShape2d::GetB2Shape()
{
	return m_pShape;
}

void HyShape2d::SetOnChangedCallback(HyShape2dChangedCallback changedCallback)
{
	m_ChangedCallback = changedCallback;
}

bool HyShape2d::IsValid() const
{
	return m_pShape != nullptr && m_eType != HYSHAPE_Unknown;
}

void HyShape2d::SetAsNothing()
{
	m_eType = HYSHAPE_Unknown;

	delete m_pShape;
	m_pShape = nullptr;

	if(m_ChangedCallback)
		m_ChangedCallback(m_pOwnerNode, this);
}

void HyShape2d::SetAsLineSegment(const glm::vec2 &pt1, const glm::vec2 &pt2)
{
	m_eType = HYSHAPE_LineSegment;

	delete m_pShape;
	m_pShape = HY_NEW b2EdgeShape();
	static_cast<b2EdgeShape *>(m_pShape)->Set(b2Vec2(pt1.x, pt1.y), b2Vec2(pt2.x, pt2.y));

	if(m_ChangedCallback)
		m_ChangedCallback(m_pOwnerNode, this);
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

	if(m_ChangedCallback)
		m_ChangedCallback(m_pOwnerNode, this);
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

	if(m_ChangedCallback)
		m_ChangedCallback(m_pOwnerNode, this);
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

	if(m_ChangedCallback)
		m_ChangedCallback(m_pOwnerNode, this);
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

	if(m_ChangedCallback)
		m_ChangedCallback(m_pOwnerNode, this);
}

void HyShape2d::SetAsBox(int32 iWidth, int32 iHeight)
{
	SetAsBox(static_cast<float>(iWidth), static_cast<float>(iHeight));
}

void HyShape2d::SetAsBox(float fWidth, float fHeight)
{
	m_eType = HYSHAPE_Polygon;

	delete m_pShape;
	m_pShape = HY_NEW b2PolygonShape();
	static_cast<b2PolygonShape *>(m_pShape)->SetAsBox(fWidth * 0.5f, fHeight * 0.5f, b2Vec2(fWidth * 0.5f, fHeight * 0.5f), 0.0f);	// Offsets Box2d's center to bottom left

	if(m_ChangedCallback)
		m_ChangedCallback(m_pOwnerNode, this);
}

void HyShape2d::SetAsBox(float fHalfWidth, float fHalfHeight, const glm::vec2 &ptBoxCenter, float fRotDeg)
{
	m_eType = HYSHAPE_Polygon;

	delete m_pShape;
	m_pShape = HY_NEW b2PolygonShape();
	static_cast<b2PolygonShape *>(m_pShape)->SetAsBox(fHalfWidth, fHalfHeight, b2Vec2(ptBoxCenter.x, ptBoxCenter.y), glm::radians(fRotDeg));

	if(m_ChangedCallback)
		m_ChangedCallback(m_pOwnerNode, this);
}

bool HyShape2d::TestPoint(const glm::vec2 &ptWorldPointRef) const
{
	glm::mat4 mtxWorld(1.0f);
	if(m_pOwnerNode)
		mtxWorld = m_pOwnerNode->GetWorldTransform();

	float fWorldRotationRadians = glm::atan(mtxWorld[0][1], mtxWorld[0][0]);

	return m_pShape && m_pShape->TestPoint(b2Transform(b2Vec2(mtxWorld[3].x, mtxWorld[3].y), b2Rot(fWorldRotationRadians)), b2Vec2(ptWorldPointRef.x, ptWorldPointRef.y));
}

bool HyShape2d::IsColliding(HyShape2d &shapeRef, b2WorldManifold &worldManifoldOut)
{
	glm::mat4 mtxWorld(1.0f);
	if(m_pOwnerNode)
		mtxWorld = m_pOwnerNode->GetWorldTransform();
	b2Transform thisTransform(b2Vec2(mtxWorld[3].x, mtxWorld[3].y), b2Rot(glm::atan(mtxWorld[0][1], mtxWorld[0][0])));

	mtxWorld = glm::mat4(1.0f);
	if(shapeRef.m_pOwnerNode)
		mtxWorld = shapeRef.m_pOwnerNode->GetWorldTransform();
	b2Transform shapeTransform(b2Vec2(mtxWorld[3].x, mtxWorld[3].y), b2Rot(glm::atan(mtxWorld[0][1], mtxWorld[0][0])));

	b2Manifold localManifold;

	switch(m_eType)
	{
	case HYSHAPE_LineSegment:
	case HYSHAPE_LineChain:
	case HYSHAPE_LineLoop:
		switch(shapeRef.GetType())
		{
		case HYSHAPE_LineSegment:
		case HYSHAPE_LineChain:
		case HYSHAPE_LineLoop:
			HyError("HyShape2d::IsColliding - Line to Line collision is not supported");
			return false;
		case HYSHAPE_Circle:
			b2CollideEdgeAndCircle(&localManifold, static_cast<b2EdgeShape *>(m_pShape), thisTransform, static_cast<b2CircleShape *>(shapeRef.GetB2Shape()), shapeTransform);
			break;
		case HYSHAPE_Polygon:
			b2CollideEdgeAndPolygon(&localManifold, static_cast<b2EdgeShape *>(m_pShape), thisTransform, static_cast<b2PolygonShape *>(shapeRef.GetB2Shape()), shapeTransform);
			break;
		}
		break;

	case HYSHAPE_Circle:
		switch(shapeRef.GetType())
		{
		case HYSHAPE_LineSegment:
		case HYSHAPE_LineChain:
		case HYSHAPE_LineLoop:
			b2CollideEdgeAndCircle(&localManifold, static_cast<b2EdgeShape *>(shapeRef.GetB2Shape()), shapeTransform, static_cast<b2CircleShape *>(m_pShape), thisTransform);
			return false;
		case HYSHAPE_Circle:
			b2CollideCircles(&localManifold, static_cast<b2CircleShape *>(m_pShape), thisTransform, static_cast<b2CircleShape *>(shapeRef.GetB2Shape()), shapeTransform);
			break;
		case HYSHAPE_Polygon:
			b2CollidePolygonAndCircle(&localManifold, static_cast<b2PolygonShape *>(shapeRef.GetB2Shape()), shapeTransform, static_cast<b2CircleShape *>(m_pShape), thisTransform);
			break;
		}
		break;

	case HYSHAPE_Polygon:
		switch(shapeRef.GetType())
		{
		case HYSHAPE_LineSegment:
		case HYSHAPE_LineChain:
		case HYSHAPE_LineLoop:
			b2CollideEdgeAndPolygon(&localManifold, static_cast<b2EdgeShape *>(shapeRef.GetB2Shape()), shapeTransform, static_cast<b2PolygonShape *>(m_pShape), thisTransform);
			break;
		case HYSHAPE_Circle:
			b2CollidePolygonAndCircle(&localManifold, static_cast<b2PolygonShape *>(m_pShape), thisTransform, static_cast<b2CircleShape *>(shapeRef.GetB2Shape()), shapeTransform);
			break;
		case HYSHAPE_Polygon:
			b2CollidePolygons(&localManifold, static_cast<b2PolygonShape *>(m_pShape), thisTransform, static_cast<b2PolygonShape *>(shapeRef.GetB2Shape()), shapeTransform);
			break;
		}
		break;
	}

	worldManifoldOut.Initialize(&localManifold, thisTransform, m_pShape->m_radius, shapeTransform, shapeRef.GetB2Shape()->m_radius);
	return localManifold.pointCount != 0;
}
