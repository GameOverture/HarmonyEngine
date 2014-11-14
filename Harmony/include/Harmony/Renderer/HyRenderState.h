/**************************************************************************
 *	HyRenderState.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyRenderState_h__
#define __HyRenderState_h__

#include "Afx/HyStdAfx.h"

#define HY_MAX_TEXTURE_BINDS 8

class HyRenderState
{
public:
	enum eAttributes
	{
		RS_SCISSORTEST				= 1 << 0,
		RS_USINGLOCALCOORDS			= 1 << 1,	// If disabled, then using world coordinates

		RS_SHADER_PRIMITIVEDRAW		= 1 << 2,
		RS_SHADER_QUADBATCH			= 1 << 3,
		RS_SHADER_CUSTOM			= 1 << 4,
		RS_SHADERMASK				= RS_SHADER_PRIMITIVEDRAW | RS_SHADER_QUADBATCH | RS_SHADER_CUSTOM,

		RS_DRAWMODE_TRIANGLESTRIP	= 1 << 5,
		RS_DRAWMODE_LINELOOP		= 1 << 6,
		RS_DRAWMODE_LINESTRIP		= 1 << 7,
		RS_DRAWMODEMASK				= RS_DRAWMODE_TRIANGLESTRIP | RS_DRAWMODE_LINELOOP | RS_DRAWMODE_LINESTRIP

		//RS_CUSTOMSHADER_ID			= // Bits 8-13, store index to custom shader vector. Valid entries [0-63]
	};

private:
	uint32		m_uiAttributeFlags;

	uint32		m_pTextureBinds[HY_MAX_TEXTURE_BINDS];

public:
	HyRenderState(void);
	~HyRenderState(void);

	bool operator==(const HyRenderState &right) const;

	void Enable(uint32 uiAttributes);
	void Disable(uint32 uiAttributes);
};

#endif /* __HyRenderState_h__ */
