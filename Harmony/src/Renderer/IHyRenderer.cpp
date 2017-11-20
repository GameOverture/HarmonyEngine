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
#include "Assets/Loadables/IHyLoadableData.h"
#include "HyEngine.h"

int32												IHyRenderer::sm_iShaderIdCount = HYSHADERPROG_CustomStartIndex;
std::map<HyShaderHandle, IHyShader *>				IHyRenderer::sm_ShaderMap;
std::map<HyStencilHandle, HyStencil *>				IHyRenderer::sm_StencilMap;

IHyRenderer::IHyRenderer(HyDiagnostics &diagnosticsRef, std::vector<HyWindow *> &windowListRef) :	m_DiagnosticsRef(diagnosticsRef),
																									m_WindowListRef(windowListRef),
																									m_pBUFFER_RENDERSTATES(HY_NEW char[HY_RENDERSTATE_BUFFER_SIZE]),
																									m_pBUFFER_VERTEX(HY_NEW char[HY_VERTEX_BUFFER_SIZE]),
																									m_pRenderStatesUserStartPos(m_pBUFFER_RENDERSTATES),
																									m_uiVertexBufferUsedBytes(0),
																									m_pCurWindow(nullptr),
																									m_uiSupportedTextureFormats(HYTEXTURE_R8G8B8A8 | HYTEXTURE_R8G8B8)
{
	memset(m_pBUFFER_VERTEX, 0, HY_VERTEX_BUFFER_SIZE);
	memset(m_pBUFFER_RENDERSTATES, 0, HY_RENDERSTATE_BUFFER_SIZE);
}

IHyRenderer::~IHyRenderer(void)
{
	delete[] m_pBUFFER_VERTEX;
	delete[] m_pBUFFER_RENDERSTATES;

	std::map<HyShaderHandle, IHyShader *>::iterator iter;
	for(iter = sm_ShaderMap.begin(); iter != sm_ShaderMap.end(); ++iter)
		delete iter->second;

	// Needed for GUI reloads
	sm_ShaderMap.clear();
	sm_iShaderIdCount = HYSHADERPROG_CustomStartIndex;
}

void IHyRenderer::PrepareBuffers(char *&pRenderStateBufferOut, char *&pVertexBufferOut, uint32 &uiStartVertOffsetOut)
{
	pRenderStateBufferOut = m_pBUFFER_RENDERSTATES;
	m_pRenderStatesUserStartPos = m_pBUFFER_RENDERSTATES;

	pVertexBufferOut = m_pBUFFER_VERTEX;
	uiStartVertOffsetOut = 0;
}

void IHyRenderer::SetVertexBufferUsed(size_t uiNumBytes)
{
	m_uiVertexBufferUsedBytes = uiNumBytes;
	HyAssert(m_uiVertexBufferUsedBytes < HY_VERTEX_BUFFER_SIZE, "HyScene::WriteUpdateBuffer() has written passed its bounds! Embiggen 'HY_VERTEX_BUFFER_SIZE'");
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

uint32 IHyRenderer::GetNumWindows()
{
	return static_cast<uint32>(m_WindowListRef.size());
}

/*virtual*/ void IHyRenderer::SetCurrentWindow(uint32 uiIndex)
{
	m_pCurWindow = m_WindowListRef[uiIndex];
}

//uint32 IHyRenderer::GetNumCameras2d()
//{
//	return *(reinterpret_cast<uint32 *>(m_pVertexBuffer + HYDRAWBUFFERHEADER->uiOffsetToCameras2d));
//}
//
//uint32 IHyRenderer::GetCameraWindowId2d(int iCameraIndex)
//{
//	return *(reinterpret_cast<uint32 *>(m_pVertexBuffer +
//										HYDRAWBUFFERHEADER->uiOffsetToCameras2d +
//										sizeof(uint32) +
//										(iCameraIndex * (sizeof(uint32) + sizeof(HyRectangle<float>) + sizeof(glm::mat4)))));
//}
//
//HyRectangle<float> *IHyRenderer::GetCameraViewportRect2d(int iIndex)
//{
//	return reinterpret_cast<HyRectangle<float> *>(m_pVertexBuffer +
//												  HYDRAWBUFFERHEADER->uiOffsetToCameras2d +
//												  sizeof(uint32) +
//												  (iIndex * (sizeof(uint32) + sizeof(HyRectangle<float>) + sizeof(glm::mat4))) +
//												  sizeof(uint32));
//}
//
//glm::mat4 *IHyRenderer::GetCameraView2d(int iIndex)
//{
//	return reinterpret_cast<glm::mat4 *>(m_pVertexBuffer +
//										 HYDRAWBUFFERHEADER->uiOffsetToCameras2d +
//										 sizeof(uint32) +
//										 (iIndex * (sizeof(uint32) + sizeof(HyRectangle<float>) + sizeof(glm::mat4))) +
//										 sizeof(uint32) +
//										 sizeof(HyRectangle<float>));
//}

//uint32 IHyRenderer::GetNumRenderStates2d()
//{
//	return *(reinterpret_cast<uint32 *>(m_pVertexBuffer + HYDRAWBUFFERHEADER->uiOffsetTo2d));
//}
//
//HyRenderState *IHyRenderer::GetRenderStatesPtr2d()
//{
//	return reinterpret_cast<HyRenderState *>(m_pVertexBuffer + HYDRAWBUFFERHEADER->uiOffsetTo2d + sizeof(uint32)); // Last sizeof(uint32) is skipping number of 2dInsts
//}
//
//char *IHyRenderer::GetVertexData2d()
//{
//	return reinterpret_cast<char *>(m_pVertexBuffer + HYDRAWBUFFERHEADER->uiOffsetToVertexData2d);
//}

//uint32 IHyRenderer::GetNumCameras3d()
//{
//	return *(reinterpret_cast<uint32 *>(m_pVertexBuffer + HYDRAWBUFFERHEADER->uiOffsetToCameras3d));
//}
//
//uint32 IHyRenderer::GetNumInsts3d()
//{
//	return *(reinterpret_cast<uint32 *>(m_pVertexBuffer + HYDRAWBUFFERHEADER->uiOffsetToCameras3d));
//}

/*static*/ IHyShader *IHyRenderer::FindShader(HyShaderHandle hHandle)
{
	if(sm_ShaderMap.find(hHandle) != sm_ShaderMap.end())
		return sm_ShaderMap[hHandle];

	HyError("IHyRenderer::FindShader could not find a valid shader");
	return nullptr;
}

/*static*/ IHyShader *IHyRenderer::MakeCustomShader()
{
	IHyShader *pNewShader = HY_NEW HyShaderInterop(sm_iShaderIdCount);
	sm_ShaderMap[sm_iShaderIdCount] = pNewShader;

	sm_iShaderIdCount++;
	return pNewShader;
}

/*static*/ IHyShader *IHyRenderer::MakeCustomShader(const char *szPrefix, const char *szName)
{
	IHyShader *pNewShader = HY_NEW HyShaderInterop(sm_iShaderIdCount, szPrefix, szName);
	sm_ShaderMap[sm_iShaderIdCount] = pNewShader;

	sm_iShaderIdCount++;
	return pNewShader;
}

/*static*/ HyStencil *IHyRenderer::FindStencil(HyStencilHandle hHandle)
{
	if(hHandle != HY_UNUSED_HANDLE && sm_StencilMap.find(hHandle) != sm_StencilMap.end())
		return sm_StencilMap[hHandle];

	return nullptr;
}

/*static*/ void IHyRenderer::AddStencil(HyStencil *pNewStencil)
{
	sm_StencilMap[pNewStencil->GetHandle()] = pNewStencil;
}

/*static*/ void IHyRenderer::RemoveStencil(HyStencil *pNewStencil)
{
	sm_StencilMap.erase(sm_StencilMap.find(pNewStencil->GetHandle()));
}

void IHyRenderer::Render()
{
	HY_PROFILE_BEGIN("Render")

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// HANDLE DATA MESSAGES (Which loads/unloads texture resources)
	while(m_RxDataQueue.empty() == false)
	{
		IHyLoadableData *pData = m_RxDataQueue.front();
		m_RxDataQueue.pop();

		pData->OnRenderThread(*this);
		m_TxDataQueue.push(pData);
	}

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
