/**************************************************************************
 *	IHyRenderer.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2012 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Afx/HyInteropAfx.h"
#include "Renderer/IHyRenderer.h"
#include "Renderer/Components/HyRenderState.h"
#include "Renderer/Components/HyWindow.h"
#include "Renderer/Components/HyStencil.h"
#include "Scene/Nodes/Draws/Instances/IHyDrawInst2d.h"
#include "Assets/Loadables/IHyLoadableData.h"
#include "HyEngine.h"

std::map<HyShaderHandle, HyShader *>	IHyRenderer::sm_ShaderMap;
std::map<HyStencilHandle, HyStencil *>	IHyRenderer::sm_StencilMap;

IHyRenderer::IHyRenderer(HyDiagnostics &diagnosticsRef, std::vector<HyWindow *> &windowListRef) :	m_DiagnosticsRef(diagnosticsRef),
																									m_WindowListRef(windowListRef),
																									m_pBUFFER_RENDERSTATES(HY_NEW char[HY_RENDERSTATE_BUFFER_SIZE]),
																									m_pBUFFER_VERTEX(HY_NEW char[HY_VERTEX_BUFFER_SIZE]),
																									m_pRenderStatesUserStartPos(nullptr),
																									m_pCurRenderStateWritePos(nullptr),
																									m_pCurVertexWritePos(nullptr),
																									m_uiVertexBufferUsedBytes(0),
																									m_pShaderQuadBatch(HY_NEW HyShader()),
																									m_pShaderPrimitive(HY_NEW HyShader()),
																									m_pCurWindow(nullptr),
																									m_uiSupportedTextureFormats(HYTEXTURE_R8G8B8A8 | HYTEXTURE_R8G8B8)
{
	HyShader::sm_pRenderer = this;

	memset(m_pBUFFER_VERTEX, 0, HY_VERTEX_BUFFER_SIZE);
	memset(m_pBUFFER_RENDERSTATES, 0, HY_RENDERSTATE_BUFFER_SIZE);
}

IHyRenderer::~IHyRenderer(void)
{
	HyShader::sm_pRenderer = nullptr;

	delete[] m_pBUFFER_VERTEX;
	delete[] m_pBUFFER_RENDERSTATES;

	for(auto iter = sm_ShaderMap.begin(); iter != sm_ShaderMap.end(); ++iter)
		delete iter->second;
	sm_ShaderMap.clear();

	for(auto iter = sm_StencilMap.begin(); iter != sm_StencilMap.end(); ++iter)
		delete iter->second;
	sm_StencilMap.clear();
}

void IHyRenderer::PrepareBuffers()
{
	// Init everything to beginning of buffers
	m_pCurRenderStateWritePos = m_pBUFFER_RENDERSTATES;
	m_pCurVertexWritePos = m_pBUFFER_VERTEX;
	m_pRenderStatesUserStartPos = nullptr;
	m_uiVertexBufferUsedBytes = 0;

	// Write internal render states first, used by things like HyStencil
	for(auto iter = sm_StencilMap.begin(); iter != sm_StencilMap.end(); ++iter)
	{
		HyStencil *pStencil = iter->second;
		pStencil->SetRenderStatePtr(reinterpret_cast<HyRenderState *>(m_pCurRenderStateWritePos));

		const std::vector<IHyDrawInst2d *> &instanceListRef = pStencil->GetInstanceList();
		for(uint32 i = 0; i < static_cast<uint32>(instanceListRef.size()); ++i)
		{
			instanceListRef[i]->OnUpdateUniforms();
			AppendRenderState(*instanceListRef[i], HY_FULL_CULL_MASK);
		}
	}

	// Set pointers to be ready for HyScene to call AppendRenderState()
	m_pRenderStatesUserStartPos = m_pCurRenderStateWritePos;
	IHyRenderer::RenderStateBufferHeader *pHeader = reinterpret_cast<IHyRenderer::RenderStateBufferHeader *>(m_pRenderStatesUserStartPos);
	memset(pHeader, 0, sizeof(IHyRenderer::RenderStateBufferHeader));

	m_pCurRenderStateWritePos += sizeof(IHyRenderer::RenderStateBufferHeader);
}

void IHyRenderer::AppendRenderState(/*const*/ IHyDrawInst2d &instanceRef, HyCullMask uiCullMask)
{
	HyRenderState *pRenderState = new (m_pCurRenderStateWritePos)HyRenderState(instanceRef,
																			   uiCullMask,
																			   m_uiVertexBufferUsedBytes);

	m_pCurRenderStateWritePos += sizeof(HyRenderState);

	char *pStartOfExData = m_pCurRenderStateWritePos;
	instanceRef.WriteShaderUniformBuffer(m_pCurRenderStateWritePos);	// This function is responsible for incrementing the draw pointer to after what's written
	pRenderState->SetExSize(reinterpret_cast<size_t>(m_pCurRenderStateWritePos) - reinterpret_cast<size_t>(pStartOfExData));
	HyAssert(reinterpret_cast<size_t>(m_pCurRenderStateWritePos) - reinterpret_cast<size_t>(m_pBUFFER_RENDERSTATES) < HY_RENDERSTATE_BUFFER_SIZE, "IHyRenderer::AppendRenderState() has written passed its render state bounds! Embiggen 'HY_RENDERSTATE_BUFFER_SIZE'");

	// OnWriteDrawBufferData() is responsible for incrementing the draw pointer to after what's written
	instanceRef.AcquireData();
	instanceRef.OnWriteDrawBufferData(m_pCurVertexWritePos);
	m_uiVertexBufferUsedBytes = reinterpret_cast<size_t>(m_pCurVertexWritePos) - reinterpret_cast<size_t>(m_pBUFFER_VERTEX);
	HyAssert(m_uiVertexBufferUsedBytes < HY_VERTEX_BUFFER_SIZE, "IHyRenderer::AppendRenderState() has written passed its vertex bounds! Embiggen 'HY_VERTEX_BUFFER_SIZE'");

	if(m_pRenderStatesUserStartPos)
	{
		IHyRenderer::RenderStateBufferHeader *pHeader = reinterpret_cast<IHyRenderer::RenderStateBufferHeader *>(m_pRenderStatesUserStartPos);
		pHeader->uiNum2dRenderStates++;
	}
}

void IHyRenderer::TxData(IHyLoadableData *pData)
{
	m_RxDataQueue.push(pData);
}

std::queue<IHyLoadableData *> &IHyRenderer::RxData()
{
	return m_TxDataQueue;
}

void IHyRenderer::SetRendererInfo(const std::string &sApiName, const std::string &sVersion, const std::string &sVendor, const std::string &sRenderer, const std::string &sShader, int32 iMaxTextureSize, const std::string &sCompressedTextures)
{
	m_DiagnosticsRef.SetRendererInfo(sApiName, sVersion, sVendor, sRenderer, sShader, iMaxTextureSize, sCompressedTextures);
}

HyShaderHandle IHyRenderer::GetDefaultShaderHandle(HyType eType)
{
	switch(eType)
	{
	case HYTYPE_Sprite2d:
	case HYTYPE_TexturedQuad2d:
	case HYTYPE_Text2d:
		return m_pShaderQuadBatch->GetHandle();

	case HYTYPE_Primitive2d:
		return m_pShaderPrimitive->GetHandle();

	default:
		HyError("IHyRenderer::GetDefaultShaderHandle - Unknown instance type");
	}

	return HYTYPE_Unknown;
}

uint32 IHyRenderer::GetNumWindows()
{
	return static_cast<uint32>(m_WindowListRef.size());
}

/*virtual*/ void IHyRenderer::SetCurrentWindow(uint32 uiIndex)
{
	m_pCurWindow = m_WindowListRef[uiIndex];
}

/*static*/ HyShader *IHyRenderer::FindShader(HyShaderHandle hHandle)
{
	if(hHandle != HY_UNUSED_HANDLE && sm_ShaderMap.find(hHandle) != sm_ShaderMap.end())
		return sm_ShaderMap[hHandle];

	return nullptr;
}

/*static*/ void IHyRenderer::AddShader(HyShader *pShader)
{
	sm_ShaderMap[pShader->GetHandle()] = pShader;
}

/*static*/ void IHyRenderer::RemoveShader(HyShader *pShader)
{
	// TODO: Unload shader in graphics API
	sm_ShaderMap.erase(sm_ShaderMap.find(pShader->GetHandle()));
}

/*static*/ HyStencil *IHyRenderer::FindStencil(HyStencilHandle hHandle)
{
	if(hHandle != HY_UNUSED_HANDLE && sm_StencilMap.find(hHandle) != sm_StencilMap.end())
		return sm_StencilMap[hHandle];

	return nullptr;
}

/*static*/ void IHyRenderer::AddStencil(HyStencil *pStencil)
{
	sm_StencilMap[pStencil->GetHandle()] = pStencil;
}

/*static*/ void IHyRenderer::RemoveStencil(HyStencil *pStencil)
{
	sm_StencilMap.erase(sm_StencilMap.find(pStencil->GetHandle()));
}

void IHyRenderer::ProcessMsgs()
{
	// HANDLE DATA MESSAGES (Which loads/unloads texture resources)
	while(m_RxDataQueue.empty() == false)
	{
		IHyLoadableData *pData = m_RxDataQueue.front();
		m_RxDataQueue.pop();

		pData->OnRenderThread(*this);
		m_TxDataQueue.push(pData);
	}
}

void IHyRenderer::Render()
{
	HY_PROFILE_BEGIN("Render")

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Setup render state buffer
	RenderStateBufferHeader *pRsHeader = reinterpret_cast<RenderStateBufferHeader *>(m_pRenderStatesUserStartPos);
	
	char *pRsBufferPos = nullptr;
	HyRenderState *pCurRenderState = nullptr;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Render all Windows
	for(uint32 i = 0; i < static_cast<uint32>(m_WindowListRef.size()); ++i)
	{
		SetCurrentWindow(i);
		StartRender();

		pRsBufferPos = m_pRenderStatesUserStartPos;
		pRsBufferPos += sizeof(RenderStateBufferHeader);

		Begin_3d();
		for(uint32 k = 0; k < pRsHeader->uiNum3dRenderStates; k++)
		{
			pCurRenderState = reinterpret_cast<HyRenderState *>(pRsBufferPos);
			DrawRenderState_3d(pCurRenderState);
			pRsBufferPos += pCurRenderState->GetExSize() + sizeof(HyRenderState);
		}

		Begin_2d();
		for(uint32 k = 0; k < pRsHeader->uiNum2dRenderStates; k++)
		{
			pCurRenderState = reinterpret_cast<HyRenderState *>(pRsBufferPos);
			if(pCurRenderState->GetCoordinateSystem() < 0 || pCurRenderState->GetCoordinateSystem() == m_pCurWindow->GetIndex())
				DrawRenderState_2d(pCurRenderState);

			pRsBufferPos += pCurRenderState->GetExSize() + sizeof(HyRenderState);
		}

		FinishRender();
	}
	
	HY_PROFILE_END
}
