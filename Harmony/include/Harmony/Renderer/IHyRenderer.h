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

#include <queue>

class HyRenderState;
class IHyShader;
class HyStencil;
class HyWindow;
class HyGfxComms;
class HyDiagnostics;
class HyCamera2d;
class IHyLoadableData;

#define HY_MAX_PASSES_PER_BUFFER 32
#define HY_MAX_SHADER_PASSES_PER_INSTANCE 4

#define HY_RENDERSTATE_BUFFER_SIZE ((1024 * 1024) * 1) // 1MB
#define HY_VERTEX_BUFFER_SIZE ((1024 * 1024) * 2) // 2MB

class IHyRenderer
{
public:
	// Note: All offsets below are from the beginning of the buffer pointer, containing this structure
	struct RenderStateBufferHeader
	{
		uint32		uiNum3dRenderStates;
		uint32		uiNum2dRenderStates;
	};

protected:
	HyDiagnostics &									m_DiagnosticsRef;
	std::vector<HyWindow *> &						m_WindowListRef;

	char *											m_pRenderStateBuffer;
	char *											m_pVertexBuffer;
	size_t											m_uiVertexBufferUsedBytes;

	HyWindow *										m_pCurWindow;

	std::queue<IHyLoadableData *>					m_TxDataQueue;
	std::queue<IHyLoadableData *>					m_RxDataQueue;

	static int32									sm_iShaderIdCount;
	static std::map<HyShaderHandle, IHyShader *>	sm_ShaderMap;

	static std::map<HyStencilHandle, HyStencil *>	sm_StencilMap;

	// Diagnostics/Metrics
	uint32											m_uiSupportedTextureFormats;	// Bitflags that represent supported texture in 'HyTextureFormat' enum

public:
	IHyRenderer(HyDiagnostics &diagnosticsRef, std::vector<HyWindow *> &windowListRef);
	virtual ~IHyRenderer(void);

	char *GetRenderStateBuffer();
	char *GetVertexBuffer();
	void SetVertexBufferUsed(size_t uiNumBytes);

	void TxData(IHyLoadableData *pData);
	std::queue<IHyLoadableData *> &RxData();

	void SetRendererInfo(const std::string &sApiName, const std::string &sVersion, const std::string &sVendor, const std::string &sRenderer, const std::string &sShader, int32 iMaxTextureSize, const std::string &sCompressedTextures);
	
	uint32 GetNumWindows();
	virtual void SetCurrentWindow(uint32 uiIndex);

	virtual void StartRender() = 0;

	virtual void Begin_3d() = 0;
	virtual void DrawRenderState_3d(HyRenderState *pRenderState) = 0;

	virtual void Begin_2d() = 0;
	virtual void DrawRenderState_2d(HyRenderState *pRenderState) = 0;

	virtual void FinishRender() = 0;

	virtual uint32 AddTexture(HyTextureFormat eDesiredFormat, int32 iNumLodLevels, uint32 uiWidth, uint32 uiHeight, unsigned char *pPixelData, uint32 uiPixelDataSize, HyTextureFormat ePixelDataFormat) = 0;
	// Returns texture's ID used for API specific drawing. May not fit entire array, 'uiNumTexturesUploaded' is how many textures it did upload.
	virtual uint32 AddTextureArray(uint32 uiNumColorChannels, uint32 uiWidth, uint32 uiHeight, std::vector<unsigned char *> &pixelDataList, uint32 &uiNumTexturesUploadedOut) = 0;
	virtual void DeleteTexture(uint32 uiTextureHandle) = 0;

	// 2D buffer accessors
	//uint32 GetNumCameras2d();
	//uint32 GetCameraWindowId2d(int iCameraIndex);
	//HyRectangle<float> *GetCameraViewportRect2d(int iIndex);
	//glm::mat4 *GetCameraView2d(int iIndex);

	//uint32 GetNumRenderStates2d();
	//HyRenderState *GetRenderStatesPtr2d();
	//char *GetVertexData2d();

	//// 3D buffer accessors
	//uint32 GetNumCameras3d();
	//uint32 GetNumInsts3d();

	static IHyShader *FindShader(HyShaderHandle hHandle);
	static IHyShader *MakeCustomShader();
	static IHyShader *MakeCustomShader(const char *szPrefix, const char *szName);

	static HyStencil *FindStencil(HyStencilHandle hHandle);
	static void AddStencil(HyStencil *pNewStencil);
	static void RemoveStencil(HyStencil *pNewStencil);

	void Render();
};

#endif /* IHyRenderer_h__ */
