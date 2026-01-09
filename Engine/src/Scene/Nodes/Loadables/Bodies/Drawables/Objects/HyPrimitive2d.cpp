/**************************************************************************
 *	HyPrimitive2d.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Loadables/Bodies/Drawables/Objects/HyPrimitive2d.h"
#include "Scene/Nodes/Loadables/Bodies/Objects/HyEntity2d.h"
#include "HyEngine.h"
#include "Utilities/HyMath.h"

HyPrimitive2d::HyPrimitive2d(HyEntity2d *pParent /*= nullptr*/) :
	IHyDrawable2d(HYTYPE_Primitive, HyNodePath(), pParent),
	m_pVertBuffer(nullptr),
	m_uiNumVerts(0),
	m_bWireframe(false),
	m_fLineThickness(1.0f),
	m_uiNumSegments(16),
	m_eRenderMode(HYRENDERMODE_Unknown),
	m_bUpdateShaderUniforms(true),
	m_pChainData(nullptr)
{
}

HyPrimitive2d::HyPrimitive2d(const HyPrimitive2d &copyRef) :
	IHyDrawable2d(copyRef),
	m_pVertBuffer(nullptr),
	m_uiNumVerts(0),
	m_bWireframe(copyRef.m_bWireframe),
	m_fLineThickness(copyRef.m_fLineThickness),
	m_uiNumSegments(copyRef.m_uiNumSegments),
	m_eRenderMode(copyRef.m_eRenderMode),
	m_bUpdateShaderUniforms(true),
	m_pChainData(nullptr)
{
	operator=(copyRef);
}

HyPrimitive2d::~HyPrimitive2d(void)
{
	ClearChainData();
	ClearVertexData();
}

const HyPrimitive2d &HyPrimitive2d::operator=(const HyPrimitive2d &rhs)
{
	IHyDrawable2d::operator=(rhs);

	ClearChainData();
	ClearVertexData();

	m_bWireframe = rhs.m_bWireframe;
	m_fLineThickness = rhs.m_fLineThickness;
	m_uiNumSegments = rhs.m_uiNumSegments;
	m_eRenderMode = rhs.m_eRenderMode;
	
	if(rhs.m_pChainData)
		SetAsLineChain(rhs.m_pChainData->pPointList, rhs.m_pChainData->iCount, rhs.m_pChainData->bLoop);
	else
		SetAsShape(rhs.m_Shape);

	return *this;
}

/*virtual*/ void HyPrimitive2d::CalcLocalBoundingShape(HyShape2d &shapeOut) /*override*/
{
	if(m_pChainData)
	{
		std::vector<b2Vec2> pointList;
		for(int i = 0; i < m_pChainData->iCount; ++i)
			pointList.push_back({ m_pChainData->pPointList[i].x, m_pChainData->pPointList[i].y });

		b2AABB aabb = b2MakeAABB(pointList.data(), pointList.size(), 0.0f);
		b2Vec2 vExtents = b2AABB_Extents(aabb);
		b2Vec2 ptCenter = b2AABB_Center(aabb);
		shapeOut.SetAsBox(HyRect(vExtents.x, vExtents.y, glm::vec2(ptCenter.x, ptCenter.y), 0.0f));
	}
	else
		shapeOut = m_Shape;
}

/*virtual*/ float HyPrimitive2d::GetWidth(float fPercent /*= 1.0f*/) /*override*/
{
	b2AABB aabb;
	if(m_pChainData)
	{
		std::vector<b2Vec2> pointList;
		for(int i = 0; i < m_pChainData->iCount; ++i)
			pointList.push_back({ m_pChainData->pPointList[i].x, m_pChainData->pPointList[i].y });

		aabb = b2MakeAABB(pointList.data(), pointList.size(), 0.0f);
	}
	else
		m_Shape.ComputeAABB(aabb, glm::mat4(1.0f));

	return (b2AABB_Extents(aabb).x * 2.0f) * fPercent;
}

/*virtual*/ float HyPrimitive2d::GetHeight(float fPercent /*= 1.0f*/) /*override*/
{
	b2AABB aabb;
	if(m_pChainData)
	{
		std::vector<b2Vec2> pointList;
		for(int i = 0; i < m_pChainData->iCount; ++i)
			pointList.push_back({ m_pChainData->pPointList[i].x, m_pChainData->pPointList[i].y });

		aabb = b2MakeAABB(pointList.data(), pointList.size(), 0.0f);
	}
	else
		m_Shape.ComputeAABB(aabb, glm::mat4(1.0f));

	return (b2AABB_Extents(aabb).y * 2.0f) * fPercent;
}

HyFixtureType HyPrimitive2d::GetShapeType() const
{
	if(m_pChainData)
		return HYFIXTURE_LineChain;

	return m_Shape.GetType();
}

void HyPrimitive2d::SetAsNothing()
{
	ClearChainData();
	m_Shape.SetAsNothing();
	AssembleData();
}

void HyPrimitive2d::SetAsShape(const HyShape2d &shapeRef)
{
	ClearChainData();
	m_Shape = shapeRef;
	AssembleData();
}

void HyPrimitive2d::SetAsLineSegment(const glm::vec2 &pt1, const glm::vec2 &pt2)
{
	ClearChainData();
	m_Shape.SetAsLineSegment(pt1, pt2);
	AssembleData();
}

void HyPrimitive2d::SetAsLineChain(const glm::vec2 *pVertices, uint32 uiNumVerts, bool bLoop)
{
	if(uiNumVerts < 4)
	{
		HyLogWarning("HyPrimitive2d::SetAsLineChain() failed - Line chains must be initialized with at least 4 vertices");
		return;
	}

	ClearChainData();
	m_Shape.SetAsNothing();

	m_pChainData = HY_NEW HyChainData();
	m_pChainData->iCount = uiNumVerts;
	m_pChainData->bLoop = bLoop;
	m_pChainData->pPointList = HY_NEW glm::vec2[uiNumVerts];
	for(uint32 i = 0; i < uiNumVerts; ++i)
		m_pChainData->pPointList[i] = pVertices[i];

	AssembleData();
}

void HyPrimitive2d::SetAsLineChain(const std::vector<glm::vec2> &verticesList, bool bLoop)
{
	SetAsLineChain(verticesList.data(), verticesList.size(), bLoop);
}

void HyPrimitive2d::SetAsLineChain(const HyChainData &chainData)
{
	SetAsLineChain(chainData.pPointList, chainData.iCount, chainData.bLoop);
}

void HyPrimitive2d::SetAsCircle(float fRadius)
{
	ClearChainData();
	m_Shape.SetAsCircle(fRadius);
	AssembleData();
}

void HyPrimitive2d::SetAsCircle(const glm::vec2 &ptCenter, float fRadius)
{
	ClearChainData();
	m_Shape.SetAsCircle(ptCenter, fRadius);
	AssembleData();
}

void HyPrimitive2d::SetAsPolygon(const glm::vec2 *pPointArray, uint32 uiCount)
{
	ClearChainData();
	m_Shape.SetAsPolygon(pPointArray, uiCount);
	AssembleData();
}

void HyPrimitive2d::SetAsPolygon(const std::vector<glm::vec2> &verticesList)
{
	ClearChainData();
	m_Shape.SetAsPolygon(verticesList);
	AssembleData();
}

void HyPrimitive2d::SetAsBox(float fWidth, float fHeight)
{
	ClearChainData();
	m_Shape.SetAsBox(fWidth, fHeight);
	AssembleData();
}

void HyPrimitive2d::SetAsBox(const HyRect &rect)
{
	ClearChainData();
	m_Shape.SetAsBox(rect);
	AssembleData();
}

void HyPrimitive2d::SetAsCapsule(const glm::vec2 &pt1, const glm::vec2 &pt2, float fRadius)
{
	ClearChainData();
	m_Shape.SetAsCapsule(pt1, pt2, fRadius);
	AssembleData();
}

const HyChainData *HyPrimitive2d::GetChainData() const
{
	return m_pChainData;
}

uint32 HyPrimitive2d::GetNumVerts() const
{
	return m_uiNumVerts;
}

const glm::vec2 *HyPrimitive2d::GetVerts() const
{
	return m_pVertBuffer;
}

void HyPrimitive2d::GetCentroid(glm::vec2 &ptCentroidOut) const
{
	if(m_pChainData)
	{
		std::vector<b2Vec2> pointList;
		for(int i = 0; i < m_pChainData->iCount; ++i)
			pointList.push_back({ m_pChainData->pPointList[i].x, m_pChainData->pPointList[i].y });

		b2AABB aabb = b2MakeAABB(pointList.data(), pointList.size(), 0.0f);
		b2Vec2 ptCenter = b2AABB_Center(aabb);
		ptCentroidOut.x = ptCenter.x;
		ptCentroidOut.y = ptCenter.y;
	}
	else
		m_Shape.GetCentroid(ptCentroidOut);
}

bool HyPrimitive2d::IsWireframe()
{
	return m_bWireframe;
}

void HyPrimitive2d::SetWireframe(bool bIsWireframe)
{
	if(m_bWireframe == bIsWireframe)
		return;

	m_bWireframe = bIsWireframe;
	AssembleData();
}

float HyPrimitive2d::GetLineThickness()
{
	return m_fLineThickness;
}

void HyPrimitive2d::SetLineThickness(float fThickness)
{
	if(m_fLineThickness == fThickness)
		return;

	m_fLineThickness = fThickness;
	AssembleData();
}

uint32 HyPrimitive2d::GetNumCircleSegments()
{
	return m_uiNumSegments;
}

void HyPrimitive2d::SetNumCircleSegments(uint32 uiNumSegments)
{
	m_uiNumSegments = uiNumSegments;
	AssembleData();
}

/*virtual*/ bool HyPrimitive2d::IsLoadDataValid() /*override*/
{
	return m_pVertBuffer != nullptr && (m_pChainData || m_Shape.IsValid());
}

/*virtual*/ void HyPrimitive2d::SetDirty(uint32 uiDirtyFlags) /*override*/
{
	IHyDrawable2d::SetDirty(uiDirtyFlags);

	if(uiDirtyFlags & DIRTY_Transform | DIRTY_Color)
		m_bUpdateShaderUniforms = true;
}

/*virtual*/ bool HyPrimitive2d::OnIsValidToRender() /*override*/
{
	return m_pVertBuffer != nullptr && (m_pChainData || m_Shape.IsValid());
}

/*virtual*/ void HyPrimitive2d::OnUpdateUniforms(float fExtrapolatePercent) /*override*/
{
	// TODO: get rid of this check and improve m_ShaderUniforms
	if(m_bUpdateShaderUniforms)
	{
		glm::mat4 mtx = GetSceneTransform(fExtrapolatePercent);

		// TODO: Get rid of top/bot color
		glm::vec3 tint = CalculateTopTint(fExtrapolatePercent);
		glm::vec4 vTop;
		vTop.x = tint.x;
		vTop.y = tint.y;
		vTop.z = tint.z;
		vTop.a = CalculateAlpha(fExtrapolatePercent);

		m_ShaderUniforms.Set("u_mtxTransform", mtx);
		m_ShaderUniforms.Set("u_vColor", vTop);

		m_bUpdateShaderUniforms = false;
	}
}

/*virtual*/ void HyPrimitive2d::PrepRenderStage(uint32 uiStageIndex, HyRenderMode &eRenderModeOut, HyBlendMode &eBlendModeOut, uint32 &uiNumInstancesOut, uint32 &uiNumVerticesPerInstOut, bool &bIsBatchable) /*override*/
{
	eRenderModeOut = m_eRenderMode;
	eBlendModeOut = HYBLENDMODE_Normal;
	uiNumInstancesOut = 1;
	uiNumVerticesPerInstOut = GetNumVerts();
	bIsBatchable = false;
}

/*virtual*/ bool HyPrimitive2d::WriteVertexData(uint32 uiNumInstances, HyVertexBuffer &vertexBufferRef, float fExtrapolatePercent) /*override*/
{
	vertexBufferRef.AppendVertexData(m_pVertBuffer, m_uiNumVerts * sizeof(glm::vec2));
	return true;
}

void HyPrimitive2d::ClearChainData()
{
	if(m_pChainData)
		delete[] m_pChainData->pPointList;
	delete m_pChainData;
	m_pChainData = nullptr;
}

void HyPrimitive2d::ClearVertexData()
{
	delete [] m_pVertBuffer;
	m_pVertBuffer = nullptr;
	m_uiNumVerts = 0;

	m_eRenderMode = HYRENDERMODE_Unknown;
	m_ShaderUniforms.Clear();
}

void HyPrimitive2d::AssembleData()
{
	if(m_pChainData)
	{
		std::vector<b2Vec2> pointList(m_pChainData->iCount);
		for(int32 i = 0; i < m_pChainData->iCount; ++i)
			pointList[i] = { m_pChainData->pPointList[i].x, m_pChainData->pPointList[i].y };
		AssembleLineChain(pointList.data(), m_pChainData->iCount);
	}
	else
	{
		switch(m_Shape.GetType())
		{
		case HYFIXTURE_Nothing:	// Shape hasn't been set yet by user
			break;

		case HYFIXTURE_Circle:
			AssembleCircle(glm::vec2(m_Shape.GetAsCircle().center.x, m_Shape.GetAsCircle().center.y), m_Shape.GetAsCircle().radius, m_uiNumSegments);
			break;

		case HYFIXTURE_LineSegment: {
			std::vector<b2Vec2> pointList;
			pointList.push_back(m_Shape.GetAsSegment().point1);
			pointList.push_back(m_Shape.GetAsSegment().point2);
			AssembleLineChain(pointList.data(), 2);
			break; }

		case HYFIXTURE_Polygon:
			if(m_bWireframe)
			{
				int32 iNumVerts = m_Shape.GetAsPolygon().count;
				HyAssert(iNumVerts >= 3, "HyPrimitive error, not enough verts for HYSHAPE_Polygon");

				std::vector<b2Vec2> vertList;
				for(int32 i = 0; i < iNumVerts; ++i)
					vertList.push_back(m_Shape.GetAsPolygon().vertices[i]);

				// Make it loop
				vertList.push_back(m_Shape.GetAsPolygon().vertices[0]);

				AssembleLineChain(vertList.data(), iNumVerts + 1);
			}
			else
				AssemblePolygon(m_Shape.GetAsPolygon().vertices, m_Shape.GetAsPolygon().count);
			break;

		case HYFIXTURE_Capsule:
			if(m_bWireframe)
				HyError("HyPrimitive2d::AssembleData() - Wireframe capsule not implemented");
			else
				AssembleCapsule(m_Shape.GetAsCapsule().center1, m_Shape.GetAsCapsule().center2, m_Shape.GetAsCapsule().radius, m_uiNumSegments);
			break;

		default:
			HyLogError("HyPrimitive2d::AssembleData() - Unknown shape type: " << m_Shape.GetType());
		}
	}

	SetDirty(DIRTY_SceneAABB);
	Load();
}

void HyPrimitive2d::AssembleLineChain(b2Vec2 *pVertexList, uint32 uiNumVertices)
{
	HyAssert(uiNumVertices > 1, "HyPrimitive2d::SetAsLineChain was passed an empty vertexList or a vertexList of only '1' vertex");
	ClearVertexData();

	m_eRenderMode = HYRENDERMODE_Triangles;

	m_uiNumVerts = (uiNumVertices - 1) * 6;
	m_pVertBuffer = HY_NEW glm::vec2[m_uiNumVerts];

	uint32 uiBufferIndex = 0;

	// TODO: THIS NEEDS TO CHECK FOR CHAIN LOOP!!!!!!

	std::vector<glm::vec2> vertList;
	for(uint32 i = 0; i < uiNumVertices - 1; ++i)
	{
		b2Vec2 vDir = pVertexList[i + 1] - pVertexList[i];
		vDir = b2Normalize(vDir);

		b2Vec2 ptExtents[4];
		
		ptExtents[0] = { vDir.y, -vDir.x };
		ptExtents[0] *= (m_fLineThickness * 0.5f);
		ptExtents[0] += pVertexList[i];

		ptExtents[1] = { -vDir.y, vDir.x };
		ptExtents[1] *= (m_fLineThickness * 0.5f);
		ptExtents[1] += pVertexList[i];

		ptExtents[2] = { -vDir.y, vDir.x };
		ptExtents[2] *= (m_fLineThickness * 0.5f);
		ptExtents[2] += pVertexList[i + 1];

		ptExtents[3] = { vDir.y, -vDir.x };
		ptExtents[3] *= (m_fLineThickness * 0.5f);
		ptExtents[3] += pVertexList[i + 1];

		for(uint32 i = 1; i < 3; ++i)
		{
			m_pVertBuffer[uiBufferIndex].x = ptExtents[0].x;
			m_pVertBuffer[uiBufferIndex].y = ptExtents[0].y;
			uiBufferIndex++;

			m_pVertBuffer[uiBufferIndex].x = ptExtents[i].x;
			m_pVertBuffer[uiBufferIndex].y = ptExtents[i].y;
			uiBufferIndex++;

			m_pVertBuffer[uiBufferIndex].x = ptExtents[i + 1].x;
			m_pVertBuffer[uiBufferIndex].y = ptExtents[i + 1].y;
			uiBufferIndex++;
		}
	}

	return;
}

void HyPrimitive2d::AssembleCircle(glm::vec2 ptCenter, float fRadius, uint32 uiSegments)
{
	const float k_segments = static_cast<float>(uiSegments);
	const float k_increment = 2.0f * glm::pi<float>() / k_segments;
	float sinInc = sinf(k_increment);
	float cosInc = cosf(k_increment);
	glm::vec2 r1(cosInc, sinInc);
	glm::vec2 v1 = ptCenter + fRadius * r1;

	if(m_bWireframe == false)
	{
		ClearVertexData();

		m_eRenderMode = HYRENDERMODE_Triangles;
		m_uiNumVerts = static_cast<uint32>(k_segments) * 3;
		m_pVertBuffer = HY_NEW glm::vec2[m_uiNumVerts];

		//m_RenderState.SetShaderId(HYSHADERPROG_Primitive);
		//m_RenderState.SetNumVerticesPerInstance(uiNumVerts);

		uint32 uiBufferIndex = 0;
		for(int32 i = 0; i < k_segments; ++i)
		{
			// Perform rotation to avoid additional trigonometry.
			glm::vec2 r2;
			r2.x = cosInc * r1.x - sinInc * r1.y;
			r2.y = sinInc * r1.x + cosInc * r1.y;
			glm::vec2 v2 = ptCenter + fRadius * r2;

			m_pVertBuffer[uiBufferIndex++] = ptCenter;
			m_pVertBuffer[uiBufferIndex++] = v1;
			m_pVertBuffer[uiBufferIndex++] = v2;

			r1 = r2;
			v1 = v2;
		}
	}
	else
	{
		r1.x = 1.0f;
		r1.y = 0.0f;
		v1 = ptCenter + fRadius * r1;

		std::vector<b2Vec2> vertexList;
		vertexList.push_back({ v1.x, v1.y });
		
		for(int32 i = 0; i < k_segments; ++i)
		{
			glm::vec2 r2;
			r2.x = cosInc * r1.x - sinInc * r1.y;
			r2.y = sinInc * r1.x + cosInc * r1.y;
			glm::vec2 v2 = ptCenter + fRadius * r2;

			vertexList.push_back({ v2.x, v2.y });

			//m_lines->Vertex(v1, color);
			//m_lines->Vertex(v2, color);
			r1 = r2;
			//v1 = v2;
		}

		AssembleLineChain(&vertexList[0], static_cast<uint32>(vertexList.size()));
	}
}

void HyPrimitive2d::AssemblePolygon(const b2Vec2 *pVertexList, uint32 uiNumVertices)
{
	ClearVertexData();

	m_eRenderMode = HYRENDERMODE_Triangles;
	m_uiNumVerts = (uiNumVertices - 2) * 3;
	m_pVertBuffer = HY_NEW glm::vec2[m_uiNumVerts];

	//m_RenderState.SetShaderId(HYSHADERPROG_Primitive);
	//m_RenderState.SetNumVerticesPerInstance(uiNumBufferVerts);

	uint32 uiBufferIndex = 0;
	for(uint32 i = 1; i < uiNumVertices - 1; ++i)
	{
		m_pVertBuffer[uiBufferIndex].x = pVertexList[0].x;
		m_pVertBuffer[uiBufferIndex].y = pVertexList[0].y;
		uiBufferIndex++;

		m_pVertBuffer[uiBufferIndex].x = pVertexList[i].x;
		m_pVertBuffer[uiBufferIndex].y = pVertexList[i].y;
		uiBufferIndex++;

		m_pVertBuffer[uiBufferIndex].x = pVertexList[i+1].x;
		m_pVertBuffer[uiBufferIndex].y = pVertexList[i+1].y;
		uiBufferIndex++;
	}
}

void HyPrimitive2d::AssembleCapsule(const b2Vec2 &ptCenter1, const b2Vec2 &ptCenter2, float fRadius, uint32 uiSegments)
{
	ClearVertexData();

	m_eRenderMode = HYRENDERMODE_Triangles;

	glm::vec2 a = glm::vec2(ptCenter1.x, ptCenter1.y);
	glm::vec2 b = glm::vec2(ptCenter2.x, ptCenter2.y);

	// Direction and normal
	glm::vec2 dir = glm::normalize(b - a);
	glm::vec2 normal = glm::vec2(-dir.y, dir.x);
	glm::vec2 offset = normal * fRadius;

	// Rectangle corners
	glm::vec2 p0 = a + offset;
	glm::vec2 p1 = b + offset;
	glm::vec2 p2 = b - offset;
	glm::vec2 p3 = a - offset;

	// Add rectangle as two triangles
	std::vector<glm::vec2> verts;
	verts.push_back(p0);
	verts.push_back(p1);
	verts.push_back(p2);

	verts.push_back(p2);
	verts.push_back(p3);
	verts.push_back(p0);

	// Circle caps
	float angleStart = glm::atan(dir.y, dir.x);
	float angleStep = glm::pi<float>() / static_cast<float>(uiSegments);

	// Cap at 'a'
	for(int i = 0; i < static_cast<int>(uiSegments); ++i) {
		float angle1 = angleStart + glm::pi<float>() / 2 + i * angleStep;
		float angle2 = angle1 + angleStep;

		glm::vec2 pA1 = a + fRadius * glm::vec2(cos(angle1), sin(angle1));
		glm::vec2 pA2 = a + fRadius * glm::vec2(cos(angle2), sin(angle2));

		verts.push_back(a);
		verts.push_back(pA1);
		verts.push_back(pA2);
	}

	// Cap at 'b'
	for(int i = 0; i < static_cast<int>(uiSegments); ++i) {
		float angle1 = angleStart - glm::pi<float>() / 2 - i * angleStep;
		float angle2 = angle1 - angleStep;

		glm::vec2 pB1 = b + fRadius * glm::vec2(cos(angle1), sin(angle1));
		glm::vec2 pB2 = b + fRadius * glm::vec2(cos(angle2), sin(angle2));

		verts.push_back(b);
		verts.push_back(pB1);
		verts.push_back(pB2);
	}

	// Output
	m_uiNumVerts = static_cast<uint32_t>(verts.size());
	m_pVertBuffer = HY_NEW glm::vec2[m_uiNumVerts];
	std::copy(verts.begin(), verts.end(), m_pVertBuffer);
}

/*static*/ void HyPrimitive2d::OnShapeChanged(void *pData)
{
	HyPrimitive2d *pThis = reinterpret_cast<HyPrimitive2d *>(pData);
	pThis->AssembleData();
}
