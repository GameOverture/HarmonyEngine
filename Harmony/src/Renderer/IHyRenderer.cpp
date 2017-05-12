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

std::map<int32, IHyShader *>	IHyRenderer::sm_ShaderMap;
int32							IHyRenderer::sm_iShaderIdCount = HYSHADERPROG_CustomStartIndex;

IHyRenderer::IHyRenderer(HyGfxComms &gfxCommsRef, IHyInput &inputRef, std::vector<HyWindow *> &windowListRef) : m_GfxCommsRef(gfxCommsRef),
																												m_InputRef(inputRef),
																												m_WindowListRef(windowListRef)
{
	// TODO: Make the application's HyWindow (ref to 'm_WindowListRef') threadsafe
	for(uint32 i = 0; i < static_cast<uint32>(m_WindowListRef.size()); ++i)
		m_RenderSurfaces.push_back(HyRenderSurface(HYRENDERSURFACE_Window, i, m_WindowListRef[i]->GetResolution().x, m_WindowListRef[i]->GetResolution().y));
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

void IHyRenderer::StartUp()
{
#if defined(HYSETTING_MultithreadedRenderer) && !defined(HY_PLATFORM_GUI)
	m_pRenderThread = ThreadManager::Get()->BeginThread(_T("Render Thread"), THREAD_START_PROCEDURE(RenderThread), this);
#else
	Initialize();
#endif
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

void IHyRenderer::Update()
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Iterate through 'm_WindowListRef' to find any dirty RenderSurface's that need processing
	// TODO: Make the application's HyWindow (ref to 'm_WindowListRef') threadsafe
	for(uint32 i = 0; i < static_cast<uint32>(m_WindowListRef.size()); ++i)
	{
		HyWindowInfo &windowInfoRef = m_WindowListRef[i]->Update_Render();
		if(windowInfoRef.uiDirtyFlags)
		{
			HyRenderSurface *pRenderSurface = NULL;
			for(uint32 j = 0; j < m_RenderSurfaces.size(); ++j)
			{
				if(m_RenderSurfaces[j].GetType() == HYRENDERSURFACE_Window && m_RenderSurfaces[j].GetId() == i)
				{
					pRenderSurface = &m_RenderSurfaces[j];
					break;
				}
			}
			HyAssert(pRenderSurface, "Could not find associated render surface from application's window reference");

			if(windowInfoRef.uiDirtyFlags & HyWindowInfo::FLAG_Resolution)
			{
				glm::ivec2 vResolution = m_WindowListRef[i]->GetResolution();
				pRenderSurface->Resize(vResolution.x, vResolution.y);
			}

			OnRenderSurfaceChanged(*pRenderSurface, windowInfoRef.uiDirtyFlags);

			windowInfoRef.uiDirtyFlags = 0;
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Swap to newest draw buffers (is only thread-safe on Render thread)
	if(!m_GfxCommsRef.Render_TakeSharedPointers(m_pRxDataQueue, m_pTxDataQueue, m_pDrawBuffer))
	{
		//InteropSleep(10);

		//HyLogWarning("Renderer got stale buffer");
		return;
	}
	m_pDrawBufferHeader = reinterpret_cast<HyGfxComms::tDrawHeader *>(m_pDrawBuffer);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// HANDLE DATA MESSAGES (Which loads/unloads texture resources)
	//if(!m_pRxDataQueue->empty())
	//	HyLog("Renderer Data Queue: " << m_pRxDataQueue->size());

	while(!m_pRxDataQueue->empty())
	{
		IHyLoadableData *pData = m_pRxDataQueue->front();
		m_pRxDataQueue->pop();

		pData->OnRenderThread(*this);
		m_pTxDataQueue->push(pData);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Render all surfaces
	m_RenderSurfaceIter = m_RenderSurfaces.begin();
	while(m_RenderSurfaceIter != m_RenderSurfaces.end())
	{
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
}

void IHyRenderer::Draw2d()
{
	// Each render state will require its own draw. The order of these render states should be 
	// depth sorted with render states batched together to reduce state changes.
	m_pCurRenderState = GetRenderStatesPtr2d();
	memset(&m_PrevRenderState, 0, sizeof(HyRenderState));

	int32 iNumRenderStates = GetNumRenderStates2d();
	for(int32 i = 0; i < iNumRenderStates; ++i, ++m_pCurRenderState)
	{
		DrawRenderState_2d(*m_pCurRenderState);
		m_PrevRenderState = *m_pCurRenderState;
	}
}

void IHyRenderer::SetMonitorDeviceInfo(std::vector<HyMonitorDeviceInfo> &info)
{
	HyWindow::SetMonitorDeviceInfo(info);
}

bool IHyRenderer::PollPlatformApi()
{
#if defined(HY_PLATFORM_WINDOWS) && !defined(HY_PLATFORM_GUI)
	// TODO: return false when windows close message comes in or something similar
	MSG msg = { 0 };
	int32 iWindowIndex = 0;
	HWND hWnd = static_cast<HyOpenGL_Win *>(this)->GetHWND(iWindowIndex);
	while(hWnd != nullptr)
	{
		while(PeekMessage(&msg, hWnd, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			static_cast<HyInputInterop &>(m_InputRef).HandleMsg(iWindowIndex, m_RenderSurfaces[iWindowIndex].GetWidth(), m_RenderSurfaces[iWindowIndex].GetHeight(), msg);
		}

		iWindowIndex++;
		hWnd = static_cast<HyOpenGL_Win *>(this)->GetHWND(iWindowIndex);
	}
#endif

	return true;
}

/*static*/ void IHyRenderer::RenderThread(void *pParam)
{
	IHyRenderer *pRenderer = reinterpret_cast<IHyRenderer *>(pParam);

	if(false == pRenderer->Initialize())
		HyError("Renderer API's Initialize() failed");

	while(pRenderer->PollPlatformApi())
		pRenderer->Update();
}
