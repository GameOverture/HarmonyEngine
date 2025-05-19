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

#define HY_VERTEX_BUFFER_SIZE ((1024 * 1024) * 4) // 4MB
#define HY_INDEX_BUFFER_SIZE ((1024 * 1024) * 1) // 1MB

class IHyRenderer;

class HyVertexBuffer
{
	IHyRenderer &							m_RendererRef;

	struct Buffer
	{
		uint32								m_hGfxApiHandle;
		uint8 * const						m_pBUFFER;
		uint8 *								m_pCurWritePosition;

		Buffer(size_t uiNumBytes) :
			m_hGfxApiHandle(HY_UNUSED_HANDLE),
			m_pBUFFER(HY_NEW uint8[uiNumBytes]),
			m_pCurWritePosition(m_pBUFFER)
		{
		#ifdef HY_DEBUG
			memset(m_pBUFFER, 0, uiNumBytes);
		#endif
		}

		~Buffer()
		{
			delete[] m_pBUFFER;
		}
	};
	Buffer									m_Buffer2d;
	Buffer									m_Indices;

public:
	HyVertexBuffer(IHyRenderer &rendererRef);
	~HyVertexBuffer();

	void Initialize();	// Should be invoked once IHyRenderer::GenerateVertexBuffer() is valid
	void Reset();
	uint32 GetNumUsedVertexBytes();
	uint32 GetNumUsedIndicesBytes();
	uint32 GetVertexApiHandle();
	uint32 GetIndicesApiHandle();
	void AppendVertexData(const void *pData, uint32 uiSize);
	void AppendIndicesData(const uint16_t *pData, int32 iNumIndices);
	uint8 * const GetVertexData();
	uint8 *const GetIndicesData();

	//HyVertexBufferHandle AddDataWithHandle(const uint8 *pData, uint32 uiSize);
	//uint32 GetByteOffset(HyVertexBufferHandle hHandle);
};

#if HY_VERTEX_BUFFER_SIZE > ((1024 * 1024) * 16) // 16MB
	#error "HY_VERTEX_BUFFER_SIZE is larger than 16MB. Only 3 bytes is used to map byte offsets"
#endif

#if HY_INDEX_BUFFER_SIZE > ((1024 * 1024) * 16) // 16MB
#error "HY_INDEX_BUFFER_SIZE is larger than 16MB. Only 3 bytes is used to map byte offsets"
#endif

#endif /* HyVertexBuffer_h__ */
