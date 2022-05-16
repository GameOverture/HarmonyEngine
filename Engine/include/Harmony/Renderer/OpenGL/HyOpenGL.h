/**************************************************************************
 *	HyOpenGL.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyOpenGL_h__
#define HyOpenGL_h__

#include "Afx/HyStdAfx.h"
#include "Renderer/IHyRenderer.h"

#define HY_NUM_PBO 10

#ifndef HY_PLATFORM_BROWSER
	#define HyErrorCheck_OpenGL(funcLoc, funcName) { GLenum eError = glGetError(); HyAssert(eError == GL_NO_ERROR, "HyOpenGL error in " << funcLoc << " on function " << funcName << ": " << eError); }
#else
	#define HyErrorCheck_OpenGL(funcLoc, funcName) // Avoid GPU-CPU Sync Points with Emscripten
#endif

class HyOpenGL : public IHyRenderer
{
#if defined(HY_USE_SDL2)
	SDL_GLContext									m_Context;
#endif

	// VAO's cannot be shared between windows (aka OpenGL contexts), so there is a std::map<> for each window
	std::vector<std::map<HyShaderHandle, GLuint> >	m_VaoMapList;
	std::map<HyShaderHandle, GLuint>				m_GLShaderMap;

	glm::mat4										m_mtxView;
	glm::mat4										m_mtxProj;

	enum PboState
	{
		PBO_Free = 0,
		PBO_Mapped,
		PBO_Pending3,	// Pending frames to give GPU time to finish its usage with the PBO
		PBO_Pending2,
		PBO_Pending1
	};
	GLuint *										m_pPboHandles;
	PboState *										m_pPboStates;

	//GLuint										m_hVBO3d;
	//bool											m_bVBO3dDirty;

public:
	HyOpenGL(HyDiagnostics &diagnosticsRef, std::vector<HyWindow *> &windowListRef);
	virtual ~HyOpenGL(void);

	virtual void SetCurrentWindow(uint32 uiIndex) override;

	virtual void StartRender() override;
	
	virtual void Begin_3d() override;
	virtual void DrawRenderState_3d(HyRenderBuffer::State *pRenderState) override;

	virtual void Begin_2d() override;
	virtual void DrawRenderState_2d(HyRenderBuffer::State *pRenderState, IHyCamera<IHyNode2d> *pCamera) override;

	virtual void FinishRender() override;

	virtual void UploadShader(HyShader *pShader) override;
	virtual uint32 AddTexture(const HyTextureInfo &infoRef, uint32 uiWidth, uint32 uiHeight, unsigned char *pPixelData, uint32 uiPixelDataSize, uint32 hPBO) override;
	virtual uint32 AddTextureArray(const HyTextureInfo &infoRef, uint32 uiWidth, uint32 uiHeight, std::vector<unsigned char *> &pixelDataList, uint32 &uiNumTexturesUploadedOut) override;
	virtual void DeleteTexture(uint32 uiTextureHandle) override;
	virtual uint32 GenerateVertexBuffer() override;
	virtual uint8 *GetPixelBufferPtr(uint32 uiMaxBufferSize, uint32 &hPboOut) override;

private:
	void CompileShader(HyShader *pShader, HyShaderType eType);
	void RenderPass2d(HyRenderBuffer::State *pRenderState, IHyCamera<IHyNode2d> *pCamera);
};

#endif /* HyOpenGL_h__ */
