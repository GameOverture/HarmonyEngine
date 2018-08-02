/**************************************************************************
 *	HyVertexBuffer.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2018 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyVertexBuffer_h__
#define HyVertexBuffer_h__

#include "Afx/HyStdAfx.h"

#define HY_VERTEX_BUFFER_SIZE ((1024 * 1024) * 2)		// 2MB

class HyVertexBuffer
{
public:
	uint8 * const									m_pBUFFER;
	uint8 *											m_pCurWritePosition;
	uint32											m_uiNumUsedBytes;

	HyVertexOffsetHandle							m_hNextHandle;
	std::map<HyVertexOffsetHandle, uint32>			m_OffsetHandleList;

public:
	HyVertexBuffer();
	~HyVertexBuffer();

	void Reset();

	HyVertexOffsetHandle AddDataWithHandle(const uint8 *pData, uint32 uiSize);
};

#endif /* HyVertexBuffer_h__ */
