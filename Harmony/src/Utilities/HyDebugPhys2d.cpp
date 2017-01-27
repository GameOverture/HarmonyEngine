/**************************************************************************
 *	HyDebugPhys2d.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Utilities/HyDebugPhys2d.h"

#include "Scene/Nodes/Draws/HyPrimitive2d.h"

HyDebugPhys2d::HyDebugPhys2d(void) : m_bDrawEnabled(false)
{
}


HyDebugPhys2d::~HyDebugPhys2d(void)
{
}

void HyDebugPhys2d::Reset()
{ 
	//for(uint32 i = 0; i < m_vDrawInsts.size(); ++i)
	//	delete m_vDrawInsts[i];
	//m_vDrawInsts.clear();
}

/// Draw a closed polygon provided in CCW order.
/*virtual*/ void HyDebugPhys2d::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
	//HyEntity2d *pNewEnt = new HyEntity2d();
	//HyPrimitive2d *pInst = pNewEnt->SetPrimitive();

	//pInst->Color().Set(color.r, color.g, color.b, 1.0f);
	//pInst->SetAsEdgeChain(reinterpret_cast<const vec2 *>(vertices), static_cast<uint32>(vertexCount), true, HYCOORD_Meter);

	//m_vDrawInsts.push_back(pNewEnt);
}

/// Draw a solid closed polygon provided in CCW order.
/*virtual*/ void HyDebugPhys2d::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
	//HyEntity2d *pNewEnt = new HyEntity2d();
	//HyPrimitive2d *pInst = pNewEnt->SetPrimitive();

	//pInst->Color().Set(color.r, color.g, color.b, 1.0f);
	//pInst->SetAsEdgeChain(reinterpret_cast<const vec2 *>(vertices), static_cast<uint32>(vertexCount), true, HYCOORD_Meter);

	//m_vDrawInsts.push_back(pNewEnt);
}

/// Draw a circle.
/*virtual*/ void HyDebugPhys2d::DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color)
{
	//HyEntity2d *pNewEnt = new HyEntity2d();
	//HyPrimitive2d *pInst = pNewEnt->SetPrimitive();

	//pInst->Color().Set(color.r, color.g, color.b, 1.0f);
	//vec2 vCenter(center.x, center.y);
	//pInst->SetAsCircle(radius, 24, true, HYCOORD_Meter, vCenter);

	//m_vDrawInsts.push_back(pNewEnt);
}

/// Draw a solid circle.
/*virtual*/ void HyDebugPhys2d::DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color)
{
	//HyEntity2d *pNewEnt = new HyEntity2d();
	//HyPrimitive2d *pInst = pNewEnt->SetPrimitive();

	//pInst->Color().Set(color.r, color.g, color.b, 1.0f);
	//vec2 vCenter(center.x, center.y);
	//pInst->SetAsCircle(radius, 24, false, HYCOORD_Meter, vCenter);

	//m_vDrawInsts.push_back(pNewEnt);
}

/// Draw a line segment.
/*virtual*/ void HyDebugPhys2d::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
{
	//HyEntity2d *pNewEnt = new HyEntity2d();
	//HyPrimitive2d *pInst = pNewEnt->SetPrimitive();

	//vec2 ptVerts[2];
	//memcpy(&ptVerts[0], &p1, sizeof(b2Vec2));
	//memcpy(&ptVerts[1], &p2, sizeof(b2Vec2));

	//pInst->Color().Set(color.r, color.g, color.b, 1.0f);
	//pInst->SetAsEdgeChain(ptVerts, 2, false, HYCOORD_Meter);

	//m_vDrawInsts.push_back(pNewEnt);
}

/// Draw a transform. Choose your own length scale.
/// @param xf a transform.
/*virtual*/ void HyDebugPhys2d::DrawTransform(const b2Transform& xf)
{
}
