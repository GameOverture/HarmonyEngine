/**************************************************************************
 *	IGfxApi.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Renderer/Interop/IGfxApi.h"
#include "Renderer/DrawData/IDraw2d.h"

IGfxApi::IGfxApi() :	m_pGfxComms(NULL),
						m_uiCurRenderState(0),
						m_fpDraw2d(NULL)
{
}

IGfxApi::~IGfxApi(void)
{
}

bool IGfxApi::Update()
{
	if(PollApi())
	{
		DrawBuffers();
		return true;
	}
	else
	{
		Shutdown();
		return false;
	}
}

/*virtual*/ void IGfxApi::DrawBuffers()
{
	// Swap to newest draw buffers (is only threadsafe on Render thread)
	if(!m_pGfxComms->Render_GetSharedPtrs(m_pMsgQueuePtr, m_pSendMsgQueuePtr, m_pDrawBufferPtr))
	{
		InteropSleep(10);
		return;
	}
	m_DrawpBufferHeader = reinterpret_cast<HyGfxComms::tDrawHeader *>(m_pDrawBufferPtr);

	ProcessGameMsgs();

	if(CheckDevice() == false)
		return;

	StartRender();

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

	reinterpret_cast<HyGfxComms::tDrawHeader *>(m_pDrawBufferPtr)->uiReturnFlags |= GFXFLAG_HasRendered;
}

void IGfxApi::ProcessGameMsgs()
{
	// Handle each command message first. Which loads/unloads gfx resources.
	while(!m_pMsgQueuePtr->empty())
	{
		IData *pData = m_pMsgQueuePtr->front();
		m_pMsgQueuePtr->pop();

		if(pData->GetLoadState() == HYLOADSTATE_Queued)
			pData->OnGfxLoad(*this);
		else
			pData->OnGfxRemove(*this);

		m_pSendMsgQueuePtr->push(pData);
	}
}

void IGfxApi::Draw2d()
{
	int32 iNumInstances = GetNumInsts2d();// *(reinterpret_cast<int32 *>(m_pDrawBufferPtr + m_DrawpBufferHeader->uiOffsetToInst2d));

	// Set the render state of the first instance to be rendered. This render state will be compared with other instances
	// to determine whether we need to switch states. The order of these instances should be depth sorted with render states
	// batched together to reduce state changes.
	m_pCurDataPtr = m_pDrawBufferPtr + m_DrawpBufferHeader->uiOffsetToInst2d + sizeof(int32); // Last sizeof(int32) is skipping number of 2dInsts
	m_pCurDrawData = reinterpret_cast<IDraw2d *>(m_pCurDataPtr);

	m_uiCurRenderState = 0;
	SetRenderState_2d(m_pCurDrawData->GetRenderState());
	m_uiCurRenderState = m_pCurDrawData->GetRenderState();

	for(int32 i = 0; i < iNumInstances; i++)
	{
		if(m_pCurDrawData->GetRenderState() != m_uiCurRenderState)
		{
			// Render state needs to change. End this current draw and begin a new render state.
			SetRenderState_2d(m_pCurDrawData->GetRenderState());
			m_uiCurRenderState = m_pCurDrawData->GetRenderState();
		}

		// Let API draw the instance (which should have set the function pointer prior in SetRenderState_2d)
		m_fpDraw2d(m_pCurDrawData, this);

		// Get next instance (or crap data if this is the last one)
		m_pCurDataPtr += m_pCurDrawData->GetClassSizeBytes();
		m_pCurDrawData = reinterpret_cast<IDraw2d *>(m_pCurDataPtr);
	}
}

