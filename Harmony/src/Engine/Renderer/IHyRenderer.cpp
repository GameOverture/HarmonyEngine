/**************************************************************************
 *	IHyRenderer.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2012 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Afx/HyInteropAfx.h"
#include "Renderer/IHyRenderer.h"
#include "Renderer/Components/HyWindow.h"
#include "Renderer/Effects/HyStencil.h"
#include "Scene/Nodes/Loadables/Visables/Drawables/IHyDrawable2d.h"
#include "Assets/Loadables/IHyLoadableData.h"
#include "Assets/Nodes/HyPrefabData.h"
#include "HyEngine.h"

IHyRenderer *IHyRenderer::sm_pInstance = nullptr;

IHyRenderer::IHyRenderer(HyDiagnostics &diagnosticsRef, std::vector<HyWindow *> &windowListRef) :	m_DiagnosticsRef(diagnosticsRef),
																									m_WindowListRef(windowListRef),
																									m_pCurWindow(nullptr),
																									m_pShaderQuadBatch(HY_NEW HyShader(HYSHADERPROG_QuadBatch)),
																									m_pShaderPrimitive(HY_NEW HyShader(HYSHADERPROG_Primitive)),
																									m_uiSupportedTextureFormats(HYTEXTURE_R8G8B8A8 | HYTEXTURE_R8G8B8)
{
	HyAssert(sm_pInstance == nullptr, "IHyRenderer ctor called twice");
	sm_pInstance = this;
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

void IHyRenderer::PrepareBuffers()
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Init everything to beginning of buffers
	m_RenderBuffer.Reset();
	m_VertexBuffer2d.Reset();

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Write internal render states first, used by things like HyStencil
	for(auto iter = m_StencilMap.begin(); iter != m_StencilMap.end(); ++iter)
	{
		HyStencil *pStencil = iter->second;
		if(pStencil->IsMaskReady() == false && pStencil->ConfirmMaskReady() == false)
			continue;

		pStencil->SetRenderStatePtr(reinterpret_cast<HyRenderState *>(m_RenderBuffer.m_pCurWritePosition));

		const std::vector<IHyDrawable2d *> &instanceListRef = pStencil->GetInstanceList();
		for(uint32 i = 0; i < static_cast<uint32>(instanceListRef.size()); ++i)
		{
			instanceListRef[i]->OnUpdateUniforms();
			AppendDrawable2d(0, *instanceListRef[i], HY_FULL_CAMERA_MASK);
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Set pointers to be ready for HyScene to call AppendDrawable
	m_RenderBuffer.PrepUserRenderState();
}

void IHyRenderer::AppendDrawable3d(uint32 uiId, IHyDrawable3d &instanceRef, HyCameraMask uiCameraMask)
{
	if(instanceRef.GetType() != HYTYPE_Prefab3d)
	{
		HyError("IHyRenderer::AppendDrawable3d - Unknown instance type");
		return;
	}
	
	const HyPrefabData *pData = static_cast<const HyPrefabData *>(instanceRef.AcquireData());
	pData->GetGltf()->AppendRenderStates(m_RenderBuffer);
}

void IHyRenderer::AppendDrawable2d(uint32 uiId, IHyDrawable2d &instanceRef, HyCameraMask uiCameraMask)
{
	uint32 uiNumInstances, uiNumVerticesPerInstance;
	switch(instanceRef.GetType())
	{
	case HYTYPE_Sprite2d:
	case HYTYPE_TexturedQuad2d:
		uiNumInstances = 1;
		uiNumVerticesPerInstance = 4;
		break;

	case HYTYPE_Primitive2d:
		uiNumInstances = 1;
		uiNumVerticesPerInstance = static_cast<HyPrimitive2d &>(instanceRef).GetNumVerts();
		break;
		
	case HYTYPE_Text2d:
		uiNumInstances = static_cast<HyText2d &>(instanceRef).GetNumRenderQuads();
		uiNumVerticesPerInstance = 4;
		break;

	default:
		HyError("HyRenderState - Unknown instance type");
	}

	m_RenderBuffer.AppendRenderState(uiId, instanceRef, uiCameraMask, m_VertexBuffer2d.m_uiNumUsedBytes, uiNumInstances, uiNumVerticesPerInstance);
	
	// OnWriteVertexData() is responsible for incrementing the draw pointer to after what's written
	instanceRef.AcquireData();
	instanceRef.OnWriteVertexData(m_VertexBuffer2d.m_pCurWritePosition);
	m_VertexBuffer2d.m_uiNumUsedBytes = static_cast<uint32>(m_VertexBuffer2d.m_pCurWritePosition - m_VertexBuffer2d.m_pBUFFER);
	HyAssert(m_VertexBuffer2d.m_uiNumUsedBytes < HY_VERTEX_BUFFER_SIZE, "IHyRenderer::AppendDrawable2d() has written passed its vertex bounds! Embiggen 'HY_VERTEX_BUFFER_SIZE'");
}

HyVertexOffsetHandle IHyRenderer::AppendVertexData3d(const uint8 *pData, uint32 uiSize)
{
	HyVertexOffsetHandle hReturnHandle = m_VertexBuffer3d.AddDataWithHandle(pData, uiSize);
	NewVertexData3d();

	return hReturnHandle;
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

	case HYTYPE_Prefab3d:
		return HYTYPE_Unknown; // TODO: write proper shader

	default:
		HyError("IHyRenderer::GetDefaultShaderHandle - Unknown instance type");
	}

	return HYTYPE_Unknown;
}

uint32 IHyRenderer::GetNumWindows()
{
	return static_cast<uint32>(m_WindowListRef.size());
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
	HY_PROFILE_BEGIN(HYPROFILERSECTION_Render)

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Setup render state buffer
	HyRenderBuffer::Header *pRsHeader = reinterpret_cast<HyRenderBuffer::Header *>(m_RenderBuffer.m_pRenderStatesUserStartPos);
	
	uint8 *pRsBufferPos = nullptr;
	HyRenderBuffer::State *pCurRenderState = nullptr;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Render all Windows
	for(uint32 i = 0; i < static_cast<uint32>(m_WindowListRef.size()); ++i)
	{
		SetCurrentWindow(i);
		StartRender();

		pRsBufferPos = m_RenderBuffer.m_pRenderStatesUserStartPos;
		pRsBufferPos += sizeof(HyRenderBuffer::Header);

		Begin_3d();
		for(uint32 k = 0; k < pRsHeader->uiNum3dRenderStates; k++)
		{
			pCurRenderState = reinterpret_cast<HyRenderBuffer::State *>(pRsBufferPos);
			DrawRenderState_3d(pCurRenderState);
			pRsBufferPos += pCurRenderState->m_uiExDataSize + sizeof(HyRenderBuffer::State);
		}

		Begin_2d();
		HyWindow::CameraIterator2d cameraIter(m_pCurWindow->GetCamera2dList());
		for(uint32 k = 0; k < pRsHeader->uiNum2dRenderStates; k++)
		{
			pCurRenderState = reinterpret_cast<HyRenderBuffer::State *>(pRsBufferPos);
			if(pCurRenderState->iCOORDINATE_SYSTEM < 0 || pCurRenderState->iCOORDINATE_SYSTEM == m_pCurWindow->GetIndex())
			{
				cameraIter.Reset();
				do
				{
					// Check the cull mask to exit rendering under this camera early if not in frustum
					if(pCurRenderState->iCOORDINATE_SYSTEM < 0 && 0 == (pCurRenderState->uiCAMERA_MASK & (1 << cameraIter.Get()->GetCameraBitFlag())))
					{
						++cameraIter;
						continue;
					}

					DrawRenderState_2d(pCurRenderState, cameraIter.Get());
					
					++cameraIter;
				} while(pCurRenderState->iCOORDINATE_SYSTEM < 0 && cameraIter.IsEnd() == false);	// Check whether there are other cameras to render from
			}

			pRsBufferPos += pCurRenderState->m_uiExDataSize + sizeof(HyRenderBuffer::State);
		}

		FinishRender();
	}
	
	HY_PROFILE_END
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
