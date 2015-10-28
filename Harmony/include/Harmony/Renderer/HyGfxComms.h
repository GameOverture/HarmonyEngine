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

#include "FileIO/Data/IHyData2d.h"
#include "Threading/BasicSync.h"

#include <vector>
#include <queue>
using std::vector;
using std::queue;

#define RENDER_BUFFER_SIZE ((1024 * 1024) * 2) // 2MB

class HyGfxComms
{
public:
	enum eGfxReturnFlags
	{
		GFXFLAG_HasRendered = 1 << 0
	};

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

private:
	char *						m_pBuffer_Update;
	char *						m_pBuffer_Shared;
	char *						m_pBuffer_Render;

	queue<IHyData2d *> *		m_pAtlasSendQueue_Update;
	queue<IHyData2d *> *		m_pAtlasSendQueue_Shared;
	queue<IHyData2d *> *		m_pAtlasSendQueue_Render;

	queue<IHyData2d *> *		m_pAtlasReceiveQueue_Update;
	queue<IHyData2d *> *		m_pAtlasReceiveQueue_Shared;
	queue<IHyData2d *> *		m_pAtlasReceiveQueue_Render;

	BasicSection				m_csBuffers;

public:
	HyGfxComms();
	~HyGfxComms();
	
	// This should only be invoked from the Update/Game thread
	inline char *GetWriteBufferPtr()		{ return m_pBuffer_Update; }

	// This should only be invoked from the Update/Game thread
	void SendAtlasGroup(IHyData2d *pAtlasGrp)	{ m_pAtlasSendQueue_Update->push(pAtlasGrp); }

	// This should only be invoked from the Update/Game thread
	queue<IHyData2d *> *RetrieveAtlasGroups()	{ return m_pAtlasReceiveQueue_Update; }

	// This should only be invoked from the Update/Game thread
	void Update_SetSharedPtrs();

	// This should only be invoked from the Render thread
	bool Render_GetSharedPtrs(queue<IHyData2d *> *&pMsgQueuePtr, queue<IHyData2d *> *&pSendMsgQueuePtr, char *&pDrawBufferPtr);
};

#endif /* __HyGfxBuffers_h__ */
