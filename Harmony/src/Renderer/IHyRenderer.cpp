/**************************************************************************
 *	IHyRenderer.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2012 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Renderer/IHyRenderer.h"

#include "Renderer/Viewport/HyWindow.h"

std::map<int32, IHyShader *>	IHyRenderer::sm_ShaderMap;
int32							IHyRenderer::sm_iShaderIdCount = HYSHADERPROG_CustomStartIndex;

IHyRenderer::RenderSurface::RenderSurface(eRenderSurfaceType eType, uint32 iID, int32 iRenderSurfaceWidth, int32 iRenderSurfaceHeight) :	m_eType(eType),
																																			m_iID(iID),
																																			m_iRenderSurfaceWidth(iRenderSurfaceWidth),
																																			m_iRenderSurfaceHeight(iRenderSurfaceHeight),
																																			m_pExData(NULL)
{ }

void IHyRenderer::RenderSurface::Resize(int32 iWidth, int32 iHeight)
{
	// Prevent A Divide By Zero
	if(iHeight == 0)
		iHeight = 1;

	if(m_iRenderSurfaceWidth == iWidth && m_iRenderSurfaceHeight == iHeight)
		return;

	m_iRenderSurfaceWidth = iWidth;
	m_iRenderSurfaceHeight = iHeight;
}

IHyRenderer::IHyRenderer(HyGfxComms &gfxCommsRef, vector<HyWindow *> &windowListRef) :	m_GfxCommsRef(gfxCommsRef),
																						m_vWindowRef(windowListRef)
{
	// TODO: Make the application's HyWindow (ref to 'm_vWindowRef') threadsafe
	for(uint32 i = 0; i < static_cast<uint32>(m_vWindowRef.size()); ++i)
		m_RenderSurfaces.push_back(RenderSurface(RENDERSURFACE_Window, i, m_vWindowRef[i]->GetResolution().x, m_vWindowRef[i]->GetResolution().y));
}

IHyRenderer::~IHyRenderer(void)
{
	std::map<int32, IHyShader *>::iterator iter;
	for(iter = sm_ShaderMap.begin(); iter != sm_ShaderMap.end(); ++iter)
		delete iter->second;
}

void IHyRenderer::Update()
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Iterate through 'm_vWindowRef' to find any dirty RenderSurface's that need processing
	// TODO: Make the application's HyWindow (ref to 'm_vWindowRef') threadsafe
	for(uint32 i = 0; i < static_cast<uint32>(m_vWindowRef.size()); ++i)
	{
		HyWindowInfo &windowInfoRef = m_vWindowRef[i]->Update_Render();
		if(windowInfoRef.uiDirtyFlags)
		{
			RenderSurface *pRenderSurface = NULL;
			for(uint32 j = 0; j < m_RenderSurfaces.size(); ++j)
			{
				if(m_RenderSurfaces[j].m_eType == RENDERSURFACE_Window && m_RenderSurfaces[j].m_iID == i)
				{
					pRenderSurface = &m_RenderSurfaces[j];
					break;
				}
			}
			HyAssert(pRenderSurface, "Could not find associated render surface from application's window reference");

			if(windowInfoRef.uiDirtyFlags & HyWindowInfo::FLAG_Resolution)
			{
				glm::ivec2 vResolution = m_vWindowRef[i]->GetResolution();
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
		return;
	}
	m_pDrawBufferHeader = reinterpret_cast<HyGfxComms::tDrawHeader *>(m_pDrawBuffer);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// HANDLE DATA MESSAGES (Which loads/unloads texture resources)
	while(!m_pRxDataQueue->empty())
	{
		IHy2dData *pData = m_pRxDataQueue->front();
		m_pRxDataQueue->pop();

		const std::set<HyAtlasGroup *> &associatedAtlasesSetRef = pData->GetAssociatedAtlases();
		for(std::set<HyAtlasGroup *>::const_iterator iter = associatedAtlasesSetRef.begin(); iter != associatedAtlasesSetRef.end(); ++iter)
			(*iter)->OnRenderThread(*this, pData);

		const std::set<IHyShader *> &associatedShadersSetRef = pData->GetAssociatedShaders();
		for(std::set<IHyShader *>::const_iterator iter = associatedShadersSetRef.begin(); iter != associatedShadersSetRef.end(); ++iter)
			(*iter)->OnRenderThread(*this, pData);

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

void IHyRenderer::SetMonitorDeviceInfo(vector<HyMonitorDeviceInfo> &info)
{
	HyWindow::SetMonitorDeviceInfo(info);
}

/*static*/ IHyShader *IHyRenderer::FindShader(int32 iId)
{
	if(sm_ShaderMap.find(iId) != sm_ShaderMap.end())
		return sm_ShaderMap[iId];
	else
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
