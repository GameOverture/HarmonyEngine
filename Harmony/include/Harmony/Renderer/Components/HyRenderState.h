/**************************************************************************
 *	HyRenderState.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyRenderState_h__
#define HyRenderState_h__

#include "Afx/HyStdAfx.h"
#include "Renderer/IHyRenderer.h"
#include "Utilities/HyMath.h"

class IHyDrawInst2d;

class HyRenderState
{
	const uint32		m_uiCULL_PASS_MASK;
	const size_t		m_uiDATA_OFFSET;

	HyRenderMode		m_eRenderMode;
	HyTextureHandle		m_hTextureHandle;

	HyScreenRect<int32>	m_ScissorRect;
	HyStencilHandle		m_hStencil;

	int32				m_iCoordinateSystem;	// -1 (or any negative value) means using world/camera coordinates. Otherwise it represents the Window index

	HyShaderHandle		m_hShaderList[HY_MAX_SHADER_PASSES_PER_INSTANCE];

	uint32				m_uiNumInstances;
	uint32				m_uiNumVerticesPerInstance;		// Or total number of vertices if single instance

	size_t				m_uiExDataSize;

public:
	HyRenderState(/*const*/ IHyDrawInst2d &instanceRef, uint32 uiCullPassMask, size_t uiDataOffset);
	~HyRenderState(void);

	size_t GetDataOffset() const;
	uint32 GetCullMask() const;
	HyRenderMode GetRenderMode() const;

	void AppendInstances(uint32 uiNumInstsToAppend);
	uint32 GetNumInstances() const;
	uint32 GetNumVerticesPerInstance() const;

	bool IsScissorRect() const;
	const HyScreenRect<int32> &GetScissorRect() const;
	
	HyStencilHandle GetStencilHandle() const;

	int32 GetCoordinateSystem() const;

	HyShaderHandle GetShaderHandle(uint32 uiShaderPass) const;

	HyTextureHandle GetTextureHandle() const;

	void SetExSize(size_t uiSize);
	size_t GetExSize() const;
};

#endif /* HyRenderState_h__ */
