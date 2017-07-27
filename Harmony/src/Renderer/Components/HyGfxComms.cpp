/**************************************************************************
 *	HyGfxComms.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Renderer/Components/HyGfxComms.h"
#include "Afx/HyInteropAfx.h"

HyGfxComms::HyGfxComms() : m_eThreadState(HYTHREADSTATE_Run)
{
	m_pDrawBuffer_Update = HY_NEW char[HY_GFX_BUFFER_SIZE];
	memset(m_pDrawBuffer_Update, 0, HY_GFX_BUFFER_SIZE);

	m_pDrawBuffer_Shared = HY_NEW char[HY_GFX_BUFFER_SIZE];
	memset(m_pDrawBuffer_Shared, 0, HY_GFX_BUFFER_SIZE);

	m_pDrawBuffer_Render = HY_NEW char[HY_GFX_BUFFER_SIZE];
	memset(m_pDrawBuffer_Render, 0, HY_GFX_BUFFER_SIZE);

	m_pTxQueue_Update = HY_NEW std::queue<IHyLoadableData *>();
	m_pTxQueue_Shared = HY_NEW std::queue<IHyLoadableData *>();
	m_pTxQueue_Render = HY_NEW std::queue<IHyLoadableData *>();

	m_pRxQueue_Update = HY_NEW std::queue<IHyLoadableData *>();
	m_pRxQueue_Shared = HY_NEW std::queue<IHyLoadableData *>();
	m_pRxQueue_Render = HY_NEW std::queue<IHyLoadableData *>();
}

HyGfxComms::~HyGfxComms()
{
	delete [] m_pDrawBuffer_Update;
	delete [] m_pDrawBuffer_Shared;
	delete [] m_pDrawBuffer_Render;

	delete m_pTxQueue_Update;
	delete m_pTxQueue_Shared;
	delete m_pTxQueue_Render;

	delete m_pRxQueue_Update;
	delete m_pRxQueue_Shared;
	delete m_pRxQueue_Render;
}

// This should only be invoked from the Update/Game thread
char *HyGfxComms::GetDrawBuffer()
{
	return m_pDrawBuffer_Update;
}

// This should only be invoked from the Update/Game thread
void HyGfxComms::SetSharedPointers()
{
	m_csPointers.Lock();

	std::queue<IHyLoadableData *> *pTmpQueue = m_pTxQueue_Shared;
	m_pTxQueue_Shared = m_pTxQueue_Update;
	m_pTxQueue_Update = pTmpQueue;

	pTmpQueue = m_pRxQueue_Shared;
	m_pRxQueue_Shared = m_pRxQueue_Update;
	m_pRxQueue_Update = pTmpQueue;

	char *pTmp = m_pDrawBuffer_Shared;
	m_pDrawBuffer_Shared = m_pDrawBuffer_Update;
	m_pDrawBuffer_Update = pTmp;

	m_csPointers.Unlock();
}

// This should only be invoked from the Update/Game thread
void HyGfxComms::TxData(IHyLoadableData *pData)
{
	m_pTxQueue_Update->push(pData);
}

// This should only be invoked from the Update/Game thread
std::queue<IHyLoadableData *> *HyGfxComms::RxData()
{
	return m_pRxQueue_Update;
}

// This should only be invoked from the Render thread
bool HyGfxComms::Render_TakeSharedPointers(std::queue<IHyLoadableData *> *&pRxDataQueue, std::queue<IHyLoadableData *> *&pTxDataQueue, char *&pDrawBuffer)
{
	m_csPointers.Lock();

	// Check to see if these buffers have already been rendered, if so return false to try next update.
	HyGfxComms::tDrawHeader *pTest = reinterpret_cast<HyGfxComms::tDrawHeader *>(m_pDrawBuffer_Shared);
	if(reinterpret_cast<HyGfxComms::tDrawHeader *>(m_pDrawBuffer_Shared)->uiReturnFlags != 0)
	{
		m_csPointers.Unlock();
		return false;
	}

	// Data queues
	std::queue<IHyLoadableData *> *pTmpQueue = m_pTxQueue_Render;
	m_pTxQueue_Render = m_pTxQueue_Shared;
	m_pTxQueue_Shared = pTmpQueue;
	pRxDataQueue =  m_pTxQueue_Render;

	pTmpQueue = m_pRxQueue_Render;
	m_pRxQueue_Render = m_pRxQueue_Shared;
	m_pRxQueue_Shared = pTmpQueue;
	pTxDataQueue = m_pRxQueue_Render;

	// Buffers
	char *pTmp = m_pDrawBuffer_Render;
	m_pDrawBuffer_Render = m_pDrawBuffer_Shared;
	m_pDrawBuffer_Shared = pTmp;
	pDrawBuffer = m_pDrawBuffer_Render;

	m_csPointers.Unlock();

	return true;
}

// This should only be invoked from the Render thread
bool HyGfxComms::Render_PollPlatformApi(IHyRenderer *pRenderer)
{
	m_csApiMsgQueue.Lock();

#if defined(HY_PLATFORM_WINDOWS) && !defined(HY_PLATFORM_GUI)
	// TODO: return false when windows close message comes in or something similar
	MSG msg = { 0 };
	int32 iWindowIndex = 0;
	HWND hWnd = static_cast<HyOpenGL_Win *>(pRenderer)->GetHWND(iWindowIndex);

	while(hWnd != nullptr)
	{
		while(PeekMessage(&msg, hWnd, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			m_ApiMsgQueue.push(msg);

			//static_cast<HyInputInterop &>(m_InputRef).HandleMsg(iWindowIndex, m_RenderSurfaces[iWindowIndex].GetWidth(), m_RenderSurfaces[iWindowIndex].GetHeight(), msg);
		}

		iWindowIndex++;
		hWnd = static_cast<HyOpenGL_Win *>(pRenderer)->GetHWND(iWindowIndex);
	}
#endif

	bool bRetValue = true;
	if(m_eThreadState == HYTHREADSTATE_ShouldExit)
	{
		m_eThreadState = HYTHREADSTATE_HasExited;
		bRetValue = false;
	}

	m_csApiMsgQueue.Unlock();

	return bRetValue;
}

// This should only be invoked from the Update/Game thread
void HyGfxComms::RxApiMsgs(std::queue<HyApiMsgInterop> &msgQueueOut)
{
	m_csApiMsgQueue.Lock();

	while(m_ApiMsgQueue.empty() == false)
	{
		msgQueueOut.push(m_ApiMsgQueue.front());
		m_ApiMsgQueue.pop();
	}

	m_csApiMsgQueue.Unlock();
}

void HyGfxComms::RequestThreadExit()
{
	m_csApiMsgQueue.Lock();
	if(m_eThreadState != HYTHREADSTATE_HasExited)
		m_eThreadState = HYTHREADSTATE_ShouldExit;
	m_csApiMsgQueue.Unlock();
}

bool HyGfxComms::IsShutdown()
{
	m_csApiMsgQueue.Lock();
	bool bRetVal = (m_eThreadState == HYTHREADSTATE_HasExited);
	m_csApiMsgQueue.Unlock();

	return bRetVal;
}
