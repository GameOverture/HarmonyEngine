/**************************************************************************
 *	HyVertexBuffer.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2018 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Renderer/Components/HyVertexBuffer.h"
#include "Renderer/IHyRenderer.h"

HyVertexBuffer::HyVertexBuffer(IHyRenderer &rendererRef) :
	m_RendererRef(rendererRef),
	m_Buffer2d(HY_VERTEX_BUFFER_SIZE),
	m_Indices(HY_INDEX_BUFFER_SIZE)
{
	//m_StaticBufferList.emplace_back(false);
}

HyVertexBuffer::~HyVertexBuffer()
{
}

void HyVertexBuffer::Initialize()
{
	m_Buffer2d.m_hGfxApiHandle = m_RendererRef.GenerateVertexBuffer();
	m_Indices.m_hGfxApiHandle = m_RendererRef.GenerateIndexBuffer();
}

void HyVertexBuffer::Reset()
{
	m_Buffer2d.m_pCurWritePosition = m_Buffer2d.m_pBUFFER;
	m_Indices.m_pCurWritePosition = m_Indices.m_pBUFFER;
}

uint32 HyVertexBuffer::GetNumUsedVertexBytes()
{
	return static_cast<uint32>(m_Buffer2d.m_pCurWritePosition - m_Buffer2d.m_pBUFFER);
}

uint32 HyVertexBuffer::GetNumUsedIndicesBytes()
{
	return static_cast<uint32>(m_Indices.m_pCurWritePosition - m_Indices.m_pBUFFER);
}

uint32 HyVertexBuffer::GetVertexApiHandle()
{
	return m_Buffer2d.m_hGfxApiHandle;
}

uint32 HyVertexBuffer::GetIndicesApiHandle()
{
	return m_Indices.m_hGfxApiHandle;
}

void HyVertexBuffer::AppendVertexData(const void *pData, uint32 uiSize)
{
	HyAssert((static_cast<uint32>(m_Buffer2d.m_pCurWritePosition - m_Buffer2d.m_pBUFFER) + uiSize) < HY_VERTEX_BUFFER_SIZE, "HyVertexBuffer::AppendVertexData() has written passed its vertex bounds! Embiggen 'HY_VERTEX_BUFFER_SIZE'");
	memcpy(m_Buffer2d.m_pCurWritePosition, pData, uiSize);
	m_Buffer2d.m_pCurWritePosition += uiSize;
}

void HyVertexBuffer::AppendIndicesData(const uint16_t *pData, int32 iNumIndices)
{
	size_t uiSize = sizeof(uint16_t) * iNumIndices;
	HyAssert((static_cast<uint32>(m_Indices.m_pCurWritePosition - m_Indices.m_pBUFFER) + uiSize) < HY_INDEX_BUFFER_SIZE, "HyVertexBuffer::AppendIndicesData() has written passed its vertex bounds! Embiggen 'HY_INDEX_BUFFER_SIZE'");
	memcpy(m_Indices.m_pCurWritePosition, pData, uiSize);
	m_Indices.m_pCurWritePosition += uiSize;
}

uint8 * const HyVertexBuffer::GetVertexData()
{
	return m_Buffer2d.m_pBUFFER;
}

uint8 *const HyVertexBuffer::GetIndicesData()
{
	return m_Indices.m_pBUFFER;
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
