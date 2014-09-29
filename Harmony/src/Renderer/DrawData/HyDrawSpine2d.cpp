/**************************************************************************
 *	HyDrawSpine2d.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Renderer/DrawData/HyDrawSpine2d.h"
#include "Utilities/HySpineRuntime.h"

HyDrawSpine2d::HyDrawSpine2d(HySpine2d &inst, uint32 uiVertexDataOffset, char *&pCurVertexWritePos) :	IDraw2d(inst.GetRenderState(), uiVertexDataOffset, sizeof(HyDrawSpine2d)),
																										m_uiTextureId(0)
{
	inst.GetWorldTransform(m_mtxTransform);

	m_uiNumSprites = 0;
	spSlot *pCurSlot;
	for (int i = 0; i < inst.GetSkeleton()->slotCount; ++i)
	{
		pCurSlot = inst.GetSkeleton()->drawOrder[i];

		spAttachment* attachment = pCurSlot->attachment;
		if(!attachment || attachment->type != SP_ATTACHMENT_REGION)
			continue;

		m_uiNumSprites++;

		spRegionAttachment* regionAttachment = (spRegionAttachment*)attachment;

		m_uiTextureId = reinterpret_cast<HyTexture *>(reinterpret_cast<spAtlasRegion *>(regionAttachment->rendererObject)->page->rendererObject)->GetId();

		float fVertexPositions[8];
			
		spRegionAttachment_computeWorldVertices(regionAttachment, pCurSlot->skeleton->x, pCurSlot->skeleton->y, pCurSlot->bone, fVertexPositions);

		vec4 vVertColorRGBA;
		vVertColorRGBA.r = inst.GetSkeleton()->r * pCurSlot->r;
		vVertColorRGBA.g = inst.GetSkeleton()->g * pCurSlot->g;
		vVertColorRGBA.b = inst.GetSkeleton()->b * pCurSlot->b;
		vVertColorRGBA.a = inst.GetSkeleton()->a * pCurSlot->a;

		// Write each vertex and its attributes [POS->COLOR->UV]
		//
		// Special Case OpenGL: In order to terminate primitives in TRIANGLE_STRIP properly, I need to flip the [3]rd and [4]th vertex in the quad
		WriteVertexData(0, fVertexPositions, regionAttachment, &vVertColorRGBA, pCurVertexWritePos);
		WriteVertexData(1, fVertexPositions, regionAttachment, &vVertColorRGBA, pCurVertexWritePos);
		WriteVertexData(3, fVertexPositions, regionAttachment, &vVertColorRGBA, pCurVertexWritePos);
		WriteVertexData(2, fVertexPositions, regionAttachment, &vVertColorRGBA, pCurVertexWritePos);
	}
}


HyDrawSpine2d::~HyDrawSpine2d(void)
{
}

void HyDrawSpine2d::WriteVertexData(int32 iVertIndex, float *pVertexPositions, spRegionAttachment* regionAttachment, const vec4 *pVertColorRGBA, char *&pCurVertexWritePos)
{
	vec4 ptVertPosition;
	ptVertPosition.x = pVertexPositions[iVertIndex*2];
	ptVertPosition.y = pVertexPositions[iVertIndex*2+1];
	ptVertPosition.z = 0.0f;
	ptVertPosition.w = 1.0f;

	vec2 vVertUVs;
	vVertUVs.x = regionAttachment->uvs[iVertIndex*2];
	vVertUVs.y = regionAttachment->uvs[iVertIndex*2+1];

	// Write data
	memcpy(pCurVertexWritePos, &ptVertPosition, sizeof(vec4));
	pCurVertexWritePos += sizeof(vec4);

	memcpy(pCurVertexWritePos, pVertColorRGBA, sizeof(vec4));
	pCurVertexWritePos += sizeof(vec4);

	memcpy(pCurVertexWritePos, &vVertUVs, sizeof(vec2));
	pCurVertexWritePos += sizeof(vec2);
}
