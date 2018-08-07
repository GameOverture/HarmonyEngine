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

HyVertexBuffer::HyVertexBuffer() : m_DynamicBuffer(true)
{
	m_StaticBufferList.emplace_back(false);
}

HyVertexBuffer::~HyVertexBuffer()
{
}

void HyVertexBuffer::ResetDynamicBuffer()
{
	m_Buffer2d.m_pCurWritePosition = m_Buffer2d.m_pBUFFER;
	m_Buffer2d.m_uiNumUsedBytes = 0;
}

uint32 HyVertexBuffer::GetCurByteOffset2d()
{
	return m_Buffer2d.m_uiNumUsedBytes;
}

uint8 *HyVertexBuffer::GetCurWritePosPtr2d()
{
	return m_Buffer2d.m_pCurWritePosition;
}

void HyVertexBuffer::AppendDynamicData(int8 *pData, uint32 uiSize)
{
	HyAssert((m_DynamicBuffer.m_uiNumUsedBytes + uiSize) < HY_DYNAMIC_VERTEX_BUFFER_SIZE, "HyVertexBuffer::AppendDynamicData() has written passed its vertex bounds! Embiggen 'HY_DYNAMIC_VERTEX_BUFFER_SIZE'");
	memcpy(m_DynamicBuffer.m_pCurWritePosition

	m_DynamicBuffer.m_uiNumUsedBytes = static_cast<uint32>(m_DynamicBuffer.m_pCurWritePosition - m_VertexBuffer2d.m_pBUFFER);
}

//HyVertexBufferHandle HyVertexBuffer::AddDataWithHandle(const uint8 *pData, uint32 uiSize)
//{
//	HyVertexBufferHandle hReturnHandle = m_hNextHandle;
//	m_OffsetHandleMap[hReturnHandle] = m_uiNumUsedBytes;
//	m_hNextHandle++;
//
//	memcpy(m_pCurWritePosition, pData, uiSize);
//	m_pCurWritePosition += uiSize;
//	m_uiNumUsedBytes = static_cast<uint32>(m_pCurWritePosition - m_pBUFFER);
//	HyAssert(m_uiNumUsedBytes < HY_VERTEX_BUFFER_SIZE, "HyVertexBuffer::AddDataWithHandle() has written passed its vertex bounds! Embiggen 'HY_VERTEX_BUFFER_SIZE'");
//
//	return hReturnHandle;
//}
//
//uint32 HyVertexBuffer::GetByteOffset(HyVertexBufferHandle hHandle)
//{
//	return m_OffsetHandleMap.at(hHandle);
//}
