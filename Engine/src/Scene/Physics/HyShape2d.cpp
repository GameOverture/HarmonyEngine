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
	m_hPhysics({}),
	m_bPhysicsAllowed(true),
	m_pPhysicsInit(nullptr),
	m_bPhysicsDirty(false),
	m_fMaxPush(FLT_MAX),
	m_bClipVelocity(true)
{
	if(pParent)
		pParent->ShapeAppend(*this);
}

HyShape2d::HyShape2d(const HyShape2d &copyRef) : 
	m_pParent(nullptr),
	m_eType(HYSHAPE_Nothing),
	m_Data({}),
	m_hPhysics({}),
	m_bPhysicsAllowed(copyRef.m_bPhysicsAllowed),
	m_pPhysicsInit(nullptr),
	m_bPhysicsDirty(false),
	m_fMaxPush(copyRef.m_fMaxPush),
	m_bClipVelocity(copyRef.m_bClipVelocity)
{
	*this = copyRef;
}

/*virtual*/ HyShape2d::~HyShape2d()
{
	if(m_pParent)
		m_pParent->ShapeRemove(*this);

	if(m_eType != HYSHAPE_LineChain)
	{
		if(b2Shape_IsValid(m_hPhysics.shape))
			b2DestroyShape(m_hPhysics.shape, true);
	}
	else
	{
		if(b2Chain_IsValid(m_hPhysics.chain))
			b2DestroyChain(m_hPhysics.chain);
	}

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

	m_fMaxPush = rhs.m_fMaxPush;
	m_bClipVelocity = rhs.m_bClipVelocity;

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

const b2Circle &HyShape2d::GetAsCircle() const
{
	if(m_eType != HYSHAPE_Circle)
	{
		HyLogError("HyShape2d::GetAsCircle() - Shape is not a circle.");
		return b2Circle() = {};
	}
	return m_Data.circle;
}

const b2Segment &HyShape2d::GetAsSegment() const
{
	if(m_eType != HYSHAPE_LineSegment)
	{
		HyLogError("HyShape2d::GetAsSegment() - Shape is not a line segment.");
		return b2Segment() = {};
	}
	return m_Data.segment;
}

const b2Polygon &HyShape2d::GetAsPolygon() const
{
	if(m_eType != HYSHAPE_Polygon)
	{
		HyLogError("HyShape2d::GetAsPolygon() - Shape is not a polygon.");
		return b2Polygon() = {};
	}
	return m_Data.polygon;
}

const HyChainData &HyShape2d::GetAsChain() const
{
	if(m_eType != HYSHAPE_LineChain)
	{
		HyLogError("HyShape2d::GetAsChain() - Shape is not a line chain.");
		return HyChainData() = {};
	}
	return m_Data.chain;
}

const b2Capsule &HyShape2d::GetAsCapsule() const
{
	if(m_eType != HYSHAPE_Capsule)
	{
		HyLogError("HyShape2d::GetAsCapsule() - Shape is not a capsule.");
		return b2Capsule() = {};
	}
	return m_Data.capsule;
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

	if(IsPhysicsRegistered())
	{
		if(m_eType != HYSHAPE_LineChain)
		{
			if(shapeDefRef.isSensor != b2Shape_IsSensor(m_hPhysics.shape))
				HyLogWarning("HyShape2d::Setup() - Already simulated shape cannot change to/from a sensor <-> solid");

			b2Shape_SetFilter(m_hPhysics.shape, shapeDefRef.filter);
			b2Shape_SetFriction(m_hPhysics.shape, shapeDefRef.material.friction);
			b2Shape_SetRestitution(m_hPhysics.shape, shapeDefRef.material.restitution);
			//b2World_SetRestitutionThreshold(m_hPhysics.shape, );
			b2Shape_SetDensity(m_hPhysics.shape, shapeDefRef.density, true);
		}
		else
		{
			b2Chain_SetFriction(m_hPhysics.chain, shapeDefRef.material.friction);
			b2Chain_SetRestitution(m_hPhysics.chain, shapeDefRef.material.restitution);
		}
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
	if(IsPhysicsRegistered() && m_eType != HYSHAPE_LineChain)
		return b2Shape_GetDensity(m_hPhysics.shape);
	else if(m_pPhysicsInit)
		return m_pPhysicsInit->density;
	else
		return b2DefaultShapeDef().density;
}

void HyShape2d::SetDensity(float fDensity, bool bUpdateBodyMass /*= true*/)
{
	if(IsPhysicsRegistered() && m_eType != HYSHAPE_LineChain)
		b2Shape_SetDensity(m_hPhysics.shape, fDensity, bUpdateBodyMass);
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
	if(IsPhysicsRegistered())
	{
		if(m_eType != HYSHAPE_LineChain)
			return b2Shape_GetFriction(m_hPhysics.shape);
		else
			return b2Chain_GetFriction(m_hPhysics.chain);
	}
	else if(m_pPhysicsInit)
		return m_pPhysicsInit->material.friction;
	else
		return b2DefaultShapeDef().material.friction;
}

void HyShape2d::SetFriction(float fFriction)
{
	if(IsPhysicsRegistered())
	{
		if(m_eType != HYSHAPE_LineChain)
			b2Shape_SetFriction(m_hPhysics.shape, fFriction);
		else
			b2Chain_SetFriction(m_hPhysics.chain, fFriction);
	}
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
	if(IsPhysicsRegistered())
	{
		if(m_eType != HYSHAPE_LineChain)
			return b2Shape_GetRestitution(m_hPhysics.shape);
		else
			return b2Chain_GetRestitution(m_hPhysics.chain);
	}
	else if(m_pPhysicsInit)
		return m_pPhysicsInit->material.restitution;
	else
		return b2DefaultShapeDef().material.restitution;
}

void HyShape2d::SetRestitution(float fRestitution)
{
	if(IsPhysicsRegistered())
	{
		if(m_eType != HYSHAPE_LineChain)
			b2Shape_SetRestitution(m_hPhysics.shape, fRestitution);
		else
			b2Chain_SetRestitution(m_hPhysics.chain, fRestitution);
	}
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
	if(IsPhysicsRegistered())
	{
		if(m_eType != HYSHAPE_LineChain)
			return b2Shape_GetFilter(m_hPhysics.shape);
		else
		{
			// NOTE: Box2d API doesn't have 'b2Chain_GetFilter' function, query the first segment instead

			b2ShapeId hSegment = b2_nullShapeId;
			if(1 == b2Chain_GetSegments(m_hPhysics.chain, &hSegment, 1))
				return b2Shape_GetFilter(hSegment);
			else
				return b2DefaultFilter();
		}
	}
	else if(m_pPhysicsInit)
		return m_pPhysicsInit->filter;
	else
		return b2DefaultShapeDef().filter;
}

// WARNING: This function is potentially expensive if this shape is a line chain and its parent entity has already been physics.Activate()
void HyShape2d::SetFilter(const b2Filter &filter)
{
	if(IsPhysicsRegistered())
	{
		if(m_eType != HYSHAPE_LineChain)
			b2Shape_SetFilter(m_hPhysics.shape, filter);
		else
		{
			// NOTE: Box2d API doesn't have 'b2Chain_SetFilter' function to dissuade users from not using b2ChainDef
			// Set all the segments directly instead

			int iNumSegments = b2Chain_GetSegmentCount(m_hPhysics.chain);
			b2ShapeId *pSegmentHandles = HY_NEW b2ShapeId[iNumSegments];
			if(iNumSegments == b2Chain_GetSegments(m_hPhysics.chain, pSegmentHandles, iNumSegments))
			{
				for(int i = 0; i < iNumSegments; ++i)
					b2Shape_SetFilter(pSegmentHandles[i], filter);
			}
			delete[] pSegmentHandles;
		}
	}
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
	if(m_eType == HYSHAPE_LineChain)
		return false;

	if(IsPhysicsRegistered())
		return b2Shape_IsSensor(m_hPhysics.shape);
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

		// Ideally use Box2d to TestPoint if supported (line shapes need own implementation)
		switch(m_eType)
		{
		case HYSHAPE_Circle:
			return b2PointInCircle({ ptTestPoint.x, ptTestPoint.y }, &shapeData.circle);

		case HYSHAPE_LineSegment: {
			glm::vec2 pt1(shapeData.segment.point1.x, shapeData.segment.point1.y);
			glm::vec2 pt2(shapeData.segment.point2.x, shapeData.segment.point2.y);
			glm::vec2 ptCenter = pt1 + ((pt2 - pt1) * 0.5f);
			float fRadians = std::atan2(pt2.y - pt1.y, pt2.x - pt1.x);

			b2Polygon tmpBox = b2MakeOffsetBox(0.5f * glm::distance(pt1, pt2),
											   4.0f,
											   { ptCenter.x, ptCenter.y },
											   b2MakeRot(fRadians));

			return b2PointInPolygon({ ptTestPoint.x, ptTestPoint.y }, &tmpBox); }

		case HYSHAPE_Polygon:
			return b2PointInPolygon({ ptTestPoint.x, ptTestPoint.y }, &shapeData.polygon);

		case HYSHAPE_LineChain: {
			b2AABB aabb;
			ComputeChainAabb(aabb, shapeData);
			delete[] shapeData.chain.pPointList;
			return HyMath::TestPointAABB(aabb, ptTestPoint); }
				
		case HYSHAPE_Capsule:
			return b2PointInCapsule({ ptTestPoint.x, ptTestPoint.y }, &shapeData.capsule);

		default:
			HyError("HyShape2d::TransformShapeData() - Unhandled shape type: " << m_eType);
			break;
		}
	}

	return false;
}

b2CastOutput HyShape2d::TestRay(const glm::vec2 &ptStart, const glm::vec2 &vDirection, const glm::mat4 &mtxSelfTransform) const
{
	HyError("HyShape2d::TestRay - Not implemented");
	return b2CastOutput() = {};
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
	ShapeData shapeData = {};
	if(TransformShapeData(shapeData, mtxTransform))
	{
		switch(m_eType)
		{
		case HYSHAPE_Circle:
			aabbOut = b2ComputeCircleAABB(&shapeData.circle, b2Transform_identity);
			break;
		case HYSHAPE_LineSegment:
			aabbOut = b2ComputeSegmentAABB(&shapeData.segment, b2Transform_identity);
			break;
		case HYSHAPE_Polygon:
			aabbOut = b2ComputePolygonAABB(&shapeData.polygon, b2Transform_identity);
			break;
		case HYSHAPE_LineChain:
			ComputeChainAabb(aabbOut, shapeData);
			delete[] shapeData.chain.pPointList;
			break;
		case HYSHAPE_Capsule:
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

void HyShape2d::SetSoftCollision(float fMaxPush)
{
	m_fMaxPush = fMaxPush;
	m_bClipVelocity = false;
}

void HyShape2d::SetHardCollision()
{
	m_fMaxPush = FLT_MAX;
	m_bClipVelocity = true;
}

void HyShape2d::GetCollisionInfo(float &fPushLimitOut, bool &bClipVelocityOut) const
{
	fPushLimitOut = m_fMaxPush;
	bClipVelocityOut = m_bClipVelocity;
}

void HyShape2d::ClearShapeData()
{
	PhysicsRemove(true);

	if(m_eType == HYSHAPE_LineChain)
		delete[] m_Data.chain.pPointList;

	m_Data = {};
	m_eType = HYSHAPE_Nothing;
}

void HyShape2d::ShapeChanged()
{
	m_bPhysicsDirty = true;
}

bool HyShape2d::IsPhysicsRegistered() const
{
	if(m_eType != HYSHAPE_LineChain)
		return B2_IS_NON_NULL(m_hPhysics.shape);
	else
		return B2_IS_NON_NULL(m_hPhysics.chain);
}

bool HyShape2d::IsPhysicsDirty() const
{
	return m_bPhysicsDirty;
}

void HyShape2d::PhysicsAttach()
{
	PhysicsRemove(false);

	if(m_pPhysicsInit == nullptr)
	{
		m_pPhysicsInit = HY_NEW b2ShapeDef();
		*m_pPhysicsInit = b2DefaultShapeDef();
	}
	m_pPhysicsInit->userData = this;

	// Create Box2d "geometry" of this shape that uses pixel-per-meters
	ShapeData shapeData = { };
	if(TransformShapeData(shapeData, IHyNode::sm_pScene->GetPpmInverse()))
	{
		switch(m_eType)
		{
		case HYSHAPE_Circle:
			m_hPhysics = {};
			m_hPhysics.shape = b2CreateCircleShape(m_pParent->physics.GetHandle(), m_pPhysicsInit, &shapeData.circle);
			break;
		case HYSHAPE_LineSegment:
			m_hPhysics = {};
			m_hPhysics.shape = b2CreateSegmentShape(m_pParent->physics.GetHandle(), m_pPhysicsInit, &shapeData.segment);
			break;
		case HYSHAPE_Polygon:
			m_hPhysics = {};
			m_hPhysics.shape = b2CreatePolygonShape(m_pParent->physics.GetHandle(), m_pPhysicsInit, &shapeData.polygon);
			break;
		case HYSHAPE_LineChain: {
			std::vector<b2Vec2> pointList;
			pointList.reserve(shapeData.chain.iCount);
			for(int i = 0; i < shapeData.chain.iCount; ++i)
				pointList.push_back({ shapeData.chain.pPointList[i].x, shapeData.chain.pPointList[i].y });

			b2ChainDef chainDef = b2DefaultChainDef();
			chainDef.points = pointList.data();
			chainDef.count = shapeData.chain.iCount;
			chainDef.isLoop = shapeData.chain.bLoop;

			chainDef.filter = m_pPhysicsInit->filter;
			chainDef.enableSensorEvents = m_pPhysicsInit->enableSensorEvents;
			chainDef.materialCount = 1;
			chainDef.materials = &m_pPhysicsInit->material;
			chainDef.userData = this;

			m_hPhysics = {};
			m_hPhysics.chain = b2CreateChain(m_pParent->physics.GetHandle(), &chainDef);

			delete[] shapeData.chain.pPointList;
			break; }
		case HYSHAPE_Capsule:
			m_hPhysics = {};
			m_hPhysics.shape = b2CreateCapsuleShape(m_pParent->physics.GetHandle(), m_pPhysicsInit, &shapeData.capsule);
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

	if(m_eType != HYSHAPE_LineChain)
		b2DestroyShape(m_hPhysics.shape, bUpdateBodyMass);
	else
		b2DestroyChain(m_hPhysics.chain);

	m_hPhysics = {};
	m_bPhysicsDirty = false;
}

bool HyShape2d::ComputeChainAabb(b2AABB &aabbOut, const ShapeData &shapeData) const
{
	HyMath::InvalidateAABB(aabbOut);
	for(int32 i = 0; i < shapeData.chain.iCount; ++i)
	{
		if(b2IsValidAABB(aabbOut) == false)
		{
			aabbOut.lowerBound = { shapeData.chain.pPointList[i].x, shapeData.chain.pPointList[i].y };
			aabbOut.upperBound = { shapeData.chain.pPointList[i].x, shapeData.chain.pPointList[i].y };
		}
		else
		{
			if(shapeData.chain.pPointList[i].x < aabbOut.lowerBound.x)
				aabbOut.lowerBound.x = shapeData.chain.pPointList[i].x;
			else if(shapeData.chain.pPointList[i].x > aabbOut.upperBound.x)
				aabbOut.upperBound.x = shapeData.chain.pPointList[i].x;

			if(shapeData.chain.pPointList[i].y < aabbOut.lowerBound.y)
				aabbOut.lowerBound.y = shapeData.chain.pPointList[i].y;
			else if(shapeData.chain.pPointList[i].y > aabbOut.upperBound.y)
				aabbOut.upperBound.y = shapeData.chain.pPointList[i].y;
		}
	}
	// If the AABB is flat, inflate it by 5 pixel radius
	if(aabbOut.lowerBound.x == aabbOut.upperBound.x)
	{
		aabbOut.lowerBound.x -= 5.0f;
		aabbOut.upperBound.x += 5.0f;
	}
	if(aabbOut.lowerBound.y == aabbOut.upperBound.y)
	{
		aabbOut.lowerBound.y -= 5.0f;
		aabbOut.upperBound.y += 5.0f;
	}

	return true;
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

bool HyShape2d::TransformShapeData(ShapeData &shapeDataOut, float fPpmInverse) const
{
	if(fPpmInverse <= 0.0f)
		return false;

	std::vector<b2Vec2> vertList;
	switch(m_eType)
	{
	case HYSHAPE_Nothing:
		return false;

	case HYSHAPE_Circle:
		shapeDataOut.circle.center = { m_Data.circle.center.x * fPpmInverse, m_Data.circle.center.y * fPpmInverse };
		shapeDataOut.circle.radius = m_Data.circle.radius * fPpmInverse;
		break;

	case HYSHAPE_LineSegment:
		shapeDataOut.segment.point1 = { m_Data.segment.point1.x * fPpmInverse, m_Data.segment.point1.y * fPpmInverse };
		shapeDataOut.segment.point2 = { m_Data.segment.point2.x * fPpmInverse, m_Data.segment.point2.y * fPpmInverse };
		break;
			
	case HYSHAPE_Polygon:
		shapeDataOut.polygon = m_Data.polygon;
		for(int32 i = 0; i < m_Data.polygon.count; ++i)
			shapeDataOut.polygon.vertices[i] = { m_Data.polygon.vertices[i].x * fPpmInverse, m_Data.polygon.vertices[i].y * fPpmInverse };
		break;
			
	case HYSHAPE_LineChain:
		shapeDataOut.chain.pPointList = HY_NEW glm::vec2[m_Data.chain.iCount];
		for(int32 i = 0; i < m_Data.chain.iCount; ++i)
			shapeDataOut.chain.pPointList[i] = { m_Data.chain.pPointList[i].x * fPpmInverse, m_Data.chain.pPointList[i].y * fPpmInverse };
		
		shapeDataOut.chain.iCount = m_Data.chain.iCount;
		shapeDataOut.chain.bLoop = m_Data.chain.bLoop;
		break;
			
	case HYSHAPE_Capsule:
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
