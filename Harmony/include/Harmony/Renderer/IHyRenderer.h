/**************************************************************************
 *	IHyRenderer.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2012 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __IHyRenderer_h__
#define __IHyRenderer_h__

#include "Afx/HyStdAfx.h"

#include "Renderer/HyGfxComms.h"
#include "Renderer/HyRenderState.h"

#include "Renderer/Viewport/HyWindow.h"

class IHyRenderer
{
protected:
	HyGfxComms &				m_GfxCommsRef;
	vector<HyWindow> &			m_vWindowRef;

	queue<IHyData2d *> *		m_pMsgQueuePtr;		// The pointer to the currently active render message queue
	queue<IHyData2d *> *		m_pSendMsgQueuePtr;	// The pointer to the currently active render message queue
	char *						m_pDrawBufferPtr;	// The pointer to the currently active draw buffer

	HyGfxComms::tDrawHeader *	m_pDrawBufferHeader;
	HyRenderState *				m_pCurRenderState;
	HyRenderState				m_PrevRenderState;

	enum eRenderSurfaceType
	{
		RENDERSURFACE_Texture = 0,
		RENDERSURFACE_Window
	};
	
	struct RenderSurface
	{
		eRenderSurfaceType			m_eType;
		int32						m_iID;
		int32						m_iRenderSurfaceWidth;
		int32						m_iRenderSurfaceHeight;
		
		void *						m_pExData;

		RenderSurface(eRenderSurfaceType eType, uint32 iID, int32 iRenderSurfaceWidth, int32 iRenderSurfaceHeight);

		void Resize(int32 iWidth, int32 iHeight);
	};
	vector<RenderSurface>			m_RenderSurfaces;
	vector<RenderSurface>::iterator	m_RenderSurfaceIter;

public:
	IHyRenderer(HyGfxComms &gfxCommsRef, vector<HyWindow> &vWindowRef);
	virtual ~IHyRenderer(void);

	virtual void StartRender() = 0;

	virtual bool Begin_3d() = 0;
	virtual void SetRenderState_3d(uint32 uiNewRenderState) = 0;
	virtual void End_3d() = 0;

	virtual bool Begin_2d() = 0;
	virtual void DrawRenderState_2d(HyRenderState &renderState) = 0;
	virtual void End_2d() = 0;

	virtual void FinishRender() = 0;

	// Returns the texture ID used for API specific drawing.
	virtual uint32 AddTextureArray(uint32 uiNumColorChannels, uint32 uiWidth, uint32 uiHeight, vector<unsigned char *> &vPixelData) = 0;
	virtual void DeleteTextureArray(uint32 uiTextureHandle) = 0;

	virtual void OnRenderSurfaceChanged(RenderSurface &renderSurfaceRef, uint32 uiChangedFlags) = 0;

	int32 GetNumCameras2d()									{ return *(reinterpret_cast<int32 *>(m_pDrawBufferPtr + m_pDrawBufferHeader->uiOffsetToCameras2d)); }
	uint32 GetCameraWindowIndex2d(int iCameraIndex)			{ return *(reinterpret_cast<uint32 *>(m_pDrawBufferPtr + m_pDrawBufferHeader->uiOffsetToCameras2d + sizeof(int32)			+ (iCameraIndex		* (sizeof(uint32) + sizeof(HyRectangle<float>) + sizeof(mat4))))); }
	HyRectangle<float> *GetCameraViewportRect2d(int iIndex)	{ return reinterpret_cast<HyRectangle<float> *>(m_pDrawBufferPtr + m_pDrawBufferHeader->uiOffsetToCameras2d + sizeof(int32) + (iIndex			* (sizeof(uint32) + sizeof(HyRectangle<float>) + sizeof(mat4))) + sizeof(uint32)); }
	mat4 *GetCameraView2d(int iIndex)						{ return reinterpret_cast<mat4 *>(m_pDrawBufferPtr + m_pDrawBufferHeader->uiOffsetToCameras2d + sizeof(int32)				+ (iIndex			* (sizeof(uint32) + sizeof(HyRectangle<float>) + sizeof(mat4))) + sizeof(uint32) + sizeof(HyRectangle<float>)); }

	int32 GetNumInsts3d()									{ return *(reinterpret_cast<int32 *>(m_pDrawBufferPtr + m_pDrawBufferHeader->uiOffsetToCameras3d)); }
	int32 GetNumCameras3d()									{ return *(reinterpret_cast<int32 *>(m_pDrawBufferPtr + m_pDrawBufferHeader->uiOffsetToCameras3d)); }

	int32 GetNumRenderStates2d()							{ return *(reinterpret_cast<int32 *>(m_pDrawBufferPtr + m_pDrawBufferHeader->uiOffsetToInst2d)); }
	HyRenderState *GetRenderStatesPtr2d()					{ return reinterpret_cast<HyRenderState *>(m_pDrawBufferPtr + m_pDrawBufferHeader->uiOffsetToInst2d + sizeof(int32)); } // Last sizeof(int32) is skipping number of 2dInsts
	char *GetVertexData2d()									{ return reinterpret_cast<char *>(m_pDrawBufferPtr + m_pDrawBufferHeader->uiOffsetToVertexData2d); }

	void Update();
	void Draw2d();
	void SetMonitorDeviceInfo(vector<HyMonitorDeviceInfo> &info);
};

#endif /* __IHyRenderer_h__ */
