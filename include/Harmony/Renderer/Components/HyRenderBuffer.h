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

class IHyDrawable;
class HyShaderUniforms;

class HyRenderBuffer
{
public:
	struct Header
	{
		uint32									uiNum3dRenderStates;
		uint32									uiNum2dRenderStates;
	};

	struct State
	{
		const uint32							uiID;						// Used for debugging
		const uint32							uiCAMERA_MASK;
		const uint32							uiDATA_OFFSET;
		const HyRenderMode						eRENDER_MODE;
		const HyTextureHandle					hTEXTURE_0;
		const HyShaderHandle					hSHADER;
		const HyScreenRect<int32>				SCISSOR_RECT;
		const HyStencilHandle					hSTENCIL;
		const int32								iCOORDINATE_SYSTEM;			// -1 (or any negative value) means using world/camera coordinates. Otherwise it represents the Window index
		const uint32							uiNUM_INSTANCES;
		const uint32							uiNUM_VERTS_PER_INSTANCE;	// Or total number of vertices if single instance

		uint32									m_uiExDataSize;
		
		State(uint32 uiId,
			uint32 uiCameraMask,
			uint32 uiDataOffset,
			HyRenderMode eRenderMode,
			HyTextureHandle hTexture,
			HyShaderHandle hShader,
			HyScreenRect<int32> &scissorRect,
			HyStencilHandle hStencil,
			int32 iCoordinateSystem,
			uint32 uiNumInstances,
			uint32 uiNumVerticesPerInstance) :	uiID(uiId),
												uiCAMERA_MASK(uiCameraMask),
												uiDATA_OFFSET(uiDataOffset),
												eRENDER_MODE(eRenderMode),
												hTEXTURE_0(hTexture),
												hSHADER(hShader),
												SCISSOR_RECT(scissorRect),
												hSTENCIL(hStencil),
												iCOORDINATE_SYSTEM(iCoordinateSystem),
												uiNUM_INSTANCES(uiNumInstances),
												uiNUM_VERTS_PER_INSTANCE(uiNumVerticesPerInstance),
												m_uiExDataSize(0)
		{
			HyAssert(hSHADER != HY_UNUSED_HANDLE, "HyRenderBuffer::State was assigned a null shader");
		}
	};

private:
	uint8 * const								m_pBUFFER;
	uint8 *										m_pCurWritePosition;

	uint8 *										m_pRenderStatesUserStartPos;

public:
	HyRenderBuffer();
	~HyRenderBuffer();

	Header *GetHeaderPtr();
	HyRenderBuffer::State *GetCurWritePosPtr();

	void Reset();
	void AppendRenderState(uint32 uiId, IHyDrawable &instanceRef, HyCameraMask uiCameraMask, HyScreenRect<int32> &scissorRectRef, HyStencilHandle hStencil, int32 iCoordinateSystem, uint32 uiDataOffset, uint32 uiNumInstances, uint32 uiNumVerticesPerInstance);
	void CreateRenderHeader();

private:
	void AppendShaderUniforms(const HyShaderUniforms &shaderUniformRef);
};

#endif /* HyRenderBuffer_h__ */
