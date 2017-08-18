/**************************************************************************
 *	IHyRenderer.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2012 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef IHyRenderer_h__
#define IHyRenderer_h__

#include "Afx/HyStdAfx.h"
#include "Assets/Loadables/IHyLoadableData.h"
#include "Renderer/Components/HyRenderState.h"
#include "Threading/Threading.h"

class IHyShader;
class HyWindow;
struct HyMonitorDeviceInfo;
class HyRenderSurface;
class HyGfxComms;
class HyDiagnostics;

#define HYDRAWBUFFERHEADER reinterpret_cast<HyGfxComms::tDrawHeader *>(m_pDrawBuffer)

class IHyRenderer
{
protected:
	HyGfxComms &							m_GfxCommsRef;
	HyDiagnostics &							m_DiagnosticsRef;
	bool									m_bShowCursor;
	std::vector<HyWindow *> &				m_WindowListRef;

	std::queue<IHyLoadableData *> *			m_pRxDataQueue;		// The pointer to the currently active render message queue
	std::queue<IHyLoadableData *> *			m_pTxDataQueue;		// The pointer to the currently active render message queue
	char *									m_pDrawBuffer;		// The pointer to the currently active draw buffer

	static int32							sm_iShaderIdCount;
	static std::map<int32, IHyShader *>		sm_ShaderMap;

	HyRenderState *							m_pCurRenderState;
	HyRenderState							m_PrevRenderState;
	
	std::vector<HyRenderSurface>			m_RenderSurfaceList;
	std::vector<HyRenderSurface>::iterator	m_RenderSurfaceIter;

	uint32									m_uiSupportedTextureFormats;	// Bitflags that represent supported texture in 'HyTextureFormat' enum

	bool									m_bRequestedQuit;

public:
	IHyRenderer(HyGfxComms &gfxCommsRef, HyDiagnostics &diagnosticsRef, bool bShowCursor, std::vector<HyWindow *> &windowListRef);
	virtual ~IHyRenderer(void);

	void RequestQuit();
	bool IsQuitRequested();

	HyGfxComms &GetGfxCommsRef();

	void SetRendererInfo(const std::string &sApiName, const std::string &sVersion, const std::string &sVendor, const std::string &sRenderer, const std::string &sShader, int32 iMaxTextureSize, const std::string &sCompressedTextures);

	virtual bool Initialize() = 0;

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

	virtual uint32 AddTexture(HyTextureFormat eDesiredFormat, int32 iNumLodLevels, uint32 uiWidth, uint32 uiHeight, unsigned char *pPixelData, uint32 uiPixelDataSize, HyTextureFormat ePixelDataFormat) = 0;
	// Returns texture's ID used for API specific drawing. May not fit entire array, 'uiNumTexturesUploaded' is how many textures it did upload.
	virtual uint32 AddTextureArray(uint32 uiNumColorChannels, uint32 uiWidth, uint32 uiHeight, std::vector<unsigned char *> &pixelDataList, uint32 &uiNumTexturesUploadedOut) = 0;
	virtual void DeleteTexture(uint32 uiTextureHandle) = 0;

	virtual void OnRenderSurfaceChanged(HyRenderSurface &renderSurfaceRef, uint32 uiChangedFlags) = 0;

	uint32 GetNumRenderSurfaces();

	int32 GetNumCameras2d();
	uint32 GetCameraWindowIndex2d(int iCameraIndex);
	HyRectangle<float> *GetCameraViewportRect2d(int iIndex);
	glm::mat4 *GetCameraView2d(int iIndex);

	int32 GetNumInsts3d();
	int32 GetNumCameras3d();

	int32 GetNumRenderStates2d();
	HyRenderState *GetRenderStatesPtr2d();
	char *GetVertexData2d();

	static IHyShader *FindShader(int32 iId);
	static IHyShader *MakeCustomShader();
	static IHyShader *MakeCustomShader(const char *szPrefix, const char *szName);

	void Render();
	void Draw2d();
	void SetMonitorDeviceInfo(std::vector<HyMonitorDeviceInfo> &info);
};

#endif /* IHyRenderer_h__ */
