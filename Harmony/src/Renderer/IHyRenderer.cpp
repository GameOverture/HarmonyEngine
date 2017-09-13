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
#include "Renderer/Components/HyWindow.h"
#include "Renderer/Components/HyRenderSurface.h"
#include "Renderer/Components/HyGfxComms.h"
#include "HyEngine.h"

std::map<int32, IHyShader *>	IHyRenderer::sm_ShaderMap;
int32							IHyRenderer::sm_iShaderIdCount = HYSHADERPROG_CustomStartIndex;

IHyRenderer::IHyRenderer(HyGfxComms &gfxCommsRef, HyDiagnostics &diagnosticsRef, bool bShowCursor, std::vector<HyWindow *> &windowListRef) :	m_GfxCommsRef(gfxCommsRef),
																																				m_DiagnosticsRef(diagnosticsRef),
																																				m_bShowCursor(bShowCursor),
																																				m_WindowListRef(windowListRef),
																																				m_uiSupportedTextureFormats(HYTEXTURE_R8G8B8A8 | HYTEXTURE_R8G8B8),
																																				m_uiNumRenderStates(0),
																																				m_bRequestedQuit(false)
{
	for(uint32 i = 0; i < static_cast<uint32>(m_WindowListRef.size()); ++i)
		m_RenderSurfaceList.push_back(HyRenderSurface(HYRENDERSURFACE_Window, i, m_WindowListRef[i]->GetResolution().x, m_WindowListRef[i]->GetResolution().y));
}

IHyRenderer::~IHyRenderer(void)
{
	std::map<int32, IHyShader *>::iterator iter;
	for(iter = sm_ShaderMap.begin(); iter != sm_ShaderMap.end(); ++iter)
		delete iter->second;

	// Needed for GUI reloads
	sm_ShaderMap.clear();
	sm_iShaderIdCount = HYSHADERPROG_CustomStartIndex;
}

void IHyRenderer::RequestQuit()
{
	m_bRequestedQuit = true;
}

bool IHyRenderer::IsQuitRequested()
{
	return m_bRequestedQuit;
}

HyGfxComms &IHyRenderer::GetGfxCommsRef()
{
	return m_GfxCommsRef;
}

void IHyRenderer::SetRendererInfo(const std::string &sApiName, const std::string &sVersion, const std::string &sVendor, const std::string &sRenderer, const std::string &sShader, int32 iMaxTextureSize, const std::string &sCompressedTextures)
{
	m_DiagnosticsRef.SetRendererInfo(sApiName, sVersion, sVendor, sRenderer, sShader, iMaxTextureSize, sCompressedTextures);
}

uint32 IHyRenderer::GetNumRenderSurfaces()
{
	return static_cast<uint32>(m_RenderSurfaceList.size());
}

std::vector<HyRenderSurface> &IHyRenderer::GetRenderSurfaceList()
{
	return m_RenderSurfaceList;
}

uint32 IHyRenderer::GetNumCameras2d()
{
	return *(reinterpret_cast<uint32 *>(m_pDrawBuffer + HYDRAWBUFFERHEADER->uiOffsetToCameras2d));
}

uint32 IHyRenderer::GetNumRenderStates2d()
{
	return *(reinterpret_cast<uint32 *>(m_pDrawBuffer + HYDRAWBUFFERHEADER->uiOffsetToInst2d));
}

uint32 IHyRenderer::GetCameraWindowIndex2d(int iCameraIndex)
{
	return *(reinterpret_cast<uint32 *>(m_pDrawBuffer +
										HYDRAWBUFFERHEADER->uiOffsetToCameras2d +
										sizeof(uint32) +
										(iCameraIndex * (sizeof(uint32) + sizeof(HyRectangle<float>) + sizeof(glm::mat4)))));
}

HyRectangle<float> *IHyRenderer::GetCameraViewportRect2d(int iIndex)
{
	return reinterpret_cast<HyRectangle<float> *>(m_pDrawBuffer +
												  HYDRAWBUFFERHEADER->uiOffsetToCameras2d +
												  sizeof(uint32) +
												  (iIndex * (sizeof(uint32) + sizeof(HyRectangle<float>) + sizeof(glm::mat4))) +
												  sizeof(uint32));
}

glm::mat4 *IHyRenderer::GetCameraView2d(int iIndex)
{
	return reinterpret_cast<glm::mat4 *>(m_pDrawBuffer +
										 HYDRAWBUFFERHEADER->uiOffsetToCameras2d +
										 sizeof(uint32) +
										 (iIndex * (sizeof(uint32) + sizeof(HyRectangle<float>) + sizeof(glm::mat4))) +
										 sizeof(uint32) +
										 sizeof(HyRectangle<float>));
}

HyRenderState *IHyRenderer::GetRenderStatesPtr2d()
{
	return reinterpret_cast<HyRenderState *>(m_pDrawBuffer + HYDRAWBUFFERHEADER->uiOffsetToInst2d + sizeof(uint32)); // Last sizeof(uint32) is skipping number of 2dInsts
}

char *IHyRenderer::GetVertexData2d()
{
	return reinterpret_cast<char *>(m_pDrawBuffer + HYDRAWBUFFERHEADER->uiOffsetToVertexData2d);
}

uint32 IHyRenderer::GetNumCameras3d()
{
	return *(reinterpret_cast<uint32 *>(m_pDrawBuffer + HYDRAWBUFFERHEADER->uiOffsetToCameras3d));
}

uint32 IHyRenderer::GetNumInsts3d()
{
	return *(reinterpret_cast<uint32 *>(m_pDrawBuffer + HYDRAWBUFFERHEADER->uiOffsetToCameras3d));
}

/*static*/ IHyShader *IHyRenderer::FindShader(int32 iId)
{
	if(sm_ShaderMap.find(iId) != sm_ShaderMap.end())
		return sm_ShaderMap[iId];

	HyError("IHyRenderer::FindShader could not find a valid shader");
	return NULL;
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

void IHyRenderer::Render()
{
	HY_PROFILE_BEGIN("Render")

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Iterate through 'm_WindowListRef' to find any dirty RenderSurface's that need processing
	for(uint32 i = 0; i < static_cast<uint32>(m_WindowListRef.size()); ++i)
		m_WindowListRef[i]->Update_Render(m_RenderSurfaceList[i]);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Swap to newest draw buffers (is only thread-safe on Render thread)
	//
	// TODO: Remove this!
	if(!m_GfxCommsRef.Render_TakeSharedPointers(m_pRxDataQueue, m_pTxDataQueue, m_pDrawBuffer))
	{
		HY_PROFILE_END
		return;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// HANDLE DATA MESSAGES (Which loads/unloads texture resources)
	while(!m_pRxDataQueue->empty())
	{
		IHyLoadableData *pData = m_pRxDataQueue->front();
		m_pRxDataQueue->pop();

		pData->OnRenderThread(*this);
		m_pTxDataQueue->push(pData);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Render all surfaces
	m_RenderSurfaceIter = m_RenderSurfaceList.begin();
	while(m_RenderSurfaceIter != m_RenderSurfaceList.end())
	{
		HyErrorCheck_OpenGL("Render", "Before StartRender");
		StartRender();

		Init_3d();
		while(BeginPass_3d())
		{
			//Draw3d();
			End_3d();
		}

		Init_2d();
		while(BeginPass_2d())
		{
			Draw2d();
			End_2d();
		}

		FinishRender();
		++m_RenderSurfaceIter;
	}

	reinterpret_cast<HyGfxComms::tDrawHeader *>(m_pDrawBuffer)->uiReturnFlags |= HyGfxComms::GFXFLAG_HasRendered;
	
	HY_PROFILE_END
}

void IHyRenderer::Draw2d()
{
	// Each render state will require its own draw. The order of these render states should be 
	// depth sorted with render states batched together to reduce state changes.
	m_pCurRenderState = GetRenderStatesPtr2d();
	memset(&m_PrevRenderState, 0, sizeof(HyRenderState));

	m_uiNumRenderStates = GetNumRenderStates2d();
	for(uint32 i = 0; i < m_uiNumRenderStates; ++i, ++m_pCurRenderState)
	{
		DrawRenderState_2d(*m_pCurRenderState);
		m_PrevRenderState = *m_pCurRenderState;
	}
}

void IHyRenderer::SetMonitorDeviceInfo(std::vector<HyMonitorDeviceInfo> &info)
{
	HyWindow::SetMonitorDeviceInfo(info);
}
