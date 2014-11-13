/**************************************************************************
 *	IDraw2d.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __IDraw2d_h__
#define __IDraw2d_h__

#include "Afx/HyStdAfx.h"

// Forward declaration
class IObjInst2d;

class IDraw2d
{
	static IDraw2d *	sm_pCurBatchDraw;

	const uint32		m_kuiRenderState;
	const uint32		m_kuiVertexDataOffset;
	const uint32		m_kuiClassSizeBtyes;

protected:
	mat4				m_mtxTransform;

public:
	IDraw2d(uint32 uiRenderState, uint32 uiVertexDataOffset, uint32 uiClassSizeBytes) :	m_kuiRenderState(uiRenderState),
																							m_kuiClassSizeBtyes(uiClassSizeBytes),
																							m_kuiVertexDataOffset(uiVertexDataOffset),
																							m_mtxTransform(1.0f)
	{ }

	virtual ~IDraw2d()
	{ }

	inline uint32 GetRenderState()		{ return m_kuiRenderState; }
	inline uint32 GetVertexDataOffset()	{ return m_kuiVertexDataOffset; }
	inline uint32 GetClassSizeBytes()	{ return m_kuiClassSizeBtyes; }

	const mat4 &GetTransformMtx()		{ return m_mtxTransform; }

	static bool BatchInst(IObjInst2d *pInst, char *pCurBatchWritePos, uint32 uiVertexDataOffset, char *&pCurVertexWritePos);
};

#endif /* __IDraw2d_h__ */
