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
	m_bUpdateShaderUniforms(true)
{
}

HyPrimitive2d::HyPrimitive2d(const HyPrimitive2d &copyRef) :
	IHyDrawable2d(copyRef),
	m_bUpdateShaderUniforms(true)
{
	operator=(copyRef);
}

HyPrimitive2d::~HyPrimitive2d(void)
{
	ClearAllData();
}

const HyPrimitive2d &HyPrimitive2d::operator=(const HyPrimitive2d &rhs)
{
	IHyDrawable2d::operator=(rhs);

	ClearAllData();

	for(int i = 0; i < rhs.m_LayerList.size(); ++i)
		m_LayerList.emplace_back(rhs.m_LayerList[i]);

	return *this;
}

/*virtual*/ void HyPrimitive2d::CalcLocalBoundingShape(HyShape2d &shapeOut) /*override*/
{
	// Make b2AABB by combining each layer
	b2AABB aabb = { {0,0}, {0,0} };
	for(int i = 0; i < m_LayerList.size(); ++i)
	{
		const Layer &layerRef = m_LayerList[i];
		if(layerRef.m_uiNumVerts > 0)
		{
			std::vector<b2Vec2> pointList;
			for(int i = 0; i < layerRef.m_uiNumVerts; ++i)
				pointList.push_back({ layerRef.m_pVertBuffer[i].x, layerRef.m_pVertBuffer[i].y });
			aabb = b2AABB_Union(aabb, b2MakeAABB(pointList.data(), pointList.size(), 0.0f));
		}
	}
	if(b2IsValidAABB(aabb) == false)
	{
		shapeOut.SetAsNothing();
		return;
	}

	b2Vec2 vExtents = b2AABB_Extents(aabb);
	b2Vec2 ptCenter = b2AABB_Center(aabb);
	shapeOut.SetAsBox(HyRect(vExtents.x, vExtents.y, glm::vec2(ptCenter.x, ptCenter.y), 0.0f));
}

/*virtual*/ float HyPrimitive2d::GetWidth(float fPercent /*= 1.0f*/) /*override*/
{
	b2AABB aabb = GetSceneAABB();
	if(b2IsValidAABB(aabb))
		return (b2AABB_Extents(aabb).x * 2.0f) * fPercent;

	return 0.0f;
}

/*virtual*/ float HyPrimitive2d::GetHeight(float fPercent /*= 1.0f*/) /*override*/
{
	b2AABB aabb = GetSceneAABB();
	if(b2IsValidAABB(aabb))
		return (b2AABB_Extents(aabb).y * 2.0f) * fPercent;

	return 0.0f;
}

void HyPrimitive2d::GetCentroid(glm::vec2 &ptCentroidOut)
{
	b2AABB aabb = GetSceneAABB();
	if(b2IsValidAABB(aabb))
	{
		b2Vec2 ptCenter = b2AABB_Center(aabb);
		ptCentroidOut.x = ptCenter.x;
		ptCentroidOut.y = ptCenter.y;
	}
	else
		HySetVec(ptCentroidOut, 0.0f, 0.0f);
}

int32 HyPrimitive2d::GetNumLayers() const
{
	return static_cast<int32>(m_LayerList.size());
}

HyFixtureType HyPrimitive2d::GetLayerType(int32 iLayerIndex) const
{
	if(iLayerIndex < 0 || iLayerIndex >= m_LayerList.size())
	{
		HyLogError("HyPrimitive2d::GetShapeType() failed - Invalid layer index");
		return HYFIXTURE_Nothing;
	}

	return m_LayerList[iLayerIndex].m_eFixtureType;
}

int32 HyPrimitive2d::SetAsNothing(int32 iLayerIndex)
{
	if(iLayerIndex < 0 || iLayerIndex >= m_LayerList.size())
	{
		iLayerIndex = static_cast<int32>(m_LayerList.size());	// If index is out of bounds, add a new layer at the end
		m_LayerList.emplace_back();
	}

	DeleteLayerData(iLayerIndex);
	return iLayerIndex;
}

int32 HyPrimitive2d::SetAsLineSegment(int32 iLayerIndex, const glm::vec2 &pt1, const glm::vec2 &pt2, float fLineThickness)
{
	if(iLayerIndex < 0 || iLayerIndex >= m_LayerList.size())
	{
		iLayerIndex = static_cast<int32>(m_LayerList.size());	// If index is out of bounds, add a new layer at the end
		m_LayerList.emplace_back();
	}

	m_LayerList[iLayerIndex].m_fLineThickness = fLineThickness;

	std::vector<glm::vec2> vertList;
	vertList.push_back(pt1);
	vertList.push_back(pt2);
	AssembleLineChain(iLayerIndex, vertList.data(), vertList.size(), false);
	return iLayerIndex;
}

int32 HyPrimitive2d::SetAsLineChain(int32 iLayerIndex, const glm::vec2 *pVertices, uint32 uiNumVerts, bool bLoop, float fLineThickness)
{
	if(uiNumVerts < 2)
	{
		HyLogError("HyPrimitive2d::SetAsLineChain() failed - Line chains must be initialized with at least 2 vertices");
		return -1;
	}
	if(bLoop && uiNumVerts < 3)
	{
		HyLogError("HyPrimitive2d::SetAsLineChain() failed - Looping line chains must be initialized with at least 3 vertices");
		return -1;
	}

	if(iLayerIndex < 0 || iLayerIndex >= m_LayerList.size())
	{
		iLayerIndex = static_cast<int32>(m_LayerList.size());	// If index is out of bounds, add a new layer at the end
		m_LayerList.emplace_back();
	}

	m_LayerList[iLayerIndex].m_fLineThickness = fLineThickness;

	if(bLoop && pVertices[0] == pVertices[uiNumVerts - 1])
		uiNumVerts--; // AssembleLineChain will make this connection as bLoop is true

	AssembleLineChain(iLayerIndex, pVertices, uiNumVerts, bLoop);
	return iLayerIndex;
}

int32 HyPrimitive2d::SetAsLineChain(int32 iLayerIndex, const std::vector<glm::vec2> &verticesList, bool bLoop, float fLineThickness)
{
	return SetAsLineChain(iLayerIndex, verticesList.data(), verticesList.size(), bLoop, fLineThickness);
}

int32 HyPrimitive2d::SetAsLineChain(int32 iLayerIndex, const HyChainData &chainData, float fLineThickness)
{
	return SetAsLineChain(iLayerIndex, chainData.pPointList, chainData.iCount, chainData.bLoop, fLineThickness);
}

int32 HyPrimitive2d::SetAsShape(int32 iLayerIndex, const HyShape2d &shapeRef, float fOutlineThickness /*= 0.0f*/)
{
	if(iLayerIndex < 0 || iLayerIndex >= m_LayerList.size())
	{
		iLayerIndex = static_cast<int32>(m_LayerList.size());	// If index is out of bounds, add a new layer at the end
		m_LayerList.emplace_back();
	}

	Layer &layerRef = m_LayerList[iLayerIndex];
	layerRef.m_fLineThickness = fOutlineThickness;

	switch(shapeRef.GetType())
	{
	case HYFIXTURE_Nothing:
		DeleteLayerData(iLayerIndex);
		break;

	case HYFIXTURE_Circle:
		AssembleCircle(iLayerIndex, glm::vec2(shapeRef.GetAsCircle().center.x, shapeRef.GetAsCircle().center.y), shapeRef.GetAsCircle().radius, layerRef.m_uiNumSegments);
		break;

	case HYFIXTURE_LineSegment: {
		std::vector<glm::vec2> pointList;
		pointList.emplace_back(shapeRef.GetAsSegment().point1.x, shapeRef.GetAsSegment().point1.y);
		pointList.emplace_back(shapeRef.GetAsSegment().point2.x, shapeRef.GetAsSegment().point2.y);
		AssembleLineChain(iLayerIndex, pointList.data(), 2, false);
		break; }

	case HYFIXTURE_Polygon: {
		std::vector<glm::vec2> vertList;
		for(int32 i = 0; i < shapeRef.GetAsPolygon().count; ++i)
			vertList.emplace_back(shapeRef.GetAsPolygon().vertices[i].x, shapeRef.GetAsPolygon().vertices[i].y);
		AssemblePolygon(iLayerIndex, vertList.data(), vertList.size());
		break; }

	case HYFIXTURE_Capsule:
		AssembleCapsule(iLayerIndex,
						glm::vec2(shapeRef.GetAsCapsule().center1.x, shapeRef.GetAsCapsule().center1.y),
						glm::vec2(shapeRef.GetAsCapsule().center2.x, shapeRef.GetAsCapsule().center2.y),
						shapeRef.GetAsCapsule().radius,
						layerRef.m_uiNumSegments);
		break;

	default:
		HyLogError("HyPrimitive2d::AssembleData() - Unknown shape type: " << shapeRef.GetType());
		break;
	}

	return iLayerIndex;
}

int32 HyPrimitive2d::SetAsCircle(int32 iLayerIndex, float fRadius, float fOutlineThickness /*= 0.0f*/)
{
	return SetAsCircle(iLayerIndex, glm::vec2(0.0f, 0.0f), fRadius, fOutlineThickness);
}

int32 HyPrimitive2d::SetAsCircle(int32 iLayerIndex, const glm::vec2 &ptCenter, float fRadius, float fOutlineThickness /*= 0.0f*/)
{
	if(iLayerIndex < 0 || iLayerIndex >= m_LayerList.size())
	{
		iLayerIndex = static_cast<int32>(m_LayerList.size());	// If index is out of bounds, add a new layer at the end
		m_LayerList.emplace_back();
	}

	m_LayerList[iLayerIndex].m_fLineThickness = fOutlineThickness;

	AssembleCircle(iLayerIndex, ptCenter, fRadius, m_LayerList[iLayerIndex].m_uiNumSegments);
	return iLayerIndex;
}

int32 HyPrimitive2d::SetAsPolygon(int32 iLayerIndex, const glm::vec2 *pVertexArray, uint32 uiCount, float fOutlineThickness /*= 0.0f*/)
{
	if(iLayerIndex < 0 || iLayerIndex >= m_LayerList.size())
	{
		iLayerIndex = static_cast<int32>(m_LayerList.size());	// If index is out of bounds, add a new layer at the end
		m_LayerList.emplace_back();
	}

	m_LayerList[iLayerIndex].m_fLineThickness = fOutlineThickness;

	AssemblePolygon(iLayerIndex, pVertexArray, uiCount);
	return iLayerIndex;
}

int32 HyPrimitive2d::SetAsPolygon(int32 iLayerIndex, const std::vector<glm::vec2> &verticesList, float fOutlineThickness /*= 0.0f*/)
{
	return SetAsPolygon(iLayerIndex, verticesList.data(), verticesList.size(), fOutlineThickness);
}

int32 HyPrimitive2d::SetAsBox(int32 iLayerIndex, float fWidth, float fHeight, float fOutlineThickness /*= 0.0f*/)
{
	std::vector<glm::vec2> verticesList;
	verticesList.emplace_back(-fWidth * 0.5f, -fHeight * 0.5f);
	verticesList.emplace_back(-fWidth * 0.5f, fHeight * 0.5f);
	verticesList.emplace_back(fWidth * 0.5f, fHeight * 0.5f);
	verticesList.emplace_back(fWidth * 0.5f, -fHeight * 0.5f);

	return SetAsPolygon(iLayerIndex, verticesList.data(), verticesList.size(), fOutlineThickness);
}

int32 HyPrimitive2d::SetAsBox(int32 iLayerIndex, const HyRect &rect, float fOutlineThickness /*= 0.0f*/)
{
	glm::vec2 ptCenter = rect.GetCenter();
	b2Polygon boxPoly = b2MakeOffsetBox(rect.GetWidth(0.5f), rect.GetHeight(0.5f), {ptCenter.x, ptCenter.y}, b2MakeRot(glm::radians(rect.GetRotation())));

	std::vector<glm::vec2> verticesList;
	verticesList.emplace_back(boxPoly.vertices[0].x, boxPoly.vertices[0].y);
	verticesList.emplace_back(boxPoly.vertices[1].x, boxPoly.vertices[1].y);
	verticesList.emplace_back(boxPoly.vertices[2].x, boxPoly.vertices[2].y);
	verticesList.emplace_back(boxPoly.vertices[3].x, boxPoly.vertices[3].y);

	return SetAsPolygon(iLayerIndex, verticesList.data(), verticesList.size(), fOutlineThickness);
}

int32 HyPrimitive2d::SetAsCapsule(int32 iLayerIndex, const glm::vec2 &pt1, const glm::vec2 &pt2, float fRadius, float fOutlineThickness /*= 0.0f*/)
{
	// Make a box over the line segment pt1 and pt2 (with a width extent of fRadius), then add semicircle endcaps on both ends
	glm::vec2 vDir = pt2 - pt1;
	glm::vec2 ptCenter = vDir * 0.5f;

	glm::vec2 vRight = glm::normalize(HyMath::PerpendicularClockwise(vDir)) * fRadius;

	b2Polygon boxPoly = b2MakeOffsetBox(fRadius, glm::length(pt2 - pt1) * 0.5f, {ptCenter.x, ptCenter.y}, b2MakeRot(atan2f(vDir.y, vDir.x)));
	std::vector<glm::vec2> verticesList;
	verticesList.emplace_back(boxPoly.vertices[0].x, boxPoly.vertices[0].y);
	verticesList.emplace_back(boxPoly.vertices[1].x, boxPoly.vertices[1].y);
	verticesList.emplace_back(boxPoly.vertices[2].x, boxPoly.vertices[2].y);
	verticesList.emplace_back(boxPoly.vertices[3].x, boxPoly.vertices[3].y);

	// Add semicircle endcaps on both ends
	verticesList.emplace_back(pt2 + vRight);
	verticesList.emplace_back(pt2 - vRight);


	return SetAsPolygon(iLayerIndex, verticesList.data(), verticesList.size(), fOutlineThickness);
}

uint32 HyPrimitive2d::GetNumVerts(int32 iLayerIndex) const
{
	return m_LayerList[iLayerIndex].m_uiNumVerts;
}

const glm::vec2 *HyPrimitive2d::GetVerts(int32 iLayerIndex) const
{
	return m_LayerList[iLayerIndex].m_pVertBuffer;
}

bool HyPrimitive2d::IsOutline(int32 iLayerIndex)
{
	return m_LayerList[iLayerIndex].m_fLineThickness > 0.0f;
}

float HyPrimitive2d::GetLineThickness(int32 iLayerIndex) const
{
	return m_LayerList[iLayerIndex].m_fLineThickness;
}

/*virtual*/ bool HyPrimitive2d::IsLoadDataValid() /*override*/
{
	return true;
}

/*virtual*/ void HyPrimitive2d::SetDirty(uint32 uiDirtyFlags) /*override*/
{
	IHyDrawable2d::SetDirty(uiDirtyFlags);

	if(uiDirtyFlags & DIRTY_Transform | DIRTY_Color)
		m_bUpdateShaderUniforms = true;
}

/*virtual*/ bool HyPrimitive2d::OnIsValidToRender() /*override*/
{
	for(int i = 0; i < m_LayerList.size(); ++i)
	{
		const Layer &layerRef = m_LayerList[i];
		if(layerRef.m_uiNumVerts > 0 && layerRef.m_pVertBuffer != nullptr)
			return true;
	}

	return false;
}

/*virtual*/ void HyPrimitive2d::OnUpdateUniforms(float fExtrapolatePercent) /*override*/
{
	// TODO: get rid of this check and improve m_ShaderUniforms
	if(m_bUpdateShaderUniforms)
	{
		glm::mat4 mtx = GetSceneTransform(fExtrapolatePercent);

		m_ShaderUniforms.Set("u_mtxTransform", mtx);
		//m_ShaderUniforms.Set("u_vColor", vTop);

		m_bUpdateShaderUniforms = false;
	}
}

/*virtual*/ void HyPrimitive2d::PrepRenderStage(uint32 uiStageIndex, HyRenderMode &eRenderModeOut, HyBlendMode &eBlendModeOut, uint32 &uiNumInstancesOut, uint32 &uiNumVerticesPerInstOut, bool &bIsBatchable) /*override*/
{
	eRenderModeOut = HYRENDERMODE_Triangles;
	eBlendModeOut = HYBLENDMODE_Normal;

	// TODO: Do stages with circle batching (and its own shader)
	uiNumVerticesPerInstOut = 3;
	uiNumInstancesOut = 0;
	for(int iLayerIndex = 0; iLayerIndex < m_LayerList.size(); ++iLayerIndex)
	{
		const Layer &layerRef = m_LayerList[iLayerIndex];
		if(layerRef.m_uiNumVerts > 0 && layerRef.m_pVertBuffer != nullptr)
			uiNumInstancesOut += layerRef.m_uiNumVerts / uiNumVerticesPerInstOut;
	}

	bIsBatchable = true;
}

/*virtual*/ bool HyPrimitive2d::WriteVertexData(uint32 uiNumInstances, HyVertexBuffer &vertexBufferRef, float fExtrapolatePercent) /*override*/
{
	// TODO: Get rid of top/bot color
	glm::vec3 vTop = CalculateTopTint(fExtrapolatePercent);
	//glm::vec3 vBot = CalculateBotTint(fExtrapolatePercent);
	glm::vec4 vTopColor;
	vTopColor.x = vTop.x;
	vTopColor.y = vTop.y;
	vTopColor.z = vTop.z;
	vTopColor.a = CalculateAlpha(fExtrapolatePercent);
	//glm::vec4 vBotColor;
	//vBotColor.x = vBot.x;
	//vBotColor.y = vBot.y;
	//vBotColor.z = vBot.z;
	//vBotColor.a = vTopColor.a;

	for(int iLayerIndex = 0; iLayerIndex < m_LayerList.size(); ++iLayerIndex)
	{
		Layer &layerRef = m_LayerList[iLayerIndex];

		vTopColor.x *= layerRef.m_Color.GetRedF();
		vTopColor.y *= layerRef.m_Color.GetGreenF();
		vTopColor.z *= layerRef.m_Color.GetBlueF();
		for(int iVertIndex = 0; iVertIndex < layerRef.m_uiNumVerts; ++iVertIndex)
		{
			vertexBufferRef.AppendVertexData(&layerRef.m_pVertBuffer[iVertIndex], sizeof(glm::vec2));
			vertexBufferRef.AppendVertexData(&vTopColor, sizeof(glm::vec4)); // TODO: Cache bot most and top most vertices and use that to mix color between vTopColor and vBotColor for a vertical gradient effect
		}
	}

	return true;
}

void HyPrimitive2d::ClearAllData()
{
	for(int i = 0; i < m_LayerList.size(); ++i)
		DeleteLayerData(i);
		
	m_LayerList.clear();
	m_bUpdateShaderUniforms = true;
	m_ShaderUniforms.Clear();
}

void HyPrimitive2d::DeleteLayerData(int32 iLayerIndex)
{
	if(iLayerIndex < 0 || iLayerIndex >= m_LayerList.size())
	{
		HyLogWarning("HyPrimitive2d::ClearVertexData() - Invalid layer index: " << iLayerIndex);
		return;
	}
	Layer &layerRef = m_LayerList[iLayerIndex];

	delete[] layerRef.m_pVertBuffer;
	layerRef.m_pVertBuffer = nullptr;
	layerRef.m_uiNumVerts = 0;
	layerRef.m_eFixtureType = HYFIXTURE_Nothing;

	m_bUpdateShaderUniforms = true;
	SetDirty(DIRTY_SceneAABB);
}

void HyPrimitive2d::AssembleLineChain(int32 iLayerIndex, const glm::vec2 *pVertexList, uint32 uiNumVertices, bool bLoop)
{
	HyAssert(uiNumVertices > 1, "HyPrimitive2d::SetAsLineChain was passed an empty vertexList or a vertexList of only '1' vertex");
	HyAssert(bLoop && uiNumVertices > 2, "HyPrimitive2d::SetAsLineChain - Looping line chains must have at least 3 vertices");
	
	DeleteLayerData(iLayerIndex);

	Layer &layerRef = m_LayerList[iLayerIndex];

	if(bLoop)
		layerRef.m_uiNumVerts = uiNumVertices * 6;
	else
		layerRef.m_uiNumVerts = (uiNumVertices - 1) * 6;

	layerRef.m_pVertBuffer = HY_NEW glm::vec2[layerRef.m_uiNumVerts];

	uint32 uiBufferIndex = 0;

	std::vector<glm::vec2> vertList;
	for(uint32 i = 0; i < uiNumVertices - 1; ++i)
	{
		glm::vec2 vDir = glm::normalize(pVertexList[i + 1] - pVertexList[i]);
		glm::vec2 ptExtents[4];
		
		ptExtents[0] = { vDir.y, -vDir.x };
		ptExtents[0] *= (layerRef.m_fLineThickness * 0.5f);
		ptExtents[0] += pVertexList[i];

		ptExtents[1] = { -vDir.y, vDir.x };
		ptExtents[1] *= (layerRef.m_fLineThickness * 0.5f);
		ptExtents[1] += pVertexList[i];

		ptExtents[2] = { -vDir.y, vDir.x };
		ptExtents[2] *= (layerRef.m_fLineThickness * 0.5f);
		ptExtents[2] += pVertexList[i + 1];

		ptExtents[3] = { vDir.y, -vDir.x };
		ptExtents[3] *= (layerRef.m_fLineThickness * 0.5f);
		ptExtents[3] += pVertexList[i + 1];

		for(uint32 i = 1; i < 3; ++i)
		{
			layerRef.m_pVertBuffer[uiBufferIndex].x = ptExtents[0].x;
			layerRef.m_pVertBuffer[uiBufferIndex].y = ptExtents[0].y;
			uiBufferIndex++;

			layerRef.m_pVertBuffer[uiBufferIndex].x = ptExtents[i].x;
			layerRef.m_pVertBuffer[uiBufferIndex].y = ptExtents[i].y;
			uiBufferIndex++;

			layerRef.m_pVertBuffer[uiBufferIndex].x = ptExtents[i + 1].x;
			layerRef.m_pVertBuffer[uiBufferIndex].y = ptExtents[i + 1].y;
			uiBufferIndex++;
		}
	}
}

void HyPrimitive2d::AssembleCircle(int32 iLayerIndex, glm::vec2 ptCenter, float fRadius, uint32 uiSegments)
{
	DeleteLayerData(iLayerIndex);

	const float k_segments = static_cast<float>(uiSegments);
	const float k_increment = 2.0f * glm::pi<float>() / k_segments;
	float sinInc = sinf(k_increment);
	float cosInc = cosf(k_increment);
	glm::vec2 r1(cosInc, sinInc);
	glm::vec2 v1 = ptCenter + fRadius * r1;

	Layer &layerRef = m_LayerList[iLayerIndex];

	if(layerRef.m_fLineThickness <= 0.0f) // Solid Circle
	{
		layerRef.m_uiNumVerts = static_cast<uint32>(k_segments) * 3;
		layerRef.m_pVertBuffer = HY_NEW glm::vec2[layerRef.m_uiNumVerts];

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

			layerRef.m_pVertBuffer[uiBufferIndex++] = ptCenter;
			layerRef.m_pVertBuffer[uiBufferIndex++] = v1;
			layerRef.m_pVertBuffer[uiBufferIndex++] = v2;

			r1 = r2;
			v1 = v2;
		}
	}
	else // Circle Outline
	{
		r1.x = 1.0f;
		r1.y = 0.0f;
		v1 = ptCenter + fRadius * r1;

		std::vector<glm::vec2> vertexList;
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

		AssembleLineChain(iLayerIndex, &vertexList[0], static_cast<uint32>(vertexList.size()), true);
	}
}

void HyPrimitive2d::AssemblePolygon(int32 iLayerIndex, const glm::vec2 *pVertexList, uint32 uiNumVertices)
{
	DeleteLayerData(iLayerIndex);

	Layer &layerRef = m_LayerList[iLayerIndex];

	if(layerRef.m_fLineThickness <= 0.0f)
	{
		std::vector<glm::vec2> vertList(pVertexList, pVertexList + uiNumVertices);
		std::vector<HyTriangle2d> triangleList = HyMath::Triangulate(vertList);

		layerRef.m_uiNumVerts = static_cast<uint32>(triangleList.size()) * 3;
		layerRef.m_pVertBuffer = HY_NEW glm::vec2[layerRef.m_uiNumVerts];

		uint32 uiBufferIndex = 0;
		for(int32 i = 0; i < triangleList.size(); ++i)
		{
			layerRef.m_pVertBuffer[uiBufferIndex++] = triangleList[i].m_ptA;
			layerRef.m_pVertBuffer[uiBufferIndex++] = triangleList[i].m_ptB;
			layerRef.m_pVertBuffer[uiBufferIndex++] = triangleList[i].m_ptC;
		}
	}
	else // Polygon Outline
		AssembleLineChain(iLayerIndex, pVertexList, uiNumVertices, true);
}

void HyPrimitive2d::AssembleCapsule(int32 iLayerIndex, const glm::vec2 &ptCenter1, const glm::vec2 &ptCenter2, float fRadius, uint32 uiSegments)
{
	DeleteLayerData(iLayerIndex);

	Layer &layerRef = m_LayerList[iLayerIndex];

	if(layerRef.m_fLineThickness <= 0.0f) // Solid Capsule
	{
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
		layerRef.m_uiNumVerts = static_cast<uint32_t>(verts.size());
		layerRef.m_pVertBuffer = HY_NEW glm::vec2[layerRef.m_uiNumVerts];
		std::copy(verts.begin(), verts.end(), layerRef.m_pVertBuffer);
	}
	else // Capsule Outline
	{
		const float k_segments = static_cast<float>(uiSegments);
		const float k_increment = 2.0f * glm::pi<float>() / k_segments;
		float sinInc = sinf(k_increment);
		float cosInc = cosf(k_increment);
		glm::vec2 r1(cosInc, sinInc);
		glm::vec2 v1 = ptCenter1 + fRadius * r1;

		r1.x = 1.0f;
		r1.y = 0.0f;
		v1 = ptCenter1 + fRadius * r1;

		std::vector<glm::vec2> vertexList;
		vertexList.push_back({ v1.x, v1.y });
		
		for(int32 i = 0; i < k_segments / 2; ++i)
		{
			glm::vec2 r2;
			r2.x = cosInc * r1.x - sinInc * r1.y;
			r2.y = sinInc * r1.x + cosInc * r1.y;
			glm::vec2 v2 = ptCenter1 + fRadius * r2;

			vertexList.push_back({ v2.x, v2.y });

			//m_lines->Vertex(v1, color);
			//m_lines->Vertex(v2, color);
			r1 = r2;
			//v1 = v2;
		}





		//HyBuildCapsuleVerts(vertList, glm::vec2(shapeRef.GetAsCapsule().center1.x, shapeRef.GetAsCapsule().center1.y), glm::vec2(shapeRef.GetAsCapsule().center2.x, shapeRef.GetAsCapsule().center2.y), shapeRef.GetAsCapsule().radius, layerRef.m_uiNumSegments);
		//AssembleLineChain(iLayerIndex, vertList.data(), vertList.size(), true);
	}
}
