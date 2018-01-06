/**************************************************************************
 *	IHyRenderer.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2012 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef IHyRenderer_h__
#define IHyRenderer_h__

#include "Afx/HyStdAfx.h"
#include "Renderer/Effects/HyShader.h"

#include <queue>

class HyRenderState;
class HyStencil;
class HyPortal2d;
class IHyDrawInst2d;
class HyWindow;
class HyGfxComms;
class HyDiagnostics;
class HyCamera2d;
class IHyLoadableData;

typedef uint32 HyCullMask;
#define HY_MAX_PASSES_PER_BUFFER 32						// Number of bits held in 'HyCullMask'
#define HY_FULL_CULL_MASK 0xFFFFFFFF

#define HY_RENDERSTATE_BUFFER_SIZE ((1024 * 1024) * 1)	// 1MB
#define HY_VERTEX_BUFFER_SIZE ((1024 * 1024) * 2)		// 2MB

class IHyRenderer
{
public:
	struct RenderStateBufferHeader
	{
		uint32		uiNum3dRenderStates;
		uint32		uiNum2dRenderStates;
	};

protected:
	HyDiagnostics &									m_DiagnosticsRef;
	std::vector<HyWindow *> &						m_WindowListRef;

	// Preallocated buffers
	char * const									m_pBUFFER_RENDERSTATES;
	char * const									m_pBUFFER_VERTEX;

	// Message queues (transfer and receive)
	std::queue<IHyLoadableData *>					m_TxDataQueue;
	std::queue<IHyLoadableData *>					m_RxDataQueue;

	// Render states and their vertex data
	char *											m_pRenderStatesUserStartPos; // Includes RenderStateBufferHeader
	char *											m_pCurRenderStateWritePos;
	char *											m_pCurVertexWritePos;
	size_t											m_uiVertexBufferUsedBytes;
	HyWindow *										m_pCurWindow;

	// Effects containers
	static std::map<HyShaderHandle, HyShader *>		sm_ShaderMap;
	static std::map<HyStencilHandle, HyStencil *>	sm_StencilMap;
	static std::map<HyPortal2dHandle, HyPortal2d *>	sm_Portal2dMap;

	// Built-in shaders
	HyShader *										m_pShaderQuadBatch;
	HyShader *										m_pShaderPrimitive;

	// Diagnostics/Metrics
	uint32											m_uiSupportedTextureFormats;	// Bitflags that represent supported texture in 'HyTextureFormat' enum

public:
	IHyRenderer(HyDiagnostics &diagnosticsRef, std::vector<HyWindow *> &windowListRef);
	virtual ~IHyRenderer(void);

	void PrepareBuffers();
	void AppendRenderState(uint32 uiId, /*const*/ IHyDrawInst2d &instanceRef, HyCullMask uiCullMask);

	void TxData(IHyLoadableData *pData);
	std::queue<IHyLoadableData *> &RxData();

	void SetRendererInfo(const std::string &sApiName, const std::string &sVersion, const std::string &sVendor, const std::string &sRenderer, const std::string &sShader, int32 iMaxTextureSize, const std::string &sCompressedTextures);

	HyShaderHandle GetDefaultShaderHandle(HyType eType);
	
	uint32 GetNumWindows();
	virtual void SetCurrentWindow(uint32 uiIndex);

	virtual void StartRender() = 0;

	virtual void Begin_3d() = 0;
	virtual void DrawRenderState_3d(HyRenderState *pRenderState) = 0;

	virtual void Begin_2d() = 0;
	virtual void DrawRenderState_2d(HyRenderState *pRenderState) = 0;

	virtual void FinishRender() = 0;

	virtual void UploadShader(HyShaderProgramDefaults eDefaultsFrom, HyShader *pShader) = 0;
	virtual uint32 AddTexture(HyTextureFormat eDesiredFormat, int32 iNumLodLevels, uint32 uiWidth, uint32 uiHeight, unsigned char *pPixelData, uint32 uiPixelDataSize, HyTextureFormat ePixelDataFormat) = 0;
	virtual uint32 AddTextureArray(uint32 uiNumColorChannels, uint32 uiWidth, uint32 uiHeight, std::vector<unsigned char *> &pixelDataList, uint32 &uiNumTexturesUploadedOut) = 0;	// Returns texture's ID used for API specific drawing. May not fit entire array, 'uiNumTexturesUploaded' is how many textures it did upload.
	virtual void DeleteTexture(uint32 uiTextureHandle) = 0;

	static HyShader *FindShader(HyShaderHandle hHandle);
	static void AddShader(HyShader *pShader);
	static void RemoveShader(HyShader *pShader);

	static HyStencil *FindStencil(HyStencilHandle hHandle);
	static void AddStencil(HyStencil *pStencil);
	static void RemoveStencil(HyStencil *pStencil);

	static HyPortal2d *FindPortal2d(HyPortal2dHandle hHandle);
	static void AddPortal2d(HyPortal2d *pPortal2d);
	static void RemovePortal2d(HyPortal2d *pPortal2d);

	void ProcessMsgs();
	void Render();
};

#endif /* IHyRenderer_h__ */
