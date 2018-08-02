/**************************************************************************
 *	HyVertexBuffer.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2018 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Renderer/Components/HyVertexBuffer.h"

HyVertexBuffer::HyVertexBuffer() :	m_pBUFFER(HY_NEW uint8[HY_VERTEX_BUFFER_SIZE]),
									m_pCurWritePosition(m_pBUFFER),
									m_uiNumUsedBytes(0),
									m_hNextHandle(1)
{
#ifdef HY_DEBUG
	memset(m_pBUFFER, 0, HY_VERTEX_BUFFER_SIZE);
#endif
}

HyVertexBuffer::~HyVertexBuffer()
{
	delete[] m_pBUFFER;
}

void HyVertexBuffer::Reset()
{
	m_pCurWritePosition = m_pBUFFER;
	m_uiNumUsedBytes = 0;
}

HyVertexOffsetHandle HyVertexBuffer::AddDataWithHandle(const uint8 *pData, uint32 uiSize)
{
	HyVertexOffsetHandle hReturnHandle = m_hNextHandle;
	m_OffsetHandleMap[hReturnHandle] = m_uiNumUsedBytes;
	m_hNextHandle++;

	memcpy(m_pCurWritePosition, pData, uiSize);
	m_pCurWritePosition += uiSize;
	m_uiNumUsedBytes = static_cast<uint32>(m_pCurWritePosition - m_pBUFFER);
	HyAssert(m_uiNumUsedBytes < HY_VERTEX_BUFFER_SIZE, "HyVertexBuffer::AddDataWithHandle() has written passed its vertex bounds! Embiggen 'HY_VERTEX_BUFFER_SIZE'");

	return hReturnHandle;
}

uint32 HyVertexBuffer::GetByteOffset(HyVertexOffsetHandle hHandle)
{
	return m_OffsetHandleMap.at(hHandle);
}
