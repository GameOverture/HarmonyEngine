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
	IHyDrawable2d(HYTYPE_Primitive, "", "", pParent),
	m_pVertBuffer(nullptr),
	m_uiNumVerts(0),
	m_bWireframe(false),
	m_fLineThickness(1.0f),
	m_uiNumSegments(16),
	m_eRenderMode(HYRENDERMODE_Unknown),
	m_bUpdateShaderUniforms(true)
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
	m_bUpdateShaderUniforms(true)
{
	AssembleData();
}

HyPrimitive2d::~HyPrimitive2d(void)
{
	ClearVertexData();
}

const HyPrimitive2d &HyPrimitive2d::operator=(const HyPrimitive2d &rhs)
{
	ClearVertexData();
	IHyDrawable2d::operator=(rhs);

	m_bWireframe = rhs.m_bWireframe;
	m_fLineThickness = rhs.m_fLineThickness;
	m_uiNumSegments = rhs.m_uiNumSegments;
	m_eRenderMode = rhs.m_eRenderMode;
	AssembleData();

	return *this;
}

uint32 HyPrimitive2d::GetNumVerts() const
{
	return m_uiNumVerts;
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
	return m_pVertBuffer != nullptr && shape.IsValidShape();
}

/*virtual*/ void HyPrimitive2d::SetDirty(uint32 uiDirtyFlags) /*override*/
{
	IHyDrawable2d::SetDirty(uiDirtyFlags);

	if(uiDirtyFlags & DIRTY_Transform | DIRTY_Color)
		m_bUpdateShaderUniforms = true;
}

/*virtual*/ void HyPrimitive2d::OnShapeChanged() /*override*/
{
	AssembleData();
}

/*virtual*/ bool HyPrimitive2d::OnIsValidToRender() /*override*/
{
	return m_pVertBuffer != nullptr && shape.IsValidShape();
}

/*virtual*/ void HyPrimitive2d::OnLoadedUpdate() /*override*/
{
	if(m_bUpdateShaderUniforms)
	{
		glm::mat4 mtx = GetSceneTransform();

		// TODO: Get rid of top/bot color
		glm::vec3 tint = CalculateTopTint();
		glm::vec4 vTop;
		vTop.x = tint.x;
		vTop.y = tint.y;
		vTop.z = tint.z;
		vTop.a = CalculateAlpha();

		m_ShaderUniforms.Set("u_mtxTransform", mtx);
		m_ShaderUniforms.Set("u_vColor", vTop);

		m_bUpdateShaderUniforms = false;
	}
}

/*virtual*/ void HyPrimitive2d::OnCalcBoundingVolume() /*override*/
{
	m_LocalBoundingVolume = shape;
}

/*virtual*/ void HyPrimitive2d::PrepRenderStage(uint32 uiStageIndex, HyRenderMode &eRenderModeOut, uint32 &uiNumInstancesOut, uint32 &uiNumVerticesPerInstOut, bool &bIsBatchable) /*override*/
{
	eRenderModeOut = m_eRenderMode;
	uiNumInstancesOut = 1;
	uiNumVerticesPerInstOut = GetNumVerts();
	bIsBatchable = false;
}

/*virtual*/ bool HyPrimitive2d::WriteVertexData(uint32 uiNumInstances, HyVertexBuffer &vertexBufferRef) /*override*/
{
	vertexBufferRef.AppendData2d(m_pVertBuffer, m_uiNumVerts * sizeof(glm::vec2));
	return true;
}

/*virtual*/ void HyPrimitive2d::Load() /*override*/
{
	IHyLoadable::Load();
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
	const b2Shape *pb2Shape = shape.GetB2Shape();

	switch(shape.GetType())
	{
	case HYSHAPE_Unknown:	// Shape hasn't been set yet by user
		break;

	case HYSHAPE_LineSegment: {
		std::vector<b2Vec2> pointList;
		pointList.push_back(static_cast<const b2EdgeShape *>(pb2Shape)->m_vertex1);
		pointList.push_back(static_cast<const b2EdgeShape *>(pb2Shape)->m_vertex2);
		_SetAsLineChain(pointList.data(), 2);
		break; }

	case HYSHAPE_LineLoop:
	case HYSHAPE_LineChain: {
		_SetAsLineChain(static_cast<const b2ChainShape *>(pb2Shape)->m_vertices, static_cast<const b2ChainShape *>(pb2Shape)->m_count);
		break; }

	case HYSHAPE_Circle:
		_SetAsCircle(glm::vec2(static_cast<const b2CircleShape *>(pb2Shape)->m_p.x, static_cast<const b2CircleShape *>(pb2Shape)->m_p.y), pb2Shape->m_radius, m_uiNumSegments);
		break;

	case HYSHAPE_Polygon: {
		if(m_bWireframe)
		{
			int32 iNumVerts = static_cast<const b2PolygonShape *>(pb2Shape)->m_count;
			HyAssert(iNumVerts >= 3, "HyPrimitive error, not enough verts for HYSHAPE_Polygon");

			std::vector<b2Vec2> vertList;
			for(int32 i = 0; i < iNumVerts; ++i)
				vertList.push_back(static_cast<const b2PolygonShape *>(pb2Shape)->m_vertices[i]);

			// Make it loop
			vertList.push_back(static_cast<const b2PolygonShape *>(pb2Shape)->m_vertices[0]);

			_SetAsLineChain(vertList.data(), iNumVerts + 1);
		}
		else
			_SetAsPolygon(static_cast<const b2PolygonShape *>(pb2Shape)->m_vertices, static_cast<const b2PolygonShape *>(pb2Shape)->m_count);
		break; }

	default:
		HyLogError("HyPrimitive2d::AssembleData() - Unknown shape type: " << shape.GetType());
	}

	SetDirty(DIRTY_BoundingVolume);
	Load();
}

void HyPrimitive2d::_SetAsLineChain(b2Vec2 *pVertexList, uint32 uiNumVertices)
{
	HyAssert(uiNumVertices > 1, "HyPrimitive2d::SetAsLineChain was passed an empty vertexList or a vertexList of only '1' vertex");
	ClearVertexData();

	m_eRenderMode = HYRENDERMODE_Triangles;

	m_uiNumVerts = (uiNumVertices - 1) * 6;
	m_pVertBuffer = HY_NEW glm::vec2[m_uiNumVerts];

	uint32 uiBufferIndex = 0;

	std::vector<glm::vec2> vertList;
	for(uint32 i = 0; i < uiNumVertices - 1; ++i)
	{
		b2Vec2 vDir = pVertexList[i + 1] - pVertexList[i];
		vDir.Normalize();

		b2Vec2 ptExtents[4];
		
		ptExtents[0].Set(vDir.y, -vDir.x);
		ptExtents[0] *= (m_fLineThickness * 0.5f);
		ptExtents[0] += pVertexList[i];

		ptExtents[1].Set(-vDir.y, vDir.x);
		ptExtents[1] *= (m_fLineThickness * 0.5f);
		ptExtents[1] += pVertexList[i];

		ptExtents[2].Set(-vDir.y, vDir.x);
		ptExtents[2] *= (m_fLineThickness * 0.5f);
		ptExtents[2] += pVertexList[i + 1];

		ptExtents[3].Set(vDir.y, -vDir.x);
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

void HyPrimitive2d::_SetAsCircle(glm::vec2 ptCenter, float fRadius, uint32 uiSegments)
{
	const float k_segments = static_cast<float>(uiSegments);
	const float k_increment = 2.0f * b2_pi / k_segments;
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
		vertexList.push_back(b2Vec2(v1.x, v1.y));
		
		for(int32 i = 0; i < k_segments; ++i)
		{
			glm::vec2 r2;
			r2.x = cosInc * r1.x - sinInc * r1.y;
			r2.y = sinInc * r1.x + cosInc * r1.y;
			glm::vec2 v2 = ptCenter + fRadius * r2;

			vertexList.push_back(b2Vec2(v2.x, v2.y));

			//m_lines->Vertex(v1, color);
			//m_lines->Vertex(v2, color);
			r1 = r2;
			//v1 = v2;
		}

		_SetAsLineChain(&vertexList[0], static_cast<uint32>(vertexList.size()));
	}
}

void HyPrimitive2d::_SetAsPolygon(const b2Vec2 *pVertexList, uint32 uiNumVertices)
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
