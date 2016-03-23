/**************************************************************************
 *	HyPrimitive2d.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Scene/Instances/HyPrimitive2d.h"

#include "IHyApplication.h"
#include "Utilities/HyMath.h"

// TODO: Use more appropriate math lib
#include "Box2D/Box2D.h"

HyPrimitive2d::HyPrimitive2d() :	IHyInst2d(HYINST_Primitive2d, NULL, NULL),
									m_pVertices(NULL),
									m_uiNumVerts(0)
{
	m_RenderState.SetShaderIndex(IHyShader::SHADER_Primitive);
	m_RenderState.SetNumInstances(1);
}


HyPrimitive2d::~HyPrimitive2d(void)
{
	ClearData();
}

const HyPrimitive2d &HyPrimitive2d::operator=(const HyPrimitive2d& p)
{
	m_RenderState = p.m_RenderState;
	m_uiNumVerts = p.m_uiNumVerts;

	m_eCoordUnit = p.m_eCoordUnit;

	ClearData();
	if(m_uiNumVerts != 0)
	{
		m_pVertices = new glm::vec4[m_uiNumVerts];
		memcpy(m_pVertices, p.m_pVertices, m_uiNumVerts * sizeof(glm::vec4));
	}
	
	return *this;
}

void HyPrimitive2d::SetAsQuad(float fWidth, float fHeight, bool bWireframe, glm::vec2 &vOffset /*= vec2(0.0f)*/)
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
	vOffset *= fCoordMod;

	m_uiNumVerts = 4;
	m_pVertices = new glm::vec4[m_uiNumVerts];

	m_pVertices[0].x = vOffset.x;
	m_pVertices[0].y = vOffset.y;
	m_pVertices[0].z = 0.0f;
	m_pVertices[0].w = 1.0f;

	m_pVertices[1].x = fWidth + vOffset.x;
	m_pVertices[1].y = vOffset.y;
	m_pVertices[1].z = 0.0f;
	m_pVertices[1].w = 1.0f;

	if(bWireframe)
	{
		m_pVertices[2].x = fWidth + vOffset.x;
		m_pVertices[2].y = fHeight + vOffset.y;
		m_pVertices[3].x = vOffset.x;
		m_pVertices[3].y = fHeight + vOffset.y;
	}
	else
	{
		m_pVertices[2].x = vOffset.x;
		m_pVertices[2].y = fHeight + vOffset.y;
		m_pVertices[3].x = fWidth + vOffset.x;
		m_pVertices[3].y = fHeight + vOffset.y;
	}
	m_pVertices[2].z = 0.0f;
	m_pVertices[2].w = 1.0f;
	m_pVertices[3].z = 0.0f;
	m_pVertices[3].w = 1.0f;
}

void HyPrimitive2d::SetAsCircle(float fRadius, int32 iNumSegments, bool bWireframe, glm::vec2 &vOffset /*= vec2(0.0f)*/)
{
	ClearData();

	if(bWireframe)
		m_RenderState.Enable(HyRenderState::DRAWMODE_LINELOOP);
	else
		m_RenderState.Enable(HyRenderState::DRAWMODE_TRIANGLEFAN);

	m_uiNumVerts = iNumSegments;
	m_pVertices = new glm::vec4[m_uiNumVerts];

	if(m_eCoordUnit == HYCOORDUNIT_Default)
		m_eCoordUnit = IHyApplication::DefaultCoordinateUnit();
	float fCoordMod = (m_eCoordUnit == HYCOORDUNIT_Meters) ? IHyApplication::PixelsPerMeter() : 1.0f;
	fRadius *= fCoordMod;
	vOffset *= fCoordMod;

	float t = 0.0f;
	for(uint32 n = 0; n <= m_uiNumVerts; ++n)
	{
		t = 2.0f * HY_PI * static_cast<float>(n) / static_cast<float>(m_uiNumVerts);

		m_pVertices[n].x = (sin(t) * fRadius) + vOffset.x;
		m_pVertices[n].y = (cos(t) * fRadius) + vOffset.y;
		m_pVertices[n].z = 0.0f;
		m_pVertices[n].w = 1.0f;
	}
}

void HyPrimitive2d::SetAsEdgeChain(const glm::vec2 *pVertices, uint32 uiNumVerts, bool bChainLoop, glm::vec2 &vOffset /*= vec2(0.0f)*/)
{
	ClearData();

	if(bChainLoop)
		m_RenderState.Enable(HyRenderState::DRAWMODE_LINELOOP);
	else
		m_RenderState.Enable(HyRenderState::DRAWMODE_LINESTRIP);

	m_uiNumVerts = uiNumVerts;
	m_pVertices = new glm::vec4[m_uiNumVerts];

	if(m_eCoordUnit == HYCOORDUNIT_Default)
		m_eCoordUnit = IHyApplication::DefaultCoordinateUnit();
	float fCoordMod = (m_eCoordUnit == HYCOORDUNIT_Meters) ? IHyApplication::PixelsPerMeter() : 1.0f;
	
	for(uint32 i = 0; i < m_uiNumVerts; ++i)
	{
		m_pVertices[i].x = (pVertices[i].x + vOffset.x) * fCoordMod;
		m_pVertices[i].y = (pVertices[i].y + vOffset.y) * fCoordMod;
		m_pVertices[i].z = 0.0f;
		m_pVertices[i].w = 1.0f;
	}
}

void HyPrimitive2d::OffsetVerts(glm::vec2 vOffset, float fAngleOffset)
{
	HyAssert(m_pVertices, "HyPrimitive2d::OffsetVerts() was invoked with an unset instance.");

	if(m_eCoordUnit == HYCOORDUNIT_Default)
		m_eCoordUnit = IHyApplication::DefaultCoordinateUnit();
	float fCoordMod = (m_eCoordUnit == HYCOORDUNIT_Meters) ? IHyApplication::PixelsPerMeter() : 1.0f;
	vOffset *= fCoordMod;

	b2Transform xf;
	xf.p.x = vOffset.x;
	xf.p.y = vOffset.y;
	xf.q.Set(fAngleOffset);

	b2Vec2 tmp;
	// Transform vertices and normals.
	for(uint32 i = 0; i < m_uiNumVerts; ++i)
	{
		tmp = b2Mul(xf, b2Vec2(m_pVertices[i].x, m_pVertices[i].y));
		m_pVertices[i].x = tmp.x;
		m_pVertices[i].y = tmp.y;
	}
}

void HyPrimitive2d::ClearData()
{
	delete [] m_pVertices;
	m_pVertices = NULL;
	m_uiNumVerts = 0;

	m_RenderState.Disable(HyRenderState::DRAWMODEMASK);
}

/*virtual*/ void HyPrimitive2d::OnDataLoaded()
{
}

/*virtual*/ void HyPrimitive2d::OnUpdate()
{
}

/*virtual*/ void HyPrimitive2d::OnUpdateUniforms(HyShaderUniforms *pShaderUniformsRef)
{
}

/*virtual*/ void HyPrimitive2d::DefaultWriteDrawBufferData(char *&pRefDataWritePos)
{
	memcpy(pRefDataWritePos, &color.Get(), sizeof(glm::vec4));
	pRefDataWritePos += sizeof(glm::vec4);

	glm::mat4 mtx;
	GetWorldTransform(mtx);
	memcpy(pRefDataWritePos, &mtx, sizeof(glm::mat4));
	pRefDataWritePos += sizeof(glm::mat4);

	memcpy(pRefDataWritePos, &m_uiNumVerts, sizeof(uint32));
	pRefDataWritePos += sizeof(uint32);

	memcpy(pRefDataWritePos, m_pVertices, m_uiNumVerts * sizeof(glm::vec4));
	pRefDataWritePos += m_uiNumVerts * sizeof(glm::vec4);
}
