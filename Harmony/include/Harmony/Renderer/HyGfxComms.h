/**************************************************************************
 *	HyGfxComms.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyGfxBuffers_h__
#define __HyGfxBuffers_h__

#include "Afx/HyStdAfx.h"

#include "Threading/BasicSync.h"
#include "Creator/Instances/HyText2d.h"
#include "Creator/Viewport/HyCamera2d.h"

#include "Creator/Viewport/HyViewport.h"

#include <vector>
#include <queue>
using std::vector;
using std::queue;

#define RENDER_BUFFER_SIZE ((1024 * 1024) * 2) // 2MB

enum eGfxReturnFlags
{
	GFXFLAG_HasRendered			= 1 << 0
};

class HyGfxComms
{
public:
	// Note: All offsets below are from the beginning of the buffer pointer, containing this structure
	struct tDrawHeader
	{
		uint32		uiReturnFlags;

		size_t		uiOffsetToInst3d;
		
		size_t		uiOffsetToInst2d;

		size_t		uiOffsetToVertexData2d;
		size_t		uiVertexBufferSize2d;

		size_t		uiOffsetToCameras3d;
		size_t		uiOffsetToCameras2d;
	};

	struct tGfxInfo
	{
		int32			uiNumNativeResolutions;
		//HyResolution *	pResolutionList;
	};

private:
	tGfxInfo *					m_pGfxInfo;

	char *						m_pBuffer_Update;
	char *						m_pBuffer_Shared;
	char *						m_pBuffer_Render;

	queue<IData *> *			m_pSendMsg_Update;
	queue<IData *> *			m_pSendMsg_Shared;
	queue<IData *> *			m_pSendMsg_Render;

	queue<IData *> *			m_pReceiveData_Update;
	queue<IData *> *			m_pReceiveData_Shared;
	queue<IData *> *			m_pReceiveData_Render;

	BasicSection				m_csBuffers;
	BasicSection				m_csInfo;

	int32						m_iStrTableIndexCount;

public:
	HyGfxComms();
	~HyGfxComms();

	void SetGfxInfo(tGfxInfo *pInfo);
	const tGfxInfo *GetGfxInfo();

	bool IsRendererInitialized()			{ return m_pGfxInfo != NULL; }
	
	// This should only be invoked from the Update/Game thread
	inline char *GetWriteBufferPtr()		{ return m_pBuffer_Update; }

	// This should only be invoked from the Update/Game thread
	void Update_SendData(IData *pMsg)		{ m_pSendMsg_Update->push(pMsg); }

	// This should only be invoked from the Update/Game thread
	queue<IData *> *Update_RetrieveData()	{ return m_pReceiveData_Update; }

	// This should only be invoked from the Update/Game thread
	void Update_SetSharedPtrs();

	// This should only be invoked from the Render thread
	bool Render_GetSharedPtrs(queue<IData *> *&pMsgQueuePtr, queue<IData *> *&pSendMsgQueuePtr, char *&pDrawBufferPtr);
	
private:

	inline void LockBuffers()				{ m_csBuffers.Lock(); }
	inline void UnlockBuffers()				{ m_csBuffers.Unlock(); }

	inline void LockInfo()					{ m_csInfo.Lock(); }
	inline void UnlockInfo()				{ m_csInfo.Unlock(); }
};

#endif /* __HyGfxBuffers_h__ */
