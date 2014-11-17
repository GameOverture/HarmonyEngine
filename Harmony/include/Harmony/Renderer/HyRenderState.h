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

// Forward declaration
class IObjInst2d;

#define HY_MAX_TEXTURE_BINDS 3 // increase as needed up to glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &texture_units)

class HyRenderState
{
public:
	enum eAttributes
	{
		SCISSORTEST				= 1 << 0,
		USINGLOCALCOORDS		= 1 << 1,	// If disabled, then using world coordinates

		SHADER_PRIMITIVEDRAW	= 1 << 2,
		SHADER_QUADBATCH		= 1 << 3,
		SHADER_CUSTOM			= 1 << 4,
		SHADERMASK				= SHADER_PRIMITIVEDRAW | SHADER_QUADBATCH | SHADER_CUSTOM,

		DRAWMODE_TRIANGLESTRIP	= 1 << 5,
		DRAWMODE_TRIANGLEFAN	= 1 << 6,
		DRAWMODE_LINELOOP		= 1 << 7,
		DRAWMODE_LINESTRIP		= 1 << 8,
		DRAWMODEMASK			= DRAWMODE_TRIANGLESTRIP | DRAWMODE_TRIANGLEFAN | DRAWMODE_LINELOOP | DRAWMODE_LINESTRIP,

		//RS_CUSTOMSHADER_ID			= // Bits 9-14, store index to custom shader vector. Valid entries [0-63]
	};

private:
	uint32			m_uiAttributeFlags;
	uint32			m_pTextureBinds[HY_MAX_TEXTURE_BINDS];
	
	uint32			m_uiAux;
	uint32			m_uiDataOffset;

public:
	HyRenderState();
	~HyRenderState(void);

	void SetDataOffset(uint32 uiVertexDataOffset);
	uint32 GetDataOffset();
	
	void SetAux(uint32 uiAux);
	uint32 GetAux();

	void Enable(uint32 uiAttributes);
	void Disable(uint32 uiAttributes);
	bool CompareAttribute(const HyRenderState &rs, uint32 uiMask);
	bool IsEnabled(eAttributes eAttrib);

	uint32 GetTextureHandle(uint32 uiTextureIndex);

	bool operator==(const HyRenderState &right) const;
};

#endif /* __HyRenderState_h__ */
