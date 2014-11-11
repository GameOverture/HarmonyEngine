/**************************************************************************
 *	IGfxApi.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __IGfxApi_h__
#define __IGfxApi_h__

#include "Harmony/Renderer/HyGfxComms.h"

#include <map>
#include <queue>
using std::map;
using std::queue;

// Forward declarations
class IData;
class IDraw2d;

class IGfxApi
{
protected:
	HyGfxComms *				m_pGfxComms;

	queue<IData *> *			m_pMsgQueuePtr;		// The pointer to the currently active render message queue
	queue<IData *> *			m_pSendMsgQueuePtr;	// The pointer to the currently active render message queue
	char *						m_pDrawBufferPtr;	// The pointer to the currently active draw buffer

	char *						m_pCurDataPtr;
	HyGfxComms::tDrawHeader *	m_DrawpBufferHeader;
	IDraw2d *					m_pCurDrawData;

	uint32						m_uiCurRenderState;
	void (*m_fpDraw2d)(IDraw2d *pInst, void *pApi);

public:
	IGfxApi();
	virtual ~IGfxApi(void);

	void SetGfxComms(HyGfxComms *pGfxComms)	{ m_pGfxComms = pGfxComms; }
	const HyGfxComms::tGfxInfo *GetGfxInfo(){ return m_pGfxComms->GetGfxInfo(); }

	virtual bool Initialize() = 0;
	virtual bool PollApi() = 0;
	virtual bool CheckDevice() = 0;

	virtual void StartRender() = 0;
	
	virtual bool Begin_3d() = 0;
	virtual void SetRenderState_3d(uint32 uiNewRenderState) = 0;
	virtual void End_3d() = 0;

	virtual bool Begin_2d() = 0;
	virtual void SetRenderState_2d(uint32 uiNewRenderState) = 0;
	virtual void End_2d() = 0;

	virtual void FinishRender() = 0;

	virtual bool Shutdown() = 0;

	virtual void DrawBuffers();

	// Returns the texture ID used for API specific drawing.
	virtual uint32 AddTexture(uint32 uiNumColorChannels, uint32 uiWidth, uint32 uiHeight, void *pPixelData) = 0;
	virtual void DeleteTexture(uint32 uiTextureId) = 0;

	int32 GetNumCameras2d()					{ return *(reinterpret_cast<int32 *>(m_pDrawBufferPtr + m_DrawpBufferHeader->uiOffsetToCameras2d)); }
	mat4 *GetCameraView2d(int iIndex)		{ return reinterpret_cast<mat4 *>(m_pDrawBufferPtr+m_DrawpBufferHeader->uiOffsetToCameras2d+sizeof(int32)+(iIndex * (sizeof(HyRectangle)+sizeof(mat4))) + sizeof(HyRectangle) ); }
	
	int32 GetNumInsts3d()					{ return *(reinterpret_cast<int32 *>(m_pDrawBufferPtr + m_DrawpBufferHeader->uiOffsetToCameras3d)); }
	int32 GetNumCameras3d()					{ return *(reinterpret_cast<int32 *>(m_pDrawBufferPtr + m_DrawpBufferHeader->uiOffsetToCameras3d)); }

	int32 GetNumInsts2d()					{ return *(reinterpret_cast<int32 *>(m_pDrawBufferPtr + m_DrawpBufferHeader->uiOffsetToInst2d)); }
	float *GetVertexData2d()				{ return reinterpret_cast<float *>(m_pDrawBufferPtr+m_DrawpBufferHeader->uiOffsetToVertexData2d); } 

	bool Update();
	void UpdateLoop();
	void ProcessGameMsgs();
	void Draw2d();
};

#endif /* __IGfxApi_h__ */
