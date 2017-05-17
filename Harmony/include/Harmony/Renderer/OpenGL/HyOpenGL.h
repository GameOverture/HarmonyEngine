/**************************************************************************
 *	HyOpenGL.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyOpenGL_h__
#define __HyOpenGL_h__

#include "Afx/HyStdAfx.h"

#include "Renderer/IHyRenderer.h"
#include "Renderer/OpenGL/HyOpenGLShader.h"

#define HyErrorCheck_OpenGL(funcLoc, funcName) HyAssert(glGetError() == GL_NO_ERROR, "HyOpenGL error in " << funcLoc << " on function " << funcName << ": " << glGetError());

class HyOpenGL : public IHyRenderer
{
protected:
	glm::mat4				m_mtxView;
	glm::mat4				m_mtxProj;

	int32					m_iCurCamIndex;
	
	enum eMatrixStack
	{
		MTX_NOTSET = 0,
		MTX_CAMVIEW,
		MTX_SCREENVIEW
	};

	///////// 2D MEMBERS ////////////////////
	GLuint					m_hVBO2d;
	GLenum					m_eDrawMode;

public:
	HyOpenGL(HyGfxComms &gfxCommsRef, IHyInput &inputRef, HyDiagnostics &diagnosticsRef, bool bShowCursor, std::vector<HyWindow *> &windowListRef);
	virtual ~HyOpenGL(void);

	virtual bool Initialize() override;

	virtual void StartRender() override;
	
	virtual void Init_3d() override;
	virtual bool BeginPass_3d() override;
	virtual void SetRenderState_3d(uint32 uiNewRenderState) override;
	virtual void End_3d() override;

	virtual void Init_2d() override;
	virtual bool BeginPass_2d() override;
	virtual void DrawRenderState_2d(HyRenderState &renderState) override;
	virtual void End_2d() override;

	virtual void FinishRender() override;

	virtual uint32 AddTexture(uint32 uiNumColorChannels, uint32 uiWidth, uint32 uiHeight, unsigned char *pPixelData) override;
	// Returns texture's ID used for API specific drawing. May not fit entire array, 'uiNumTexturesUploaded' is how many textures it did upload.
	virtual uint32 AddTextureArray(uint32 uiNumColorChannels, uint32 uiWidth, uint32 uiHeight, std::vector<unsigned char *> &pixelDataList, uint32 &uiNumTexturesUploadedOut) override;
	virtual void DeleteTextureArray(uint32 uiTextureHandle) override;

	virtual void OnRenderSurfaceChanged(HyRenderSurface &renderSurfaceRef, uint32 uiChangedFlags) override;

protected:
	void SetCameraMatrices_2d(eMatrixStack eMtxStack);
};

#endif /* __HyOpenGL_h__ */
