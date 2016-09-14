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
	m_pDrawBuffer_Update = m_pDrawBuffer_Shared = m_pDrawBuffer_Render = NULL;
	m_pTxDataQueue_Update = m_pTxDataQueue_Shared = m_pTxDataQueue_Render = NULL;

	m_pDrawBuffer_Update = HY_NEW char[HY_GFX_BUFFER_SIZE];
	memset(m_pDrawBuffer_Update, 0, HY_GFX_BUFFER_SIZE);

	m_pDrawBuffer_Shared = HY_NEW char[HY_GFX_BUFFER_SIZE];
	memset(m_pDrawBuffer_Shared, 0, HY_GFX_BUFFER_SIZE);

	m_pDrawBuffer_Render = HY_NEW char[HY_GFX_BUFFER_SIZE];
	memset(m_pDrawBuffer_Render, 0, HY_GFX_BUFFER_SIZE);

	m_pTxDataQueue_Update = HY_NEW queue<IHy2dData *>();
	m_pTxDataQueue_Shared = HY_NEW queue<IHy2dData *>();
	m_pTxDataQueue_Render = HY_NEW queue<IHy2dData *>();

	m_pRxDataQueue_Update = HY_NEW queue<IHy2dData *>();
	m_pRxDataQueue_Shared = HY_NEW queue<IHy2dData *>();
	m_pRxDataQueue_Render = HY_NEW queue<IHy2dData *>();
}

HyGfxComms::~HyGfxComms()
{
	delete [] m_pDrawBuffer_Update;
	delete [] m_pDrawBuffer_Shared;
	delete [] m_pDrawBuffer_Render;

	delete m_pTxDataQueue_Update;
	delete m_pTxDataQueue_Shared;
	delete m_pTxDataQueue_Render;

	delete m_pRxDataQueue_Update;
	delete m_pRxDataQueue_Shared;
	delete m_pRxDataQueue_Render;
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

	queue<IHy2dData *> *pTmpQueue = m_pTxDataQueue_Shared;
	m_pTxDataQueue_Shared = m_pTxDataQueue_Update;
	m_pTxDataQueue_Update = pTmpQueue;

	pTmpQueue = m_pRxDataQueue_Shared;
	m_pRxDataQueue_Shared = m_pRxDataQueue_Update;
	m_pRxDataQueue_Update = pTmpQueue;

	char *pTmp = m_pDrawBuffer_Shared;
	m_pDrawBuffer_Shared = m_pDrawBuffer_Update;
	m_pDrawBuffer_Update = pTmp;

	m_csPointers.Unlock();
}

// This should only be invoked from the Update/Game thread
void HyGfxComms::TxData(IHy2dData *pAtlasGrp)
{
	m_pTxDataQueue_Update->push(pAtlasGrp);
}

// This should only be invoked from the Update/Game thread
queue<IHy2dData *> *HyGfxComms::RxData()
{
	return m_pRxDataQueue_Update;
}

// This should only be invoked from the Render thread
bool HyGfxComms::Render_TakeSharedPointers(queue<IHy2dData *> *&pRxDataQueue, queue<IHy2dData *> *&pTxDataQueue, char *&pDrawBuffer)
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
	queue<IHy2dData *> *pTmpQueue = m_pTxDataQueue_Render;
	m_pTxDataQueue_Render = m_pTxDataQueue_Shared;
	m_pTxDataQueue_Shared = pTmpQueue;
	pRxDataQueue =  m_pTxDataQueue_Render;

	pTmpQueue = m_pRxDataQueue_Render;
	m_pRxDataQueue_Render = m_pRxDataQueue_Shared;
	m_pRxDataQueue_Shared = pTmpQueue;
	pTxDataQueue = m_pRxDataQueue_Render;

	// Buffers
	char *pTmp = m_pDrawBuffer_Render;
	m_pDrawBuffer_Render = m_pDrawBuffer_Shared;
	m_pDrawBuffer_Shared = pTmp;
	pDrawBuffer = m_pDrawBuffer_Render;

	m_csPointers.Unlock();

	return true;
}
