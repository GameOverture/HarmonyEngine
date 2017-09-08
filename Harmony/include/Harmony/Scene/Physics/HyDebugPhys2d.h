/**************************************************************************
 *	HyDebugPhys2d.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyDebugPhys2d_h__
#define HyDebugPhys2d_h__

#include "Afx/HyStdAfx.h"

#include <vector>

// forward declaration
class HyEntity2d;

class HyDebugPhys2d : public b2Draw
{
	bool					m_bDrawEnabled;

	//vector<HyEntity2d *>	m_vDrawInsts;

public:
	HyDebugPhys2d(void);
	virtual ~HyDebugPhys2d(void);

	void SetDrawEnabled(bool bEnable)			{ m_bDrawEnabled = bEnable; }
	bool IsDrawEnabled()						{ return m_bDrawEnabled; }

	void Reset();

	//size_t GetNumPhysDraws()					{ return m_vDrawInsts.size(); }
	//HyEntity2d *GetInstPtr(int32 iIndex)		{ return m_vDrawInsts[iIndex]; }

	/// Draw a closed polygon provided in CCW order.
	virtual void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);

	/// Draw a solid closed polygon provided in CCW order.
	virtual void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);

	/// Draw a circle.
	virtual void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color);

	/// Draw a solid circle.
	virtual void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color);

	/// Draw a line segment.
	virtual void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color);

	/// Draw a transform. Choose your own length scale.
	/// @param xf a transform.
	virtual void DrawTransform(const b2Transform& xf);
};

#endif /* HyDebugPhys2d_h__ */
