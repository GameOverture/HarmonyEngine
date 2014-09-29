/**************************************************************************
 *	HyDrawSprite2d.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Renderer/DrawData/HyDrawSprite2d.h"

HyDrawSprite2d::HyDrawSprite2d(HySprite2d &inst, uint32 uiVertexDataOffset, char *&pCurVertexWritePos) : IDraw2d(inst.GetRenderState(), uiVertexDataOffset, sizeof(HyDrawSprite2d))
{
	inst.GetWorldTransform(m_mtxTransform);
	vec4 vColor = inst.Color().Get();

	if(inst.IsSimpleSprite())
	{
		HyRectangle rSrcRect;
		inst.GetSimpleSpriteData(m_uiTextureId, rSrcRect);

		vec4 ptVertPosition;
		vec2 vVertUVs;
		for(int i = 0; i < 4; ++i)
		{
			switch(i)
			{
			case 0:
				ptVertPosition.x = 0.0f;
				ptVertPosition.y = 0.0f;
				ptVertPosition.z = 0.0f;
				ptVertPosition.w = 1.0f;

				vVertUVs.x = 0.0f;
				vVertUVs.y = 0.0f;
				break;

			case 1:
				ptVertPosition.x = rSrcRect.width;
				ptVertPosition.y = 0.0f;
				ptVertPosition.z = 0.0f;
				ptVertPosition.w = 1.0f;

				vVertUVs.x = 1.0f;
				vVertUVs.y = 0.0f;
				break;

			case 2:
				ptVertPosition.x = rSrcRect.width;
				ptVertPosition.y = rSrcRect.height;
				ptVertPosition.z = 0.0f;
				ptVertPosition.w = 1.0f;

				vVertUVs.x = 1.0f;
				vVertUVs.y = 1.0f;
				break;

			case 3:
				ptVertPosition.x = 0.0f;
				ptVertPosition.y = rSrcRect.height;
				ptVertPosition.z = 0.0f;
				ptVertPosition.w = 1.0f;

				vVertUVs.x = 0.0f;
				vVertUVs.y = 1.0f;
				break;
			}

			WriteVertex(ptVertPosition, vColor, vVertUVs, pCurVertexWritePos);
		}
		
	}
}

HyDrawSprite2d::~HyDrawSprite2d(void)
{
}

void HyDrawSprite2d::WriteVertex(const vec4 &ptVertPosition, const vec4 &vVertColorRGBA, const vec2 &vVertUVs, char *&pCurVertexWritePos)
{
	// Write data
	memcpy(pCurVertexWritePos, &ptVertPosition, sizeof(vec4));
	pCurVertexWritePos += sizeof(vec4);

	memcpy(pCurVertexWritePos, &vVertColorRGBA, sizeof(vec4));
	pCurVertexWritePos += sizeof(vec4);

	memcpy(pCurVertexWritePos, &vVertUVs, sizeof(vec2));
	pCurVertexWritePos += sizeof(vec2);
}
