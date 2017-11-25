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
	// VAO's cannot be shared between windows (aka OpenGL contexts), so there is a std::vector<> element for each window
	std::vector<std::map<HyShaderHandle, GLuint> >	m_VaoMapList;
	std::map<HyShaderHandle, GLuint>				m_GLShaderMap;

	glm::mat4										m_mtxView;
	glm::mat4										m_mtxProj;

	///////// 2D MEMBERS ////////////////////
	GLuint											m_hVBO2d;

public:
	HyOpenGL(HyDiagnostics &diagnosticsRef, std::vector<HyWindow *> &windowListRef);
	virtual ~HyOpenGL(void);

	virtual void SetCurrentWindow(uint32 uiIndex);

	virtual void StartRender() override;
	
	virtual void Begin_3d() override;
	virtual void DrawRenderState_3d(HyRenderState *pRenderState) override;

	virtual void Begin_2d() override;
	virtual void DrawRenderState_2d(HyRenderState *pRenderState) override;

	virtual void FinishRender() override;

	virtual void UploadShader(HyShaderProgramDefaults eDefaultsFrom, HyShader *pShader) override;
	virtual uint32 AddTexture(HyTextureFormat eDesiredFormat, int32 iNumLodLevels, uint32 uiWidth, uint32 uiHeight, unsigned char *pPixelData, uint32 uiPixelDataSize, HyTextureFormat ePixelDataFormat) override;
	virtual uint32 AddTextureArray(uint32 uiNumColorChannels, uint32 uiWidth, uint32 uiHeight, std::vector<unsigned char *> &pixelDataList, uint32 &uiNumTexturesUploadedOut) override;	// Returns texture's ID used for API specific drawing. May not fit entire array, 'uiNumTexturesUploaded' is how many textures it did upload.
	virtual void DeleteTexture(uint32 uiTextureHandle) override;

private:
	void CompileShader(HyShader *pShader, HyShaderType eType);
	void RenderPass2d(HyRenderState *pRenderState, HyCamera2d *pCamera);
};

#endif /* HyOpenGL_h__ */
