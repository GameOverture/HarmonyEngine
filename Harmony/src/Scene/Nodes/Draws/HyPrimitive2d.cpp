/**************************************************************************
 *	HyPrimitive2d.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Scene/Nodes/Draws/HyPrimitive2d.h"

#include "IHyApplication.h"
#include "Utilities/HyMath.h"

HyPrimitive2d::HyPrimitive2d() :	IHyDraw2d(HYTYPE_Primitive2d, nullptr, nullptr),
									m_pVertices(nullptr)
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

	ClearData();
	if(m_RenderState.GetNumVerticesPerInstance() != 0)
	{
		m_pVertices = HY_NEW glm::vec2[m_RenderState.GetNumVerticesPerInstance()];
		memcpy(m_pVertices, p.m_pVertices, m_RenderState.GetNumVerticesPerInstance() * sizeof(glm::vec2));
	}

	MakeBoundingVolumeDirty();
	
	return *this;
}

void HyPrimitive2d::SetAsQuad(int iWidth, int iHeight, bool bWireframe)
{
	SetAsQuad(static_cast<float>(iWidth), static_cast<float>(iHeight), bWireframe);
}

void HyPrimitive2d::SetAsQuad(float fWidth, float fHeight, bool bWireframe)
{
	ClearData();

	if(bWireframe)
		m_RenderState.Enable(HyRenderState::DRAWMODE_LINELOOP);
	else
		m_RenderState.Enable(HyRenderState::DRAWMODE_TRIANGLESTRIP);

	if(m_eCoordUnit == HYCOORDUNIT_Default)
		m_eCoordUnit = IHyApplication::DefaultCoordinateUnit();
	float fCoordMod = (m_eCoordUnit == HYCOORDUNIT_Meters) ? IHyApplication::PixelsPerMeter() : 1.0f;
	fWidth *= fCoordMod;
	fHeight *= fCoordMod;

	m_RenderState.SetShaderId(HYSHADERPROG_Primitive);
	m_RenderState.SetNumVerticesPerInstance(4);

	m_pVertices = HY_NEW glm::vec2[4];

	m_pVertices[0].x = 0.0f;
	m_pVertices[0].y = 0.0f;
	m_pVertices[1].x = fWidth;
	m_pVertices[1].y = 0.0f;

	if(bWireframe)
	{
		m_pVertices[2].x = fWidth;
		m_pVertices[2].y = fHeight;
		m_pVertices[3].x = 0.0f;
		m_pVertices[3].y = fHeight;
	}
	else
	{
		m_pVertices[2].x = 0.0f;
		m_pVertices[2].y = fHeight;
		m_pVertices[3].x = fWidth;
		m_pVertices[3].y = fHeight;
	}

	MakeBoundingVolumeDirty();
}

void HyPrimitive2d::SetAsCircle(float fRadius, int32 iNumSegments, bool bWireframe)
{
	ClearData();

	if(bWireframe)
		m_RenderState.Enable(HyRenderState::DRAWMODE_LINELOOP);
	else
		m_RenderState.Enable(HyRenderState::DRAWMODE_TRIANGLEFAN);

	m_RenderState.SetShaderId(HYSHADERPROG_Primitive);
	m_RenderState.SetNumVerticesPerInstance(iNumSegments);
	m_pVertices = HY_NEW glm::vec2[iNumSegments];

	if(m_eCoordUnit == HYCOORDUNIT_Default)
		m_eCoordUnit = IHyApplication::DefaultCoordinateUnit();
	float fCoordMod = (m_eCoordUnit == HYCOORDUNIT_Meters) ? IHyApplication::PixelsPerMeter() : 1.0f;
	fRadius *= fCoordMod;

	float t = 0.0f;
	for(uint32 n = 0; n <= m_RenderState.GetNumVerticesPerInstance(); ++n)
	{
		t = 2.0f * HY_PI * static_cast<float>(n) / static_cast<float>(m_RenderState.GetNumVerticesPerInstance());

		m_pVertices[n].x = (sin(t) * fRadius);
		m_pVertices[n].y = (cos(t) * fRadius);
	}

	MakeBoundingVolumeDirty();
}

void HyPrimitive2d::SetAsLineChain(std::vector<glm::vec2> &vertexList)
{
	ClearData();

	HyAssert(vertexList.size() > 1, "HyPrimitive2d::SetAsLineChain was passed an empty vertexList or a vertexList of only '1' vertex");

	m_RenderState.SetShaderId(HYSHADERPROG_Lines2d);
	m_RenderState.Enable(HyRenderState::DRAWMODE_TRIANGLESTRIP);
	m_RenderState.SetNumInstances(vertexList.size() - 1);
	m_RenderState.SetNumVerticesPerInstance(8);				// 8 vertices per instance because of '2' duplicate vertex positions and normals on each end of line segment
	
	m_pVertices = HY_NEW glm::vec2[vertexList.size() * 4];	// size*4 = Each vertex of segment has '2' duplicate vertex positions that are offset by '2' corresponding normals within vertex shader 

	if(m_eCoordUnit == HYCOORDUNIT_Default)
		m_eCoordUnit = IHyApplication::DefaultCoordinateUnit();
	float fCoordMod = (m_eCoordUnit == HYCOORDUNIT_Meters) ? IHyApplication::PixelsPerMeter() : 1.0f;

	uint32 i, j;
	for(i = j = 0; (i+1) < vertexList.size(); ++i, j += 8)
	{
		glm::vec2 vVecDirection = vertexList[i + 1] - vertexList[i + 0];

		/*postion 1    */ m_pVertices[j + 0] = vertexList[i + 0] * fCoordMod;
		/*Normal  1    */ m_pVertices[j + 1] = glm::normalize(glm::vec2(-vVecDirection.y, vVecDirection.x));
		/*postion 1 dup*/ m_pVertices[j + 2] = m_pVertices[j + 0];
		/*Normal  1 inv*/ m_pVertices[j + 3] = m_pVertices[j + 1] * -1.0f;

		/*postion 2    */ m_pVertices[j + 4] = vertexList[i + 1] * fCoordMod;
		/*Normal  2    */ m_pVertices[j + 5] = glm::normalize(glm::vec2(vVecDirection.y, -vVecDirection.x));
		/*postion 2 dup*/ m_pVertices[j + 6] = m_pVertices[j + 4];
		/*Normal  2 inv*/ m_pVertices[j + 7] = m_pVertices[j + 5] * -1.0f;
	}

	MakeBoundingVolumeDirty();
}

float HyPrimitive2d::GetLineThickness()
{
	return m_RenderState.GetLineThickness();
}

void HyPrimitive2d::SetLineThickness(float fThickness)
{
	m_RenderState.SetLineThickness(fThickness);
}

void HyPrimitive2d::ClearData()
{
	delete [] m_pVertices;
	m_pVertices = nullptr;
	m_RenderState.SetNumVerticesPerInstance(0);
	m_RenderState.SetNumInstances(1);
	m_RenderState.Disable(HyRenderState::DRAWMODEMASK);

	MakeBoundingVolumeDirty();
}

/*virtual*/ void HyPrimitive2d::OnCalcBoundingVolume()
{
	uint32 uiNumVerts = m_RenderState.GetNumVerticesPerInstance();
	glm::vec2 vLowerBounds(0.0f);
	glm::vec2 vUpperBounds(0.0f);

	if(uiNumVerts == 0)
	{
		m_BoundingVolume.SetLocalAABB(vLowerBounds, vUpperBounds);
		return;
	}

	vLowerBounds.x = m_pVertices[0].x;
	vLowerBounds.y = m_pVertices[0].y;
	vUpperBounds.x = m_pVertices[0].x;
	vUpperBounds.y = m_pVertices[0].y;

	for(uint32 i = 1; i < uiNumVerts; ++i)
	{
		vLowerBounds.x = HyMin(m_pVertices[i].x, vLowerBounds.x);
		vLowerBounds.y = HyMin(m_pVertices[i].y, vLowerBounds.y);
		vUpperBounds.x = HyMax(m_pVertices[i].x, vUpperBounds.x);
		vUpperBounds.y = HyMax(m_pVertices[i].y, vUpperBounds.y);
	}

	m_BoundingVolume.SetLocalAABB(vLowerBounds, vUpperBounds);
}

/*virtual*/ void HyPrimitive2d::OnUpdateUniforms()
{
	glm::mat4 mtx;
	GetWorldTransform(mtx);

	glm::vec4 vTop;
	vTop.x = topColor.X();
	vTop.y = topColor.Y();
	vTop.z = topColor.Z();
	vTop.a = alpha.Get();
	
	glm::vec4 vBot;
	vTop.x = botColor.X();
	vTop.y = botColor.Y();
	vTop.z = botColor.Z();
	vBot.a = alpha.Get();

	m_ShaderUniforms.Set("u_mtxTransform", mtx);
	m_ShaderUniforms.Set("u_vColor", vTop);
}

/*virtual*/ void HyPrimitive2d::OnWriteDrawBufferData(char *&pRefDataWritePos)
{
	memcpy(pRefDataWritePos, m_pVertices, m_RenderState.GetNumVerticesPerInstance() * sizeof(glm::vec4));
	pRefDataWritePos += m_RenderState.GetNumVerticesPerInstance() * sizeof(glm::vec4);
}
