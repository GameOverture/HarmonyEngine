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
#include "Scene/Nodes/Objects/HyCamera.h"

class HyStencil;
class HyPortal2d;
class IHyDrawable3d;
class IHyDrawable2d;
class HyWindow;
class HyGfxComms;
class HyDiagnostics;
class IHyFile;

class IHyRenderer
{
protected:
	static IHyRenderer *							sm_pInstance;

	std::vector<HyWindow *> &						m_WindowListRef;
	HyDiagnostics &									m_DiagnosticsRef;

	// Preallocated buffers
	HyRenderBuffer									m_RenderBuffer;
	HyVertexBuffer									m_VertexBuffer;

	HyWindow *										m_pCurWindow;
	
	// Message queues (transfer and receive)
	std::queue<IHyFile *>							m_TxDataQueue;
	std::queue<IHyFile *>							m_RxDataQueue;
	
	// Effects containers
	std::map<HyShaderHandle, HyShader *>			m_ShaderMap;
	std::map<HyStencilHandle, HyStencil *>			m_StencilMap;

	// Built-in shaders
	HyShader *										m_pShaderQuadBatch;
	//HyShader *										m_pShaderTileMapLayer; // Tile Maps have multiple potential shader programs
	HyShader *										m_pShaderPrimitive;
	HyShader *										m_pShaderSpine;

public:
	IHyRenderer(std::vector<HyWindow *> &windowListRef, HyDiagnostics &diagnosticsRef);
	virtual ~IHyRenderer(void);

	void SetRendererInfo(const std::string &sApiName, const std::string &sVersion, const std::string &sVendor, const std::string &sRenderer, const std::string &sShader, int32 iMaxTextureSize, const std::string &sCompressedTextures);

	void PrepareBuffers(float fExtrapolatePercent);
	void AppendDrawable3d(uint32 uiId, IHyDrawable3d &instanceRef, HyCameraMask uiCameraMask, float fExtrapolatePercent);
	void AppendDrawable2d(uint32 uiId, IHyDrawable2d &instanceRef, HyCameraMask uiCameraMask, float fExtrapolatePercent);

	HyVertexBufferHandle AppendVertexData3d(const uint8 *pData, uint32 uiSize);

	void TxData(IHyFile *pData);
	std::queue<IHyFile *> &RxData();

	HyShaderHandle GetDefaultShaderHandle(HyType eType);
	
	virtual void SetCurrentWindow(uint32 uiIndex);
	
	void ProcessMsgs();
	void Render();

	int32 GetVSync(uint32 uiWindowIndex) const;
	virtual void SetVSync(int32 iVSync, uint32 uiWindowIndex) = 0;

	virtual void StartRender() = 0;
	virtual void Begin_3d() = 0;
	virtual void DrawRenderState_3d(HyRenderBuffer::State *pRenderState) = 0;
	virtual void Begin_2d() = 0;
	virtual void DrawRenderState_2d(HyRenderBuffer::State *pRenderState, IHyCamera<IHyNode2d> *pCamera) = 0;
	virtual void FinishRender() = 0;
	virtual void UploadShader(HyShader *pShader) = 0;
	virtual HyTextureHandle AddTexture(const HyTextureInfo formatInfo, uint32 uiWidth, uint32 uiHeight, unsigned char *pPixelData, uint32 uiPixelDataSize) = 0; // Returns texture's API specific ID used for drawing
	virtual HyTextureHandle AddTextureArray(const HyTextureInfo formatInfo, uint32 uiWidth, uint32 uiHeight, const std::vector<unsigned char *> &pixelDataList, uint32 uiPixelDataSizePerTexture) = 0; // Returns texture's API specific ID used for drawing
	virtual void DeleteTexture(HyTextureHandle hTexture) = 0;
	virtual std::pair<HyBufferHandle, HyTextureHandle> AddTextureBufferObject(const HyTextureInfo formatInfo, unsigned char *pData, uint32 uiDataSize) = 0;
	virtual void DeleteTextureBufferObject(std::pair<HyBufferHandle, HyTextureHandle> hTboPair) = 0;
	virtual HyBufferHandle GenerateVertexBuffer() = 0;	// Returns the graphics API handle to a new vertex buffer
	virtual HyBufferHandle GenerateIndexBuffer() = 0;
	virtual void GetTextureSize(HyTextureHandle hTexture, uint32 &uiWidthOut, uint32 &uiHeightOut) = 0;
	virtual void WriteTexels(HyTextureHandle hTexture) = 0;
	virtual bool SaveScreenshot(const std::string &sFilePath, uint32 uiWindowIndex) = 0;

	static HyShader *FindShader(HyShaderHandle hHandle);
	static void AddShader(HyShader *pShader);
	static void RemoveShader(HyShader *pShader);

	static HyStencil *FindStencil(HyStencilHandle hHandle);
	static void AddStencil(HyStencil *pStencil);
	static void RemoveStencil(HyStencil *pStencil);
};

#endif /* IHyRenderer_h__ */
