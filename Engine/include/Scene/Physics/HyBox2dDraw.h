/**************************************************************************
 *	HyBox2dDraw.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2022 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyBox2dDraw_h__
#define HyBox2dDraw_h__

#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Loadables/Bodies/Objects/HyEntity2d.h"
#include "Scene/Nodes/Loadables/Bodies/Drawables/Objects/HyPrimitive2d.h"

class HyPhysicsGrid2d;

class HyBox2dDraw : public HyEntity2d, public b2Draw
{
	float						m_fPixelsPerMeter;
	std::vector<HyPrimitive2d>	m_DrawList;

	int32						m_iDrawIndex;

public:
	HyBox2dDraw(float fPixelsPerMeter);
	virtual ~HyBox2dDraw(void);

	void BeginFrame();

	// Draw a closed polygon provided in CCW order.
	virtual void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override;

	// Draw a solid closed polygon provided in CCW order.
	virtual void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override;

	// Draw a circle.
	virtual void DrawCircle(const b2Vec2& center, float radius, const b2Color& color) override;

	// Draw a solid circle.
	virtual void DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis, const b2Color& color) override;

	// Draw a line segment.
	virtual void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) override;

	// Draw a transform. Choose your own length scale.
	// @param xf a transform.
	virtual void DrawTransform(const b2Transform& xf) override;

	// Draw a point.
	virtual void DrawPoint(const b2Vec2& p, float size, const b2Color& color) override;

	void EndFrame();
};

#endif /* HyBox2dDraw_h__ */
