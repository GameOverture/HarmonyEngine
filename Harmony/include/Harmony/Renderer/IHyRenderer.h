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

#include <queue>

class IHyShader;
class HyWindow;
class HyGfxComms;
class HyDiagnostics;

#define HY_GFX_BUFFER_SIZE ((1024 * 1024) * 2) // 2MB
#define HYDRAWBUFFERHEADER reinterpret_cast<DrawBufferHeader *>(m_pDrawBuffer)

class IHyRenderer
{
public:
	// Note: All offsets below are from the beginning of the buffer pointer, containing this structure
	struct DrawBufferHeader
	{
		uint32		uiReturnFlags;
		size_t		uiOffsetTo3d;
		size_t		uiOffsetTo2d;
		size_t		uiOffsetToVertexData2d;
		size_t		uiVertexBufferSize2d;
		size_t		uiOffsetToCameras3d;
		size_t		uiOffsetToCameras2d;
	};

protected:
	HyDiagnostics &							m_DiagnosticsRef;
	bool									m_bShowCursor;
	std::vector<HyWindow *> &				m_WindowListRef;
	HyWindow *								m_pCurWindow;

	char *									m_pDrawBuffer;

	std::queue<IHyLoadableData *>			m_TxDataQueue;
	std::queue<IHyLoadableData *>			m_RxDataQueue;

	static int32							sm_iShaderIdCount;
	static std::map<int32, IHyShader *>		sm_ShaderMap;

	HyRenderState *							m_pCurRenderState;

	uint32									m_uiSupportedTextureFormats;	// Bitflags that represent supported texture in 'HyTextureFormat' enum

	// Diagnostics/Metrics
	uint32									m_uiNumRenderStates;

public:
	IHyRenderer(HyDiagnostics &diagnosticsRef, bool bShowCursor, std::vector<HyWindow *> &windowListRef);
	virtual ~IHyRenderer(void);

	char *GetDrawBuffer();

	void TxData(IHyLoadableData *pData);
	std::queue<IHyLoadableData *> &RxData();

	void SetRendererInfo(const std::string &sApiName, const std::string &sVersion, const std::string &sVendor, const std::string &sRenderer, const std::string &sShader, int32 iMaxTextureSize, const std::string &sCompressedTextures);
	
	uint32 GetNumWindows();
	virtual void SetCurrentWindow(uint32 uiIndex);

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

	// 2D buffer accessors
	uint32 GetNumCameras2d();
	uint32 GetNumRenderStates2d();
	uint32 GetCameraWindowId2d(int iCameraIndex);
	HyRectangle<float> *GetCameraViewportRect2d(int iIndex);
	glm::mat4 *GetCameraView2d(int iIndex);
	HyRenderState *GetRenderStatesPtr2d();
	char *GetVertexData2d();

	// 3D buffer accessors
	uint32 GetNumCameras3d();
	uint32 GetNumInsts3d();

	static IHyShader *FindShader(int32 iId);
	static IHyShader *MakeCustomShader();
	static IHyShader *MakeCustomShader(const char *szPrefix, const char *szName);

	void Render();
	void Draw2d();
};

#endif /* IHyRenderer_h__ */
