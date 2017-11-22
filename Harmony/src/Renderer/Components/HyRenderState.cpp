/**************************************************************************
 *	HyRenderState.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Renderer/Components/HyRenderState.h"
#include "Renderer/Components/HyStencil.h"
#include "Scene/Nodes/Draws/Instances/IHyDrawInst2d.h"
#include "Scene/Nodes/Draws/Instances/HyText2d.h"
#include "Scene/Nodes/Draws/Instances/HyPrimitive2d.h"

HyRenderState::HyRenderState(/*const*/ IHyDrawInst2d &instanceRef, uint32 uiCullPassMask, size_t uiDataOffset) :	m_uiCULL_PASS_MASK(uiCullPassMask),
																													m_uiDATA_OFFSET(uiDataOffset),
																													m_uiExDataSize(0)
{
	m_eRenderMode = instanceRef.GetRenderMode();
	m_hTextureHandle = instanceRef.GetTextureHandle();

	m_ScissorRect.iTag = 0;
	instanceRef.GetWorldScissor(m_ScissorRect);

	if(instanceRef.GetStencil())
		m_hStencil = instanceRef.GetStencil()->GetHandle();
	else
		m_hStencil = HY_UNUSED_HANDLE;

	m_iCoordinateSystem = instanceRef.GetCoordinateSystem();


	//instanceRef.SetCustomShader

	memset(m_hShaderList, 0, sizeof(HyShaderHandle) * HY_MAX_SHADER_PASSES_PER_INSTANCE);
	switch(instanceRef.GetType())
	{
	case HYTYPE_Sprite2d:
		m_hShaderList[0] = HYSHADERPROG_QuadBatch;
		m_uiNumInstances = 1;
		m_uiNumVerticesPerInstance = 4;
		break;

	case HYTYPE_TexturedQuad2d:
		m_hShaderList[0] = HYSHADERPROG_QuadBatch;
		m_uiNumInstances = 1;
		m_uiNumVerticesPerInstance = 4;
		break;

	case HYTYPE_Primitive2d:
		m_hShaderList[0] = HYSHADERPROG_Primitive;
		m_uiNumInstances = 1;
		m_uiNumVerticesPerInstance = static_cast<HyPrimitive2d &>(instanceRef).GetNumVerts();
		break;
		
	case HYTYPE_Text2d:
		m_hShaderList[0] = HYSHADERPROG_QuadBatch;
		m_uiNumInstances = static_cast<HyText2d &>(instanceRef).GetNumRenderQuads();
		m_uiNumVerticesPerInstance = 4;
		break;

	default:
		HyError("HyRenderState - Unknown instance type");
	}
}

HyRenderState::~HyRenderState(void)
{
	HyError("~HyRenderState dtor was called somehow");
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
	return m_ScissorRect.iTag != 0;
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

HyShaderHandle HyRenderState::GetShaderHandle(uint32 uiShaderPass) const
{
	HyAssert(uiShaderPass < HY_MAX_SHADER_PASSES_PER_INSTANCE, "HyRenderState::GetShaderHandle was passed an invalid index");
	return m_hShaderList[uiShaderPass];
}

HyTextureHandle HyRenderState::GetTextureHandle() const
{
	return m_hTextureHandle;
}

void HyRenderState::SetExSize(size_t uiSize)
{
	m_uiExDataSize = uiSize;
}

size_t HyRenderState::GetExSize() const
{
	return m_uiExDataSize;
}
