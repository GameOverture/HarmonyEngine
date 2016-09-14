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

#include "Assets/Data/IHy2dData.h"
#include "Threading/BasicSync.h"

#include <vector>
#include <queue>
using std::vector;
using std::queue;

#define HY_GFX_BUFFER_SIZE ((1024 * 1024) * 2) // 2MB

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

	// Note: All member variables and functions are named in reference of calling from the calling thread.
private:
	char *						m_pDrawBuffer_Update;
	char *						m_pDrawBuffer_Shared;
	char *						m_pDrawBuffer_Render;

	queue<IHy2dData *> *		m_pTxDataQueue_Update;
	queue<IHy2dData *> *		m_pTxDataQueue_Shared;
	queue<IHy2dData *> *		m_pTxDataQueue_Render;

	queue<IHy2dData *> *		m_pRxDataQueue_Update;
	queue<IHy2dData *> *		m_pRxDataQueue_Shared;
	queue<IHy2dData *> *		m_pRxDataQueue_Render;

	BasicSection				m_csPointers;

public:
	HyGfxComms();
	~HyGfxComms();
	
	// This should only be invoked from the Update/Game thread
	char *GetDrawBuffer();

	// This should only be invoked from the Update/Game thread
	void SetSharedPointers();

	// This should only be invoked from the Update/Game thread
	void TxData(IHy2dData *pAtlasGrp);

	// This should only be invoked from the Update/Game thread
	queue<IHy2dData *> *RxData();

	// This should only be invoked from the Render thread
	bool Render_TakeSharedPointers(queue<IHy2dData *> *&pRxDataQueue, queue<IHy2dData *> *&pTxDataQueue, char *&pDrawBuffer);
};

#endif /* __HyGfxBuffers_h__ */
