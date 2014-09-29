/**************************************************************************
 *	HyDrawText2d.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyDrawText2d_h__
#define __HyDrawText2d_h__

#include "Afx/HyStdAfx.h"

#include "Creator\Instances\HyText2d.h"
#include "Renderer\DrawData\IDraw2d.h"

class HyDrawText2d : public IDraw2d
{
	uint32			m_uiTextureId;
	uint32			m_uiNumCharacters;

	vec4			m_vColorAlpha;

public:
	HyDrawText2d(HyText2d &inst, uint32 uiVertexDataOffset, char *&pCurVertexWritePos);
	virtual ~HyDrawText2d(void);

	vec4 &			GetColorAlpha()		{ return m_vColorAlpha; }

	uint32			GetTextureId()		{ return m_uiTextureId; }
	uint32			GetNumChars()		{ return m_uiNumCharacters; }
};

#endif /* __HyDrawText2d_h__ */
