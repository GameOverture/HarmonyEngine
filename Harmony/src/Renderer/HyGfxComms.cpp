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
	m_pBuffer_Update = m_pBuffer_Shared = m_pBuffer_Render = NULL;
	m_pAtlasSendQueue_Update = m_pAtlasSendQueue_Shared = m_pAtlasSendQueue_Render = NULL;

	m_pBuffer_Update = HY_NEW char[HY_GFX_BUFFER_SIZE];
	memset(m_pBuffer_Update, 0, HY_GFX_BUFFER_SIZE);

	m_pBuffer_Shared = HY_NEW char[HY_GFX_BUFFER_SIZE];
	memset(m_pBuffer_Shared, 0, HY_GFX_BUFFER_SIZE);

	m_pBuffer_Render = HY_NEW char[HY_GFX_BUFFER_SIZE];
	memset(m_pBuffer_Render, 0, HY_GFX_BUFFER_SIZE);

	m_pAtlasSendQueue_Update = HY_NEW queue<IHyData2d *>();
	m_pAtlasSendQueue_Shared = HY_NEW queue<IHyData2d *>();
	m_pAtlasSendQueue_Render = HY_NEW queue<IHyData2d *>();

	m_pAtlasReceiveQueue_Update = HY_NEW queue<IHyData2d *>();
	m_pAtlasReceiveQueue_Shared = HY_NEW queue<IHyData2d *>();
	m_pAtlasReceiveQueue_Render = HY_NEW queue<IHyData2d *>();
}

HyGfxComms::~HyGfxComms()
{
	delete [] m_pBuffer_Update;
	delete [] m_pBuffer_Shared;
	delete [] m_pBuffer_Render;

	delete m_pAtlasSendQueue_Update;
	delete m_pAtlasSendQueue_Shared;
	delete m_pAtlasSendQueue_Render;

	delete m_pAtlasReceiveQueue_Update;
	delete m_pAtlasReceiveQueue_Shared;
	delete m_pAtlasReceiveQueue_Render;
}

// This should only be invoked from the Update/Game thread
void HyGfxComms::Update_SetSharedPtrs()
{
	m_csBuffers.Lock();

	queue<IHyData2d *> *pTmpQueue = m_pAtlasSendQueue_Shared;
	m_pAtlasSendQueue_Shared = m_pAtlasSendQueue_Update;
	m_pAtlasSendQueue_Update = pTmpQueue;

	pTmpQueue = m_pAtlasReceiveQueue_Shared;
	m_pAtlasReceiveQueue_Shared = m_pAtlasReceiveQueue_Update;
	m_pAtlasReceiveQueue_Update = pTmpQueue;

	char *pTmp = m_pBuffer_Shared;
	m_pBuffer_Shared = m_pBuffer_Update;
	m_pBuffer_Update = pTmp;

	m_csBuffers.Unlock();
}

// This should only be invoked from the Render thread
bool HyGfxComms::Render_GetSharedPtrs(queue<IHyData2d *> *&pMsgQueuePtr, queue<IHyData2d *> *&pSendMsgQueuePtr, char *&pDrawBufferPtr)
{
	m_csBuffers.Lock();

	// Check to see if these buffers have already been rendered, if so return false to try next update.
	HyGfxComms::tDrawHeader *pTest = reinterpret_cast<HyGfxComms::tDrawHeader *>(m_pBuffer_Shared);
	if(reinterpret_cast<HyGfxComms::tDrawHeader *>(m_pBuffer_Shared)->uiReturnFlags != 0)
	{
		m_csBuffers.Unlock();
		return false;
	}

	// Message queues
	queue<IHyData2d *> *pTmpQueue = m_pAtlasSendQueue_Render;
	m_pAtlasSendQueue_Render = m_pAtlasSendQueue_Shared;
	m_pAtlasSendQueue_Shared = pTmpQueue;
	pMsgQueuePtr =  m_pAtlasSendQueue_Render;

	pTmpQueue = m_pAtlasReceiveQueue_Render;
	m_pAtlasReceiveQueue_Render = m_pAtlasReceiveQueue_Shared;
	m_pAtlasReceiveQueue_Shared = pTmpQueue;
	pSendMsgQueuePtr = m_pAtlasReceiveQueue_Render;

	// Buffers
	char *pTmp = m_pBuffer_Render;
	m_pBuffer_Render = m_pBuffer_Shared;
	m_pBuffer_Shared = pTmp;
	pDrawBufferPtr = m_pBuffer_Render;

	m_csBuffers.Unlock();

	return true;
}
