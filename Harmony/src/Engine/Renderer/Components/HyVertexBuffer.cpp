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

HyVertexBuffer::HyVertexBuffer(IHyRenderer &rendererRef) :	m_RendererRef(rendererRef),
															m_Buffer2d(true)
{
	m_StaticBufferList.emplace_back(false);
}

HyVertexBuffer::~HyVertexBuffer()
{
}

void HyVertexBuffer::Initialize2d()
{
	m_Buffer2d.m_hGfxApiHandle = m_RendererRef.GenerateVertexBuffer();
}

void HyVertexBuffer::Reset2d()
{
	m_Buffer2d.m_pCurWritePosition = m_Buffer2d.m_pBUFFER;
}

uint32 HyVertexBuffer::GetNumUsedBytes2d()
{
	return static_cast<uint32>(m_Buffer2d.m_pCurWritePosition - m_Buffer2d.m_pBUFFER);
}

uint32 HyVertexBuffer::GetGfxApiHandle2d()
{
	return m_Buffer2d.m_hGfxApiHandle;
}

void HyVertexBuffer::AppendData2d(const void *pData, uint32 uiSize)
{
	HyAssert((static_cast<uint32>(m_Buffer2d.m_pCurWritePosition - m_Buffer2d.m_pBUFFER) + uiSize) < HY_VERTEX_BUFFER_SIZE_2D, "HyVertexBuffer::AppendData2d() has written passed its vertex bounds! Embiggen 'HY_VERTEX_BUFFER_SIZE_2D'");
	memcpy(m_Buffer2d.m_pCurWritePosition, pData, uiSize);
	m_Buffer2d.m_pCurWritePosition += uiSize;
}

uint8 * const HyVertexBuffer::GetData2d()
{
	return m_Buffer2d.m_pBUFFER;
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
