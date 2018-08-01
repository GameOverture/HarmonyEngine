/**************************************************************************
 *	HyRenderState.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Renderer/Components/HyRenderState.h"
#include "Renderer/Effects/HyStencil.h"
#include "Scene/Nodes/Loadables/Visables/IHyVisable.h"

HyRenderState::HyRenderState(uint32 uiId,
							 uint32 uiCullPassMask,
							 size_t uiDataOffset,
							 HyRenderMode eRenderMode,
							 HyTextureHandle hTexture,
							 HyShaderHandle hShader,
							 HyScreenRect<int32> &scissorRect,
							 HyStencilHandle hStencil,
							 int32 iCoordinateSystem,
							 uint32 uiNumInstances,
							 uint32 uiNumVerticesPerInstance) :	m_uiID(uiId),
																m_uiCULL_PASS_MASK(uiCullPassMask),
																m_uiDATA_OFFSET(uiDataOffset),
																m_eRenderMode(eRenderMode),
																m_hTextureHandle(hTexture),
																m_hShader(hShader),
																m_ScissorRect(scissorRect),
																m_hStencil(hStencil),
																m_iCoordinateSystem(iCoordinateSystem),
																m_uiNumInstances(uiNumInstances),
																m_uiNumVerticesPerInstance(uiNumVerticesPerInstance),
																m_uiExDataSize(0)
{
	HyAssert(m_hShader != HY_UNUSED_HANDLE, "HyRenderState was assigned a null shader");
}

HyRenderState::~HyRenderState(void)
{
	HyError("~HyRenderState dtor was called somehow");
}

uint32 HyRenderState::GetId() const
{
	return m_uiID;
}

size_t HyRenderState::GetDataOffset() const
{
	return m_uiDATA_OFFSET;
}

uint32 HyRenderState::GetCullMask() const
{
	return m_uiCULL_PASS_MASK;
}

HyRenderMode HyRenderState::GetRenderMode() const
{
	return m_eRenderMode;
}

void HyRenderState::AppendInstances(uint32 uiNumInstsToAppend)
{
	m_uiNumInstances += uiNumInstsToAppend;
}

uint32 HyRenderState::GetNumInstances() const
{
	return m_uiNumInstances;
}

uint32 HyRenderState::GetNumVerticesPerInstance() const
{
	return m_uiNumVerticesPerInstance;
}

bool HyRenderState::IsScissorRect() const
{
	return m_ScissorRect.iTag != IHyVisable::SCISSORTAG_Disabled;
}

const HyScreenRect<int32> &HyRenderState::GetScissorRect() const
{
	return m_ScissorRect;
}

HyStencilHandle HyRenderState::GetStencilHandle() const
{
	return m_hStencil;
}

int32 HyRenderState::GetCoordinateSystem() const
{
	return m_iCoordinateSystem;
}

HyShaderHandle HyRenderState::GetShaderHandle() const
{
	return m_hShader;
}

HyTextureHandle HyRenderState::GetTextureHandle() const
{
	return m_hTextureHandle;
}

char *HyRenderState::GetExPtr()
{
	return reinterpret_cast<char *>(this) + sizeof(HyRenderState);
}

void HyRenderState::SetExSize(size_t uiSize)
{
	m_uiExDataSize = uiSize;
}

size_t HyRenderState::GetExSize() const
{
	return m_uiExDataSize;
}
