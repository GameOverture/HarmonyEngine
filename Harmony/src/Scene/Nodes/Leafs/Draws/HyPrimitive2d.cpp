/**************************************************************************
 *	HyPrimitive2d.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Scene/Nodes/Leafs/Draws/HyPrimitive2d.h"
#include "Scene/Nodes/Entities/HyEntity2d.h"
#include "HyEngine.h"
#include "Utilities/HyMath.h"

HyPrimitive2d::HyPrimitive2d(HyEntity2d *pParent /*= nullptr*/) :	IHyLeafDraw2d(HYTYPE_Primitive2d, nullptr, nullptr, pParent),
																	m_pDrawBuffer(nullptr),
																	m_uiBufferSize(0),
																	m_bWireframe(false)
{
	ClearData();
}

HyPrimitive2d::~HyPrimitive2d(void)
{
	ClearData();
}

const HyPrimitive2d &HyPrimitive2d::operator=(const HyPrimitive2d& p)
{
	m_RenderState = p.m_RenderState;
	m_eCoordUnit = p.m_eCoordUnit;
	m_BoundingVolume = p.m_BoundingVolume;

	ClearData();
	if(m_RenderState.GetNumVerticesPerInstance() != 0)
	{
		m_pDrawBuffer = HY_NEW glm::vec2[p.m_uiBufferSize / sizeof(glm::vec2)];
		m_uiBufferSize = p.m_uiBufferSize;
		memcpy(m_pDrawBuffer, p.m_pDrawBuffer, m_uiBufferSize);
	}
	
	return *this;
}

/*virtual*/ bool HyPrimitive2d::IsEnabled() /*override*/
{
	return (IHyNode::IsEnabled() && m_pDrawBuffer != nullptr);
}

HyShape2d &HyPrimitive2d::GetShape()
{
	// The bounding volume in HyPrimitive2d also doubles as the actual shape and type of this primitive
	return m_BoundingVolume;
}

float HyPrimitive2d::GetLineThickness()
{
	return m_RenderState.GetLineThickness();
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
	m_bDirty = true;
}

void HyPrimitive2d::SetLineThickness(float fThickness)
{
	m_RenderState.SetLineThickness(fThickness);
}

/*virtual*/ void HyPrimitive2d::CalcBoundingVolume() /*override*/
{
	// Do nothing as m_BoundingVolume is always up to date as it is the actual shape and type of this primitive
}

/*virtual*/ void HyPrimitive2d::AcquireBoundingVolumeIndex(uint32 &uiStateOut, uint32 &uiSubStateOut) /*override*/
{
	uiStateOut = uiSubStateOut = 0;
}

/*virtual*/ void HyPrimitive2d::OnShapeSet(HyShape2d *pShape) /*override*/
{
	IHyNode2d::OnShapeSet(pShape);

	if(pShape == &m_BoundingVolume)
		m_bDirty = true;
}

/*virtual*/ void HyPrimitive2d::DrawUpdate()
{
	if(m_bDirty == false)
		return;

	b2Shape *pb2Shape = m_BoundingVolume.GetB2Shape();

	switch(m_BoundingVolume.GetType())
	{
	case HYSHAPE_LineSegment: {
		std::vector<glm::vec2> pointList;
		pointList.push_back(glm::vec2(static_cast<b2EdgeShape *>(pb2Shape)->m_vertex1.x, static_cast<b2EdgeShape *>(pb2Shape)->m_vertex1.y));
		pointList.push_back(glm::vec2(static_cast<b2EdgeShape *>(pb2Shape)->m_vertex2.x, static_cast<b2EdgeShape *>(pb2Shape)->m_vertex2.y));
		SetAsLineChain(&pointList[0], 2);
	} break;

	case HYSHAPE_LineLoop:
	case HYSHAPE_LineChain: {
		glm::vec2 *pVertList = reinterpret_cast<glm::vec2 *>(static_cast<b2ChainShape *>(pb2Shape)->m_vertices);
		SetAsLineChain(pVertList, static_cast<b2ChainShape *>(pb2Shape)->m_count);
	} break;

	case HYSHAPE_Circle:
		SetAsCircle(glm::vec2(static_cast<b2CircleShape *>(pb2Shape)->m_p.x, static_cast<b2CircleShape *>(pb2Shape)->m_p.y), static_cast<b2CircleShape *>(pb2Shape)->m_radius);
		break;

	case HYSHAPE_Polygon: {
		glm::vec2 *pVertList = reinterpret_cast<glm::vec2 *>(static_cast<b2PolygonShape *>(pb2Shape)->m_vertices);

		if(m_bWireframe)
		{
			// Make it loop
			glm::vec2 vertLoopList[b2_maxPolygonVertices+1];
			memcpy(vertLoopList, static_cast<b2PolygonShape *>(pb2Shape)->m_vertices, sizeof(glm::vec2) * static_cast<b2PolygonShape *>(pb2Shape)->m_count);
			vertLoopList[static_cast<b2PolygonShape *>(pb2Shape)->m_count] = vertLoopList[0];

			SetAsLineChain(vertLoopList, static_cast<b2PolygonShape *>(pb2Shape)->m_count + 1);
		}
		else
			SetAsPolygon(pVertList, static_cast<b2PolygonShape *>(pb2Shape)->m_count);
	} break;

	default:
		HyLogError("HyPrimitive2d::OnShapeSet() - Unknown shape type: " << m_BoundingVolume.GetType());
	}

	m_bDirty = false;
}

void HyPrimitive2d::ClearData()
{
	delete [] m_pDrawBuffer;
	m_pDrawBuffer = nullptr;
	m_uiBufferSize = 0;
	m_RenderState.SetNumVerticesPerInstance(0);
	m_RenderState.SetNumInstances(1);
	m_RenderState.Disable(HyRenderState::DRAWMODEMASK);
	m_ShaderUniforms.Clear();
}

void HyPrimitive2d::SetAsLineChain(glm::vec2 *pVertexList, uint32 uiNumVertices)
{
	ClearData();

	HyAssert(uiNumVertices > 1, "HyPrimitive2d::SetAsLineChain was passed an empty vertexList or a vertexList of only '1' vertex");

	m_RenderState.SetShaderId(HYSHADERPROG_Lines2d);
	m_RenderState.Enable(HyRenderState::DRAWMODE_TRIANGLESTRIP);
	m_RenderState.SetNumInstances(uiNumVertices - 1);
	m_RenderState.SetNumVerticesPerInstance(4);				// 8 vertices per instance because of '2' duplicate vertex positions and normals on each end of line segment

	m_pDrawBuffer = HY_NEW glm::vec2[m_RenderState.GetNumInstances() * 8];	// size*4 = Each vertex of segment has '2' duplicate vertex positions that are offset by '2' corresponding normals within vertex shader 
	m_uiBufferSize = (m_RenderState.GetNumInstances() * 8) * sizeof(glm::vec2);

	if(m_eCoordUnit == HYCOORDUNIT_Default)
		m_eCoordUnit = HyDefaultCoordinateUnit();
	float fCoordMod = (m_eCoordUnit == HYCOORDUNIT_Meters) ? HyPixelsPerMeter() : 1.0f;

	uint32 i, j;
	for(i = j = 0; i < m_RenderState.GetNumInstances(); ++i, j += 8)
	{
		glm::vec2 vVecDirection = pVertexList[i + 1] - pVertexList[i + 0];

		/*postion 1    */ m_pDrawBuffer[j + 0] = pVertexList[i + 0] * fCoordMod;
		/*Normal  1    */ m_pDrawBuffer[j + 1] = glm::normalize(glm::vec2(-vVecDirection.y, vVecDirection.x));
		/*postion 1 dup*/ m_pDrawBuffer[j + 2] = m_pDrawBuffer[j + 0];
		/*Normal  1 inv*/ m_pDrawBuffer[j + 3] = m_pDrawBuffer[j + 1] * -1.0f;

		/*postion 2    */ m_pDrawBuffer[j + 4] = pVertexList[i + 1] * fCoordMod;
		/*Normal  2    */ m_pDrawBuffer[j + 5] = glm::normalize(glm::vec2(vVecDirection.y, -vVecDirection.x)) * -1.0f;
		/*postion 2 dup*/ m_pDrawBuffer[j + 6] = m_pDrawBuffer[j + 4];
		/*Normal  2 inv*/ m_pDrawBuffer[j + 7] = m_pDrawBuffer[j + 5] * -1.0f;
	}
}

void HyPrimitive2d::SetAsCircle(glm::vec2 &ptCenter, float fRadius)
{
	const float32 k_segments = 16.0f;
	const float32 k_increment = 2.0f * b2_pi / k_segments;
	float32 sinInc = sinf(k_increment);
	float32 cosInc = cosf(k_increment);
	glm::vec2 r1(cosInc, sinInc);
	glm::vec2 v1 = ptCenter + fRadius * r1;

	if(m_bWireframe == false)
	{
		ClearData();
		uint32 uiNumVerts = static_cast<uint32>(k_segments) * 3;

		m_RenderState.Enable(HyRenderState::DRAWMODE_TRIANGLES);
		m_RenderState.SetShaderId(HYSHADERPROG_Primitive);
		m_RenderState.SetNumVerticesPerInstance(uiNumVerts);

		m_pDrawBuffer = HY_NEW glm::vec2[uiNumVerts];
		m_uiBufferSize = uiNumVerts * sizeof(glm::vec2);

		uint32 uiBufferIndex = 0;
		for(int32 i = 0; i < k_segments; ++i)
		{
			// Perform rotation to avoid additional trigonometry.
			glm::vec2 r2;
			r2.x = cosInc * r1.x - sinInc * r1.y;
			r2.y = sinInc * r1.x + cosInc * r1.y;
			glm::vec2 v2 = ptCenter + fRadius * r2;

			m_pDrawBuffer[uiBufferIndex++] = ptCenter;
			m_pDrawBuffer[uiBufferIndex++] = v1;
			m_pDrawBuffer[uiBufferIndex++] = v2;

			r1 = r2;
			v1 = v2;
		}
	}
	else
	{
		r1.x = 1.0f;
		r1.y = 0.0f;
		v1 = ptCenter + fRadius * r1;

		std::vector<glm::vec2> vertexList;
		vertexList.push_back(v1);
		
		for(int32 i = 0; i < k_segments; ++i)
		{
			glm::vec2 r2;
			r2.x = cosInc * r1.x - sinInc * r1.y;
			r2.y = sinInc * r1.x + cosInc * r1.y;
			glm::vec2 v2 = ptCenter + fRadius * r2;

			vertexList.push_back(v2);

			//m_lines->Vertex(v1, color);
			//m_lines->Vertex(v2, color);
			r1 = r2;
			//v1 = v2;
		}

		SetAsLineChain(&vertexList[0], static_cast<uint32>(vertexList.size()));
	}
}

void HyPrimitive2d::SetAsPolygon(glm::vec2 *pVertexList, uint32 uiNumVertices)
{
	ClearData();

	m_RenderState.Enable(HyRenderState::DRAWMODE_TRIANGLES);
	m_RenderState.SetShaderId(HYSHADERPROG_Primitive);
	m_RenderState.SetNumVerticesPerInstance(uiNumVertices);

	uint32 uiNumBufferVerts = (uiNumVertices - 2) * 3;
	m_pDrawBuffer = HY_NEW glm::vec2[uiNumBufferVerts];
	m_uiBufferSize = uiNumBufferVerts * sizeof(glm::vec2);

	uint32 uiBufferIndex = 0;
	for(uint32 i = 1; i < uiNumVertices - 1; ++i)
	{
		m_pDrawBuffer[uiBufferIndex++] = pVertexList[0];
		m_pDrawBuffer[uiBufferIndex++] = pVertexList[i];
		m_pDrawBuffer[uiBufferIndex++] = pVertexList[i+1];
	}
}

/*virtual*/ void HyPrimitive2d::OnUpdateUniforms()
{
	glm::mat4 mtx;
	GetWorldTransform(mtx);

	// TODO: Get rid of top/bot color
	glm::vec3 tint = CalculateTopTint();
	glm::vec4 vTop;
	vTop.x = tint.x;
	vTop.y = tint.y;
	vTop.z = tint.z;
	vTop.a = CalculateAlpha();
	
	//glm::vec4 vBot;
	//vTop.x = botColor.X();
	//vTop.y = botColor.Y();
	//vTop.z = botColor.Z();
	//vBot.a = alpha.Get();

	m_ShaderUniforms.Set("u_mtxTransform", mtx);
	m_ShaderUniforms.Set("u_vColor", vTop);

	if(m_RenderState.GetShaderId() == HYSHADERPROG_Lines2d)
	{
		m_ShaderUniforms.Set("u_fHalfWidth", m_RenderState.GetLineThickness() * 0.5f);
		m_ShaderUniforms.Set("u_fFeatherAmt", 2.0f);	// Feather amount is how much anti-aliasing to apply. Greater values will make line fuzzier
	}
}

/*virtual*/ void HyPrimitive2d::OnWriteDrawBufferData(char *&pRefDataWritePos)
{
	memcpy(pRefDataWritePos, m_pDrawBuffer, m_uiBufferSize);
	pRefDataWritePos += m_uiBufferSize;
}
