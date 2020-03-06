/**************************************************************************
 *	HyDrawPhys2d.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2020 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Scene/Physics/HyPhysicsDebug2d.h"
#include "Scene/Nodes/Loadables/Drawables/Instances/Objects/HyPrimitive2d.h"

HyPhysicsDebug2d::HyPhysicsDebug2d(float fPixelsPerMeter) :
	m_fPIXELS_PER_METER(fPixelsPerMeter)
{
}

HyPhysicsDebug2d::~HyPhysicsDebug2d(void)
{
}

std::vector<HyPrimitive2d> &HyPhysicsDebug2d::GetDrawList()
{
	return m_DrawList;
}

// Draw a closed polygon provided in CCW order.
/*virtual*/ void HyPhysicsDebug2d::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) /*override*/
{
	std::vector<b2Vec2> convertedVertList;
	for(int i = 0; i < vertexCount; ++i)
		convertedVertList.emplace_back(vertices[i].x * m_fPIXELS_PER_METER, vertices[i].y * m_fPIXELS_PER_METER);

	m_DrawList.emplace_back();
	m_DrawList.back().SetAsPolygon(convertedVertList.data(), vertexCount);
	m_DrawList.back().SetWireframe(true);
	m_DrawList.back().SetTint(color.r, color.g, color.b);
}

// Draw a solid closed polygon provided in CCW order.
/*virtual*/ void HyPhysicsDebug2d::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) /*override*/
{
	std::vector<b2Vec2> convertedVertList;
	for(int i = 0; i < vertexCount; ++i)
		convertedVertList.emplace_back(vertices[i].x * m_fPIXELS_PER_METER, vertices[i].y * m_fPIXELS_PER_METER);

	m_DrawList.emplace_back();

	m_DrawList.back().SetAsPolygon(convertedVertList.data(), vertexCount);
	m_DrawList.back().SetWireframe(false);
	m_DrawList.back().SetTint(color.r, color.g, color.b);
}

// Draw a circle.
/*virtual*/ void HyPhysicsDebug2d::DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color) /*override*/
{
	b2Vec2 ptConvertedCenter(center.x * m_fPIXELS_PER_METER, center.y * m_fPIXELS_PER_METER);

	m_DrawList.emplace_back();

	m_DrawList.back().SetAsCircle(ptConvertedCenter, radius * m_fPIXELS_PER_METER);
	m_DrawList.back().SetWireframe(true);
	m_DrawList.back().SetTint(color.r, color.g, color.b);
}

// Draw a solid circle.
/*virtual*/ void HyPhysicsDebug2d::DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color) /*override*/
{
	b2Vec2 ptConvertedCenter(center.x * m_fPIXELS_PER_METER, center.y * m_fPIXELS_PER_METER);

	m_DrawList.emplace_back();

	m_DrawList.back().SetAsCircle(ptConvertedCenter, radius * m_fPIXELS_PER_METER);
	m_DrawList.back().SetWireframe(false);
	m_DrawList.back().SetTint(color.r, color.g, color.b);
}

// Draw a line segment.
/*virtual*/ void HyPhysicsDebug2d::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) /*override*/
{
	b2Vec2 ptConvertedP1(p1.x * m_fPIXELS_PER_METER, p1.y * m_fPIXELS_PER_METER);
	b2Vec2 ptConvertedP2(p2.x * m_fPIXELS_PER_METER, p2.y * m_fPIXELS_PER_METER);

	m_DrawList.emplace_back();

	m_DrawList.back().SetAsLineSegment(ptConvertedP1, ptConvertedP2);
	m_DrawList.back().SetTint(color.r, color.g, color.b);
}

// Draw a transform. Choose your own length scale.
/*virtual*/ void HyPhysicsDebug2d::DrawTransform(const b2Transform& xf) /*override*/
{
}

/*virtual*/ void HyPhysicsDebug2d::DrawPoint(const b2Vec2& p, float32 size, const b2Color& color) /*override*/
{
	m_DrawList.emplace_back();

	m_DrawList.back().SetAsBox(size * 0.5f, size * 0.5f, glm::vec2(p.x * m_fPIXELS_PER_METER, p.y * m_fPIXELS_PER_METER), 0.0f);
	m_DrawList.back().SetWireframe(false);
	m_DrawList.back().SetTint(color.r, color.g, color.b);
}
