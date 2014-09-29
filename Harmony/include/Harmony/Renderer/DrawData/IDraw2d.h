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

class IDraw2d
{
	const uint32		m_kuiRenderStates;
	const uint32		m_kuiVertexDataOffset;
	const uint32		m_kuiClassSizeBtyes;

protected:
	mat4				m_mtxTransform;

public:
	IDraw2d(uint32 uiRenderStates, uint32 uiVertexDataOffset, uint32 uiClassSizeBytes) :	m_kuiRenderStates(uiRenderStates),
																							m_kuiClassSizeBtyes(uiClassSizeBytes),
																							m_kuiVertexDataOffset(uiVertexDataOffset),
																							m_mtxTransform(1.0f)
	{ }

	virtual ~IDraw2d()
	{ }

	inline uint32 GetRenderState()		{ return m_kuiRenderStates; }
	inline uint32 GetVertexDataOffset()	{ return m_kuiVertexDataOffset; }
	inline uint32 GetClassSizeBytes()	{ return m_kuiClassSizeBtyes; }

	const mat4 &GetTransformMtx()		{ return m_mtxTransform; }
};

#endif /* __IDraw2d_h__ */
