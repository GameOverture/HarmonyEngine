/**************************************************************************
 *	HyDrawSprite2d.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyDrawSprite2d_h__
#define __HyDrawSprite2d_h__

#include "Afx/HyStdAfx.h"

#include "Renderer\DrawData\IDraw2d.h"

#include "Creator/Instances/HySprite2d.h"

class HyDrawSprite2d : public IDraw2d
{
	uint32		m_uiTextureId;
public:
	HyDrawSprite2d(HySprite2d &inst, uint32 uiVertexDataOffset, char *&pCurVertexWritePos);
	virtual ~HyDrawSprite2d(void);

	void WriteVertex(const vec4 &ptVertPosition, const vec4 &vVertColorRGBA, const vec2 &vVertUVs, char *&pCurVertexWritePos);
};

#endif /* __HyDrawSprite2d_h__ */
