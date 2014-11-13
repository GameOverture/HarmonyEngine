/**************************************************************************
 *	HyDrawQuadBatch2d.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Renderer/DrawData/HyDrawSpine2d.h"
#include "Utilities/HySpineRuntime.h"

#include "Creator/Instances/HyPrimitive2d.h"

HyDrawQuadBatch2d::HyDrawQuadBatch2d(IObjInst2d *pInst, uint32 uiVertexDataOffset, char *&pCurVertexWritePos) :	IDraw2d(pInst->GetRenderState(), uiVertexDataOffset, sizeof(HyDrawQuadBatch2d)),
																												m_uiTextureId(0)
{
	if(pInst->GetInstType() == HYINST_Spine2d)
		m_uiTextureId = reinterpret_cast<HySpine2d *>(pInst)->GetTextureId();

	m_pCurDataWritePos = pCurVertexWritePos;
	TryBatchInst(pInst);
	pCurVertexWritePos = m_pCurDataWritePos;
}


HyDrawQuadBatch2d::~HyDrawQuadBatch2d(void)
{
}

void HyDrawQuadBatch2d::WriteVertexData(int32 iVertIndex, float *pVertexPositions, spRegionAttachment* regionAttachment, const vec4 *pVertColorRGBA, char *&pCurVertexWritePos)
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

/*virtual*/ bool HyDrawQuadBatch2d::TryBatchInst(IObjInst2d *pInst)
{
	if(pInst->GetInstType() == HYINST_Spine2d)
	{
		HySpine2d &inst = *(reinterpret_cast<HySpine2d *>(pInst));

		// Start new batch if textures don't match
		if(inst.GetTextureId() != m_uiTextureId)
			return false;

		inst.GetWorldTransform(m_mtxTransform);


		m_uiNumQuads = 0;
		spSlot *pCurSlot;
		for (int i = 0; i < inst.GetSkeleton()->slotCount; ++i)
		{
			pCurSlot = inst.GetSkeleton()->drawOrder[i];

			spAttachment* attachment = pCurSlot->attachment;
			if(attachment == NULL || attachment->type != SP_ATTACHMENT_REGION)
				continue;

			m_uiNumQuads++;

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
			WriteVertexData(0, fVertexPositions, regionAttachment, &vVertColorRGBA, m_pCurDataWritePos);
			WriteVertexData(1, fVertexPositions, regionAttachment, &vVertColorRGBA, m_pCurDataWritePos);
			WriteVertexData(3, fVertexPositions, regionAttachment, &vVertColorRGBA, m_pCurDataWritePos);
			WriteVertexData(2, fVertexPositions, regionAttachment, &vVertColorRGBA, m_pCurDataWritePos);
		}
	}

	return false;
	//if(pInst->GetInstType() == HYINST_Primitive2d && reinterpret_cast<HyPrimitive2d *>(pInst)->IsTextured())
	//	
	//if(pInst->GetInstType() == HYINST_Sprite2d || pInst->GetInstType() == HYISNT_Text2d || 
	//int iPassedTexId;
	//switch(
	//return m_kuiRenderState == pInst->GetRenderState() && m_uiTextureId == pInst->GetTex
}
