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
#include "Renderer/Components/HyRenderBuffer.h"
#include "Renderer/Components/HyVertexBuffer.h"
#include "Renderer/Effects/HyShader.h"

class HyStencil;
class HyPortal2d;
class IHyDrawable3d;
class IHyDrawable2d;
class HyWindow;
class HyGfxComms;
class HyDiagnostics;
class IHyCamera;
class IHyLoadableData;

class IHyRenderer
{
protected:
	static IHyRenderer *							sm_pInstance;

	HyDiagnostics &									m_DiagnosticsRef;
	std::vector<HyWindow *> &						m_WindowListRef;

	// Preallocated buffers
	HyRenderBuffer									m_RenderBuffer;
	HyVertexBuffer									m_VertexBuffer;

	HyWindow *										m_pCurWindow;
	
	// Message queues (transfer and receive)
	std::queue<IHyLoadableData *>					m_TxDataQueue;
	std::queue<IHyLoadableData *>					m_RxDataQueue;
	
	// Effects containers
	std::map<HyShaderHandle, HyShader *>			m_ShaderMap;
	std::map<HyStencilHandle, HyStencil *>			m_StencilMap;

	// Built-in shaders
	HyShader *										m_pShaderQuadBatch;
	HyShader *										m_pShaderPrimitive;

	// Diagnostics/Metrics
	uint32											m_uiSupportedTextureFormats;	// Bitflags that represent supported texture in 'HyTextureFormat' enum

public:
	IHyRenderer(HyDiagnostics &diagnosticsRef, std::vector<HyWindow *> &windowListRef);
	virtual ~IHyRenderer(void);

	void PrepareBuffers();
	void AppendDrawable3d(uint32 uiId, IHyDrawable3d &instanceRef, HyCameraMask uiCameraMask);
	void AppendDrawable2d(uint32 uiId, IHyDrawable2d &instanceRef, HyCameraMask uiCameraMask);

	HyVertexBufferHandle AppendVertexData3d(const uint8 *pData, uint32 uiSize);

	void TxData(IHyLoadableData *pData);
	std::queue<IHyLoadableData *> &RxData();

	void SetRendererInfo(const std::string &sApiName, const std::string &sVersion, const std::string &sVendor, const std::string &sRenderer, const std::string &sShader, int32 iMaxTextureSize, const std::string &sCompressedTextures);

	HyShaderHandle GetDefaultShaderHandle(HyType eType);
	
	uint32 GetNumWindows();
	virtual void SetCurrentWindow(uint32 uiIndex);
	
	void ProcessMsgs();
	void Render();

	static HyShader *FindShader(HyShaderHandle hHandle);
	static void AddShader(HyShader *pShader);
	static void RemoveShader(HyShader *pShader);

	static HyStencil *FindStencil(HyStencilHandle hHandle);
	static void AddStencil(HyStencil *pStencil);
	static void RemoveStencil(HyStencil *pStencil);

	virtual void StartRender() = 0;
	virtual void Begin_3d() = 0;
	virtual void DrawRenderState_3d(HyRenderBuffer::State *pRenderState) = 0;
	virtual void Begin_2d() = 0;
	virtual void DrawRenderState_2d(HyRenderBuffer::State *pRenderState, IHyCamera *pCamera) = 0;
	virtual void FinishRender() = 0;
	virtual void UploadShader(HyShaderProgramDefaults eDefaultsFrom, HyShader *pShader) = 0;
	virtual uint32 AddTexture(HyTextureFormat eDesiredFormat, int32 iNumLodLevels, uint32 uiWidth, uint32 uiHeight, unsigned char *pPixelData, uint32 uiPixelDataSize, HyTextureFormat ePixelDataFormat) = 0;
	virtual uint32 AddTextureArray(uint32 uiNumColorChannels, uint32 uiWidth, uint32 uiHeight, std::vector<unsigned char *> &pixelDataList, uint32 &uiNumTexturesUploadedOut) = 0;	// Returns texture's ID used for API specific drawing. May not fit entire array, 'uiNumTexturesUploaded' is how many textures it did upload.
	virtual void DeleteTexture(uint32 uiTextureHandle) = 0;
};

#endif /* IHyRenderer_h__ */
