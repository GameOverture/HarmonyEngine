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
#include "Diagnostics/Console/IHyConsole.h"
#include "Utilities/HyMath.h"

const float HyShape2d::FloatSlop = b2_linearSlop;

HyShape2d::HyShape2d() :
	m_eType(HYSHAPE_Unknown),
	m_pShape(nullptr)//,
	//m_fpModifiedCallback(nullptr),
	//m_pModifiedCallbackParam(nullptr)
{
}

HyShape2d::HyShape2d(const HyShape2d &copyRef) :
	m_eType(HYSHAPE_Unknown),
	m_pShape(nullptr)//,
	//m_fpModifiedCallback(nullptr),
	//m_pModifiedCallbackParam(nullptr)
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
		static_cast<b2ChainShape *>(m_pShape)->CreateChain(pRhsChainShape->m_vertices, pRhsChainShape->m_count, b2Vec2(0.0f, 0.0f), b2Vec2(0.0f, 0.0f));
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
		// Unknown shape type (uninitialized IHyNode objects)
		break;
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

b2Shape *HyShape2d::ClonePpmShape(float fPpmInverse) const
{
	b2Shape *pCloneB2Shape = nullptr;
	std::vector<b2Vec2> vertList;

	switch(m_eType)
	{
	case HYSHAPE_LineSegment:
		pCloneB2Shape = HY_NEW b2EdgeShape();
		vertList.emplace_back(static_cast<b2EdgeShape *>(m_pShape)->m_vertex1.x * fPpmInverse,
							  static_cast<b2EdgeShape *>(m_pShape)->m_vertex1.y * fPpmInverse);
		vertList.emplace_back(static_cast<b2EdgeShape *>(m_pShape)->m_vertex2.x * fPpmInverse,
							  static_cast<b2EdgeShape *>(m_pShape)->m_vertex2.y * fPpmInverse);

		static_cast<b2EdgeShape *>(pCloneB2Shape)->SetTwoSided(vertList[0], vertList[1]);
		break;

	case HYSHAPE_LineChain:
		pCloneB2Shape = HY_NEW b2ChainShape();
		for(int32 i = 0; i < static_cast<b2ChainShape *>(m_pShape)->m_count; ++i)
		{
			vertList.emplace_back(static_cast<b2ChainShape *>(m_pShape)->m_vertices[i].x * fPpmInverse,
				static_cast<b2ChainShape *>(m_pShape)->m_vertices[i].y * fPpmInverse);
		}

		static_cast<b2ChainShape *>(pCloneB2Shape)->CreateChain(&vertList[0], static_cast<b2ChainShape *>(m_pShape)->m_count, b2Vec2(0, 0), b2Vec2(0, 0));
		break;

	case HYSHAPE_LineLoop:
		pCloneB2Shape = HY_NEW b2ChainShape();
		for(int32 i = 0; i < static_cast<b2ChainShape *>(m_pShape)->m_count; ++i)
		{
			vertList.emplace_back(static_cast<b2ChainShape *>(m_pShape)->m_vertices[i].x * fPpmInverse,
								  static_cast<b2ChainShape *>(m_pShape)->m_vertices[i].y * fPpmInverse);
		}

		static_cast<b2ChainShape *>(pCloneB2Shape)->CreateLoop(&vertList[0], static_cast<b2ChainShape *>(m_pShape)->m_count);
		break;

	case HYSHAPE_Circle:
		pCloneB2Shape = HY_NEW b2CircleShape();
		static_cast<b2CircleShape *>(pCloneB2Shape)->m_p.Set(static_cast<b2CircleShape *>(m_pShape)->m_p.x * fPpmInverse,
															 static_cast<b2CircleShape *>(m_pShape)->m_p.y * fPpmInverse);
		static_cast<b2CircleShape *>(pCloneB2Shape)->m_radius = static_cast<b2CircleShape *>(m_pShape)->m_radius * fPpmInverse;
		break;

	case HYSHAPE_Polygon:
		pCloneB2Shape = HY_NEW b2PolygonShape();
		for(int32 i = 0; i < static_cast<b2PolygonShape *>(m_pShape)->m_count; ++i)
		{
			vertList.emplace_back(static_cast<b2PolygonShape *>(m_pShape)->m_vertices[i].x * fPpmInverse,
								  static_cast<b2PolygonShape *>(m_pShape)->m_vertices[i].y * fPpmInverse);
		}

		static_cast<b2PolygonShape *>(pCloneB2Shape)->Set(vertList.data(), static_cast<b2PolygonShape *>(m_pShape)->m_count);
		break;

	default:
		HyError("Unhandled shape type used in HyShape2d::ClonePpmShape");
		break;
	}

	return pCloneB2Shape;
}

bool HyShape2d::IsValidShape() const
{
	return m_pShape != nullptr && m_eType != HYSHAPE_Unknown;
}

void HyShape2d::SetAsNothing()
{
	m_eType = HYSHAPE_Unknown;

	delete m_pShape;
	m_pShape = nullptr;
}

void HyShape2d::SetAsLineSegment(const glm::vec2 &pt1, const glm::vec2 &pt2)
{
	SetAsLineSegment(b2Vec2(pt1.x, pt1.y), b2Vec2(pt2.x, pt2.y));
}

void HyShape2d::SetAsLineSegment(const b2Vec2 &pt1, const b2Vec2 &pt2)
{
	m_eType = HYSHAPE_LineSegment;

	delete m_pShape;
	m_pShape = HY_NEW b2EdgeShape();
	static_cast<b2EdgeShape *>(m_pShape)->SetTwoSided(pt1, pt2);
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
	static_cast<b2ChainShape *>(m_pShape)->CreateChain(&vertList[0], uiNumVerts, b2Vec2(0.0f, 0.0f), b2Vec2(0.0f, 0.0f));
}

bool HyShape2d::SetAsCircle(float fRadius)
{
	return SetAsCircle(glm::vec2(0.0f, 0.0f), fRadius);
}

bool HyShape2d::SetAsCircle(const glm::vec2 &ptCenter, float fRadius)
{
	return SetAsCircle(b2Vec2(ptCenter.x, ptCenter.y), fRadius);
}

bool HyShape2d::SetAsCircle(const b2Vec2& center, float fRadius)
{
	if(fRadius < FloatSlop)
	{
		SetAsNothing();
		return false;
	}

	m_eType = HYSHAPE_Circle;

	delete m_pShape;
	m_pShape = HY_NEW b2CircleShape();
	static_cast<b2CircleShape *>(m_pShape)->m_p = center;
	static_cast<b2CircleShape *>(m_pShape)->m_radius = fRadius;

	return true;
}

void HyShape2d::SetAsPolygon(const glm::vec2 *pPointArray, uint32 uiCount)
{
	std::vector<b2Vec2> vertList;
	for(uint32 i = 0; i < uiCount; ++i)
		vertList.push_back(b2Vec2(pPointArray[i].x, pPointArray[i].y));

	SetAsPolygon(vertList.data(), static_cast<uint32>(vertList.size()));
}

void HyShape2d::SetAsPolygon(const b2Vec2 *pPointArray, uint32 uiCount)
{
	HyAssert(uiCount <= b2_maxPolygonVertices, "HyShape2d::SetAsPolygon took too many vertices. Max is " << uiCount);

	m_eType = HYSHAPE_Polygon;

	delete m_pShape;
	m_pShape = HY_NEW b2PolygonShape();
	static_cast<b2PolygonShape *>(m_pShape)->Set(pPointArray, uiCount);
}

bool HyShape2d::SetAsBox(int32 iWidth, int32 iHeight)
{
	return SetAsBox(static_cast<float>(iWidth), static_cast<float>(iHeight));
}

bool HyShape2d::SetAsBox(float fWidth, float fHeight)
{
	if(fWidth < FloatSlop || fHeight < FloatSlop)
	{
		SetAsNothing();
		return false;
	}

	m_eType = HYSHAPE_Polygon;

	delete m_pShape;
	m_pShape = HY_NEW b2PolygonShape();
	
	// Offsets Box2d's center to Harmony's default bottom left
	static_cast<b2PolygonShape *>(m_pShape)->SetAsBox(fWidth * 0.5f, fHeight * 0.5f, b2Vec2(fWidth * 0.5f, fHeight * 0.5f), 0.0f);

	return true;
}

bool HyShape2d::SetAsBox(float fHalfWidth, float fHalfHeight, const glm::vec2 &ptBoxCenter, float fRotDeg)
{
	if(fHalfWidth * 2 < FloatSlop || fHalfHeight * 2 < FloatSlop)
	{
		SetAsNothing();
		return false;
	}

	m_eType = HYSHAPE_Polygon;

	delete m_pShape;
	m_pShape = HY_NEW b2PolygonShape();
	static_cast<b2PolygonShape *>(m_pShape)->SetAsBox(fHalfWidth, fHalfHeight, b2Vec2(ptBoxCenter.x, ptBoxCenter.y), glm::radians(fRotDeg));

	return true;
}

bool HyShape2d::TestPoint(const glm::mat4 &mtxSelfTransform, const glm::vec2 &ptTestPoint) const
{
	bool bIsCollide = false;
	
	b2Shape *pTransformedSelf = CloneTransform(mtxSelfTransform);
	if(pTransformedSelf)
	{
		bIsCollide = pTransformedSelf->TestPoint(b2Transform(b2Vec2(0.0f, 0.0f), b2Rot(0.0f)), b2Vec2(ptTestPoint.x, ptTestPoint.y));
		delete pTransformedSelf;
	}

	return bIsCollide;
}

//bool HyShape2d::IsColliding(const glm::mat4 &mtxSelfTransform, const HyShape2d &testShape, const glm::mat4 &mtxTestTransform, b2WorldManifold &worldManifoldOut) const
//{
//	// TODO: Account for any scaling within 'mtxSelfTransform' and 'mtxTestTransform'
//	b2Transform selfTransform(b2Vec2(0.0f, 0.0f), b2Rot(0.0f));
//	b2Transform testTransform(b2Vec2(0.0f, 0.0f), b2Rot(0.0f));
//	//b2Transform selfTransform(b2Vec2(mtxSelfTransform[3].x, mtxSelfTransform[3].y), b2Rot(glm::atan(mtxSelfTransform[0][1], mtxSelfTransform[0][0])));
//	//b2Transform testTransform(b2Vec2(mtxTestTransform[3].x, mtxTestTransform[3].y), b2Rot(glm::atan(mtxTestTransform[0][1], mtxTestTransform[0][0])));
//
//	b2Manifold localManifold;
//
//	switch(m_eType)
//	{
//	case HYSHAPE_LineSegment:
//	case HYSHAPE_LineChain:
//	case HYSHAPE_LineLoop:
//		switch(testShape.GetType())
//		{
//		case HYSHAPE_LineSegment:
//		case HYSHAPE_LineChain:
//		case HYSHAPE_LineLoop:
//			HyError("HyShape2d::IsColliding - Line to Line collision is not supported");
//			return false;
//		case HYSHAPE_Circle:
//			b2CollideEdgeAndCircle(&localManifold, static_cast<const b2EdgeShape *>(m_pShape), selfTransform, static_cast<const b2CircleShape *>(testShape.GetB2Shape()), testTransform);
//			break;
//		case HYSHAPE_Polygon:
//			b2CollideEdgeAndPolygon(&localManifold, static_cast<const b2EdgeShape *>(m_pShape), selfTransform, static_cast<const b2PolygonShape *>(testShape.GetB2Shape()), testTransform);
//			break;
//
//		default:
//			HyError("Unhandled shape type of test shape in HyShape2d::IsColliding");
//			break;
//		}
//		break;
//
//	case HYSHAPE_Circle:
//		switch(testShape.GetType())
//		{
//		case HYSHAPE_LineSegment:
//		case HYSHAPE_LineChain:
//		case HYSHAPE_LineLoop:
//			b2CollideEdgeAndCircle(&localManifold, static_cast<const b2EdgeShape *>(testShape.GetB2Shape()), testTransform, static_cast<const b2CircleShape *>(m_pShape), selfTransform);
//			return false;
//		case HYSHAPE_Circle:
//			b2CollideCircles(&localManifold, static_cast<const b2CircleShape *>(m_pShape), selfTransform, static_cast<const b2CircleShape *>(testShape.GetB2Shape()), testTransform);
//			break;
//		case HYSHAPE_Polygon:
//			b2CollidePolygonAndCircle(&localManifold, static_cast<const b2PolygonShape *>(testShape.GetB2Shape()), testTransform, static_cast<const b2CircleShape *>(m_pShape), selfTransform);
//			break;
//
//		default:
//			HyError("Unhandled shape type of test shape in HyShape2d::IsColliding");
//			break;
//		}
//		break;
//
//	case HYSHAPE_Polygon:
//		switch(testShape.GetType())
//		{
//		case HYSHAPE_LineSegment:
//		case HYSHAPE_LineChain:
//		case HYSHAPE_LineLoop:
//			b2CollideEdgeAndPolygon(&localManifold, static_cast<const b2EdgeShape *>(testShape.GetB2Shape()), testTransform, static_cast<const b2PolygonShape *>(m_pShape), selfTransform);
//			break;
//		case HYSHAPE_Circle:
//			b2CollidePolygonAndCircle(&localManifold, static_cast<const b2PolygonShape *>(m_pShape), selfTransform, static_cast<const b2CircleShape *>(testShape.GetB2Shape()), testTransform);
//			break;
//		case HYSHAPE_Polygon:
//			b2CollidePolygons(&localManifold, static_cast<const b2PolygonShape *>(m_pShape), selfTransform, static_cast<const b2PolygonShape *>(testShape.GetB2Shape()), testTransform);
//			break;
//
//		default:
//			HyError("Unhandled shape type of test shape in HyShape2d::IsColliding");
//			break;
//		}
//		break;
//
//	default:
//		HyError("Unhandled shape type of self in HyShape2d::IsColliding");
//		break;
//	}
//
//	worldManifoldOut.Initialize(&localManifold, selfTransform, m_pShape->m_radius, testTransform, testShape.GetB2Shape()->m_radius);
//	return localManifold.pointCount != 0;
//}

bool HyShape2d::ComputeAABB(b2AABB &aabbOut, const glm::mat4 &mtxTransform)
{
	b2Shape *pTransformedSelf = CloneTransform(mtxTransform);
	if(pTransformedSelf)
	{
		pTransformedSelf->ComputeAABB(&aabbOut, b2Transform(b2Vec2(0.0f, 0.0f), b2Rot(0.0f)), 0);
		delete pTransformedSelf;

		return true;
	}

	return false;
}

b2Shape *HyShape2d::CloneTransform(const glm::mat4 &mtxTransform) const
{
	float fScaleX = glm::length(glm::vec3(mtxTransform[0][0], mtxTransform[0][1], mtxTransform[0][2]));
	float fScaleY = glm::length(glm::vec3(mtxTransform[1][0], mtxTransform[1][1], mtxTransform[1][2]));
	if(fScaleX < FloatSlop || fScaleY < FloatSlop || std::isnan(mtxTransform[3].x) || std::isnan(mtxTransform[3].y)) // TODO: Trace down why sometimes the position vector of the transform is NaN. This causes a mostly harmless assert() in b2PolygonShape::Set() (Doing isNan check in GetLocalTransform() will breakpoint)
		return nullptr;

	b2Shape *pCloneB2Shape = nullptr;
	std::vector<glm::vec4> vertList;
	std::vector<b2Vec2> b2VertList;

	switch(m_eType)
	{
	case HYSHAPE_LineSegment:
		pCloneB2Shape = HY_NEW b2EdgeShape();

		vertList.emplace_back(static_cast<b2EdgeShape *>(m_pShape)->m_vertex1.x, static_cast<b2EdgeShape *>(m_pShape)->m_vertex1.y, 0.0f, 1.0f);
		vertList.emplace_back(static_cast<b2EdgeShape *>(m_pShape)->m_vertex2.x, static_cast<b2EdgeShape *>(m_pShape)->m_vertex2.y, 0.0f, 1.0f);

		vertList[0] = mtxTransform * vertList[0];
		vertList[1] = mtxTransform * vertList[1];

		static_cast<b2EdgeShape *>(pCloneB2Shape)->SetTwoSided(b2Vec2(vertList[0].x, vertList[0].y), b2Vec2(vertList[1].x, vertList[1].y));
		break;

	case HYSHAPE_LineChain:
		pCloneB2Shape = HY_NEW b2ChainShape();
		for(int32 i = 0; i < static_cast<b2ChainShape *>(m_pShape)->m_count; ++i)
		{
			vertList.emplace_back(static_cast<b2ChainShape *>(m_pShape)->m_vertices[i].x,
								  static_cast<b2ChainShape *>(m_pShape)->m_vertices[i].y, 0.0f, 1.0f);
			vertList[i] = mtxTransform * vertList[i];
			b2VertList.emplace_back(vertList[i].x, vertList[i].y);
		}

		static_cast<b2ChainShape *>(pCloneB2Shape)->CreateChain(b2VertList.data(), static_cast<b2ChainShape *>(m_pShape)->m_count, b2Vec2(0.0f, 0.0f), b2Vec2(0.0f, 0.0f));
		break;

	case HYSHAPE_LineLoop:
		pCloneB2Shape = HY_NEW b2ChainShape();
		for(int32 i = 0; i < static_cast<b2ChainShape *>(m_pShape)->m_count; ++i)
		{
			vertList.emplace_back(static_cast<b2ChainShape *>(m_pShape)->m_vertices[i].x,
								  static_cast<b2ChainShape *>(m_pShape)->m_vertices[i].y, 0.0f, 1.0f);
			vertList[i] = mtxTransform * vertList[i];
			b2VertList.emplace_back(vertList[i].x, vertList[i].y);
		}

		static_cast<b2ChainShape *>(pCloneB2Shape)->CreateLoop(b2VertList.data(), static_cast<b2ChainShape *>(m_pShape)->m_count);
		break;

	case HYSHAPE_Circle: {
		pCloneB2Shape = HY_NEW b2CircleShape();

		vertList.emplace_back(static_cast<b2CircleShape *>(m_pShape)->m_p.x, static_cast<b2CircleShape *>(m_pShape)->m_p.y, 0.0f, 1.0f);

		vertList[0] = mtxTransform * vertList[0];
		static_cast<b2CircleShape *>(pCloneB2Shape)->m_p.Set(vertList[0].x, vertList[0].y);
		static_cast<b2CircleShape *>(pCloneB2Shape)->m_radius = static_cast<b2CircleShape *>(m_pShape)->m_radius * HyMax(fScaleX, fScaleY);
		break; }

	case HYSHAPE_Polygon:
		pCloneB2Shape = HY_NEW b2PolygonShape();
		for(int32 i = 0; i < static_cast<b2PolygonShape *>(m_pShape)->m_count; ++i)
		{
			vertList.emplace_back(static_cast<b2PolygonShape *>(m_pShape)->m_vertices[i].x,
								  static_cast<b2PolygonShape *>(m_pShape)->m_vertices[i].y, 0.0f, 1.0f);
			vertList[i] = mtxTransform * vertList[i];
			b2VertList.emplace_back(vertList[i].x, vertList[i].y);
		}

		static_cast<b2PolygonShape *>(pCloneB2Shape)->Set(b2VertList.data(), static_cast<b2PolygonShape *>(m_pShape)->m_count);
		break;

	default:
		break;
	}

	return pCloneB2Shape;
}
