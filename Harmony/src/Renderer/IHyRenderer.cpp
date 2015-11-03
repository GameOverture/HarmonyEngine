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

void HyOpenGL_Win::RenderSurface::Resize(int32 iWidth, int32 iHeight)
{
	// Prevent A Divide By Zero
	if(iHeight == 0)
		iHeight = 1;

	if(m_iRenderSurfaceWidth == iWidth && m_iRenderSurfaceHeight == iHeight)
		return;

	m_iRenderSurfaceWidth = iWidth;
	m_iRenderSurfaceHeight = iHeight;

	m_bDirty = true;
}

void HyOpenGL_Win::RenderSurface::ClearDirtyFlag()
{
	m_bDirty = false;
}

IHyRenderer::IHyRenderer(HyGfxComms &gfxCommsRef, vector<HyWindow> &vWindowRef) :	m_GfxCommsRef(gfxCommsRef),
																					m_vWindowRef(vWindowRef)
{
	// TODO: Make m_vWindowRef threadsafe
	for(uint32 i = 0; i < static_cast<uint32>(m_vWindowRef.size()); ++i)
		m_RenderSurfaces.push_back(RenderSurface(RENDERSURFACE_Window, i, m_vWindowRef[i].GetResolution().x, m_vWindowRef[i].GetResolution().y));
}

IHyRenderer::~IHyRenderer(void)
{
}

void IHyRenderer::Update()
{
	// Swap to newest draw buffers (is only thread-safe on Render thread)
	if(!m_GfxCommsRef.Render_GetSharedPtrs(m_pMsgQueuePtr, m_pSendMsgQueuePtr, m_pDrawBufferPtr))
	{
		InteropSleep(10);
		return;
	}
	m_pDrawBufferHeader = reinterpret_cast<HyGfxComms::tDrawHeader *>(m_pDrawBufferPtr);

	// HANDLE DATA MESSAGES (Which loads/unloads texture resources)
	while(!m_pMsgQueuePtr->empty())
	{
		IHyData2d *pData = m_pMsgQueuePtr->front();
		m_pMsgQueuePtr->pop();

		const std::set<HyAtlasGroup *> &associatedAtlasesSetRef = pData->GetAssociatedAtlases();

		for(std::set<HyAtlasGroup *>::const_iterator iter = associatedAtlasesSetRef.begin(); iter != associatedAtlasesSetRef.end(); ++iter)
			(*iter)->OnRenderThread(*this);

		m_pSendMsgQueuePtr->push(pData);
	}

	m_RenderSurfaceIter = m_RenderSurfaces.begin();
	while(m_RenderSurfaceIter != m_RenderSurfaces.end())
	{
		StartRender();
		m_RenderSurfaceIter->ClearDirtyFlag();

		while(Begin_3d())
		{
			//Draw3d();
			End_3d();
		}

		while(Begin_2d())
		{
			Draw2d();
			End_2d();
		}

		FinishRender();
		++m_RenderSurfaceIter;
	}

	reinterpret_cast<HyGfxComms::tDrawHeader *>(m_pDrawBufferPtr)->uiReturnFlags |= HyGfxComms::GFXFLAG_HasRendered;
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
