/**************************************************************************
 *	HyGfxComms.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Renderer/HyGfxComms.h"

HyGfxComms::HyGfxComms()
{
	m_pGfxInit = NULL;
	m_pGfxInfo = NULL;

	m_pBuffer_Update = m_pBuffer_Shared = m_pBuffer_Render = NULL;
	m_pSendMsg_Update = m_pSendMsg_Shared = m_pSendMsg_Render = NULL;

	m_pBuffer_Update = new char[RENDER_BUFFER_SIZE];
	memset(m_pBuffer_Update, 0, RENDER_BUFFER_SIZE);

	m_pBuffer_Shared = new char[RENDER_BUFFER_SIZE];
	memset(m_pBuffer_Shared, 0, RENDER_BUFFER_SIZE);

	m_pBuffer_Render = new char[RENDER_BUFFER_SIZE];
	memset(m_pBuffer_Render, 0, RENDER_BUFFER_SIZE);

	m_pSendMsg_Update = new queue<IData *>();
	m_pSendMsg_Shared = new queue<IData *>();
	m_pSendMsg_Render = new queue<IData *>();

	m_pReceiveData_Update = new queue<IData *>();
	m_pReceiveData_Shared = new queue<IData *>();
	m_pReceiveData_Render = new queue<IData *>();
}

HyGfxComms::~HyGfxComms()
{
	delete [] m_pBuffer_Update;
	delete [] m_pBuffer_Shared;
	delete [] m_pBuffer_Render;

	delete m_pSendMsg_Update;
	delete m_pSendMsg_Shared;
	delete m_pSendMsg_Render;

	delete m_pReceiveData_Update;
	delete m_pReceiveData_Shared;
	delete m_pReceiveData_Render;
}

void HyGfxComms::SetGfxInit(tGfxInit *pInit)
{
	LockInfo();
	HyAssert(m_pGfxInit == NULL, "SetGfxInit() was invoked with already set 'm_pGfxInit'");
	m_pGfxInit = pInit;
	UnlockInfo();
}
const HyGfxComms::tGfxInit *HyGfxComms::GetGfxInit()
{
	const tGfxInit *pInit;

	LockInfo();
	pInit = m_pGfxInit;
	UnlockInfo(); 

	return pInit;
}

void HyGfxComms::SetGfxInfo(tGfxInfo *pInfo)
{
	LockInfo();
	HyAssert(m_pGfxInfo == NULL, "SetGfxInfo() was invoked with already initialized 'm_pGfxInfo'");
	m_pGfxInfo = pInfo;
	UnlockInfo();
}
const HyGfxComms::tGfxInfo *HyGfxComms::GetGfxInfo()
{
	const tGfxInfo *pInfo;

	LockInfo();
	pInfo = m_pGfxInfo;
	UnlockInfo(); 

	return pInfo;
}

// This should only be invoked from the Update/Game thread
void HyGfxComms::Update_SetSharedPtrs()
{
	LockBuffers();

	queue<IData *> *pTmpQueue = m_pSendMsg_Shared;
	m_pSendMsg_Shared = m_pSendMsg_Update;
	m_pSendMsg_Update = pTmpQueue;

	pTmpQueue = m_pReceiveData_Shared;
	m_pReceiveData_Shared = m_pReceiveData_Update;
	m_pReceiveData_Update = pTmpQueue;

	char *pTmp = m_pBuffer_Shared;
	m_pBuffer_Shared = m_pBuffer_Update;
	m_pBuffer_Update = pTmp;

	UnlockBuffers();
}

// This should only be invoked from the Render thread
bool HyGfxComms::Render_GetSharedPtrs(queue<IData *> *&pMsgQueuePtr, queue<IData *> *&pSendMsgQueuePtr, char *&pDrawBufferPtr)
{
	LockBuffers();

	// Check to see if these buffers have already been rendered, if so return false to try next update.
	HyGfxComms::tDrawHeader *pTest = reinterpret_cast<HyGfxComms::tDrawHeader *>(m_pBuffer_Shared);
	if(reinterpret_cast<HyGfxComms::tDrawHeader *>(m_pBuffer_Shared)->uiReturnFlags != 0)
	{
		UnlockBuffers();
		return false;
	}

	// Message queues
	queue<IData *> *pTmpQueue = m_pSendMsg_Render;
	m_pSendMsg_Render = m_pSendMsg_Shared;
	m_pSendMsg_Shared = pTmpQueue;
	pMsgQueuePtr =  m_pSendMsg_Render;

	pTmpQueue = m_pReceiveData_Render;
	m_pReceiveData_Render = m_pReceiveData_Shared;
	m_pReceiveData_Shared = pTmpQueue;
	pSendMsgQueuePtr = m_pReceiveData_Render;

	// Buffers
	char *pTmp = m_pBuffer_Render;
	m_pBuffer_Render = m_pBuffer_Shared;
	m_pBuffer_Shared = pTmp;
	pDrawBufferPtr = m_pBuffer_Render;

	UnlockBuffers();

	return true;
}
