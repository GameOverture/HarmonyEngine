/**************************************************************************
 *	IDraw2d.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Renderer/DrawData/IDraw2d.h"

#include "Creator/Instances/IObjInst2d.h"

/*static*/ IDraw2d *	IDraw2d::sm_pCurBatchDraw = NULL;

/*static*/ bool IDraw2d::BatchInst(IObjInst2d *pInst, char *pCurBatchWritePos, uint32 uiVertexDataOffset, char *&pCurVertexWritePos)
{
	bool bMakeNewBatch = true;
	if(sm_pCurBatchDraw)
	{
		sm_pCurBatchDraw->GetRenderState() == pInst
	}
	
	if(bMakeNewBatch)
	{
		switch(pInst->GetInstType())
		{
			//case HYISNT_Text2d:
			//	new (m_pCurWritePos) HyDrawText2d(*reinterpret_cast<HyText2d *>(m_vLoadedInst2d[i]), uiVertexDataOffset, pCurVertexWritePos);
			//	m_pCurWritePos += sizeof(HyDrawText2d);
			//	break;
		case HYINST_Sprite2d:
		case HYINST_Spine2d:
		case HYISNT_Text2d:
			new (m_pCurWritePos) HyDrawQuadBatch2d(*reinterpret_cast<HySpine2d *>(m_vLoadedInst2d[i]), uiVertexDataOffset, pCurVertexWritePos);
			m_pCurWritePos += sizeof(HyDrawQuadBatch2d);
			break;
		case HYINST_Primitive2d:
			new (m_pCurWritePos) HyDrawPrimitive2d(*reinterpret_cast<HyPrimitive2d *>(m_vLoadedInst2d[i]), uiVertexDataOffset, pCurVertexWritePos);
			m_pCurWritePos += sizeof(HyDrawPrimitive2d);
			break;
		}
	}
}

