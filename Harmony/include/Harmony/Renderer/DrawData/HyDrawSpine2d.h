/**************************************************************************
 *	HyDrawQuadBatch2d.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyDrawData2d_h__
#define __HyDrawData2d_h__

#include "Afx/HyStdAfx.h"

#include "IDraw2d.h"

#include "Creator/Instances/HySpine2d.h"

#define RENDER_DATA_RESERVED_SIZE 8 // bytes

class HyDrawQuadBatch2d : public IDraw2d
{
	uint32		m_uiNumSprites;
	uint32		m_uiTextureId;

public:
	HyDrawQuadBatch2d(HySpine2d &inst, uint32 uiVertexDataOffset, char *&pCurVertexWritePos);
	virtual ~HyDrawQuadBatch2d(void);

	inline uint32 GetNumSprites()				{ return m_uiNumSprites; }
	inline uint32 GetTextureId()				{ return m_uiTextureId; }

	void WriteVertexData(int32 iVertIndex, float *pVertexPositions, spRegionAttachment* regionAttachment, const vec4 *pVertColorRGBA, char *&pCurVertexWritePos);
};

#endif /* __HyDrawData2d_h__ */
