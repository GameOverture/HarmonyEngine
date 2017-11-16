/**************************************************************************
 *	HyOpenGL.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyOpenGL_h__
#define HyOpenGL_h__

#include "Afx/HyStdAfx.h"

#include "Renderer/IHyRenderer.h"
#include "Renderer/OpenGL/HyOpenGLShader.h"

#define HyErrorCheck_OpenGL(funcLoc, funcName) { GLenum eError = glGetError(); HyAssert(eError == GL_NO_ERROR, "HyOpenGL error in " << funcLoc << " on function " << funcName << ": " << eError); }

class HyOpenGL : public IHyRenderer
{
protected:
	std::vector<std::map<HyOpenGLShader *, uint32> >	m_VaoMapList;

	glm::mat4					m_mtxView;
	glm::mat4					m_mtxProj;

	int32						m_iCurCamIndex;

	///////// 2D MEMBERS ////////////////////
	GLuint						m_hVBO2d;
	GLenum						m_eDrawMode;

public:
	HyOpenGL(HyDiagnostics &diagnosticsRef, std::vector<HyWindow *> &windowListRef);
	virtual ~HyOpenGL(void);

	void GenVAOs(HyOpenGLShader *pShaderKey);
	void BindVao(HyOpenGLShader *pShaderKey);

	virtual void SetCurrentWindow(uint32 uiIndex);

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

	virtual uint32 AddTexture(HyTextureFormat eDesiredFormat, int32 iNumLodLevels, uint32 uiWidth, uint32 uiHeight, unsigned char *pPixelData, uint32 uiPixelDataSize, HyTextureFormat ePixelDataFormat) override;
	// Returns texture's ID used for API specific drawing. May not fit entire array, 'uiNumTexturesUploaded' is how many textures it did upload.
	virtual uint32 AddTextureArray(uint32 uiNumColorChannels, uint32 uiWidth, uint32 uiHeight, std::vector<unsigned char *> &pixelDataList, uint32 &uiNumTexturesUploadedOut) override;
	virtual void DeleteTexture(uint32 uiTextureHandle) override;
};

#endif /* HyOpenGL_h__ */
