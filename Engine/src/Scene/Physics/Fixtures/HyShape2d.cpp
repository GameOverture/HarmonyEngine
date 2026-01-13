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
#include "Scene/Physics/Fixtures/HyShape2d.h"
#include "Scene/Nodes/Loadables/Bodies/Objects/HyEntity2d.h"
#include "Diagnostics/Console/IHyConsole.h"

HyShape2d::HyShape2d(HyEntity2d *pParent /*= nullptr*/) :
	IHyFixture2d(pParent),
	m_Data({}),
	m_hPhysics(b2_nullShapeId),
	m_pPhysicsInit(nullptr)
{
}

HyShape2d::HyShape2d(const HyShape2d &copyRef) :
	IHyFixture2d(copyRef),
	m_Data({}),
	m_hPhysics(b2_nullShapeId),
	m_pPhysicsInit(nullptr)
{
	*this = copyRef;
}

/*virtual*/ HyShape2d::~HyShape2d()
{
	if(b2Shape_IsValid(m_hPhysics))
	{
		b2DestroyShape(m_hPhysics, true);
		m_hPhysics = b2_nullShapeId;
	}

	delete m_pPhysicsInit;

	if(m_pParent)
		m_pParent->FixtureRemove(*this);
	m_pParent = nullptr; // Null this pointer to indicate to baseclass that it properly disposed of
}

const HyShape2d &HyShape2d::operator=(const HyShape2d &rhs)
{
	ClearShapeData();
	m_Data = rhs.m_Data;

	IHyFixture2d::operator=(rhs);

	if(rhs.IsPhysicsInitialized())
		Setup(*rhs.m_pPhysicsInit);

	ShapeChanged();
	return *this;
}

b2Circle HyShape2d::GetAsCircle() const
{
	if(m_eType != HYFIXTURE_Circle)
	{
		HyLogError("HyShape2d::GetAsCircle() - Shape is not a circle.");
		return b2Circle() = {};
	}
	return m_Data.circle;
}

b2Segment HyShape2d::GetAsSegment() const
{
	if(m_eType != HYFIXTURE_LineSegment)
	{
		HyLogError("HyShape2d::GetAsSegment() - Shape is not a line segment.");
		return b2Segment() = {};
	}
	return m_Data.segment;
}

b2Polygon HyShape2d::GetAsPolygon() const
{
	if(m_eType != HYFIXTURE_Polygon)
	{
		HyLogError("HyShape2d::GetAsPolygon() - Shape is not a polygon.");
		return b2Polygon() = {};
	}
	return m_Data.polygon;
}

b2Capsule HyShape2d::GetAsCapsule() const
{
	if(m_eType != HYFIXTURE_Capsule)
	{
		HyLogError("HyShape2d::GetAsCapsule() - Shape is not a capsule.");
		return b2Capsule() = {};
	}
	return m_Data.capsule;
}

/*virtual*/ void HyShape2d::TransformSelf(const glm::mat4 &mtxTransform) /*override*/
{
	ShapeData shapeData = { };
	if(TransformShapeData(shapeData, mtxTransform))
	{
		HyFixtureType ePreserveType = m_eType;
		ClearShapeData();

		m_Data = shapeData;
		m_eType = ePreserveType;
		ShapeChanged();
	}
}

/*virtual*/ std::vector<float> HyShape2d::SerializeSelf() const /*override*/
{
	std::vector<float> floatList;

	switch(m_eType)
	{
	case HYFIXTURE_Nothing:
		break;

	case HYFIXTURE_Polygon:
		for(int i = 0; i < m_Data.polygon.count; ++i)
		{
			floatList.push_back(m_Data.polygon.vertices[i].x);
			floatList.push_back(m_Data.polygon.vertices[i].y);
		}
		break;

	case HYFIXTURE_Circle:
		floatList.push_back(m_Data.circle.center.x);
		floatList.push_back(m_Data.circle.center.y);
		floatList.push_back(m_Data.circle.radius);
		break;

	case HYFIXTURE_LineSegment:
		floatList.push_back(m_Data.segment.point1.x);
		floatList.push_back(m_Data.segment.point1.y);
		floatList.push_back(m_Data.segment.point2.x);
		floatList.push_back(m_Data.segment.point2.y);
		break;

	case HYFIXTURE_Capsule:
		floatList.push_back(m_Data.capsule.center1.x);
		floatList.push_back(m_Data.capsule.center1.y);
		floatList.push_back(m_Data.capsule.center2.x);
		floatList.push_back(m_Data.capsule.center2.y);
		floatList.push_back(m_Data.capsule.radius);
		break;

	default:
		HyLogWarning("HyShape2d::SerializeSelf() - Unsupported shape type: " << m_eType);
		break;
	}

	return floatList;
}

/*virtual*/ std::vector<glm::vec2> HyShape2d::DeserializeSelf(HyFixtureType eFixtureType, const std::vector<float> &floatList) /*override*/
{
	std::vector<glm::vec2> vertList;

	switch(eFixtureType)
	{
	case HYFIXTURE_Nothing:
		SetAsNothing();
		break;

	case HYFIXTURE_Polygon:
		vertList.reserve(floatList.size() / 2);
		for(int i = 0; i < floatList.size(); i += 2)
			vertList.emplace_back(glm::vec2(floatList[i], floatList[i + 1]));

		SetAsPolygon(vertList);
		break;

	case HYFIXTURE_Circle:
		vertList.reserve(5);
		vertList.emplace_back(floatList[0], floatList[1]);
		SetAsCircle(vertList[0], floatList[2]);

		// Also add the circumference grab points (top, right, bottom, left)
		vertList.emplace_back(floatList[0], floatList[1] + floatList[2]); // Top
		vertList.emplace_back(floatList[0] + floatList[2], floatList[1]); // Right
		vertList.emplace_back(floatList[0], floatList[1] - floatList[2]); // Bottom
		vertList.emplace_back(floatList[0] - floatList[2], floatList[1]); // Left
		break;

	case HYFIXTURE_LineSegment:
		vertList.reserve(2);
		vertList.emplace_back(floatList[0], floatList[1]);
		vertList.emplace_back(floatList[2], floatList[3]);
		
		SetAsLineSegment(vertList[0], vertList[1]);
		break;

	case HYFIXTURE_Capsule: {
		vertList.reserve(6);
		vertList.emplace_back(floatList[0], floatList[1]);
		vertList.emplace_back(floatList[2], floatList[3]);
	
		SetAsCapsule(vertList[0], vertList[1], floatList[4]);

		// Also add the circumference grab points (just left and right) of both semicircles
		b2Vec2 dir = m_Data.capsule.center2 - m_Data.capsule.center1;
		b2Vec2 norm = b2Normalize({ -dir.y, dir.x });
		vertList.emplace_back(m_Data.capsule.center1.x + (norm.x * m_Data.capsule.radius), m_Data.capsule.center1.y + (norm.y * m_Data.capsule.radius)); // Center1 Right
		vertList.emplace_back(m_Data.capsule.center1.x - (norm.x * m_Data.capsule.radius), m_Data.capsule.center1.y - (norm.y * m_Data.capsule.radius)); // Center1 Left
		vertList.emplace_back(m_Data.capsule.center2.x + (norm.x * m_Data.capsule.radius), m_Data.capsule.center2.y + (norm.y * m_Data.capsule.radius)); // Center2 Right
		vertList.emplace_back(m_Data.capsule.center2.x - (norm.x * m_Data.capsule.radius), m_Data.capsule.center2.y - (norm.y * m_Data.capsule.radius)); // Center2 Left
		break; }

	default:
		HyLogWarning("HyShape2d::DeserializeSelf() - Unsupported shape type: " << eFixtureType);
		break;
	}

	return vertList;
}

bool HyShape2d::GetCentroid(glm::vec2 &ptCentroidOut) const
{
	switch(m_eType)
	{
	case HYFIXTURE_Circle:
		ptCentroidOut.x = m_Data.circle.center.x;
		ptCentroidOut.y = m_Data.circle.center.y;
		break;

	case HYFIXTURE_LineSegment: {
		b2Vec2 vMidwayDir = (m_Data.segment.point2 - m_Data.segment.point1) * 0.5f;
		vMidwayDir = m_Data.segment.point1 + vMidwayDir;
		ptCentroidOut.x = vMidwayDir.x;
		ptCentroidOut.y = vMidwayDir.y;
		break; }

	case HYFIXTURE_Polygon:
		ptCentroidOut.x = m_Data.polygon.centroid.x;
		ptCentroidOut.y = m_Data.polygon.centroid.y;
		break;

	case HYFIXTURE_Capsule: {
		b2Vec2 vMidwayDir = (m_Data.capsule.center2 - m_Data.capsule.center1) * 0.5f;
		vMidwayDir = m_Data.capsule.center1 + vMidwayDir;
		ptCentroidOut.x = vMidwayDir.x;
		ptCentroidOut.y = vMidwayDir.y;
		break; }

	default:
		HyLogWarning("HyShape2d::GetCentoid() - Unsupported shape type: " << m_eType);
		return false;
	}

	return true;
}

float HyShape2d::CalcArea() const
{
	float fArea = 0.0f;

	switch(m_eType)
	{
	case HYFIXTURE_LineSegment:
		fArea = 0.0f;
		break;

	case HYFIXTURE_Circle: {
		float fRadius = m_Data.circle.radius;
		fArea = glm::pi<float>() * (fRadius * fRadius);
		break; }

	case HYFIXTURE_Polygon: {
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

	case HYFIXTURE_Capsule:
		fArea = glm::pi<float>() * m_Data.capsule.radius * m_Data.capsule.radius + 2 * m_Data.capsule.radius * b2Distance(m_Data.capsule.center1, m_Data.capsule.center2);
		break;

	default:
		HyLogWarning("HyShape2d::CalcArea() - Unsupported shape type: " << m_eType);
		break;
	}

	return fArea;
}

void HyShape2d::SetAsNothing()
{
	ClearShapeData();
	ShapeChanged();
}

void HyShape2d::SetAsLineSegment(const glm::vec2 &pt1, const glm::vec2 &pt2, const b2ShapeDef *pPhysicsInit /*= nullptr*/)
{
	ClearShapeData();
	m_eType = HYFIXTURE_LineSegment;

	m_Data.segment.point1 = { pt1.x, pt1.y };
	m_Data.segment.point2 = { pt2.x, pt2.y };

	if(pPhysicsInit)
		Setup(*pPhysicsInit);

	ShapeChanged();
}

bool HyShape2d::SetAsCircle(float fRadius, const b2ShapeDef *pPhysicsInit /*= nullptr*/)
{
	return SetAsCircle(glm::vec2(0.0f, 0.0f), fRadius, pPhysicsInit);
}

bool HyShape2d::SetAsCircle(const glm::vec2 &ptCenter, float fRadius, const b2ShapeDef *pPhysicsInit /*= nullptr*/)
{
	if(fRadius < HyMath::FloatSlop)
		return false;

	ClearShapeData();
	m_eType = HYFIXTURE_Circle;

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
	m_eType = HYFIXTURE_Polygon;

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
	if(rect.GetWidth() < HyMath::FloatSlop || rect.GetHeight() < HyMath::FloatSlop)
		return false;

	ClearShapeData();
	m_eType = HYFIXTURE_Polygon;

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
	if(fRadius < HyMath::FloatSlop || glm::distance(pt1, pt2) < HyMath::FloatSlop)
		return false;

	ClearShapeData();
	m_eType = HYFIXTURE_Capsule;

	m_Data.capsule.center1 = { pt1.x, pt1.y };
	m_Data.capsule.center2 = { pt2.x, pt2.y };
	m_Data.capsule.radius = fRadius;

	if(pPhysicsInit)
		Setup(*pPhysicsInit);

	ShapeChanged();
	return true;
}

void HyShape2d::Setup(const b2ShapeDef &shapeDefRef)
{
	HyAssert(shapeDefRef.internalValue != 0, "HyShape2d::Setup() - Invalid b2ShapeDef. Must be initalized with b2DefaultShapeDef()");

	if(IsPhysicsRegistered())
	{
		if(shapeDefRef.isSensor != b2Shape_IsSensor(m_hPhysics))
			HyLogWarning("HyShape2d::Setup() - Already simulated shape cannot change to/from a sensor <-> solid");

		b2Shape_SetFilter(m_hPhysics, shapeDefRef.filter);
		b2Shape_SetFriction(m_hPhysics, shapeDefRef.material.friction);
		b2Shape_SetRestitution(m_hPhysics, shapeDefRef.material.restitution);
		//b2World_SetRestitutionThreshold(m_hPhysics, );
		b2Shape_SetDensity(m_hPhysics, shapeDefRef.density, true);
	}
	else
	{
		if(IsPhysicsInitialized() == false)
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
	if(IsPhysicsRegistered())
		return b2Shape_GetDensity(m_hPhysics);
	else if(IsPhysicsInitialized())
		return m_pPhysicsInit->density;
	else
		return b2DefaultShapeDef().density;
}

void HyShape2d::SetDensity(float fDensity, bool bUpdateBodyMass /*= true*/)
{
	if(IsPhysicsRegistered())
		b2Shape_SetDensity(m_hPhysics, fDensity, bUpdateBodyMass);
	else if(IsPhysicsInitialized())
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
	if(IsValid() == false || fWeightKg < 0.0f)
		fWeightKg = 0.0f;

	float fAreaMetersSq = CalcArea();
	if(fAreaMetersSq <= 0.0f)
		SetDensity(0.0f, bUpdateBodyMass);
	else
		SetDensity(fWeightKg / fAreaMetersSq, bUpdateBodyMass);
}

float HyShape2d::GetFriction() const
{
	if(IsPhysicsRegistered())
		return b2Shape_GetFriction(m_hPhysics);
	else if(IsPhysicsInitialized())
		return m_pPhysicsInit->material.friction;
	else
		return b2DefaultShapeDef().material.friction;
}

void HyShape2d::SetFriction(float fFriction)
{
	if(IsPhysicsRegistered())
		b2Shape_SetFriction(m_hPhysics, fFriction);
	else if(IsPhysicsInitialized())
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
	if(IsPhysicsRegistered())
		return b2Shape_GetRestitution(m_hPhysics);
	else if(IsPhysicsInitialized())
		return m_pPhysicsInit->material.restitution;
	else
		return b2DefaultShapeDef().material.restitution;
}

void HyShape2d::SetRestitution(float fRestitution)
{
	if(IsPhysicsRegistered())
		b2Shape_SetRestitution(m_hPhysics, fRestitution);
	else if(IsPhysicsInitialized())
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
	if(IsPhysicsRegistered())
		return b2Shape_GetFilter(m_hPhysics);
	else if(IsPhysicsInitialized())
		return m_pPhysicsInit->filter;
	else
		return b2DefaultShapeDef().filter;
}

void HyShape2d::SetFilter(const b2Filter &filter)
{
	if(IsPhysicsRegistered())
		b2Shape_SetFilter(m_hPhysics, filter);
	else if(IsPhysicsInitialized())
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
	if(IsPhysicsRegistered())
		return b2Shape_IsSensor(m_hPhysics);
	else if(IsPhysicsInitialized())
		return m_pPhysicsInit->isSensor;
	else
		return b2DefaultShapeDef().isSensor;
}

/*virtual*/ bool HyShape2d::TestPoint(const glm::vec2 &ptTestPoint, const glm::mat4 &mtxSelfTransform) const /*override*/
{
	ShapeData shapeData = {};
	if(TransformShapeData(shapeData, mtxSelfTransform))
	{
		bool bIsCollide = false;

		// Ideally use Box2d to TestPoint if supported (line shapes need own implementation)
		switch(m_eType)
		{
		case HYFIXTURE_Circle:
			return b2PointInCircle({ ptTestPoint.x, ptTestPoint.y }, &shapeData.circle);

		case HYFIXTURE_LineSegment: {
			glm::vec2 pt1(shapeData.segment.point1.x, shapeData.segment.point1.y);
			glm::vec2 pt2(shapeData.segment.point2.x, shapeData.segment.point2.y);
			glm::vec2 ptCenter = pt1 + ((pt2 - pt1) * 0.5f);
			float fRadians = std::atan2(pt2.y - pt1.y, pt2.x - pt1.x);

			b2Polygon tmpBox = b2MakeOffsetBox(0.5f * glm::distance(pt1, pt2),
											   4.0f,
											   { ptCenter.x, ptCenter.y },
											   b2MakeRot(fRadians));

			return b2PointInPolygon({ ptTestPoint.x, ptTestPoint.y }, &tmpBox); }

		case HYFIXTURE_Polygon:
			return b2PointInPolygon({ ptTestPoint.x, ptTestPoint.y }, &shapeData.polygon);

		case HYFIXTURE_Capsule:
			return b2PointInCapsule({ ptTestPoint.x, ptTestPoint.y }, &shapeData.capsule);

		default:
			HyError("HyShape2d::TransformShapeData() - Unhandled shape type: " << m_eType);
			break;
		}
	}

	return false;
}

/*virtual*/ b2CastOutput HyShape2d::TestRay(const glm::vec2 &ptStart, const glm::vec2 &vDirection, const glm::mat4 &mtxSelfTransform) const /*override*/
{
	HyError("HyShape2d::TestRay - Not implemented");
	return b2CastOutput() = {};
}

/*virtual*/ bool HyShape2d::IsColliding(const IHyFixture2d &testShape, b2Manifold *pManifoldOut /*= nullptr*/) const /*override*/
{
	if(testShape.IsValid() == false)
		return false;

	switch(m_eType)
	{
	case HYFIXTURE_Nothing:
		return false;

	case HYFIXTURE_LineSegment:
		switch(testShape.GetType())
		{
		case HYFIXTURE_Circle:
			if(pManifoldOut)
			{
				*pManifoldOut = b2CollideSegmentAndCircle(&m_Data.segment, b2Transform_identity, &static_cast<const HyShape2d &>(testShape).GetAsCircle(), b2Transform_identity);
				return pManifoldOut->pointCount > 0;
			}
			else
			{
				b2Manifold man = b2CollideSegmentAndCircle(&m_Data.segment, b2Transform_identity, &static_cast<const HyShape2d &>(testShape).GetAsCircle(), b2Transform_identity);
				return man.pointCount > 0;
			}
			break;

		case HYFIXTURE_LineSegment: {
			// Custom implementation required for line to line collision (box2d does not support)
			glm::vec2 pt1(m_Data.segment.point1.x, m_Data.segment.point1.y);
			glm::vec2 pt2(m_Data.segment.point2.x, m_Data.segment.point2.y);
			glm::vec2 pt3(static_cast<const HyShape2d &>(testShape).GetAsSegment().point1.x, static_cast<const HyShape2d &>(testShape).GetAsSegment().point1.y);
			glm::vec2 pt4(static_cast<const HyShape2d &>(testShape).GetAsSegment().point2.x, static_cast<const HyShape2d &>(testShape).GetAsSegment().point2.y);

			glm::vec2 ptIntersection;
			bool bCollide = HyMath::TestSegmentsOverlap(pt1, pt2, pt3, pt4, ptIntersection);
			if(pManifoldOut)
			{
				*pManifoldOut = {};
				pManifoldOut->pointCount = bCollide ? 1 : 0;
				if(bCollide)
				{
					pManifoldOut->points[0] = {};
					pManifoldOut->points[0].point = { ptIntersection.x, ptIntersection.y };
					// Normal is not well defined for line to line collision, so just set to zero
					pManifoldOut->normal = { 0.0f, 0.0f };
					pManifoldOut->rollingImpulse = 0.0f;
				}
			}
			return bCollide; }

		case HYFIXTURE_Polygon:
			if(pManifoldOut)
			{
				*pManifoldOut = b2CollideSegmentAndPolygon(&m_Data.segment, b2Transform_identity, &static_cast<const HyShape2d &>(testShape).GetAsPolygon(), b2Transform_identity);
				return pManifoldOut->pointCount > 0;
			}
			else
			{
				b2Manifold man = b2CollideSegmentAndPolygon(&m_Data.segment, b2Transform_identity, &static_cast<const HyShape2d &>(testShape).GetAsPolygon(), b2Transform_identity);
				return man.pointCount > 0;
			}
			break;

		case HYFIXTURE_Capsule:
			if(pManifoldOut)
			{
				*pManifoldOut = b2CollideSegmentAndCapsule(&m_Data.segment, b2Transform_identity, &static_cast<const HyShape2d &>(testShape).GetAsCapsule(), b2Transform_identity);
				return pManifoldOut->pointCount > 0;
			}
			else
			{
				b2Manifold man = b2CollideSegmentAndCapsule(&m_Data.segment, b2Transform_identity, &static_cast<const HyShape2d &>(testShape).GetAsCapsule(), b2Transform_identity);
				return man.pointCount > 0;
			}
			break;

		default:
			HyError("HyShape2d::IsColliding - LineSegment - Not implemented for shape type: " << testShape.GetType());
			break;
		}
		break;

	default:
		HyError("HyShape2d::IsColliding - Not implemented for shape type: " << m_eType);
		break;
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

/*virtual*/ bool HyShape2d::ComputeAABB(b2AABB &aabbOut, const glm::mat4 &mtxTransform) const /*override*/
{
	ShapeData shapeData = {};
	if(TransformShapeData(shapeData, mtxTransform))
	{
		switch(m_eType)
		{
		case HYFIXTURE_Circle:
			aabbOut = b2ComputeCircleAABB(&shapeData.circle, b2Transform_identity);
			break;
		case HYFIXTURE_LineSegment:
			aabbOut = b2ComputeSegmentAABB(&shapeData.segment, b2Transform_identity);
			break;
		case HYFIXTURE_Polygon:
			aabbOut = b2ComputePolygonAABB(&shapeData.polygon, b2Transform_identity);
			break;
		case HYFIXTURE_Capsule:
			aabbOut = b2ComputeCapsuleAABB(&shapeData.capsule, b2Transform_identity);
			break;
		default:
			HyError("HyShape2d::ComputeAABB() - Unhandled shape type: " << m_eType);
			return false;
		}

		return true;
	}

	return false;
}

void HyShape2d::ClearShapeData()
{
	PhysicsRemove(true);

	m_Data = {};
	m_eType = HYFIXTURE_Nothing;
}

void HyShape2d::ShapeChanged()
{
	m_bPhysicsDirty = true;
}

bool HyShape2d::IsPhysicsRegistered() const
{
	return B2_IS_NON_NULL(m_hPhysics);
}

bool HyShape2d::IsPhysicsInitialized() const
{
	return m_pPhysicsInit != nullptr;
}

void HyShape2d::PhysicsAttach()
{
	PhysicsRemove(false);

	if(IsPhysicsInitialized() == false)
		Setup(b2DefaultShapeDef());

	// Create Box2d "geometry" of this shape that uses pixel-per-meters
	ShapeData shapeData = { };
	if(TransformShapeData(shapeData, IHyNode::sm_pScene->GetPpmInverse()))
	{
		switch(m_eType)
		{
		case HYFIXTURE_Circle:
			m_hPhysics = b2CreateCircleShape(m_pParent->physics.GetHandle(), m_pPhysicsInit, &shapeData.circle);
			break;
		case HYFIXTURE_LineSegment:
			m_hPhysics = b2CreateSegmentShape(m_pParent->physics.GetHandle(), m_pPhysicsInit, &shapeData.segment);
			break;
		case HYFIXTURE_Polygon:
			m_hPhysics = b2CreatePolygonShape(m_pParent->physics.GetHandle(), m_pPhysicsInit, &shapeData.polygon);
			break;
		case HYFIXTURE_Capsule:
			m_hPhysics = b2CreateCapsuleShape(m_pParent->physics.GetHandle(), m_pPhysicsInit, &shapeData.capsule);
			break;
		default:
			HyError("HyShape2d::PhysicsAttach() - Unhandled shape type: " << m_eType);
			break;
		}
	}

	m_bPhysicsDirty = false;
}

void HyShape2d::PhysicsRemove(bool bUpdateBodyMass)
{
	if(IsPhysicsRegistered() == false)
		return;

	b2DestroyShape(m_hPhysics, bUpdateBodyMass);
	m_hPhysics = b2_nullShapeId;

	m_bPhysicsDirty = false;
}

// NOTE: Assumes 'shapeDataOut' starts as zeroed-out ShapeData.
bool HyShape2d::TransformShapeData(ShapeData &shapeDataOut, const glm::mat4 &mtxTransform) const
{
	float fScaleX = glm::length(glm::vec3(mtxTransform[0][0], mtxTransform[0][1], mtxTransform[0][2]));
	float fScaleY = glm::length(glm::vec3(mtxTransform[1][0], mtxTransform[1][1], mtxTransform[1][2]));
	if(fScaleX < HyMath::FloatSlop || fScaleY < HyMath::FloatSlop || std::isnan(mtxTransform[3].x) || std::isnan(mtxTransform[3].y)) // TODO: Trace down why sometimes the position vector of the transform is NaN. This causes a mostly harmless assert() in b2PolygonShape::Set() (Doing isNan check in GetLocalTransform() will break)
		return false;

	std::vector<glm::vec4> vertList;
	switch(m_eType)
	{
	case HYFIXTURE_Nothing:
		return false;

	case HYFIXTURE_Circle:
		vertList.emplace_back(m_Data.circle.center.x, m_Data.circle.center.y, 0.0f, 1.0f);
		vertList[0] = mtxTransform * vertList[0];

		shapeDataOut.circle.center = { vertList[0].x, vertList[0].y };
		shapeDataOut.circle.radius = m_Data.circle.radius * HyMath::Max(fScaleX, fScaleY);
		break;

	case HYFIXTURE_LineSegment:
		vertList.emplace_back(m_Data.segment.point1.x, m_Data.segment.point1.y, 0.0f, 1.0f);
		vertList.emplace_back(m_Data.segment.point2.x, m_Data.segment.point2.y, 0.0f, 1.0f);
		vertList[0] = mtxTransform * vertList[0];
		vertList[1] = mtxTransform * vertList[1];

		shapeDataOut.segment.point1 = { vertList[0].x, vertList[0].y };
		shapeDataOut.segment.point2 = { vertList[1].x, vertList[1].y };
		break;

	case HYFIXTURE_Polygon:
		shapeDataOut.polygon = m_Data.polygon;
		for(int32 i = 0; i < m_Data.polygon.count; ++i)
		{
			vertList.emplace_back(m_Data.polygon.vertices[i].x, m_Data.polygon.vertices[i].y, 0.0f, 1.0f);
			vertList[i] = mtxTransform * vertList[i];
			shapeDataOut.polygon.vertices[i] = { vertList[i].x, vertList[i].y };
		}
		break;

	case HYFIXTURE_Capsule:
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

bool HyShape2d::TransformShapeData(ShapeData &shapeDataOut, float fPpmInverse) const
{
	if(fPpmInverse <= 0.0f)
		return false;

	std::vector<b2Vec2> vertList;
	switch(m_eType)
	{
	case HYFIXTURE_Nothing:
		return false;

	case HYFIXTURE_Circle:
		shapeDataOut.circle.center = { m_Data.circle.center.x * fPpmInverse, m_Data.circle.center.y * fPpmInverse };
		shapeDataOut.circle.radius = m_Data.circle.radius * fPpmInverse;
		break;

	case HYFIXTURE_LineSegment:
		shapeDataOut.segment.point1 = { m_Data.segment.point1.x * fPpmInverse, m_Data.segment.point1.y * fPpmInverse };
		shapeDataOut.segment.point2 = { m_Data.segment.point2.x * fPpmInverse, m_Data.segment.point2.y * fPpmInverse };
		break;

	case HYFIXTURE_Polygon:
		shapeDataOut.polygon = m_Data.polygon;
		for(int32 i = 0; i < m_Data.polygon.count; ++i)
			shapeDataOut.polygon.vertices[i] = { m_Data.polygon.vertices[i].x * fPpmInverse, m_Data.polygon.vertices[i].y * fPpmInverse };
		break;

	case HYFIXTURE_Capsule:
		shapeDataOut.capsule.center1 = { vertList[0].x * fPpmInverse, vertList[0].y * fPpmInverse };
		shapeDataOut.capsule.center2 = { vertList[1].x * fPpmInverse, vertList[1].y * fPpmInverse };
		shapeDataOut.capsule.radius = m_Data.capsule.radius * fPpmInverse;
		break;

	default:
		HyError("HyShape2d::TransformShapeData() - Unhandled shape type: " << m_eType);
		return false;
	}

	return true;
}
