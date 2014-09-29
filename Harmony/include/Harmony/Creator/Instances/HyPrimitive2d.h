/**************************************************************************
 *	HyPrimitive2d.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyPrimitive2d_h__
#define __HyPrimitive2d_h__

#include "Afx/HyStdAfx.h"

#include "Creator/Instances/IObjInst2d.h"

class HyPrimitive2d : public IObjInst2d
{
	vec4 *			m_pVertices;
	uint32			m_uiNumVerts;

public:
	HyPrimitive2d();
	virtual ~HyPrimitive2d(void);

	const HyPrimitive2d &HyPrimitive2d::operator=(const HyPrimitive2d& p);

	uint32 GetNumVerts()		{ return m_uiNumVerts; }
	vec4 *GetVertexDataPtr()	{ return m_pVertices; }

	void SetAsBox(float fHalfWidth, float fHalfHeight, bool bWireframe, HyCoordinateType eCoordType = HYCOORD_Pixel, vec2 &vOffset = vec2(0.0f));

	void SetAsCircle(float fRadius, int32 iNumSegments, bool bWireframe, HyCoordinateType eCoordType = HYCOORD_Pixel, vec2 &vOffset = vec2(0.0f));

	void SetAsEdgeChain(const vec2 *pVertices, uint32 uiNumVerts, bool bChainLoop, HyCoordinateType eCoordType = HYCOORD_Pixel, vec2 &vOffset = vec2(0.0f));

	void OffsetVerts(vec2 vOffset, float fAngleOffset, HyCoordinateType eCoordType = HYCOORD_Pixel);

private:
	virtual void OnDataLoaded()	{ }
	virtual void Update()		{ }
};

#endif /* __HyPrimitive2d_h__ */
