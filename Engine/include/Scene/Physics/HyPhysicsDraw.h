/**************************************************************************
 *	HyPhysicsDraw.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2022 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyPhysicsDraw_h__
#define HyPhysicsDraw_h__

#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Loadables/Bodies/Objects/HyEntity2d.h"
#include "Scene/Nodes/Loadables/Bodies/Drawables/Objects/HyPrimitive2d.h"

class HyPhysicsDraw : public HyEntity2d
{
	b2DebugDraw					m_DebugDraw;

	float						m_fPixelsPerMeter;
	std::vector<HyPrimitive2d>	m_DrawList;

	int32						m_iDrawIndex;

public:
	HyPhysicsDraw(float fPixelsPerMeter);
	virtual ~HyPhysicsDraw(void);

	b2DebugDraw *GetDrawPtr();
	void SetShowFlags(uint32 uiDiagFlags);

	void BeginFrame();

	void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, b2HexColor color); // Draw a closed polygon provided in CCW order.
	void DrawSolidPolygon(b2Transform transform, const b2Vec2 *vertices, int32_t vertexCount, float radius, b2HexColor color); // Draw a solid closed polygon provided in CCW order.
	void DrawCircle(const b2Vec2& center, float radius, b2HexColor color);
	void DrawSolidCircle(b2Transform transform, b2Vec2 center, float radius, b2HexColor color);
	void DrawSolidCapsule(b2Vec2 p1, b2Vec2 p2, float radius, b2HexColor color);
	void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, b2HexColor color);
	void DrawTransform(const b2Transform& xf);
	void DrawPoint(const b2Vec2& p, float size, b2HexColor color);
	void DrawString(b2Vec2 p, const char *s, b2HexColor color);

	void EndFrame();
};

#endif /* HyPhysicsDraw_h__ */
