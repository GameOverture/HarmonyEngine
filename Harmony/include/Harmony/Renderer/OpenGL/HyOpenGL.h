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
	HyOpenGL(HyGfxComms &gfxCommsRef, std::vector<HyWindow *> &windowListRef);
	virtual ~HyOpenGL(void);

	virtual void StartRender();
	
	virtual void Init_3d();
	virtual bool BeginPass_3d();
	virtual void SetRenderState_3d(uint32 uiNewRenderState);
	virtual void End_3d();

	virtual void Init_2d();
	virtual bool BeginPass_2d();
	virtual void DrawRenderState_2d(HyRenderState &renderState);
	virtual void End_2d();

	virtual void FinishRender();

	// AddTextureArray will attempt to create entire array. If out of memory it will continually slice array in halfs, and fail once beyond single texture per array
	//
	// vector pair:
	// uint32 (out param)			= each texture's ID used for API specific drawing.
	// unsigned char * (in param)	= pixel data for each texture
	virtual void AddTextureArray(uint32 uiNumColorChannels, uint32 uiWidth, uint32 uiHeight, std::vector<std::pair<uint32, unsigned char *> > &PixelDataList);
	virtual void DeleteTextureArray(uint32 uiTextureHandle);

	virtual void OnRenderSurfaceChanged(RenderSurface &renderSurfaceRef, uint32 uiChangedFlags);

protected:
	bool Initialize();

	void SetCameraMatrices_2d(eMatrixStack eMtxStack);
};

#endif /* __HyOpenGL_h__ */
