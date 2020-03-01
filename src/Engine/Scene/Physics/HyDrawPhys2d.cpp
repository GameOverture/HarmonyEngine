/**************************************************************************
 *	HyDrawPhys2d.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Scene/Physics/HyDrawPhys2d.h"
#include "Scene/Nodes/Loadables/Drawables/Instances/Objects/HyPrimitive2d.h"

HyDrawPhys2d::HyDrawPhys2d() :
	m_bDrawEnabled(false)
{
}

HyDrawPhys2d::~HyDrawPhys2d(void)
{
}

std::vector<HyPrimitive2d> &HyDrawPhys2d::GetDrawList()
{
	return m_DrawList;
}

// Draw a closed polygon provided in CCW order.
/*virtual*/ void HyDrawPhys2d::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) /*override*/
{
	m_DrawList.emplace_back();

	m_DrawList.back().SetAsPolygon(vertices, vertexCount);
	m_DrawList.back().SetWireframe(true);
	m_DrawList.back().SetTint(color.r, color.g, color.b);
}

// Draw a solid closed polygon provided in CCW order.
/*virtual*/ void HyDrawPhys2d::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) /*override*/
{
	m_DrawList.emplace_back();

	m_DrawList.back().SetAsPolygon(vertices, vertexCount);
	m_DrawList.back().SetWireframe(false);
	m_DrawList.back().SetTint(color.r, color.g, color.b);
}

// Draw a circle.
/*virtual*/ void HyDrawPhys2d::DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color) /*override*/
{
	m_DrawList.emplace_back();

	m_DrawList.back().SetAsCircle(center, radius);
	m_DrawList.back().SetWireframe(true);
	m_DrawList.back().SetTint(color.r, color.g, color.b);
}

// Draw a solid circle.
/*virtual*/ void HyDrawPhys2d::DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color) /*override*/
{
	m_DrawList.emplace_back();

	m_DrawList.back().SetAsCircle(center, radius);
	m_DrawList.back().SetWireframe(false);
	m_DrawList.back().SetTint(color.r, color.g, color.b);
}

// Draw a line segment.
/*virtual*/ void HyDrawPhys2d::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) /*override*/
{
	m_DrawList.emplace_back();

	m_DrawList.back().SetAsLineSegment(p1, p2);
	m_DrawList.back().SetTint(color.r, color.g, color.b);
}

// Draw a transform. Choose your own length scale.
/*virtual*/ void HyDrawPhys2d::DrawTransform(const b2Transform& xf) /*override*/
{
}

/*virtual*/ void HyDrawPhys2d::DrawPoint(const b2Vec2& p, float32 size, const b2Color& color) /*override*/
{
	m_DrawList.emplace_back();

	m_DrawList.back().SetAsBox(size * 0.5f, size * 0.5f, glm::vec2(p.x, p.y), 0.0f);
	m_DrawList.back().SetWireframe(false);
	m_DrawList.back().SetTint(color.r, color.g, color.b);
}
