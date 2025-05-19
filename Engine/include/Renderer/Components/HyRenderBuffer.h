/**************************************************************************
 *	HyRenderBuffer.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2018 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyRenderBuffer_h__
#define HyRenderBuffer_h__

#include "Afx/HyStdAfx.h"
#include "Utilities/HyMath.h"

typedef uint32 HyCameraMask;
#define HY_MAX_CAMERA_MASK_BITS 32
#define HY_FULL_CAMERA_MASK 0xFFFFFFFF

#define HY_RENDERSTATE_BUFFER_SIZE ((1024 * 1024) * 1)	// 1MB

class IHyDrawable2d;
class HyShaderUniforms;
class HyVertexBuffer;

class HyRenderBuffer
{
public:
	struct Header
	{
		uint32									m_uiNum3dRenderStates;
		uint32									m_uiNum2dRenderStates;
	};

	struct State
	{
		const uint32							m_uiID;							// Used for debugging
		const uint32							m_uiCAMERA_MASK;
		const uint32							m_uiDATA_OFFSET;				// Offset into vertex buffer
		const uint32							m_uiINDICES_OFFSET;				// Offset into indices buffer
		const HyRenderMode						m_eRENDER_MODE;
		const HyBlendMode						m_eBLEND_MODE;
		const HyShaderHandle					m_hSHADER;
		const HyStencilHandle					m_hSCISSOR_STENCIL;
		const HyStencilHandle					m_hSTENCIL;
		const int32								m_iCOORDINATE_SYSTEM;			// -1 (or any negative value) means using world/camera coordinates. Otherwise it represents the Window index
		
		uint32									m_uiNumInstances;				// Used with 'glDrawArraysInstanced'. When '0' is passed, render with glDrawElements instead
		const uint32							m_uiNUM_VERTS_PER_INSTANCE;		// Or total number of vertices or indices if single instance

																				//                  uint32       uint32 uint32    uint32       HY_SHADER_UNIFORM_NAME_LENGTH       uint32   XXX
		uint32									m_uiExDataSize;					// Buffer Layout:   [NumTexUnits][Tex 0][Tex 1]...[NumUniforms][Uniform Name for next var/val pair][varType][varValue]...
		
		State(uint32 uiId,
			uint32 uiCameraMask,
			uint32 uiDataOffset,
			uint32 uiIndicesOffset,
			HyRenderMode eRenderMode,
			HyBlendMode eBlendMode,
			HyShaderHandle hShader,
			HyStencilHandle hScissorStencil,
			HyStencilHandle hStencil,
			int16 iCoordinateSystem,
			uint32 uiNumInstances,
			uint32 uiNumVerticesPerInstance) :	m_uiID(uiId),
												m_uiCAMERA_MASK(uiCameraMask),
												m_uiDATA_OFFSET(uiDataOffset),
												m_uiINDICES_OFFSET(uiIndicesOffset),
												m_eRENDER_MODE(eRenderMode),
												m_eBLEND_MODE(eBlendMode),
												m_hSHADER(hShader),
												m_hSCISSOR_STENCIL(hScissorStencil),
												m_hSTENCIL(hStencil),
												m_iCOORDINATE_SYSTEM(iCoordinateSystem),
												m_uiNumInstances(uiNumInstances),
												m_uiNUM_VERTS_PER_INSTANCE(uiNumVerticesPerInstance),
												m_uiExDataSize(0)
		{
			HyAssert(m_hSHADER != HY_UNUSED_HANDLE, "HyRenderBuffer::State was assigned a null shader");
		}

		bool operator==(State &rhs)
		{
			return m_uiCAMERA_MASK == rhs.m_uiCAMERA_MASK &&
				   //m_uiDATA_OFFSET == rhs.m_uiDATA_OFFSET && // Don't check
				   //m_uiINDICES_OFFSET == rhs.m_uiINDICES_OFFSET && // Don't check
				   m_eRENDER_MODE == rhs.m_eRENDER_MODE &&
				   m_hSHADER == rhs.m_hSHADER &&
				   m_hSCISSOR_STENCIL == rhs.m_hSCISSOR_STENCIL &&
				   m_hSTENCIL == rhs.m_hSTENCIL &&
				   m_iCOORDINATE_SYSTEM == rhs.m_iCOORDINATE_SYSTEM &&
				   m_uiNUM_VERTS_PER_INSTANCE == rhs.m_uiNUM_VERTS_PER_INSTANCE;
		}
	};

private:
	uint8 * const								m_pBUFFER;
	uint8 *										m_pCurWritePosition;

	uint8 *										m_pRenderStatesUserStartPos;

	uint64										m_uiPrevUniformCrc;
	State *										m_pPrevRenderState;

public:
	HyRenderBuffer();
	~HyRenderBuffer();

	Header *GetHeaderPtr();
	HyRenderBuffer::State *GetCurWritePosPtr();

	void Reset();
	void AppendRenderState(uint32 uiId, IHyDrawable2d &instanceRef, HyCameraMask uiCameraMask, HyVertexBuffer &vertexBufferRef, float fExtrapolatePercent);
	void CreateRenderHeader();

private:
	void AppendExData(HyShaderUniforms &shaderUniformRef);
};

#endif /* HyRenderBuffer_h__ */
