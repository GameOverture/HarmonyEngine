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

HyBox2dDraw::HyBox2dDraw(float fPixelsPerMeter) :
	HyEntity2d(nullptr),
	m_fPixelsPerMeter(fPixelsPerMeter),
	m_iDrawIndex(0)
{
	SetDisplayOrder(HYDISPLAYORDER_BOX2DDRAW);
}

HyBox2dDraw::~HyBox2dDraw(void)
{
}

void HyBox2dDraw::BeginFrame()
{
	m_iDrawIndex = 0;
}

// Draw a closed polygon provided in CCW order.
/*virtual*/ void HyBox2dDraw::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) /*override*/
{
	while(m_iDrawIndex >= m_DrawList.size())
		m_DrawList.emplace_back();

	std::vector<b2Vec2> convertedVertList;
	for(int i = 0; i < vertexCount; ++i)
		convertedVertList.emplace_back(vertices[i].x * m_fPixelsPerMeter, vertices[i].y * m_fPixelsPerMeter);

	m_DrawList[m_iDrawIndex].shape.SetAsPolygon(convertedVertList.data(), vertexCount);
	m_DrawList[m_iDrawIndex].SetWireframe(true);
	m_DrawList[m_iDrawIndex].SetTint(HyColor(color.r, color.g, color.b));

	m_iDrawIndex++;
}

// Draw a solid closed polygon provided in CCW order.
/*virtual*/ void HyBox2dDraw::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) /*override*/
{
	while(m_iDrawIndex >= m_DrawList.size())
		m_DrawList.emplace_back();

	std::vector<b2Vec2> convertedVertList;
	for(int i = 0; i < vertexCount; ++i)
		convertedVertList.emplace_back(vertices[i].x * m_fPixelsPerMeter, vertices[i].y * m_fPixelsPerMeter);

	m_DrawList[m_iDrawIndex].shape.SetAsPolygon(convertedVertList.data(), vertexCount);
	m_DrawList[m_iDrawIndex].SetWireframe(false);
	m_DrawList[m_iDrawIndex].SetTint(HyColor(color.r, color.g, color.b));

	m_iDrawIndex++;
}

// Draw a circle.
/*virtual*/ void HyBox2dDraw::DrawCircle(const b2Vec2& center, float radius, const b2Color& color) /*override*/
{
	while(m_iDrawIndex >= m_DrawList.size())
		m_DrawList.emplace_back();

	b2Vec2 ptConvertedCenter(center.x * m_fPixelsPerMeter, center.y * m_fPixelsPerMeter);

	m_DrawList[m_iDrawIndex].shape.SetAsCircle(ptConvertedCenter, radius * m_fPixelsPerMeter);
	m_DrawList[m_iDrawIndex].SetWireframe(true);
	m_DrawList[m_iDrawIndex].SetTint(HyColor(color.r, color.g, color.b));

	m_iDrawIndex++;
}

// Draw a solid circle.
/*virtual*/ void HyBox2dDraw::DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis, const b2Color& color) /*override*/
{
	while(m_iDrawIndex >= m_DrawList.size())
		m_DrawList.emplace_back();

	b2Vec2 ptConvertedCenter(center.x * m_fPixelsPerMeter, center.y * m_fPixelsPerMeter);

	m_DrawList[m_iDrawIndex].shape.SetAsCircle(ptConvertedCenter, radius * m_fPixelsPerMeter);
	m_DrawList[m_iDrawIndex].SetWireframe(false);
	m_DrawList[m_iDrawIndex].SetTint(HyColor(color.r, color.g, color.b));

	m_iDrawIndex++;
}

// Draw a line segment.
/*virtual*/ void HyBox2dDraw::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) /*override*/
{
	while(m_iDrawIndex >= m_DrawList.size())
		m_DrawList.emplace_back();

	b2Vec2 ptConvertedP1(p1.x * m_fPixelsPerMeter, p1.y * m_fPixelsPerMeter);
	b2Vec2 ptConvertedP2(p2.x * m_fPixelsPerMeter, p2.y * m_fPixelsPerMeter);

	m_DrawList[m_iDrawIndex].shape.SetAsLineSegment(ptConvertedP1, ptConvertedP2);
	m_DrawList[m_iDrawIndex].SetTint(HyColor(color.r, color.g, color.b));

	m_iDrawIndex++;
}

// Draw a transform. Choose your own length scale.
/*virtual*/ void HyBox2dDraw::DrawTransform(const b2Transform& xf) /*override*/
{
}

/*virtual*/ void HyBox2dDraw::DrawPoint(const b2Vec2& p, float size, const b2Color& color) /*override*/
{
	//m_DrawList.emplace_back();

	//m_DrawList.back().SetAsBox(size * 0.5f, size * 0.5f, glm::vec2(p.x * m_fPixelsPerMeter, p.y * m_fPixelsPerMeter), 0.0f);
	//m_DrawList.back().SetWireframe(false);
	//m_DrawList.back().SetTint(color.r, color.g, color.b);
}

void HyBox2dDraw::EndFrame()
{
	for(int32 i = m_iDrawIndex; i < m_DrawList.size(); ++i)
		m_DrawList[i].SetVisible(false);
}
