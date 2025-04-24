/**************************************************************************
 *	HyBox2dDraw.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2022 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Scene/Physics/HyBox2dDraw.h"

#define HYDISPLAYORDER_BOX2DDRAW 99999999

void DrawPolygonFcn(const b2Vec2 *vertices, int vertexCount, b2HexColor color, void *context)
{
	static_cast<HyBox2dDraw *>(context)->DrawPolygon(vertices, vertexCount, color);
}

void DrawSolidPolygonFcn(b2Transform transform, const b2Vec2 *vertices, int vertexCount, float radius, b2HexColor color, void *context)
{
	static_cast<HyBox2dDraw *>(context)->DrawSolidPolygon(transform, vertices, vertexCount, radius, color);
}

void DrawCircleFcn(b2Vec2 center, float radius, b2HexColor color, void *context)
{
	static_cast<HyBox2dDraw *>(context)->DrawCircle(center, radius, color);
}

void DrawSolidCircleFcn(b2Transform transform, float radius, b2HexColor color, void *context)
{
	static_cast<HyBox2dDraw *>(context)->DrawSolidCircle(transform, b2Vec2_zero, radius, color);
}

void DrawSolidCapsuleFcn(b2Vec2 p1, b2Vec2 p2, float radius, b2HexColor color, void *context)
{
	static_cast<HyBox2dDraw *>(context)->DrawSolidCapsule(p1, p2, radius, color);
}

void DrawSegmentFcn(b2Vec2 p1, b2Vec2 p2, b2HexColor color, void *context)
{
	static_cast<HyBox2dDraw *>(context)->DrawSegment(p1, p2, color);
}

void DrawTransformFcn(b2Transform transform, void *context)
{
	static_cast<HyBox2dDraw *>(context)->DrawTransform(transform);
}

void DrawPointFcn(b2Vec2 p, float size, b2HexColor color, void *context)
{
	static_cast<HyBox2dDraw *>(context)->DrawPoint(p, size, color);
}

void DrawStringFcn(b2Vec2 p, const char *s, b2HexColor color, void *context)
{
	static_cast<HyBox2dDraw *>(context)->DrawString(p, s, color);
}

HyBox2dDraw::HyBox2dDraw(float fPixelsPerMeter) :
	HyEntity2d(nullptr),
	m_fPixelsPerMeter(fPixelsPerMeter),
	m_iDrawIndex(0)
{
	m_DebugDraw = {};
	m_DebugDraw.DrawPolygonFcn = DrawPolygonFcn;
	m_DebugDraw.DrawSolidPolygonFcn = DrawSolidPolygonFcn;
	m_DebugDraw.DrawCircleFcn = DrawCircleFcn;
	m_DebugDraw.DrawSolidCircleFcn = DrawSolidCircleFcn;
	m_DebugDraw.DrawSolidCapsuleFcn = DrawSolidCapsuleFcn;
	m_DebugDraw.DrawSegmentFcn = DrawSegmentFcn;
	m_DebugDraw.DrawTransformFcn = DrawTransformFcn;
	m_DebugDraw.DrawPointFcn = DrawPointFcn;
	m_DebugDraw.DrawStringFcn = DrawStringFcn;
	m_DebugDraw.context = this;

	SetDisplayOrder(HYDISPLAYORDER_BOX2DDRAW);
}

HyBox2dDraw::~HyBox2dDraw(void)
{
}

b2DebugDraw *HyBox2dDraw::GetDrawPtr()
{
	return &m_DebugDraw;
}

void HyBox2dDraw::SetShowFlags(uint32 uiDiagFlags)
{
	m_DebugDraw.drawShapes = (uiDiagFlags & HYDIAG_PhysShapes);
	m_DebugDraw.drawJoints = (uiDiagFlags & HYDIAG_PhysJoints);
	m_DebugDraw.drawBounds = (uiDiagFlags & HYDIAG_PhysAabb);
	m_DebugDraw.drawContacts = (uiDiagFlags & HYDIAG_PhysContacts);
	m_DebugDraw.drawMass = (uiDiagFlags & HYDIAG_PhysCenterOfMass);
}

void HyBox2dDraw::BeginFrame()
{
	m_iDrawIndex = 0;
}

void HyBox2dDraw::DrawPolygon(const b2Vec2 *vertices, int32 vertexCount, b2HexColor color)
{
	while(m_iDrawIndex >= m_DrawList.size())
		m_DrawList.emplace_back(this);

	std::vector<glm::vec2> convertedVertList;
	for(int i = 0; i < vertexCount; ++i)
		convertedVertList.emplace_back(vertices[i].x * m_fPixelsPerMeter, vertices[i].y * m_fPixelsPerMeter);

	m_DrawList[m_iDrawIndex].SetAsPolygon(convertedVertList.data(), vertexCount);
	m_DrawList[m_iDrawIndex].SetWireframe(true);
	m_DrawList[m_iDrawIndex].SetTint(HyColor(color));
	m_DrawList[m_iDrawIndex].SetVisible(true);

	m_iDrawIndex++;
}

void HyBox2dDraw::DrawSolidPolygon(b2Transform transform, const b2Vec2 *vertices, int32_t vertexCount, float radius, b2HexColor color)
{
	while(m_iDrawIndex >= m_DrawList.size())
		m_DrawList.emplace_back(this);

	std::vector<glm::vec2> convertedVertList;
	for(int i = 0; i < vertexCount; ++i)
		convertedVertList.emplace_back(vertices[i].x * m_fPixelsPerMeter, vertices[i].y * m_fPixelsPerMeter);

	m_DrawList[m_iDrawIndex].SetAsPolygon(convertedVertList.data(), vertexCount);
	m_DrawList[m_iDrawIndex].SetWireframe(false);
	m_DrawList[m_iDrawIndex].SetTint(HyColor(color));
	m_DrawList[m_iDrawIndex].SetVisible(true);

	m_iDrawIndex++;
}

void HyBox2dDraw::DrawCircle(const b2Vec2 &center, float radius, b2HexColor color)
{
	while(m_iDrawIndex >= m_DrawList.size())
		m_DrawList.emplace_back(this);

	glm::vec2 ptConvertedCenter(center.x * m_fPixelsPerMeter, center.y * m_fPixelsPerMeter);

	m_DrawList[m_iDrawIndex].SetAsCircle(ptConvertedCenter, radius * m_fPixelsPerMeter);
	m_DrawList[m_iDrawIndex].SetWireframe(true);
	m_DrawList[m_iDrawIndex].SetTint(HyColor(color));
	m_DrawList[m_iDrawIndex].SetVisible(true);

	m_iDrawIndex++;
}

void HyBox2dDraw::DrawSolidCircle(b2Transform transform, b2Vec2 center, float radius, b2HexColor color)
{
	while(m_iDrawIndex >= m_DrawList.size())
		m_DrawList.emplace_back(this);

	glm::vec2 ptConvertedCenter(center.x * m_fPixelsPerMeter, center.y * m_fPixelsPerMeter);

	m_DrawList[m_iDrawIndex].SetAsCircle(ptConvertedCenter, radius * m_fPixelsPerMeter);
	m_DrawList[m_iDrawIndex].SetWireframe(false);
	m_DrawList[m_iDrawIndex].SetTint(HyColor(color));
	m_DrawList[m_iDrawIndex].SetVisible(true);

	m_iDrawIndex++;
}

void HyBox2dDraw::DrawSolidCapsule(b2Vec2 p1, b2Vec2 p2, float radius, b2HexColor color)
{
}

void HyBox2dDraw::DrawSegment(const b2Vec2 &p1, const b2Vec2 &p2, b2HexColor color)
{
	while(m_iDrawIndex >= m_DrawList.size())
		m_DrawList.emplace_back(this);

	glm::vec2 ptConvertedP1(p1.x * m_fPixelsPerMeter, p1.y * m_fPixelsPerMeter);
	glm::vec2 ptConvertedP2(p2.x * m_fPixelsPerMeter, p2.y * m_fPixelsPerMeter);

	m_DrawList[m_iDrawIndex].SetAsLineSegment(ptConvertedP1, ptConvertedP2);
	m_DrawList[m_iDrawIndex].SetTint(HyColor(color));
	m_DrawList[m_iDrawIndex].SetVisible(true);

	m_iDrawIndex++;
}

void HyBox2dDraw::DrawTransform(const b2Transform &xf)
{
}

void HyBox2dDraw::DrawPoint(const b2Vec2 &p, float size, b2HexColor color)
{
}

void HyBox2dDraw::DrawString(b2Vec2 p, const char *s, b2HexColor color)
{
}

void HyBox2dDraw::EndFrame()
{
	for(int32 i = m_iDrawIndex; i < m_DrawList.size(); ++i)
		m_DrawList[i].SetVisible(false);
}
