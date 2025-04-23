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
#include "Scene/HyScene.h"
#include "Scene/Physics/HyShape2d.h"
#include "Scene/Nodes/Loadables/Bodies/Objects/HyEntity2d.h"
#include "Diagnostics/Console/IHyConsole.h"
#include "Utilities/HyMath.h"

const float HyShape2d::FloatSlop = 0.005f;

HyShape2d::HyShape2d(HyEntity2d *pParent /*= nullptr*/) :
	m_pParent(nullptr),
	m_eType(HYSHAPE_Nothing),
	m_Data({}),
	m_bPhysicsAllowed(true),
	m_hPhysicsShape(b2_nullShapeId),
	m_pPhysicsInit(nullptr),
	m_bPhysicsDirty(false)
{
	if(pParent)
		pParent->ShapeAppend(*this);
}

HyShape2d::HyShape2d(const HyShape2d &copyRef) : 
	m_pParent(nullptr),
	m_eType(HYSHAPE_Nothing),
	m_Data({}),
	m_bPhysicsAllowed(copyRef.m_bPhysicsAllowed),
	m_hPhysicsShape(b2_nullShapeId),
	m_pPhysicsInit(nullptr),
	m_bPhysicsDirty(false)
{
	*this = copyRef;
}

/*virtual*/ HyShape2d::~HyShape2d()
{
	if(m_pParent)
		m_pParent->ShapeRemove(*this);

	if(b2Shape_IsValid(m_hPhysicsShape))
		b2DestroyShape(m_hPhysicsShape, true);
	m_hPhysicsShape = b2_nullShapeId;
	delete m_pPhysicsInit;
}

const HyShape2d &HyShape2d::operator=(const HyShape2d &rhs)
{
	ClearShapeData();
	if(rhs.m_eType == HYSHAPE_LineChain)
	{
		m_Data.chain.pPointList = HY_NEW glm::vec2[rhs.m_Data.chain.iCount];
		m_Data.chain.iCount = rhs.m_Data.chain.iCount;
		m_Data.chain.bLoop = rhs.m_Data.chain.bLoop;
	}
	else
		m_Data = rhs.m_Data;

	m_eType = rhs.m_eType;
	m_bPhysicsAllowed = rhs.m_bPhysicsAllowed;

	if(rhs.m_pParent)
		rhs.m_pParent->ShapeAppend(*this);

	ShapeChanged();
	return *this;
}

HyShapeType HyShape2d::GetType() const
{
	return m_eType;
}

bool HyShape2d::IsValidShape() const
{
	return m_eType != HYSHAPE_Nothing;
}

void HyShape2d::TransformSelf(const glm::mat4 &mtxTransform)
{
	ShapeData shapeData = { };
	if(TransformShapeData(shapeData, mtxTransform))
	{
		HyShapeType ePreserveType = m_eType;
		ClearShapeData();

		m_Data = shapeData;
		m_eType = ePreserveType;
		ShapeChanged();
	}
}

glm::vec2 HyShape2d::ComputeSize() const
{
	b2AABB aabb;
	ComputeAABB(aabb, glm::mat4(1.0f));

	b2Vec2 vExtents = b2AABB_Extents(aabb);
	return glm::vec2(vExtents.x, vExtents.y) * 2.0f;
}

void HyShape2d::GetCentroid(glm::vec2 &ptCentroidOut) const
{
	switch(m_eType)
	{
	case HYSHAPE_Circle:
		ptCentroidOut.x = m_Data.circle.center.x;
		ptCentroidOut.y = m_Data.circle.center.y;
		break;

	case HYSHAPE_Polygon:
		ptCentroidOut.x = m_Data.polygon.centroid.x;
		ptCentroidOut.y = m_Data.polygon.centroid.y;
		break;

	default:
		HyLogWarning("HyShape2d::GetCentoid() - Unsupported shape type: " << m_eType);
		break;
	}
}

float HyShape2d::CalcArea() const
{
	float fArea = 0.0f;

	switch(m_eType)
	{
	case HYSHAPE_LineSegment:
	case HYSHAPE_LineChain:
	default:
		fArea = 0.0f;
		break;

	case HYSHAPE_Circle: {
		float fRadius = m_Data.circle.radius;
		fArea = glm::pi<float>() * (fRadius * fRadius);
		break; }

	case HYSHAPE_Polygon: {
		// Get a reference point for forming triangles.
		// Use the first vertex to reduce round-off errors.
		auto &verticesRef = m_Data.polygon.vertices;
		b2Vec2 s = verticesRef[0];

		int32 iNumVerts = m_Data.polygon.count;
		for(int32 i = 0; i < iNumVerts; ++i)
		{
			// Triangle vertices.
			b2Vec2 e1 = verticesRef[i] - s;
			b2Vec2 e2 = i + 1 < iNumVerts ? verticesRef[i + 1] - s : verticesRef[0] - s;

			float fTriangleArea = 0.5f * b2Cross(e1, e2);
			fArea += fTriangleArea;
		}
		break; }
	}

	return fArea;
}

//const b2Shape *HyShape2d::GetB2Shape() const
//{
//	return m_pShape;
//}

//b2Shape *HyShape2d::ClonePpmShape(float fPpmInverse) const
//{
//	b2Shape *pCloneB2Shape = nullptr;
//	std::vector<b2Vec2> vertList;
//
//	switch(m_eType)
//	{
//	case HYSHAPE_LineSegment:
//		pCloneB2Shape = HY_NEW b2EdgeShape();
//		vertList.emplace_back(static_cast<b2EdgeShape *>(m_pShape)->m_vertex1.x * fPpmInverse,
//							  static_cast<b2EdgeShape *>(m_pShape)->m_vertex1.y * fPpmInverse);
//		vertList.emplace_back(static_cast<b2EdgeShape *>(m_pShape)->m_vertex2.x * fPpmInverse,
//							  static_cast<b2EdgeShape *>(m_pShape)->m_vertex2.y * fPpmInverse);
//
//		static_cast<b2EdgeShape *>(pCloneB2Shape)->SetTwoSided(vertList[0], vertList[1]);
//		break;
//
//	case HYSHAPE_LineChain:
//		pCloneB2Shape = HY_NEW b2ChainShape();
//		for(int32 i = 0; i < static_cast<b2ChainShape *>(m_pShape)->m_count; ++i)
//		{
//			vertList.emplace_back(static_cast<b2ChainShape *>(m_pShape)->m_vertices[i].x * fPpmInverse,
//				static_cast<b2ChainShape *>(m_pShape)->m_vertices[i].y * fPpmInverse);
//		}
//
//		static_cast<b2ChainShape *>(pCloneB2Shape)->CreateChain(vertList.data(), static_cast<b2ChainShape *>(m_pShape)->m_count, b2Vec2(0, 0), b2Vec2(0, 0));
//		break;
//
//	//case HYSHAPE_LineLoop:
//	//	pCloneB2Shape = HY_NEW b2ChainShape();
//	//	for(int32 i = 0; i < static_cast<b2ChainShape *>(m_pShape)->m_count - 1; ++i) // minus 1 to account for the CreateLoop auto connecting
//	//	{
//	//		vertList.emplace_back(static_cast<b2ChainShape *>(m_pShape)->m_vertices[i].x * fPpmInverse,
//	//							  static_cast<b2ChainShape *>(m_pShape)->m_vertices[i].y * fPpmInverse);
//	//	}
//
//	//	static_cast<b2ChainShape *>(pCloneB2Shape)->CreateLoop(vertList.data(), static_cast<b2ChainShape *>(m_pShape)->m_count - 1);
//	//	break;
//
//	case HYSHAPE_Circle:
//		pCloneB2Shape = HY_NEW b2CircleShape();
//		static_cast<b2CircleShape *>(pCloneB2Shape)->m_p.Set(static_cast<b2CircleShape *>(m_pShape)->m_p.x * fPpmInverse,
//															 static_cast<b2CircleShape *>(m_pShape)->m_p.y * fPpmInverse);
//		static_cast<b2CircleShape *>(pCloneB2Shape)->m_radius = static_cast<b2CircleShape *>(m_pShape)->m_radius * fPpmInverse;
//		break;
//
//	case HYSHAPE_Polygon:
//		pCloneB2Shape = HY_NEW b2PolygonShape();
//		for(int32 i = 0; i < static_cast<b2PolygonShape *>(m_pShape)->m_count; ++i)
//		{
//			vertList.emplace_back(static_cast<b2PolygonShape *>(m_pShape)->m_vertices[i].x * fPpmInverse,
//								  static_cast<b2PolygonShape *>(m_pShape)->m_vertices[i].y * fPpmInverse);
//		}
//
//		static_cast<b2PolygonShape *>(pCloneB2Shape)->Set(vertList.data(), static_cast<b2PolygonShape *>(m_pShape)->m_count);
//		break;
//
//	default:
//		HyError("Unhandled shape type used in HyShape2d::ClonePpmShape");
//		break;
//	}
//
//	return pCloneB2Shape;
//}

void HyShape2d::ParentDetach()
{
	HyEntity2d *pParent = ParentGet();
	if(pParent == nullptr)
		return;

	pParent->ShapeRemove(*this);
}

HyEntity2d *HyShape2d::ParentGet() const
{
	return m_pParent;
}

void HyShape2d::SetAsNothing()
{
	ClearShapeData();
	ShapeChanged();
}

//void HyShape2d::SetAsB2Shape(const b2Shape *pShape, const b2ShapeDef *pPhysicsInit /*= nullptr*/)
//{
//	if(m_pShape == pShape)
//		return;
//
//	if(pShape == nullptr)
//	{
//		SetAsNothing();
//		return;
//	}
//
//	delete m_pShape;
//	m_pShape = nullptr;
//
//	switch(pShape->GetType())// rhs.GetType())
//	{
//	case b2Shape::e_circle: {
//		m_eType = HYSHAPE_Circle;
//		const b2CircleShape *pRhsCircleShape = static_cast<const b2CircleShape *>(pShape);
//		m_pShape = HY_NEW b2CircleShape(*pRhsCircleShape);
//		break; }
//
//	case b2Shape::e_edge: {
//		m_eType = HYSHAPE_LineSegment;
//		const b2EdgeShape *pRhsEdgeShape = static_cast<const b2EdgeShape *>(pShape);
//		m_pShape = HY_NEW b2EdgeShape(*pRhsEdgeShape);
//		break; }
//
//	case b2Shape::e_polygon: {
//		m_eType = HYSHAPE_Polygon;
//		const b2PolygonShape *pRhsPolygonShape = static_cast<const b2PolygonShape *>(pShape);
//		m_pShape = HY_NEW b2PolygonShape(*pRhsPolygonShape);
//		break; }
//
//	//case HYSHAPE_LineLoop: {
//	//	m_eType = HYSHAPE_LineLoop;
//
//	//	b2ChainShape *pRhsChainShape = static_cast<b2ChainShape *>(rhs.m_pShape);
//	//	m_pShape = HY_NEW b2ChainShape(); // NOTE: Box2d doesn't have a proper copy constructor for b2ChainShape as it uses its own dynamic memory
//
//	//	static_cast<b2ChainShape *>(m_pShape)->m_vertices = nullptr;
//	//	static_cast<b2ChainShape *>(m_pShape)->CreateLoop(pRhsChainShape->m_vertices, pRhsChainShape->m_count - 1); // minus 1 to account for the CreateLoop auto connecting
//	//} break;
//
//	case b2Shape::e_chain: {
//		m_eType = HYSHAPE_LineChain;
//		const b2ChainShape *pRhsChainShape = static_cast<const b2ChainShape *>(pShape);
//
//		// NOTE: Box2d doesn't have a proper copy constructor for b2ChainShape as it uses its own dynamic memory
//		m_pShape = HY_NEW b2ChainShape();
//		static_cast<b2ChainShape *>(m_pShape)->m_vertices = nullptr;
//		static_cast<b2ChainShape *>(m_pShape)->CreateChain(pRhsChainShape->m_vertices, pRhsChainShape->m_count, b2Vec2(0.0f, 0.0f), b2Vec2(0.0f, 0.0f));
//		break; }
//
//	default:
//		// Unknown shape type (uninitialized IHyNode objects)
//		break;
//	}
//
//	if(pPhysicsInit)
//		Setup(*pPhysicsInit);
//
//	ShapeChanged();
//}

void HyShape2d::SetAsLineSegment(const glm::vec2 &pt1, const glm::vec2 &pt2, const b2ShapeDef *pPhysicsInit /*= nullptr*/)
{
	ClearShapeData();
	m_eType = HYSHAPE_LineSegment;

	m_Data.segment.point1 = { pt1.x, pt1.y };
	m_Data.segment.point2 = { pt2.x, pt2.y };

	if(pPhysicsInit)
		Setup(*pPhysicsInit);

	ShapeChanged();
}

void HyShape2d::SetAsLineChain(const glm::vec2 *pVertices, uint32 uiNumVerts, bool bLoop, const b2ShapeDef *pPhysicsInit /*= nullptr*/)
{
	if(bLoop && pVertices[0] == pVertices[uiNumVerts - 1])
		uiNumVerts--; // Correct the vert list to not include the redundant final loop point
	HyAssert(uiNumVerts >= 3, "HyShape2d::SetAsLineLoop - not enough verts. Must be >= 3");

	ClearShapeData();
	m_eType = HYSHAPE_LineChain;

	m_Data.chain.pPointList = HY_NEW glm::vec2[uiNumVerts];
	for(uint32 i = 0; i < uiNumVerts; ++i)
		m_Data.chain.pPointList[i] = pVertices[i];
	m_Data.chain.iCount = uiNumVerts;
	m_Data.chain.bLoop = bLoop;

	if(pPhysicsInit)
		Setup(*pPhysicsInit);

	ShapeChanged();
}

void HyShape2d::SetAsLineChain(const std::vector<glm::vec2> &verticesList, bool bLoop, const b2ShapeDef *pPhysicsInit /*= nullptr*/)
{
	SetAsLineChain(verticesList.data(), verticesList.size(), bLoop, pPhysicsInit);
}

bool HyShape2d::SetAsCircle(float fRadius, const b2ShapeDef *pPhysicsInit /*= nullptr*/)
{
	return SetAsCircle(glm::vec2(0.0f, 0.0f), fRadius, pPhysicsInit);
}

bool HyShape2d::SetAsCircle(const glm::vec2 &ptCenter, float fRadius, const b2ShapeDef *pPhysicsInit /*= nullptr*/)
{
	if(fRadius < FloatSlop)
		return false;

	ClearShapeData();
	m_eType = HYSHAPE_Circle;

	m_Data.circle.center = { ptCenter.x, ptCenter.y };
	m_Data.circle.radius = fRadius;

	if(pPhysicsInit)
		Setup(*pPhysicsInit);

	ShapeChanged();
	return true;
}

bool HyShape2d::SetAsPolygon(const glm::vec2 *pVertices, uint32 uiCount, const b2ShapeDef *pPhysicsInit /*= nullptr*/)
{
	HyAssert(uiCount <= B2_MAX_POLYGON_VERTICES, "HyShape2d::SetAsPolygon took too many vertices (" << uiCount << "). Max is " << B2_MAX_POLYGON_VERTICES);
	b2Vec2 b2Verts[B2_MAX_POLYGON_VERTICES];
	for(int i = 0; i < uiCount; ++i)
	{
		b2Verts[i].x = pVertices[i].x;
		b2Verts[i].y = pVertices[i].y;
	}

	b2Hull hull = b2ComputeHull(b2Verts, static_cast<int>(uiCount));
	if(hull.count == 0)
		return false;

	ClearShapeData();
	m_eType = HYSHAPE_Polygon;

	m_Data.polygon = b2MakePolygon(&hull, 0.0f);

	if(pPhysicsInit)
		Setup(*pPhysicsInit);

	ShapeChanged();
	return true;
}

bool HyShape2d::SetAsPolygon(const std::vector<glm::vec2> &verticesList, const b2ShapeDef *pPhysicsInit /*= nullptr*/)
{
	return SetAsPolygon(verticesList.data(), static_cast<uint32>(verticesList.size()));
}

bool HyShape2d::SetAsBox(float fWidth, float fHeight, const b2ShapeDef *pPhysicsInit /*= nullptr*/)
{
	return SetAsBox(HyRect(fWidth, fHeight));
}

bool HyShape2d::SetAsBox(const HyRect &rect, const b2ShapeDef *pPhysicsInit /*= nullptr*/)
{
	if(rect.GetWidth() < FloatSlop || rect.GetHeight() < FloatSlop)
		return false;

	ClearShapeData();
	m_eType = HYSHAPE_Polygon;

	if(rect.GetCenter().x == 0.0f && rect.GetCenter().y == 0.0f && rect.GetRotation() == 0.0f)
		m_Data.polygon = b2MakeBox(rect.GetWidth(0.5f), rect.GetHeight(0.5f));
	else
		m_Data.polygon = b2MakeOffsetBox(rect.GetWidth(0.5f), rect.GetHeight(0.5f), { rect.GetCenter().x, rect.GetCenter().y }, b2MakeRot(glm::radians(rect.GetRotation())));

	if(pPhysicsInit)
		Setup(*pPhysicsInit);

	ShapeChanged();
	return true;
}

bool HyShape2d::SetAsCapsule(const glm::vec2 &pt1, const glm::vec2 &pt2, float fRadius, const b2ShapeDef *pPhysicsInit /*= nullptr*/)
{
	if(fRadius < FloatSlop || glm::distance(pt1, pt2) < FloatSlop)
		return false;

	ClearShapeData();
	m_eType = HYSHAPE_Capsule;

	m_Data.capsule.center1 = { pt1.x, pt1.y };
	m_Data.capsule.center2 = { pt2.x, pt2.y };
	m_Data.capsule.radius = fRadius;

	if(pPhysicsInit)
		Setup(*pPhysicsInit);

	ShapeChanged();
	return true;
}

//m_pInit->m_FixtureDef.friction = fFriction;
//m_pInit->m_FixtureDef.restitution = fRestitution;
//m_pInit->m_FixtureDef.restitutionThreshold = fRestitutionThreshold;
//m_pInit->m_FixtureDef.density = fDensity;
//m_pInit->m_FixtureDef.isSensor = bIsSensor;
//m_pInit->m_FixtureDef.filter = filter;
//m_pInit->m_FixtureDef.userData.pointer = reinterpret_cast<uintptr_t>(&m_NodeRef);

bool HyShape2d::IsPhysicsAllowed() const
{
	return m_bPhysicsAllowed;
}

void HyShape2d::SetPhysicsAllowed(bool bIsPhysicsAllowed)
{
	m_bPhysicsAllowed = bIsPhysicsAllowed;
	ShapeChanged();
}

void HyShape2d::Setup(const b2ShapeDef &shapeDefRef)
{
	HyAssert(shapeDefRef.internalValue != 0, "HyShape2d::Setup() - Invalid b2ShapeDef. Must be initalized with b2DefaultShapeDef()");

	if(B2_IS_NON_NULL(m_hPhysicsShape))
	{
		if(shapeDefRef.isSensor != b2Shape_IsSensor(m_hPhysicsShape))
			HyLogWarning("HyShape2d::Setup() - Already simulated shape cannot change to/from a sensor <-> solid");

		b2Shape_SetFilter(m_hPhysicsShape, shapeDefRef.filter);
		b2Shape_SetFriction(m_hPhysicsShape, shapeDefRef.material.friction);
		b2Shape_SetRestitution(m_hPhysicsShape, shapeDefRef.material.restitution);
		//b2World_SetRestitutionThreshold(m_hPhysicsShape, );
		b2Shape_SetDensity(m_hPhysicsShape, shapeDefRef.density, true);
	}
	else
	{
		if(m_pPhysicsInit == nullptr)
		{
			m_pPhysicsInit = HY_NEW b2ShapeDef();
			*m_pPhysicsInit = b2DefaultShapeDef();
		}

		*m_pPhysicsInit = shapeDefRef;
		m_pPhysicsInit->userData = this;
	}
}

float HyShape2d::GetDensity() const
{
	if(B2_IS_NON_NULL(m_hPhysicsShape))
		return b2Shape_GetDensity(m_hPhysicsShape);
	else if(m_pPhysicsInit)
		return m_pPhysicsInit->density;
	else
		return b2DefaultShapeDef().density;
}

void HyShape2d::SetDensity(float fDensity, bool bUpdateBodyMass /*= true*/)
{
	if(B2_IS_NON_NULL(m_hPhysicsShape))
		b2Shape_SetDensity(m_hPhysicsShape, fDensity, bUpdateBodyMass);
	else if(m_pPhysicsInit)
		m_pPhysicsInit->density = fDensity;
	else
	{
		b2ShapeDef def = b2DefaultShapeDef();
		def.density = fDensity;
		Setup(def);
	}
}

void HyShape2d::SetDensityInKg(float fWeightKg, bool bUpdateBodyMass /*= true*/)
{
	if(IsValidShape() == false || fWeightKg < 0.0f)
		fWeightKg = 0.0f;

	float fAreaMetersSq = CalcArea();
	if(fAreaMetersSq <= 0.0f)
		SetDensity(0.0f, bUpdateBodyMass);
	else
		SetDensity(fWeightKg / fAreaMetersSq, bUpdateBodyMass);
}

float HyShape2d::GetFriction() const
{
	if(B2_IS_NON_NULL(m_hPhysicsShape))
		return b2Shape_GetFriction(m_hPhysicsShape);
	else if(m_pPhysicsInit)
		return m_pPhysicsInit->material.friction;
	else
		return b2DefaultShapeDef().material.friction;
}

void HyShape2d::SetFriction(float fFriction)
{
	if(B2_IS_NON_NULL(m_hPhysicsShape))
		b2Shape_SetFriction(m_hPhysicsShape, fFriction);
	else if(m_pPhysicsInit)
		m_pPhysicsInit->material.friction = fFriction;
	else
	{
		b2ShapeDef def = b2DefaultShapeDef();
		def.material.friction = fFriction;
		Setup(def);
	}
}

float HyShape2d::GetRestitution() const
{
	if(B2_IS_NON_NULL(m_hPhysicsShape))
		return b2Shape_GetRestitution(m_hPhysicsShape);
	else if(m_pPhysicsInit)
		return m_pPhysicsInit->material.restitution;
	else
		return b2DefaultShapeDef().material.restitution;
}

void HyShape2d::SetRestitution(float fRestitution)
{
	if(B2_IS_NON_NULL(m_hPhysicsShape))
		b2Shape_SetRestitution(m_hPhysicsShape, fRestitution);
	else if(m_pPhysicsInit)
		m_pPhysicsInit->material.restitution = fRestitution;
	else
	{
		b2ShapeDef def = b2DefaultShapeDef();
		def.material.restitution = fRestitution;
		Setup(def);
	}
}

b2Filter HyShape2d::GetFilter() const
{
	if(B2_IS_NON_NULL(m_hPhysicsShape))
		return b2Shape_GetFilter(m_hPhysicsShape);
	else if(m_pPhysicsInit)
		return m_pPhysicsInit->filter;
	else
		return b2DefaultShapeDef().filter;
}

void HyShape2d::SetFilter(const b2Filter &filter)
{
	if(B2_IS_NON_NULL(m_hPhysicsShape))
		b2Shape_SetFilter(m_hPhysicsShape, filter);
	else if(m_pPhysicsInit)
		m_pPhysicsInit->filter = filter;
	else
	{
		b2ShapeDef def = b2DefaultShapeDef();
		def.filter = filter;
		Setup(def);
	}
}

bool HyShape2d::IsSensor() const
{
	if(B2_IS_NON_NULL(m_hPhysicsShape))
		return b2Shape_IsSensor(m_hPhysicsShape);
	else if(m_pPhysicsInit)
		return m_pPhysicsInit->isSensor;
	else
		return b2DefaultShapeDef().isSensor;
}

bool HyShape2d::TestPoint(const glm::vec2 &ptTestPoint, const glm::mat4 &mtxSelfTransform) const
{
	ShapeData shapeData = {};
	if(TransformShapeData(shapeData, mtxSelfTransform))
	{
		bool bIsCollide = false;

		// Ideally use Box2d to TestPoint if supported (line shapes aren't supported)
		switch(m_eType)
		{
		case HYSHAPE_Circle:
			return b2PointInCircle({ ptTestPoint.x, ptTestPoint.y }, &shapeData.circle);

		case HYSHAPE_LineSegment: {
			b2AABB aabb;
			aabb.lowerBound.Set(shapeData.segment.point1.x, shapeData.segment.point1.y);
			aabb.upperBound.Set(static_cast<b2EdgeShape *>(pTransformedSelf)->m_vertex1.x, static_cast<b2EdgeShape *>(pTransformedSelf)->m_vertex1.y);
			if(static_cast<b2EdgeShape *>(pTransformedSelf)->m_vertex2.x < aabb.lowerBound.x)
				aabb.lowerBound.x = static_cast<b2EdgeShape *>(pTransformedSelf)->m_vertex2.x;
			if(static_cast<b2EdgeShape *>(pTransformedSelf)->m_vertex2.y < aabb.lowerBound.y)
				aabb.lowerBound.y = static_cast<b2EdgeShape *>(pTransformedSelf)->m_vertex2.y;
			if(static_cast<b2EdgeShape *>(pTransformedSelf)->m_vertex2.x > aabb.upperBound.x)
				aabb.upperBound.x = static_cast<b2EdgeShape *>(pTransformedSelf)->m_vertex2.x;
			if(static_cast<b2EdgeShape *>(pTransformedSelf)->m_vertex2.y > aabb.upperBound.y)
				aabb.upperBound.y = static_cast<b2EdgeShape *>(pTransformedSelf)->m_vertex2.y;

			// If the AABB is flat, inflate it by 5 pixel radius
			if(aabb.lowerBound.x == aabb.upperBound.x)
			{
				aabb.lowerBound.x -= 5.0f;
				aabb.upperBound.x += 5.0f;
			}
			if(aabb.lowerBound.y == aabb.upperBound.y)
			{
				aabb.lowerBound.y -= 5.0f;
				aabb.upperBound.y += 5.0f;
			}

			bIsCollide = HyMath::TestPointAABB(aabb, ptTestPoint);
			break; }
				
		case HYSHAPE_Polygon:
			return b2PointInPolygon({ ptTestPoint.x, ptTestPoint.y }, &shapeData.polygon);

		case HYSHAPE_LineChain:

			delete [] shapeData.chain.pPointList;
			break;
				
		case HYSHAPE_Capsule:
			break;


		case b2Shape::e_circle:
		case b2Shape::e_polygon:
			bIsCollide = pTransformedSelf->TestPoint(b2Transform(b2Vec2(0.0f, 0.0f), b2Rot(0.0f)), b2Vec2(ptTestPoint.x, ptTestPoint.y));
			break;
		
		case b2Shape::e_edge: {
			
			break; }

		case b2Shape::e_chain: {
			b2AABB aabb;
			HyMath::InvalidateAABB(aabb);
			for(int32 i = 0; i < static_cast<b2ChainShape *>(pTransformedSelf)->m_count; ++i)
			{
				if(aabb.IsValid() == false)
				{
					aabb.lowerBound.Set(static_cast<b2ChainShape *>(pTransformedSelf)->m_vertices[i].x, static_cast<b2ChainShape *>(pTransformedSelf)->m_vertices[i].y);
					aabb.upperBound.Set(static_cast<b2ChainShape *>(pTransformedSelf)->m_vertices[i].x, static_cast<b2ChainShape *>(pTransformedSelf)->m_vertices[i].y);
				}
				else
				{
					if(static_cast<b2ChainShape *>(pTransformedSelf)->m_vertices[i].x < aabb.lowerBound.x)
						aabb.lowerBound.x = static_cast<b2ChainShape *>(pTransformedSelf)->m_vertices[i].x;
					else if(static_cast<b2ChainShape *>(pTransformedSelf)->m_vertices[i].x > aabb.upperBound.x)
						aabb.upperBound.x = static_cast<b2ChainShape *>(pTransformedSelf)->m_vertices[i].x;

					if(static_cast<b2ChainShape *>(pTransformedSelf)->m_vertices[i].y < aabb.lowerBound.y)
						aabb.lowerBound.y = static_cast<b2ChainShape *>(pTransformedSelf)->m_vertices[i].y;
					else if(static_cast<b2ChainShape *>(pTransformedSelf)->m_vertices[i].y > aabb.upperBound.y)
						aabb.upperBound.y = static_cast<b2ChainShape *>(pTransformedSelf)->m_vertices[i].y;
				}
			}
			// If the AABB is flat, inflate it by 5 pixel radius
			if(aabb.lowerBound.x == aabb.upperBound.x)
			{
				aabb.lowerBound.x -= 5.0f;
				aabb.upperBound.x += 5.0f;
			}
			if(aabb.lowerBound.y == aabb.upperBound.y)
			{
				aabb.lowerBound.y -= 5.0f;
				aabb.upperBound.y += 5.0f;
			}
			
			bIsCollide = HyMath::TestPointAABB(aabb, ptTestPoint);
			break; }
		}

		delete pTransformedSelf;
		return bIsCollide;
	}

	return false;
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

bool HyShape2d::ComputeAABB(b2AABB &aabbOut, const glm::mat4 &mtxTransform) const
{
	// CloneTransform applies scaling
	b2Shape *pTransformedSelf = CloneTransform(mtxTransform);
	if(pTransformedSelf)
	{
		if(m_eType == HYSHAPE_LineChain)
		{
			for(int i = 0; i < static_cast<b2ChainShape *>(pTransformedSelf)->m_count; ++i)
			{
				if(i == 0)
					pTransformedSelf->ComputeAABB(&aabbOut, b2Transform(b2Vec2(0.0f, 0.0f), b2Rot(0.0f)), i);
				else
				{
					b2AABB tmpAabb;
					pTransformedSelf->ComputeAABB(&tmpAabb, b2Transform(b2Vec2(0.0f, 0.0f), b2Rot(0.0f)), i);
					aabbOut.Combine(tmpAabb);
				}
			}
		}
		else
			pTransformedSelf->ComputeAABB(&aabbOut, b2Transform(b2Vec2(0.0f, 0.0f), b2Rot(0.0f)), 0);

		delete pTransformedSelf;

		return true;
	}

	return false;
}

void HyShape2d::ClearShapeData()
{
	if(m_eType == HYSHAPE_LineChain)
		delete [] m_Data.chain.pPointList;

	m_Data = {};
	m_eType = HYSHAPE_Nothing;
}

void HyShape2d::CreateFixture(b2BodyId hBody)
{
	DestroyFixture();

	if(m_pPhysicsInit == nullptr)
	{
		m_pPhysicsInit = HY_NEW b2ShapeDef();
		*m_pPhysicsInit = b2DefaultShapeDef();
	}
	m_pPhysicsInit->userData = this;
	
	// Create Box2d "geometry" of this shape that uses pixel-per-meters
	std::vector<b2Vec2> vertList;
	switch(m_eType)
	{
	case HYSHAPE_LineSegment: {
		b2Segment ppmShape;
		ppmShape.point1 = { m_Data.segment };

		b2CreateSegmentShape(hBody, m_pPhysicsInit, &ppmShape);

		pCloneB2Shape = HY_NEW b2EdgeShape();
		vertList.emplace_back(static_cast<b2EdgeShape *>(m_pShape)->m_vertex1.x * fPpmInverse,
							  static_cast<b2EdgeShape *>(m_pShape)->m_vertex1.y * fPpmInverse);
		vertList.emplace_back(static_cast<b2EdgeShape *>(m_pShape)->m_vertex2.x * fPpmInverse,
			static_cast<b2EdgeShape *>(m_pShape)->m_vertex2.y * fPpmInverse);

		static_cast<b2EdgeShape *>(pCloneB2Shape)->SetTwoSided(vertList[0], vertList[1]);
		break; }

	case HYSHAPE_LineChain:
		pCloneB2Shape = HY_NEW b2ChainShape();
		for(int32 i = 0; i < static_cast<b2ChainShape *>(m_pShape)->m_count; ++i)
		{
			vertList.emplace_back(static_cast<b2ChainShape *>(m_pShape)->m_vertices[i].x * fPpmInverse,
				static_cast<b2ChainShape *>(m_pShape)->m_vertices[i].y * fPpmInverse);
		}

		static_cast<b2ChainShape *>(pCloneB2Shape)->CreateChain(vertList.data(), static_cast<b2ChainShape *>(m_pShape)->m_count, b2Vec2(0, 0), b2Vec2(0, 0));
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







	b2Circle ppmCircle = m_Data.circle;
	ppmCircle.center *= asdf;
	b2CreateCircleShape(hBody, m_pPhysicsInit, &ppmCircle);

	m_pInit->userData.pointer = reinterpret_cast<uintptr_t>(this);
	m_pInit->shape = ClonePpmShape(IHyNode::sm_pScene->GetPpmInverse());
	m_pFixture = pBody->CreateFixture(m_pInit);
	delete m_pInit->shape;

	delete m_pInit;
	m_pInit = nullptr;

	m_bFixtureDirty = false;
}

void HyShape2d::DestroyFixture()
{
	if(m_pFixture == nullptr)
		return;
	
	b2Body *pBody = m_pFixture->GetBody();
	pBody->DestroyFixture(m_pFixture);
	m_pFixture = nullptr;

	m_bFixtureDirty = false;
}

void HyShape2d::ShapeChanged()
{
	m_bPhysicsDirty = true;
}

bool HyShape2d::IsPhysicsDirty()
{
	return m_bPhysicsDirty;
}

// NOTE: Assumes 'shapeDataOut' starts as zeroed-out ShapeData. Will newly dynamically allocate for chain types
bool HyShape2d::TransformShapeData(ShapeData &shapeDataOut, const glm::mat4 &mtxTransform) const
{
	float fScaleX = glm::length(glm::vec3(mtxTransform[0][0], mtxTransform[0][1], mtxTransform[0][2]));
	float fScaleY = glm::length(glm::vec3(mtxTransform[1][0], mtxTransform[1][1], mtxTransform[1][2]));
	if(fScaleX < FloatSlop || fScaleY < FloatSlop || std::isnan(mtxTransform[3].x) || std::isnan(mtxTransform[3].y)) // TODO: Trace down why sometimes the position vector of the transform is NaN. This causes a mostly harmless assert() in b2PolygonShape::Set() (Doing isNan check in GetLocalTransform() will break)
		return false;

	std::vector<glm::vec4> vertList;
	switch(m_eType)
	{
	case HYSHAPE_Nothing:
		return false;

	case HYSHAPE_Circle:
		vertList.emplace_back(m_Data.circle.center.x, m_Data.circle.center.y, 0.0f, 1.0f);
		vertList[0] = mtxTransform * vertList[0];

		shapeDataOut.circle.center = { vertList[0].x, vertList[0].y };
		shapeDataOut.circle.radius = m_Data.circle.radius * HyMath::Max(fScaleX, fScaleY);
		break;

	case HYSHAPE_LineSegment:
		vertList.emplace_back(m_Data.segment.point1.x, m_Data.segment.point1.y, 0.0f, 1.0f);
		vertList.emplace_back(m_Data.segment.point2.x, m_Data.segment.point2.y, 0.0f, 1.0f);
		vertList[0] = mtxTransform * vertList[0];
		vertList[1] = mtxTransform * vertList[1];

		shapeDataOut.segment.point1 = { vertList[0].x, vertList[0].y };
		shapeDataOut.segment.point2 = { vertList[1].x, vertList[1].y };
		break;

	case HYSHAPE_Polygon:
		shapeDataOut.polygon = m_Data.polygon;
		for(int32 i = 0; i < m_Data.polygon.count; ++i)
		{
			vertList.emplace_back(m_Data.polygon.vertices[i].x, m_Data.polygon.vertices[i].y, 0.0f, 1.0f);
			vertList[i] = mtxTransform * vertList[i];
			shapeDataOut.polygon.vertices[i] = { vertList[i].x, vertList[i].y };
		}
		break;

	case HYSHAPE_LineChain:
		shapeDataOut.chain.pPointList = HY_NEW glm::vec2[m_Data.chain.iCount];
		for(int32 i = 0; i < m_Data.chain.iCount; ++i)
		{
			vertList.emplace_back(m_Data.chain.pPointList[i].x, m_Data.chain.pPointList[i].y, 0.0f, 1.0f);
			vertList[i] = mtxTransform * vertList[i];
			shapeDataOut.chain.pPointList[i] = { vertList[i].x, vertList[i].y };
		}
		shapeDataOut.chain.iCount = m_Data.chain.iCount;
		shapeDataOut.chain.bLoop = m_Data.chain.bLoop;
		break;

	case HYSHAPE_Capsule:
		vertList.emplace_back(m_Data.capsule.center1.x, m_Data.capsule.center1.y, 0.0f, 1.0f);
		vertList.emplace_back(m_Data.capsule.center2.x, m_Data.capsule.center2.y, 0.0f, 1.0f);
		vertList[0] = mtxTransform * vertList[0];
		vertList[1] = mtxTransform * vertList[1];

		shapeDataOut.capsule.center1 = { vertList[0].x, vertList[0].y };
		shapeDataOut.capsule.center2 = { vertList[1].x, vertList[1].y };
		shapeDataOut.capsule.radius = m_Data.capsule.radius * HyMath::Max(fScaleX, fScaleY);
		break;

	default:
		HyError("HyShape2d::TransformShapeData() - Unhandled shape type: " << m_eType);
		return false;
	}

	return true;
}
