/**************************************************************************
 *	HyRenderBuffer.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2018 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Renderer/Components/HyRenderBuffer.h"
#include "Renderer/IHyRenderer.h"
#include "Scene/Nodes/Loadables/Visables/Drawables/IHyDrawable3d.h"
#include "Scene/Nodes/Loadables/Visables/Drawables/IHyDrawable2d.h"

HyRenderBuffer::HyRenderBuffer() :	m_pBUFFER(HY_NEW uint8[HY_RENDERSTATE_BUFFER_SIZE]),
									m_pCurWritePosition(m_pBUFFER),
									m_uiNumUsedBytes(0),
									m_pRenderStatesUserStartPos(nullptr)
{
#ifdef HY_DEBUG
	memset(m_pBUFFER, 0, HY_RENDERSTATE_BUFFER_SIZE);
#endif
}

HyRenderBuffer::~HyRenderBuffer()
{
	delete[] m_pBUFFER;
}

HyRenderBuffer::Header *HyRenderBuffer::GetHeaderPtr()
{
	return reinterpret_cast<Header *>(m_pRenderStatesUserStartPos);
}

HyRenderBuffer::State *HyRenderBuffer::GetCurWritePosPtr()
{
	return reinterpret_cast<State *>(m_pCurWritePosition);
}

void HyRenderBuffer::Reset()
{
	m_pCurWritePosition = m_pBUFFER;
	m_uiNumUsedBytes = 0;
	m_pRenderStatesUserStartPos = nullptr;
}

void HyRenderBuffer::AppendRenderState(uint32 uiId, IHyDrawable3d &instanceRef, HyCameraMask uiCameraMask, uint32 uiDataOffset, uint32 uiNumInstances, uint32 uiNumVerticesPerInstance)
{
	HyScreenRect<int32> scissorRect;
	instanceRef.GetWorldScissor(scissorRect);

	State *pRenderState = new (m_pCurWritePosition)State(uiId,
														 uiCameraMask,
														 uiDataOffset,
														 instanceRef.GetRenderMode(),
														 instanceRef.GetTextureHandle(),
														 instanceRef.GetShaderHandle(),
														 scissorRect,
														 (instanceRef.GetStencil() != nullptr && instanceRef.GetStencil()->IsMaskReady()) ? instanceRef.GetStencil()->GetHandle() : HY_UNUSED_HANDLE,
														 instanceRef.GetCoordinateSystem(),
														 uiNumInstances,
														 uiNumVerticesPerInstance);

	m_pCurWritePosition += sizeof(State);

	uint8 *pStartOfExData = m_pCurWritePosition;
	instanceRef.WriteShaderUniformBuffer(m_pCurWritePosition);
	pRenderState->m_uiExDataSize = (static_cast<uint32>(m_pCurWritePosition - pStartOfExData));
	HyAssert(static_cast<uint32>(m_pCurWritePosition - m_pBUFFER) < HY_RENDERSTATE_BUFFER_SIZE, "IHyRenderer::AppendDrawable2d() has written passed its render state bounds! Embiggen 'HY_RENDERSTATE_BUFFER_SIZE'");

	if(m_pRenderStatesUserStartPos)
	{
		Header *pHeader = reinterpret_cast<Header *>(m_pRenderStatesUserStartPos);
		pHeader->uiNum3dRenderStates++;
	}
}

void HyRenderBuffer::AppendRenderState(uint32 uiId, IHyDrawable2d &instanceRef, HyCameraMask uiCameraMask, uint32 uiDataOffset, uint32 uiNumInstances, uint32 uiNumVerticesPerInstance)
{
	HyScreenRect<int32> scissorRect;
	instanceRef.GetWorldScissor(scissorRect);

	State *pRenderState = new (m_pCurWritePosition)State(uiId,
														 uiCameraMask,
														 uiDataOffset,
														 instanceRef.GetRenderMode(),
														 instanceRef.GetTextureHandle(),
														 instanceRef.GetShaderHandle(),
														 scissorRect,
														 (instanceRef.GetStencil() != nullptr && instanceRef.GetStencil()->IsMaskReady()) ? instanceRef.GetStencil()->GetHandle() : HY_UNUSED_HANDLE,
														 instanceRef.GetCoordinateSystem(),
														 uiNumInstances,
														 uiNumVerticesPerInstance);

	m_pCurWritePosition += sizeof(State);

	uint8 *pStartOfExData = m_pCurWritePosition;
	instanceRef.WriteShaderUniformBuffer(m_pCurWritePosition);
	pRenderState->m_uiExDataSize = (static_cast<uint32>(m_pCurWritePosition - pStartOfExData));
	HyAssert(static_cast<uint32>(m_pCurWritePosition - m_pBUFFER) < HY_RENDERSTATE_BUFFER_SIZE, "IHyRenderer::AppendDrawable2d() has written passed its render state bounds! Embiggen 'HY_RENDERSTATE_BUFFER_SIZE'");

	if(m_pRenderStatesUserStartPos)
	{
		Header *pHeader = reinterpret_cast<Header *>(m_pRenderStatesUserStartPos);
		pHeader->uiNum2dRenderStates++;
	}
}

void HyRenderBuffer::CreateRenderHeader()
{
	m_pRenderStatesUserStartPos = m_pCurWritePosition;
	Header *pHeader = reinterpret_cast<Header *>(m_pCurWritePosition);
	memset(pHeader, 0, sizeof(Header));

	m_pCurWritePosition += sizeof(Header);
}
