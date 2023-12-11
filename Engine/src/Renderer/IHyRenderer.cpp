/**************************************************************************
 *	IHyRenderer.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2012 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Afx/HyInteropAfx.h"
#include "Renderer/IHyRenderer.h"
#include "Window/HyWindow.h"
#include "Renderer/Effects/HyStencil.h"
#include "Scene/Nodes/Loadables/Bodies/Drawables/IHyDrawable2d.h"
#include "Assets/Files/HyGLTF.h"
#include "Assets/Nodes/HyPrefabData.h"
#include "HyEngine.h"

IHyRenderer *IHyRenderer::sm_pInstance = nullptr;

IHyRenderer::IHyRenderer(int32 iVSync, std::vector<HyWindow *> &windowListRef, HyDiagnostics &diagnosticsRef) :
	m_iVSync(iVSync),
	m_WindowListRef(windowListRef),
	m_DiagnosticsRef(diagnosticsRef),
	m_VertexBuffer(*this),
	m_pCurWindow(nullptr),
	m_pShaderQuadBatch(HY_NEW HyShader(HYSHADERPROG_QuadBatch)),
	m_pShaderPrimitive(HY_NEW HyShader(HYSHADERPROG_Primitive))
{
	HyAssert(sm_pInstance == nullptr, "IHyRenderer ctor called twice");
	sm_pInstance = this;

	// Built-in shaders
	m_pShaderQuadBatch->Finalize();
	m_pShaderPrimitive->Finalize();
}

IHyRenderer::~IHyRenderer(void)
{
	sm_pInstance = nullptr;

	for(auto iter = m_ShaderMap.begin(); iter != m_ShaderMap.end(); ++iter)
		delete iter->second;
	m_ShaderMap.clear();

	for(auto iter = m_StencilMap.begin(); iter != m_StencilMap.end(); ++iter)
		delete iter->second;
	m_StencilMap.clear();
}

void IHyRenderer::SetRendererInfo(const std::string &sApiName, const std::string &sVersion, const std::string &sVendor, const std::string &sRenderer, const std::string &sShader, int32 iMaxTextureSize, const std::string &sCompressedTextures)
{
	m_DiagnosticsRef.SetRendererInfo(sApiName, sVersion, sVendor, sRenderer, sShader, iMaxTextureSize, sCompressedTextures);
}

void IHyRenderer::PrepareBuffers(float fExtrapolatePercent)
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Init everything to beginning of buffers
	m_RenderBuffer.Reset();
	m_VertexBuffer.Reset2d();

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Write internal render states first, used by things like HyStencil
	for(auto iter = m_StencilMap.begin(); iter != m_StencilMap.end(); ++iter)
	{
		HyStencil *pStencil = iter->second;
		if(pStencil->IsMaskReady() == false && pStencil->ConfirmMaskReady() == false)
			continue;

		pStencil->SetRenderStatePtr(m_RenderBuffer.GetCurWritePosPtr());

		const std::vector<IHyDrawable2d *> &instanceListRef = pStencil->GetInstanceList();
		for(uint32 i = 0; i < static_cast<uint32>(instanceListRef.size()); ++i)
			AppendDrawable2d(0, *instanceListRef[i], HY_FULL_CAMERA_MASK, fExtrapolatePercent);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Set pointers to be ready for HyScene to call AppendDrawable
	m_RenderBuffer.CreateRenderHeader();
}

void IHyRenderer::AppendDrawable3d(uint32 uiId, IHyDrawable3d &instanceRef, HyCameraMask uiCameraMask, float fExtrapolatePercent)
{
	if(instanceRef.GetType() != HYTYPE_Prefab)
	{
		HyError("IHyRenderer::AppendDrawable3d - Unknown instance type");
		return;
	}

	instanceRef.OnUpdateUniforms(fExtrapolatePercent);
	
	const HyPrefabData *pData = static_cast<const HyPrefabData *>(instanceRef.AcquireData());
	pData->GetGltf()->AppendRenderStates(m_RenderBuffer);
}

void IHyRenderer::AppendDrawable2d(uint32 uiId, IHyDrawable2d &instanceRef, HyCameraMask uiCameraMask, float fExtrapolatePercent)
{
	instanceRef.OnUpdateUniforms(fExtrapolatePercent);
	m_RenderBuffer.AppendRenderState(uiId, instanceRef, uiCameraMask, m_VertexBuffer, fExtrapolatePercent);
}

HyVertexBufferHandle IHyRenderer::AppendVertexData3d(const uint8 *pData, uint32 uiSize)
{
	HyVertexBufferHandle hReturnHandle = 0;// m_VertexBuffer3d.AddDataWithHandle(pData, uiSize);

	return hReturnHandle;
}

void IHyRenderer::TxData(IHyFile *pData)
{
	m_RxDataQueue.push(pData);
}

std::queue<IHyFile *> &IHyRenderer::RxData()
{
	return m_TxDataQueue;
}

HyShaderHandle IHyRenderer::GetDefaultShaderHandle(HyType eType)
{
	switch(eType)
	{
	case HYTYPE_Sprite:
	case HYTYPE_TexturedQuad:
	case HYTYPE_Text:
	case HYTYPE_Spine:
		return m_pShaderQuadBatch->GetHandle();

	case HYTYPE_Primitive:
		return m_pShaderPrimitive->GetHandle();

	case HYTYPE_Prefab:
		return HYTYPE_Unknown; // TODO: write proper shader

	default:
		HyError("IHyRenderer::GetDefaultShaderHandle - Unknown instance type");
	}

	return HYTYPE_Unknown;
}

void IHyRenderer::ProcessMsgs()
{
	// HANDLE DATA MESSAGES (Which loads/unloads texture resources)
	while(m_RxDataQueue.empty() == false)
	{
		IHyFile *pData = m_RxDataQueue.front();
		m_RxDataQueue.pop();

		pData->OnRenderThread(*this);
		m_TxDataQueue.push(pData);
	}
}

void IHyRenderer::Render()
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Setup render state buffer
	HyRenderBuffer::Header *pRsHeader = m_RenderBuffer.GetHeaderPtr();
	
	uint8 *pRsBufferPos = nullptr;
	HyRenderBuffer::State *pCurRenderState = nullptr;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Render all Windows
	for(uint32 i = 0; i < static_cast<uint32>(m_WindowListRef.size()); ++i)
	{
		SetCurrentWindow(i);
		StartRender();

		pRsBufferPos = reinterpret_cast<uint8 *>(m_RenderBuffer.GetHeaderPtr());
		pRsBufferPos += sizeof(HyRenderBuffer::Header);

		Begin_3d();
		for(uint32 k = 0; k < pRsHeader->m_uiNum3dRenderStates; k++)
		{
			pCurRenderState = reinterpret_cast<HyRenderBuffer::State *>(pRsBufferPos);
			DrawRenderState_3d(pCurRenderState);
			pRsBufferPos += pCurRenderState->m_uiExDataSize + sizeof(HyRenderBuffer::State);
		}

		Begin_2d();
		HyWindow::CameraIterator2d cameraIter(m_pCurWindow->GetCamera2dList());
		HyAssert(cameraIter.IsEnd() == false, "No cameras have been created for this window");
		for(uint32 k = 0; k < pRsHeader->m_uiNum2dRenderStates; k++)
		{
			pCurRenderState = reinterpret_cast<HyRenderBuffer::State *>(pRsBufferPos);
			if(pCurRenderState->m_iCOORDINATE_SYSTEM < 0 || pCurRenderState->m_iCOORDINATE_SYSTEM == m_pCurWindow->GetIndex())
			{
				cameraIter.Reset();
				do
				{
					// Check the cull mask to exit rendering under this camera early if not in frustum
					if(pCurRenderState->m_iCOORDINATE_SYSTEM < 0 && 0 == (pCurRenderState->m_uiCAMERA_MASK & (1 << cameraIter.Get()->GetCameraBitFlag())))
					{
						++cameraIter;
						continue;
					}

					DrawRenderState_2d(pCurRenderState, cameraIter.Get());
					
					++cameraIter;
				} while(pCurRenderState->m_iCOORDINATE_SYSTEM < 0 && cameraIter.IsEnd() == false);	// Check whether there are other cameras to render from
			}

			pRsBufferPos += pCurRenderState->m_uiExDataSize + sizeof(HyRenderBuffer::State);
		}

		FinishRender();
	}
}

/*virtual*/ void IHyRenderer::SetCurrentWindow(uint32 uiIndex)
{
	m_pCurWindow = m_WindowListRef[uiIndex];
}

/*static*/ HyShader *IHyRenderer::FindShader(HyShaderHandle hHandle)
{
	if(hHandle != HY_UNUSED_HANDLE && sm_pInstance->m_ShaderMap.find(hHandle) != sm_pInstance->m_ShaderMap.end())
		return sm_pInstance->m_ShaderMap[hHandle];

	return nullptr;
}

/*static*/ void IHyRenderer::AddShader(HyShader *pShader)
{
	sm_pInstance->m_ShaderMap[pShader->GetHandle()] = pShader;
	sm_pInstance->TxData(pShader);
}

/*static*/ void IHyRenderer::RemoveShader(HyShader *pShader)
{
	// TODO: Unload shader in graphics API
	sm_pInstance->m_ShaderMap.erase(sm_pInstance->m_ShaderMap.find(pShader->GetHandle()));
}

/*static*/ HyStencil *IHyRenderer::FindStencil(HyStencilHandle hHandle)
{
	if(hHandle != HY_UNUSED_HANDLE && sm_pInstance->m_StencilMap.find(hHandle) != sm_pInstance->m_StencilMap.end())
		return sm_pInstance->m_StencilMap[hHandle];

	return nullptr;
}

/*static*/ void IHyRenderer::AddStencil(HyStencil *pStencil)
{
	sm_pInstance->m_StencilMap[pStencil->GetHandle()] = pStencil;
}

/*static*/ void IHyRenderer::RemoveStencil(HyStencil *pStencil)
{
	sm_pInstance->m_StencilMap.erase(sm_pInstance->m_StencilMap.find(pStencil->GetHandle()));
}
