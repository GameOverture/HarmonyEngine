/**************************************************************************
 *	HyDrawPrimitive2d.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyDrawPrimitive2d_h__
#define __HyDrawPrimitive2d_h__

#include "Afx/HyStdAfx.h"

#include "IDraw2d.h"

#include "Creator/Instances/HyPrimitive2d.h"

class HyDrawPrimitive2d : public IDraw2d
{
	uint32				m_uiNumVerts;
	vec4				m_vColorAlpha;

public:
	HyDrawPrimitive2d(HyPrimitive2d &inst, uint32 uiVertexDataOffset, char *&pCurVertexWritePos);
	//HyDrawPrimitive2d(vec4 *pVerts, uint32 uiNumVerts, vec4 vColorAlpha, uint32 uiVertexDataOffset, char *&pCurVertexWritePos);
	~HyDrawPrimitive2d(void);

	const vec4 & GetColorAlpha()			{ return m_vColorAlpha; }
	uint32 GetNumVerts()					{ return m_uiNumVerts; }
};

#endif /* __HyDrawPrimitive2d_h__ */
