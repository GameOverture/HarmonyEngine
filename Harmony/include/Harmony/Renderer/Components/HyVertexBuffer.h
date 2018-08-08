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

#define HY_DYNAMIC_VERTEX_BUFFER_SIZE ((1024 * 1024) * 2) // 2MB
#define HY_VERTEX_BUFFER_SIZE ((1024 * 1024) * 2) // 2MB

class HyVertexBuffer
{
	struct Buffer
	{
		uint32								m_hGfxApiHandle;
		uint8 * const						m_pBUFFER;
		uint8 *								m_pCurWritePosition;

		Buffer(bool bIsDynamic) :	m_hGfxApiHandle(HY_UNUSED_HANDLE),
									m_pBUFFER(HY_NEW uint8[bIsDynamic ? HY_DYNAMIC_VERTEX_BUFFER_SIZE : HY_VERTEX_BUFFER_SIZE]),
									m_pCurWritePosition(m_pBUFFER)
		{
		#ifdef HY_DEBUG
			memset(m_pBUFFER, 0, bIsDynamic ? HY_DYNAMIC_VERTEX_BUFFER_SIZE : HY_VERTEX_BUFFER_SIZE);
		#endif
		}

		~Buffer()
		{
			delete[] m_pBUFFER;
		}
	};

	Buffer									m_DynamicBuffer;	// This get cleared every frame
	std::vector<Buffer>						m_StaticBufferList;	// Somewhat persistent data

public:
	HyVertexBuffer();
	~HyVertexBuffer();

	void SetGfxApiHandle2d(uint32 hGfxApiHandle);

	void ResetDynamicBuffer();
	uint32 GetCurByteOffset2d();
	uint8 *GetCurWritePosPtr2d();
	uint32 GetDynamicBufferGfxHandle();
	uint8 * const GetDynamicBufferData();

	void AppendDynamicData(const void *pData, uint32 uiSize);

	//HyVertexBufferHandle AddDataWithHandle(const uint8 *pData, uint32 uiSize);
	//uint32 GetByteOffset(HyVertexBufferHandle hHandle);
};

#if HY_DYNAMIC_VERTEX_BUFFER_SIZE > ((1024 * 1024) * 16) || HY_VERTEX_BUFFER_SIZE > ((1024 * 1024) * 16) // 16MB
	#error "HY_VERTEX_BUFFER_SIZE > 16MB"
#endif

#endif /* HyVertexBuffer_h__ */
