/**************************************************************************
*	HyChain2d.cpp
*
*	Harmony Engine
*	Copyright (c) 2025 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Scene/HyScene.h"
#include "Scene/Physics/Fixtures/HyChain2d.h"
#include "Scene/Nodes/Loadables/Bodies/Objects/HyEntity2d.h"
#include "Diagnostics/Console/IHyConsole.h"

HyChain2d::HyChain2d(HyEntity2d *pParent /*= nullptr*/) :
	IHyFixture2d(pParent),
	m_Data({}),
	m_hPhysics(b2_nullChainId),
	m_pPhysicsInit(nullptr)
{
}

HyChain2d::HyChain2d(const HyChain2d &copyRef) :
	IHyFixture2d(copyRef),
	m_Data({}),
	m_hPhysics(b2_nullChainId),
	m_pPhysicsInit(nullptr)
{
	*this = copyRef;
}

/*virtual*/ HyChain2d::~HyChain2d()
{
	if(b2Chain_IsValid(m_hPhysics))
	{
		b2DestroyChain(m_hPhysics);
		m_hPhysics = b2_nullChainId;
	}

	delete[] m_Data.pPointList;
	delete[] m_pMaterials;
	delete m_pPhysicsInit;

	if(m_pParent)
		m_pParent->FixtureRemove(*this);
	m_pParent = nullptr; // Null this pointer to indicate to baseclass that it properly disposed of
}

const HyChain2d &HyChain2d::operator=(const HyChain2d &rhs)
{
	ClearShapeData();

	m_Data.pPointList = HY_NEW glm::vec2[rhs.m_Data.iCount];
	m_Data.iCount = rhs.m_Data.iCount;
	m_Data.bLoop = rhs.m_Data.bLoop;

	IHyFixture2d::operator=(rhs);

	if(rhs.IsPhysicsInitialized())
		Setup(*rhs.m_pPhysicsInit);

	ShapeChanged();
	return *this;
}

const HyChainData &HyChain2d::GetData() const
{
	return m_Data;
}

void HyChain2d::SetData(const glm::vec2 *pVertices, uint32 uiNumVerts, bool bLoop, const b2ChainDef *pPhysicsInit /*= nullptr*/)
{
	if(bLoop && pVertices[0] == pVertices[uiNumVerts - 1])
	{
		HyLogWarning("HyChain2d::SetData() - Removing redundant final vertex in chain loop");
		uiNumVerts--; // Correct the vert list to not include the redundant final loop point
	}
	if(uiNumVerts < 4)
	{
		HyLogWarning("HyChain2d::SetData() failed - Line chains must be initialized with at least 4 vertices");
		return;
	}

	ClearShapeData();
	m_eType = HYFIXTURE_LineChain;

	m_Data.pPointList = HY_NEW glm::vec2[uiNumVerts];
	for(uint32 i = 0; i < uiNumVerts; ++i)
		m_Data.pPointList[i] = pVertices[i];
	m_Data.iCount = uiNumVerts;
	m_Data.bLoop = bLoop;

	if(pPhysicsInit)
		Setup(*pPhysicsInit);

	ShapeChanged();
}

void HyChain2d::SetData(const std::vector<glm::vec2> &verticesList, bool bLoop, const b2ChainDef *pPhysicsInit /*= nullptr*/)
{
	SetData(verticesList.data(), static_cast<uint32>(verticesList.size()), bLoop, pPhysicsInit);
}

/*virtual*/ void HyChain2d::TransformSelf(const glm::mat4 &mtxTransform) /*override*/
{
	if(IsValid() == false)
		return;

	HyChainData chainData = { };
	if(AllocChainData(chainData, mtxTransform))
	{
		ClearShapeData();

		m_Data = chainData;
		m_eType = HYFIXTURE_LineChain;
		ShapeChanged();
	}
}

bool HyChain2d::GetCentroid(glm::vec2 &ptCentroidOut) const
{
	if(IsValid() == false)
		return false;

	ptCentroidOut = m_Data.pPointList[0];
	int32 iNumVerts = m_Data.iCount;
	for(int32 i = 1; i < iNumVerts; ++i)
	{
		ptCentroidOut += m_Data.pPointList[i];
	}
	ptCentroidOut /= static_cast<float>(iNumVerts);
	return true;
}

void HyChain2d::Setup(const b2ChainDef &chainDefRef)
{
	HyAssert(chainDefRef.internalValue != 0, "HyChain2d::Setup() - Invalid b2ChainDef. Must be initalized with b2DefaultChainDef()");

	if(IsPhysicsRegistered())
	{
		b2Chain_SetFriction(m_hPhysics, chainDefRef.materials[0].friction);
		b2Chain_SetRestitution(m_hPhysics, chainDefRef.materials[0].restitution);
	}
	else
	{
		if(IsPhysicsInitialized() == false)
		{
			m_pPhysicsInit = HY_NEW b2ChainDef();
			*m_pPhysicsInit = b2DefaultChainDef();
		}

		*m_pPhysicsInit = chainDefRef;
		m_pPhysicsInit->userData = this;

		// Deep copy materials from chainDefRef, then reassign the pointer to the new array
		delete[] m_pMaterials;
		m_pMaterials = HY_NEW b2SurfaceMaterial[chainDefRef.materialCount];
		m_iMaterialCount = chainDefRef.materialCount;
		for(int32 i = 0; i < m_iMaterialCount; ++i)
			m_pMaterials[i] = chainDefRef.materials[i];

		m_pPhysicsInit->count = m_iMaterialCount;
		m_pPhysicsInit->materials = m_pMaterials;

		// If the vertex list is specified in the 'chainDefRef' then use SetData() to deep copy
		if(chainDefRef.points != nullptr && chainDefRef.count > 4)
		{
			std::vector<glm::vec2> pointList;
			pointList.reserve(chainDefRef.count);
			for(int32 i = 0; i < chainDefRef.count; ++i)
				pointList.push_back(glm::vec2(chainDefRef.points[i].x, chainDefRef.points[i].y));

			SetData(pointList, chainDefRef.isLoop, nullptr);
		}
	}
}

float HyChain2d::GetFriction() const
{
	if(IsPhysicsRegistered())
		return b2Chain_GetFriction(m_hPhysics);
	else if(IsPhysicsInitialized())
		return m_pPhysicsInit->materials[0].friction;
	else
		return b2DefaultChainDef().materials[0].friction;
}

void HyChain2d::SetFriction(float fFriction)
{
	if(IsPhysicsRegistered())
		b2Chain_SetFriction(m_hPhysics, fFriction);
	else if(IsPhysicsInitialized())
	{
		for(int i = 0; i < m_iMaterialCount; ++i)
			m_pMaterials[i].friction = fFriction; // NOTE: m_pPhysicsInit->materials points to this array
	}
	else
	{
		b2SurfaceMaterial *pNewSurfaceList = HY_NEW b2SurfaceMaterial[1];
		pNewSurfaceList[0].friction = fFriction;

		b2ChainDef def = b2DefaultChainDef();
		def.materialCount = 1;
		def.materials = pNewSurfaceList;
		
		Setup(def); // Will deep copy the materials
		delete[] pNewSurfaceList;
	}
}

float HyChain2d::GetRestitution() const
{
	if(IsPhysicsRegistered())
		return b2Chain_GetRestitution(m_hPhysics);
	else if(IsPhysicsInitialized())
		return m_pPhysicsInit->materials[0].restitution;
	else
		return b2DefaultChainDef().materials[0].restitution;
}

void HyChain2d::SetRestitution(float fRestitution)
{
	if(IsPhysicsRegistered())
		b2Chain_SetRestitution(m_hPhysics, fRestitution);
	else if(IsPhysicsInitialized())
	{
		for(int i = 0; i < m_iMaterialCount; ++i)
			m_pMaterials[i].restitution = fRestitution; // NOTE: m_pPhysicsInit->materials points to this array
	}
	else
	{
		b2SurfaceMaterial *pNewSurfaceList = HY_NEW b2SurfaceMaterial[1];
		pNewSurfaceList[0].restitution = fRestitution;

		b2ChainDef def = b2DefaultChainDef();
		def.materialCount = 1;
		def.materials = pNewSurfaceList;

		Setup(def); // Will deep copy the materials
		delete[] pNewSurfaceList;
	}
}

b2Filter HyChain2d::GetFilter() const
{
	if(IsPhysicsRegistered())
	{
		// NOTE: Box2d API doesn't have 'b2Chain_GetFilter' function, query the first segment instead
		b2ShapeId hSegment = b2_nullShapeId;
		if(1 == b2Chain_GetSegments(m_hPhysics, &hSegment, 1))
			return b2Shape_GetFilter(hSegment);
		else
			return b2DefaultFilter();
	}
	else if(IsPhysicsInitialized())
		return m_pPhysicsInit->filter;
	else
		return b2DefaultFilter();
}

// WARNING: This function is potentially expensive if it's apart of an active physics simulation
void HyChain2d::SetFilter(const b2Filter &filter)
{
	if(IsPhysicsRegistered())
	{
		// NOTE: Box2d API doesn't have 'b2Chain_SetFilter' function to dissuade users from not using b2ChainDef
		// Set all the segments directly instead
		HyLogWarning("HyChain2d::SetFilter() - Used on actively simulated chain. This is not recommended and may have performance costs. Setup() filters before activating");

		int iNumSegments = b2Chain_GetSegmentCount(m_hPhysics);
		b2ShapeId *pSegmentHandles = HY_NEW b2ShapeId[iNumSegments];
		if(iNumSegments == b2Chain_GetSegments(m_hPhysics, pSegmentHandles, iNumSegments))
		{
			for(int i = 0; i < iNumSegments; ++i)
				b2Shape_SetFilter(pSegmentHandles[i], filter);
		}
		delete[] pSegmentHandles;
	}
	else if(IsPhysicsInitialized())
		m_pPhysicsInit->filter = filter;
	else
	{
		b2ChainDef def = b2DefaultChainDef();
		def.filter = filter;
		Setup(def);
	}
}

/*virtual*/ bool HyChain2d::TestPoint(const glm::vec2 &ptTestPoint, const glm::mat4 &mtxSelfTransform) const /*override*/
{
	if(IsValid() == false)
		return false;

	b2AABB aabb;
	ComputeAABB(aabb, mtxSelfTransform);

	return HyMath::TestPointAABB(aabb, ptTestPoint);
}

/*virtual*/ b2CastOutput HyChain2d::TestRay(const glm::vec2 &ptStart, const glm::vec2 &vDirection, const glm::mat4 &mtxSelfTransform) const /*override*/
{
	HyError("HyChain2d::TestRay - Not implemented");
	return b2CastOutput() = {};
}

/*virtual*/ bool HyChain2d::ComputeAABB(b2AABB &aabbOut, const glm::mat4 &mtxTransform) const /*override*/
{
	if(IsValid() == false)
		return false;

	HyChainData chainData = {};
	if(AllocChainData(chainData, mtxTransform))
	{
		HyMath::InvalidateAABB(aabbOut);
		for(int32 i = 0; i < chainData.iCount; ++i)
		{
			if(b2IsValidAABB(aabbOut) == false)
			{
				aabbOut.lowerBound = { chainData.pPointList[i].x, chainData.pPointList[i].y };
				aabbOut.upperBound = { chainData.pPointList[i].x, chainData.pPointList[i].y };
			}
			else
			{
				if(chainData.pPointList[i].x < aabbOut.lowerBound.x)
					aabbOut.lowerBound.x = chainData.pPointList[i].x;
				else if(chainData.pPointList[i].x > aabbOut.upperBound.x)
					aabbOut.upperBound.x = chainData.pPointList[i].x;

				if(chainData.pPointList[i].y < aabbOut.lowerBound.y)
					aabbOut.lowerBound.y = chainData.pPointList[i].y;
				else if(chainData.pPointList[i].y > aabbOut.upperBound.y)
					aabbOut.upperBound.y = chainData.pPointList[i].y;
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

		delete[] chainData.pPointList;
		return true;
	}

	return false;
}

void HyChain2d::ClearShapeData()
{
	PhysicsRemove(true);

	delete[] m_Data.pPointList;
	m_Data = {};
	m_eType = HYFIXTURE_Nothing;
}

void HyChain2d::ShapeChanged()
{
	m_bPhysicsDirty = true;
}

bool HyChain2d::IsPhysicsRegistered() const
{
	return B2_IS_NON_NULL(m_hPhysics);
}

bool HyChain2d::IsPhysicsInitialized() const
{
	return m_pPhysicsInit != nullptr;
}

/*virtual*/ void HyChain2d::PhysicsAttach() /*override*/
{
	PhysicsRemove(false);

	if(IsPhysicsInitialized() == false)
		Setup(b2DefaultChainDef());

	// Create Box2d "geometry" of this shape that uses pixel-per-meters
	HyChainData chainData = { };
	if(AllocChainData(chainData, IHyNode::sm_pScene->GetPpmInverse()))
	{
		std::vector<b2Vec2> pointList;
		pointList.reserve(chainData.iCount);
		for(int i = 0; i < chainData.iCount; ++i)
			pointList.push_back({ chainData.pPointList[i].x, chainData.pPointList[i].y });

		m_pPhysicsInit->points = pointList.data();
		m_pPhysicsInit->count = chainData.iCount;
		m_pPhysicsInit->isLoop = chainData.bLoop;

		m_hPhysics = b2CreateChain(m_pParent->physics.GetHandle(), m_pPhysicsInit);

		delete[] chainData.pPointList;
	}

	m_bPhysicsDirty = false;
}

/*virtual*/ void HyChain2d::PhysicsRemove(bool bUpdateBodyMass) /*override*/
{
	if(IsPhysicsRegistered() == false)
		return;

	b2DestroyChain(m_hPhysics);
	m_hPhysics = b2_nullChainId;

	m_bPhysicsDirty = false;
}

// NOTE: Assumes 'chainDataOut' starts as zeroed-out. Will deep copy into 'chainDataOut' (dynamically allocate)
bool HyChain2d::AllocChainData(HyChainData &chainDataOut, const glm::mat4 &mtxTransform) const
{
	if(IsValid() == false)
		return false;

	float fScaleX = glm::length(glm::vec3(mtxTransform[0][0], mtxTransform[0][1], mtxTransform[0][2]));
	float fScaleY = glm::length(glm::vec3(mtxTransform[1][0], mtxTransform[1][1], mtxTransform[1][2]));
	if(fScaleX < FloatSlop || fScaleY < FloatSlop || std::isnan(mtxTransform[3].x) || std::isnan(mtxTransform[3].y)) // TODO: Trace down why sometimes the position vector of the transform is NaN. This causes a mostly harmless assert() in b2PolygonShape::Set() (Doing isNan check in GetLocalTransform() will break)
		return false;

	std::vector<glm::vec4> vertList;
	vertList.reserve(m_Data.iCount);
	
	chainDataOut.pPointList = HY_NEW glm::vec2[m_Data.iCount];
	for(int32 i = 0; i < m_Data.iCount; ++i)
	{
		vertList.emplace_back(m_Data.pPointList[i].x, m_Data.pPointList[i].y, 0.0f, 1.0f);
		vertList[i] = mtxTransform * vertList[i];
		chainDataOut.pPointList[i] = { vertList[i].x, vertList[i].y };
	}
	chainDataOut.iCount = m_Data.iCount;
	chainDataOut.bLoop = m_Data.bLoop;

	return true;
}

// NOTE: Assumes 'chainDataOut' starts as zeroed-out. Will deep copy into 'chainDataOut' (dynamically allocate)
bool HyChain2d::AllocChainData(HyChainData &chainDataOut, float fPpmInverse) const
{
	if(IsValid() == false || fPpmInverse <= 0.0f)
		return false;

	chainDataOut.pPointList = HY_NEW glm::vec2[m_Data.iCount];
	for(int32 i = 0; i < m_Data.iCount; ++i)
		chainDataOut.pPointList[i] = { m_Data.pPointList[i].x * fPpmInverse, m_Data.pPointList[i].y * fPpmInverse };
		
	chainDataOut.iCount = m_Data.iCount;
	chainDataOut.bLoop = m_Data.bLoop;

	return true;
}
