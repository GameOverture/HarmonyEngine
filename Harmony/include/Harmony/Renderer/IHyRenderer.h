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

#include "Renderer/Components/HyGfxComms.h"
#include "Renderer/Components/HyRenderState.h"

class IHyShader;
class HyWindow;
struct HyMonitorDeviceInfo;

class IHyRenderer
{
protected:
	HyGfxComms &							m_GfxCommsRef;
	std::vector<HyWindow *> &				m_WindowListRef;

	std::queue<IHyLoadableData *> *			m_pRxDataQueue;		// The pointer to the currently active render message queue
	std::queue<IHyLoadableData *> *			m_pTxDataQueue;		// The pointer to the currently active render message queue
	char *									m_pDrawBuffer;		// The pointer to the currently active draw buffer

	static int32							sm_iShaderIdCount;
	static std::map<int32, IHyShader *>		sm_ShaderMap;

	HyGfxComms::tDrawHeader *				m_pDrawBufferHeader;
	HyRenderState *							m_pCurRenderState;
	HyRenderState							m_PrevRenderState;

	enum eRenderSurfaceType
	{
		RENDERSURFACE_Texture = 0,
		RENDERSURFACE_Window
	};
	
	struct RenderSurface
	{
		eRenderSurfaceType					m_eType;
		int32								m_iID;
		int32								m_iRenderSurfaceWidth;
		int32								m_iRenderSurfaceHeight;

		void *								m_pExData;

		RenderSurface(eRenderSurfaceType eType, uint32 iID, int32 iRenderSurfaceWidth, int32 iRenderSurfaceHeight);

		void Resize(int32 iWidth, int32 iHeight);
	};
	std::vector<RenderSurface>				m_RenderSurfaces;
	std::vector<RenderSurface>::iterator	m_RenderSurfaceIter;

public:
	IHyRenderer(HyGfxComms &gfxCommsRef, std::vector<HyWindow *> &windowListRef);
	virtual ~IHyRenderer(void);

	virtual void StartRender() = 0;

	virtual void Init_3d() = 0;
	virtual bool BeginPass_3d() = 0;
	virtual void SetRenderState_3d(uint32 uiNewRenderState) = 0;
	virtual void End_3d() = 0;

	virtual void Init_2d() = 0;
	virtual bool BeginPass_2d() = 0;
	virtual void DrawRenderState_2d(HyRenderState &renderState) = 0;
	virtual void End_2d() = 0;

	virtual void FinishRender() = 0;

	// Returns texture's ID used for API specific drawing. May not fit entire array, 'uiNumTexturesUploaded' is how many textures it did upload.
	virtual uint32 AddTextureArray(uint32 uiNumColorChannels, uint32 uiWidth, uint32 uiHeight, std::vector<unsigned char *> &pixelDataList, uint32 &uiNumTexturesUploadedOut) = 0;
	virtual void DeleteTextureArray(uint32 uiTextureHandle) = 0;

	virtual void OnRenderSurfaceChanged(RenderSurface &renderSurfaceRef, uint32 uiChangedFlags) = 0;

	int32 GetNumCameras2d()									{ return *(reinterpret_cast<int32 *>(m_pDrawBuffer + m_pDrawBufferHeader->uiOffsetToCameras2d)); }
	uint32 GetCameraWindowIndex2d(int iCameraIndex)			{ return *(reinterpret_cast<uint32 *>(m_pDrawBuffer + m_pDrawBufferHeader->uiOffsetToCameras2d + sizeof(int32) + (iCameraIndex		* (sizeof(uint32) + sizeof(HyRectangle<float>) + sizeof(glm::mat4))))); }
	HyRectangle<float> *GetCameraViewportRect2d(int iIndex)	{ return reinterpret_cast<HyRectangle<float> *>(m_pDrawBuffer + m_pDrawBufferHeader->uiOffsetToCameras2d + sizeof(int32) + (iIndex	* (sizeof(uint32) + sizeof(HyRectangle<float>) + sizeof(glm::mat4))) + sizeof(uint32)); }
	glm::mat4 *GetCameraView2d(int iIndex)					{ return reinterpret_cast<glm::mat4 *>(m_pDrawBuffer + m_pDrawBufferHeader->uiOffsetToCameras2d + sizeof(int32) + (iIndex			* (sizeof(uint32) + sizeof(HyRectangle<float>) + sizeof(glm::mat4))) + sizeof(uint32) + sizeof(HyRectangle<float>)); }

	int32 GetNumInsts3d()									{ return *(reinterpret_cast<int32 *>(m_pDrawBuffer + m_pDrawBufferHeader->uiOffsetToCameras3d)); }
	int32 GetNumCameras3d()									{ return *(reinterpret_cast<int32 *>(m_pDrawBuffer + m_pDrawBufferHeader->uiOffsetToCameras3d)); }

	int32 GetNumRenderStates2d()							{ return *(reinterpret_cast<int32 *>(m_pDrawBuffer + m_pDrawBufferHeader->uiOffsetToInst2d)); }
	HyRenderState *GetRenderStatesPtr2d()					{ return reinterpret_cast<HyRenderState *>(m_pDrawBuffer + m_pDrawBufferHeader->uiOffsetToInst2d + sizeof(int32)); } // Last sizeof(int32) is skipping number of 2dInsts
	char *GetVertexData2d()									{ return reinterpret_cast<char *>(m_pDrawBuffer + m_pDrawBufferHeader->uiOffsetToVertexData2d); }

	void Update();
	void Draw2d();
	void SetMonitorDeviceInfo(std::vector<HyMonitorDeviceInfo> &info);

	static IHyShader *FindShader(int32 iId);
	static IHyShader *MakeCustomShader();
	static IHyShader *MakeCustomShader(const char *szPrefix, const char *szName);
};

#endif /* __IHyRenderer_h__ */
