/**************************************************************************
 *	HyDrawText2d.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Renderer/DrawData/HyDrawText2d.h"
#include "Creator/Data/HyText2dData.h"
#include "freetype-gl/freetype-gl.h"

HyDrawText2d::HyDrawText2d(HyText2d &inst, uint32 uiVertexDataOffset, char *&pCurVertexWritePos) : IDraw2d(inst.GetRenderState(), uiVertexDataOffset, sizeof(HyDrawText2d))
{
	inst.GetWorldTransform(m_mtxTransform);

	m_uiTextureId = inst.GetTextureId();
	m_uiNumCharacters = inst.GetStrLen();

	m_vColorAlpha = inst.Color().Get();

	memcpy(pCurVertexWritePos, inst.GetBufferDataPtr(), inst.GetBufferSizeBytes());
	pCurVertexWritePos += inst.GetBufferSizeBytes();
}

HyDrawText2d::~HyDrawText2d(void)
{
}
