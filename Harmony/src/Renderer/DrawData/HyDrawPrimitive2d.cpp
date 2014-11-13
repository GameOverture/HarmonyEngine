/**************************************************************************
 *	HyDrawPrimitive2d.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Renderer/DrawData/HyDrawPrimitive2d.h"

HyDrawPrimitive2d::HyDrawPrimitive2d(HyPrimitive2d &inst, uint32 uiVertexDataOffset, char *&pCurVertexWritePos) :	IDraw2d(inst.GetRenderState(), uiVertexDataOffset, sizeof(HyDrawPrimitive2d)),
																													m_uiNumVerts(inst.GetNumVerts())
{
	inst.GetWorldTransform(m_mtxTransform);

	m_vColorAlpha = inst.Color().Get();
	memcpy(pCurVertexWritePos, inst.GetVertexDataPtr(), sizeof(vec4)*inst.GetNumVerts());
	pCurVertexWritePos += sizeof(vec4) * inst.GetNumVerts();
}

//HyDrawPrimitive2d::HyDrawPrimitive2d(vec4 *pVerts, uint32 uiNumVerts, vec4 vColorAlpha, uint32 uiVertexDataOffset, char *&pCurVertexWritePos) : IDraw2d(, uiVertexDataOffset, sizeof(HyDrawPrimitive2d))
//{
//	m_vColorAlpha = vColorAlpha;
//	memcpy(pCurVertexWritePos, pVerts, sizeof(vec4) * uiNumVerts);
//	pCurVertexWritePos += sizeof(vec4) * uiNumVerts;
//
//	m_mtxTransform = mat4(1.0f);
//}

HyDrawPrimitive2d::~HyDrawPrimitive2d(void)
{
}

/*virtual*/ bool HyDrawPrimitive2d::TryBatchInst(IObjInst2d *pInst)
{
	return false;
}
