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
#include "Scene/Nodes/Draws/Instances/IHyDrawInst2d.h"
#include "Scene/Nodes/Draws/Instances/HyText2d.h"
#include "Scene/Nodes/Draws/Instances/HyPrimitive2d.h"

HyRenderState::HyRenderState(uint32 uiId, uint32 uiCullPassMask, size_t uiDataOffset, /*const*/ IHyDrawInst2d &instanceRef) :	m_uiID(uiId),
																																m_uiCULL_PASS_MASK(uiCullPassMask),
																																m_uiDATA_OFFSET(uiDataOffset),
																																m_eRenderMode(instanceRef.GetRenderMode()),
																																m_hTextureHandle(instanceRef.GetTextureHandle()),
																																m_hShader(instanceRef.GetShaderHandle()),
																																m_hStencil((instanceRef.GetStencil() != nullptr && instanceRef.GetStencil()->IsMaskReady()) ? instanceRef.GetStencil()->GetHandle() : HY_UNUSED_HANDLE),
																																m_iCoordinateSystem(instanceRef.GetCoordinateSystem()),
																																m_uiExDataSize(0)
{
	HyAssert(m_hShader != HY_UNUSED_HANDLE, "HyRenderState was assigned a null shader");

	m_ScissorRect.iTag = HY_UNUSED_HANDLE;
	instanceRef.GetWorldScissor(m_ScissorRect);

	switch(instanceRef.GetType())
	{
	case HYTYPE_Sprite2d:
	case HYTYPE_TexturedQuad2d:
		m_uiNumInstances = 1;
		m_uiNumVerticesPerInstance = 4;
		break;

	case HYTYPE_Primitive2d:
		m_uiNumInstances = 1;
		m_uiNumVerticesPerInstance = static_cast<HyPrimitive2d &>(instanceRef).GetNumVerts();
		break;
		
	case HYTYPE_Text2d:
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
